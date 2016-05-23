#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _Wlz3DGetSection_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         binWlzApp/Wlz3DGetSection.c
* \author       Richard Baldock
* \date         March 1999
* \version      $Id$
* \par
* Address:
*               MRC Human Genetics Unit,
*               MRC Institute of Genetics and Molecular Medicine,
*               University of Edinburgh,
*               Western General Hospital,
*               Edinburgh, EH4 2XU, UK.
* \par
* Copyright (C), [2012],
* The University Court of the University of Edinburgh,
* Old College, Edinburgh, UK.
* 
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be
* useful but WITHOUT ANY WARRANTY; without even the implied
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
* PURPOSE.  See the GNU General Public License for more
* details.
*
* You should have received a copy of the GNU General Public
* License along with this program; if not, write to the Free
* Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA  02110-1301, USA.
* \brief	Gets an arbitrary slice from a 3D object.
* \ingroup	BinWlzApp
*
* \par Binary
* \ref wlz3dgetsection "Wlz3DGetSection"
*/

/*!
\ingroup BinWlzApp
\defgroup wlz3dgetsection Wlz3DGetSection
\par Name
Wlz3DGetSection - gets an arbitrary slice from a 3D object.
\par Synopsis
\verbatim
Wlz3DGetSection  [-h] [-A] [-C] [-L] [-N]
                 [-a <pitch,yaw[,roll]>] [-f <fx,fy,fz>]
                 [-d <dist>] [-b <parameter bibfile>] [-m <mode>]
		 [-r <vox scaling>] [-s <scale>] [-o <output file>]
		 [-t<view transform>] [-u<ux,uy,uz>] [<3D object input file>]
\endverbatim
\par Options
<table width="500" border="0">
  <tr> 
    <td><b>-h</b></td>
    <td>Prints usage information.</td>
  </tr>
  <tr> 
    <td><b>-A</b></td>
    <td>Output all sections in the transformed view.
        This will output each section with the given
	view orientation into files using the output
	filename as a stub. If no output file is
	defined plane_????.wlz will be used.</td>
  </tr>
  <tr> 
    <td><b>-C</b></td>
    <td>Use classifier interpolation.</td>
  </tr>
  <tr> 
    <td><b>-L</b></td>
    <td>Use linear interpolation.</td>
  </tr>
  <tr> 
    <td><b>-N</b></td>
    <td>Use nearest neighbour interpolation.</td>
  </tr>
  <tr> 
    <td><b>-a</b></td>
    <td>Viewing angles in degrees.
        If roll is defined then the mode is "absolute"..</td>
  </tr>
  <tr> 
    <td><b>-b</b></td>
    <td>Bibfile defining the view parameters e.g. from MAPaint
        or warp input I/O.
	Override all other parameter input.</td>
  </tr>
  <tr> 
    <td><b>-f</b></td>
    <td>Fixed point position, default (0,0,0).</td>
  </tr>
  <tr> 
    <td><b>-d</b></td>
    <td>Distance parameter, default 0.0.</td>
  </tr>
  <tr> 
    <td><b>-m</b></td>
    <td>Viewing mode, possible values:
    <table width="500" border="0">
      <tr> <td><b>Parameter value</b></td> <td><b>Viewing mode</b></td> </tr>
      <tr> <td>0</td> <td>up-is-up, default</td> </tr>
      <tr> <td>1</td> <td>statue</td> </tr>
      <tr> <td>2</td> <td>absolute</td> </tr>
    </table>
    </td>
  </tr>
  <tr> 
    <td><b>-r</b></td>
    <td>Voxel size rescaling mode flags:
	 bit 1 set - use voxel-size rescaling,
	 bit 2 set - enable global scaling,
        default 1.0.</td>
  </tr>
  <tr> 
    <td><b>-s</b></td>
    <td>Scale factor, default 1.0.</td>
  </tr>
  <tr> 
    <td><b>-o</b></td>
    <td>Output filename, default is stdout.</td>
  </tr>
  <tr> 
    <td><b>-t</b></td>
    <td>View transform file.</td>
  </tr>
  <tr> 
    <td><b>-u</b></td>
    <td>Up vector for up-is-up mode, default (0, 0, -1).</td>
  </tr>
</table>
\par Description
Gets an arbitrary slice from a 3D object,
 writing the 2D object to standard output.
\par Examples
\verbatim
\endverbatim
\par File
\ref Wlz3DGetSection.c "Wlz3DGetSection.c"
\par See Also
\ref BinWlzApp "WlzIntro(1)"
\ref WlzGetSectionFromObject "WlzGetSectionFromObject(3)"
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <sys/time.h>
#include <Wlz.h>
#include <WlzExtFF.h>
#include <bibFile.h>

/* externals required by getopt  - not in ANSI C standard */
#ifdef __STDC__ /* [ */
extern int      getopt(int argc, char * const *argv, const char *optstring);
 
extern int 	optind, opterr, optopt;
extern char     *optarg;
#endif /* __STDC__ ] */

static void usage(char *proc_str)
{
  (void )fprintf(stderr,
	  "Usage:\t%s [-a <pitch,yaw[,roll]>] [-f <fx,fy,fz>] [-d <dist>]\n"
	  "\t[-b <parameter bibfile>] [-m <mode>] [-s <scale>]\n"
	  "\t[-o <output file>] [-u<ux,uy,uz>] [-A] [-C] [-L] [-N]\n"
	  "\t[-r<vox scaling>] [-R <ROI domain>] [<3D object input file>]\n"
	  "\tGet an arbitrary slice from a 3D object\n"
	  "\twriting the 2D object to standard output\n"
	  "Version: %s\n"
	  "Options:\n"
	  "\t  -A                 Output all sections in the transformed view.\n"
	  "\t                     This will output each section with the given\n"
	  "\t                     view orientation into files using the output\n"
	  "\t                     filename as a stub. If no output file is\n"
	  "\t                     defined plane_????.wlz will be used\n"
	  "\t  -a<pitch,yaw[,roll]> viewing angles in degrees. If roll\n"
	  "\t                       is defined then the mode is \"absolute\"\n"
	  "\t  -b<bibfile>        bibfile defining the view parameters e.g.\n"
	  "\t                     from MAPaint view or warp input I/O\n"
	  "\t                     Override all other parameter input\n"
	  "\t  -f<fx,fy,fz>       fixed point position, default - (0,0,0)\n"
	  "\t  -d<dist>           distance parameter, default - 0.0\n"
	  "\t  -m<mode>           viewing mode, possible values:\n"
	  "\t                       mode = 0 - up-is-up (default)\n"
	  "\t                       mode = 1 - statue\n"
	  "\t                       mode = 2 - absolute\n"
	  "\t  -o<output file>    Output filename, default to stdout\n"
	  "\t  -r<vox rescale>    Voxel size rescaling mode flags:\n"
	  "\t                       bit 1 set - use voxel-size rescaling\n"
	  "\t                       bit 2 set - enable global scaling\n"
	  "\t                     default - 0.\n"
	  "\t  -s<scale>          Scale factor, default - 1.0\n"
	  "\t  -u<ux,uy,uz>       Up vector for up-is-up mode.\n"
	  "\t			  Default: (0,0,-1)\n"
	  "\t  -C                 Use classifier interpolation\n"
	  "\t  -L                 Use linear interpolation\n"
	  "\t  -N                 Use nearest neighbour interpolation\n"
          "\t  -r<vox scaling>    Voxel size rescaling flags:\n"
	  "\t                       bit 1 set - use voxel-size rescaling,\n"
	  "\t                       bit 2 set - enable global scaling,\n"
          "\t                       default 1.0.\n"
	  "\t  -R<ROI domain>     Only calculate values within the ROI domain.\n"
	  "\t  -T                 Report section cutting time.\n"
	  "\t  -t                 View transform file.\n"
	  "\t  -h                 Help - prints this usage message\n",
	  proc_str,
	  WlzVersion());
  return;
}
 
int main(int	argc,
	 char	**argv)
{

  WlzObject	*obj = NULL, *nobj = NULL, *subDomain = NULL;
  FILE		*inFP = NULL, *outFP = NULL, *trFP = NULL, *bibFP = NULL;
  char		*outFile = NULL, *trFile = NULL, *bibFile = NULL;
  char 		optList[] = "ACLNTa:b:d:f:m:o:r:s:t:u:R:h";
  int		option;
  int		i,
  		j,
		iVal,
		timer = 0,
		voxRescale = 0,
  		allFlg = 0;
  double	dist=0.0, pitch=0.0, yaw=0.0, roll=0.0;
  double	scale=1.0;
  WlzDVertex3	fixed={0.0,0.0,0.0};
  WlzDVertex3	up={0.0,0.0,-1.0};
  WlzThreeDViewStruct	*viewStr=NULL;
  WlzThreeDViewMode mode=WLZ_UP_IS_UP_MODE;
  WlzInterpolationType interp = WLZ_INTERPOLATION_NEAREST;
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  BibFileRecord	*bibfileRecord;
  BibFileError	bibFileErr;
  struct timeval times[3];
  char		*errMsg;
    
  /* additional defaults */
  outFile = "-";
  bibFile = NULL;
  trFile = NULL;
  subDomain = NULL;

  /* read the argument list and check for an input file */
  opterr = 0;
  while( (option = getopt(argc, argv, optList)) != EOF ){
    switch( option ){

    case 'A':
      allFlg = 1;
      break;

    case 'C':
      interp = WLZ_INTERPOLATION_CLASSIFY_1;
      break;

    case 'L':
      interp = WLZ_INTERPOLATION_LINEAR;
      break;

    case 'N':
      interp = WLZ_INTERPOLATION_NEAREST;
      break;

    case 'a':
      switch( sscanf(optarg, "%lg,%lg,%lg", &pitch, &yaw, &roll) ){
      default:
	usage(argv[0]);
	return 1;
      case 2:
	break;
      case 3:
	mode = WLZ_ZETA_MODE;
	break;
      }
      break;

    case 'b':
      bibFile = optarg;
      break;

    case 'd':
      if( sscanf(optarg, "%lg", &dist) < 1 ){
	usage(argv[0]);
	return 1;
      }
      break;

    case 'f':
      if( sscanf(optarg, "%lg,%lg,%lg", &(fixed.vtX), &(fixed.vtY),
		 &(fixed.vtZ)) < 3 ){
	usage(argv[0]);
	return 1;
      }
      break;

    case 'm':
      if( sscanf(optarg, "%d", &iVal) < 1 ){
	usage(argv[0]);
	return 1;
      }
      else if( mode != WLZ_ZETA_MODE ){
	switch( iVal ){
	default:
	  usage(argv[0]);
	  return 1;
	case 0:
	  mode = WLZ_UP_IS_UP_MODE;
	  break;
	case 1:
	  mode = WLZ_STATUE_MODE;
	  break;
	case 2:
	  mode = WLZ_ZETA_MODE;
	  break;
	}
      }
      break;

    case 'o':
      outFile = optarg;
      break;

    case 'r':
      if( sscanf(optarg, "%d", &voxRescale) < 1 ){
	usage(argv[0]);
	return 1;
      }
      break;

    case 's':
      if( sscanf(optarg, "%lg", &scale) < 1 ){
	usage(argv[0]);
	return 1;
      }
      break;

    case 'u':
      if( sscanf(optarg, "%lg,%lg,%lg", &(up.vtX), &(up.vtY),
		 &(up.vtZ)) < 3 ){
	usage(argv[0]);
	return 1;
      }
      break;

    case 'R':
      if((inFP = fopen(optarg, "rb"))){
	if((subDomain = WlzReadObj(inFP, &errNum)) == NULL){
	  fprintf(stderr, "%s: can't read sub-domain object %s\n", argv[0], optarg);
	  usage(argv[0]);
	  return 1;
	}
	fclose(inFP);
      }
      else {
	fprintf(stderr, "%s: can't open file %s\n", argv[0], optarg);
	usage(argv[0]);
	return 1;
      }
      break;

    case 'T':
      timer = 1;
      break;
    case 't':
      trFile = optarg;
      break;
    case 'h':
    default:
      usage(argv[0]);
      return 0;

    }
  }

  /* check input file/stream */
  inFP = stdin;
  if( optind < argc ){
    if( (inFP = fopen(*(argv+optind), "rb")) == NULL ){
      fprintf(stderr, "%s: can't open file %s\n", argv[0], *(argv+optind));
      usage(argv[0]);
      return 1;
    }
  }

  /* check output file/stream */
  if( allFlg ){
    if(strcmp(outFile, "-"))
    {
      /* strip any file extension */
      for(i=0, j=strlen(outFile); i < strlen(outFile); i++){
	if( outFile[i] == '.' ){
	  j = i;
	}
      }
      outFile[j] = '\0';
  }
    else {
      outFile = "plane";
    }
  }
  else {
    if(strcmp(outFile, "-"))
    {
      if((outFP = fopen(outFile, "wb")) == NULL)
      {
	errNum = WLZ_ERR_WRITE_EOF;
      }
    }
    else
    {
      outFP = stdout;
    }
  }

  /* create view structure */
  if((viewStr = WlzMake3DViewStruct(WLZ_3D_VIEW_STRUCT, &errNum)) != NULL){
    viewStr->theta = yaw * WLZ_M_PI / 180.0;
    viewStr->phi = pitch * WLZ_M_PI / 180.0;
    viewStr->zeta = roll * WLZ_M_PI / 180.0;
    viewStr->dist = dist;
    viewStr->fixed = fixed;
    viewStr->up = up;
    viewStr->view_mode = mode;
    viewStr->scale = scale;
    viewStr->voxelRescaleFlg = voxRescale;
  }

  if((errNum == WLZ_ERR_NONE) && (bibFile != NULL) && (trFile != NULL)) {
    fprintf(stderr, "%s: both bibfile and transform file specified.\n",
            argv[0]);
  }

  /* check bibfile - select first section parameters in the file */
  if((errNum == WLZ_ERR_NONE) && (bibFile != NULL)){
    if((bibFP = fopen(bibFile, "r")) != NULL){
      bibFileErr = BibFileRecordRead(&bibfileRecord, &errMsg, bibFP);
      while((bibFileErr == BIBFILE_ER_NONE) &&
	    (strncmp(bibfileRecord->name, "Wlz3DSectionViewParams", 22))){
	BibFileRecordFree(&bibfileRecord);
	bibFileErr = BibFileRecordRead(&bibfileRecord, &errMsg, bibFP);
      }
      fclose( bibFP );
      if( bibFileErr != BIBFILE_ER_NONE ){
	fprintf(stderr, "%s: error reading bibfile: %s\n", argv[0], errMsg);
	AlcFree((void *) errMsg);
	return 1;
      }
      WlzEffBibParse3DSectionViewParamsRecord(bibfileRecord, viewStr);
      BibFileRecordFree(&bibfileRecord);
    }
    else {
      fprintf(stderr, "%s: can't open parameter bibfile %s\n", argv[0],
	      bibFile);
      return 1;
    }
  }
  
  if((errNum == WLZ_ERR_NONE) && (trFile != NULL)){
    if((trFP = fopen(trFile, "r")) != NULL){
      WlzObject *trObj = NULL;

      if((trObj = WlzReadObj(trFP, &errNum)) != NULL) {
        if(trObj->type != WLZ_3D_VIEW_STRUCT) {
	  errNum = WLZ_ERR_OBJECT_TYPE;
	}
	else if (trObj->domain.core == NULL) {
	  errNum = WLZ_ERR_DOMAIN_NULL;
	}
	else {
	  WlzThreeDViewStruct *fViewStr;

	  fViewStr = trObj->domain.vs3d;
	  viewStr->theta = fViewStr->theta;
	  viewStr->phi = fViewStr->phi;
	  viewStr->zeta = fViewStr->zeta;
	  viewStr->dist = fViewStr->dist;
	  viewStr->fixed = fViewStr->fixed;
	  viewStr->up = fViewStr->up;
	  viewStr->view_mode = fViewStr->view_mode;
	  viewStr->scale = fViewStr->scale;
	  viewStr->voxelRescaleFlg = fViewStr->voxelRescaleFlg;
	}
	(void )WlzFreeObj(trObj);
      }
    }
    if(errNum != WLZ_ERR_NONE) {
      fprintf(stderr,
              "%s: error reading section view transform from %s.\n",
              argv[0], trFile);
      return 1;
    }
  }

  /* read objects and section if possible */
  while((errNum == WLZ_ERR_NONE) &&
        ((obj = WlzReadObj(inFP, &errNum)) != NULL))
  {
    obj = WlzAssignObject(obj, &errNum);
    switch( obj->type )
    {
      case WLZ_CONTOUR:
      case WLZ_3D_DOMAINOBJ:
	if(voxRescale && obj->domain.core)
	{
	  viewStr->voxelSize[0] = obj->domain.p->voxel_size[0];
	  viewStr->voxelSize[1] = obj->domain.p->voxel_size[1];
	  viewStr->voxelSize[2] = obj->domain.p->voxel_size[2];
	}
	WlzInit3DViewStruct(viewStr, obj);
	if(timer)
	{
	  gettimeofday(times + 0, NULL);
	}
	if( allFlg ){
	  /* loop through all possible planes */
	  for(i=WLZ_NINT(viewStr->minvals.vtZ), j=0;
	      i <= WLZ_NINT(viewStr->maxvals.vtZ); i++, j++){
	    viewStr->dist = i;
	    WlzInit3DViewStruct(viewStr, obj);
	    nobj = WlzGetSubSectionFromObject(obj, subDomain, viewStr, interp,
					      NULL, &errNum);
	    if( nobj != NULL){
	      char	fileBuf[256];
	      sprintf(fileBuf, "%s%06d.wlz", outFile, j);
	      if((outFP = fopen(fileBuf, "w")) != NULL){
		 WlzWriteObj(outFP, nobj);
		 fclose(outFP);
	      }
	    }
	    else {
	      return errNum;
	    }
	    WlzFreeObj(nobj);
	  }
	}
	else {
	  nobj = WlzGetSubSectionFromObject(obj, subDomain, viewStr, interp,
					    NULL, &errNum);
	  if( nobj != NULL){
	    WlzWriteObj(outFP, nobj);
	  }
	  else {
	    return errNum;
	  }
	  WlzFreeObj(nobj);
	}
	if(timer)
	{
	  gettimeofday(times + 1, NULL);
	  ALC_TIMERSUB(times + 1, times + 0, times + 2);
	  (void )fprintf(stderr,
	                 "%s: Elapsed time = %g\n",
			 *argv,
			 times[2].tv_sec + (0.000001 * times[2].tv_usec));
	}
	break;

      default:
	WlzWriteObj(outFP, obj);
	break;
    }

    WlzFreeObj(obj);
  }
  if(errNum == WLZ_ERR_READ_EOF)
  {
    errNum = WLZ_ERR_NONE;
  }

  if( viewStr ){
    WlzFree3DViewStruct(viewStr);
  }
  if( subDomain ){
    WlzFreeObj(subDomain);
  }

  return errNum;
}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */
