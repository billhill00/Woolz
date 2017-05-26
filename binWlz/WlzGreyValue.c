#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _WlzGreyValue_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         binWlz/WlzGreyValue.c
* \author       Bill Hill
* \date         May 2004
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
* \brief	Gets a single grey value in an object.
* \ingroup	BinWlz
*
* \par Binary
* \ref wlzgreyvalue "WlzGreyValue"
*/

/*!
\ingroup BinWlz
\defgroup wlzgreyvalue WlzGreyValue
\par Name
WlzGreyValue - gets a single grey value in an object.
\par Synopsis
\verbatim
WlzGreyValue [-b] [-v#] [-x#] [-y#] [-z#] [-h] [<in object>]
\endverbatim
\par Options
<table width="500" border="0">
  <tr> 
    <td><b>-h</b></td>
    <td>Help, prints usage message.</td>
  </tr>
  <tr> 
    <td><b>-b</b></td>
    <td>Indicate background or foreground with b or f following value
        written, default false.</td>
  </tr>
  <tr> 
    <td><b>-c</b></td>
    <td>If getting values get and print connected values.</td>
  </tr>
  <tr> 
    <td><b>-v</b></td>
    <td>Grey value to be set, default false.</td>
  </tr>
  <tr> 
    <td><b>-</b></td>
    <td>Column position, default 0.</td>
  </tr>
  <tr> 
    <td><b>-y</b></td>
    <td>Line position.</td>
  </tr>
  <tr> 
    <td><b>-z</b></td>
    <td>Plane position.</td>
  </tr>
  <tr> 
    <td><b>-o</b></td>
    <td>Output file name.</td>
  </tr>
</table>
\par Description
Either sets or gets a grey value within the given objects values.
If the -v flag is given the objects value is set and the modified
object is written to the output file, but if the flag is not set
then the grey value (or multiple grey values for an object with
non-scalar values) is written out in ascii form with a new line
character. Only scalar values can currently be set.
\par Examples
\verbatim
WlzGreyValue -x 100 -y 100 toucan.wlz
Outputs as ascii the value of the pixel at coordinate (100,100).
\endverbatim
\par File
\ref WlzGreyValue.c "WlzGreyValue.c"
\par See Also
\ref BinWlz "WlzIntro(1)"
\ref WlzGreyValueGet "WlzGreyValueGet(3)"
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <Wlz.h>

extern int      getopt(int argc, char * const *argv, const char *optstring);
 
extern char     *optarg;
extern int      optind,
                opterr,
                optopt;

int             main(int argc, char **argv)
{
  int		option,
		bgdFlag = 0,
		conFlag = 0,
		setVal = 0,
  		ok = 1,
		usage = 0;
  unsigned int	nElm = 1,
  		vpe = 1;
  WlzDVertex3	pos;
  WlzPixelV	pix0,
  		pix1;
  WlzGreyV  	*val = NULL;
  WlzGreyValueWSpace *gVWSp = NULL;
  char		*outFileStr,
  		*inObjFileStr;
  WlzErrorNum	errNum = WLZ_ERR_NONE;
  FILE		*fP = NULL;
  WlzObject	*inObj = NULL;
  const char	*errMsg;
  static char	optList[] = "bco:v:x:y:z:h",
		outFileStrDef[] = "-",
  		inObjFileStrDef[] = "-";

  pos.vtX = 0;
  pos.vtY = 0;
  pos.vtZ = 0;
  opterr = 0;
  pix0.type = WLZ_GREY_DOUBLE;
  pix0.v.dbv = 0.0;
  inObjFileStr = inObjFileStrDef;
  outFileStr = outFileStrDef;
  while(ok && (usage == 0) &&
  	((option = getopt(argc, argv, optList)) != -1))
  {
    switch(option)
    {
      case 'b':
        bgdFlag = 1;
	break;
      case 'c':
        conFlag = 1;
	break;
      case 'o':
        outFileStr = optarg;
	break;
      case 'v':
	setVal = 1;
        if(sscanf(optarg, "%lg", &(pix0.v.dbv)) != 1)
	{
	  usage = 1;
	  ok = 0;
	}
	break;
      case 'x':
        if(sscanf(optarg, "%lg", &(pos.vtX)) != 1)
	{
	  usage = 1;
	  ok = 0;
	}
	break;
      case 'y':
        if(sscanf(optarg, "%lg", &(pos.vtY)) != 1)
	{
	  usage = 1;
	  ok = 0;
	}
	break;
      case 'z':
        if(sscanf(optarg, "%lg", &(pos.vtZ)) != 1)
	{
	  usage = 1;
	  ok = 0;
	}
	break;
      case 'h':
      default:
	usage = 1;
	ok = 0;
	break;
    }
  }
  if((inObjFileStr == NULL) || (*inObjFileStr == '\0') ||
     (outFileStr == NULL) || (*outFileStr == '\0'))
  {
    ok = 0;
    usage = 1;
  }
  if(ok && (optind < argc))
  {
    if((optind + 1) != argc)
    {
      usage = 1;
      ok = 0;
    }
    else
    {
      inObjFileStr = *(argv + optind);
    }
  }
  if(ok)
  {
    errNum = WLZ_ERR_READ_EOF;
    if((inObjFileStr == NULL) ||
       (*inObjFileStr == '\0') ||
       ((fP = (strcmp(inObjFileStr, "-")?
              fopen(inObjFileStr, "r"): stdin)) == NULL) ||
       ((inObj= WlzAssignObject(WlzReadObj(fP, &errNum), NULL)) == NULL))
    {
      ok = 0;
      (void )WlzStringFromErrorNum(errNum, &errMsg);
      (void )fprintf(stderr,
                     "%s: Failed to read object from file %s (%s).\n",
                     *argv, inObjFileStr, errMsg);
    }
    if(fP && strcmp(inObjFileStr, "-"))
    {
      fclose(fP);
    }
  }
  if(ok)
  {
    gVWSp = WlzGreyValueMakeWSp(inObj, &errNum);
    if(errNum != WLZ_ERR_NONE)
    {
      ok = 0;
      (void )WlzStringFromErrorNum(errNum, &errMsg);
      (void )fprintf(stderr,
		     "%s: Failed to make workspace (%s).\n",
		     *argv, errMsg);
    }
  }
  if(ok)
  {
    if(setVal || (conFlag == 0))
    {
      WlzGreyValueGet(gVWSp, pos.vtZ, pos.vtY, pos.vtX);
    }
    else
    {
      WlzGreyValueGetCon(gVWSp, pos.vtZ, pos.vtY, pos.vtX);
    }
    if(conFlag)
    {
      switch(gVWSp->objType)
      {
	case WLZ_2D_DOMAINOBJ:
	  nElm = 4;
	  break;
	case WLZ_3D_DOMAINOBJ:
	  nElm = 8;
	  break;
	default:
	  errNum = WLZ_ERR_OBJECT_TYPE;
	  break;
      }
    }
    if(errNum == WLZ_ERR_NONE)
    {
      if(WlzGreyTableIsTiled(gVWSp->values.core->type))
      {
	vpe = gVWSp->values.t->vpe;
      }
      if((val = (WlzGreyV *)AlcCalloc(vpe * nElm, sizeof(WlzGreyV))) == NULL)
      {
        errNum = WLZ_ERR_MEM_ALLOC;
      }
    }
    if(errNum != WLZ_ERR_NONE)
    {
      ok = 0;
      (void )WlzStringFromErrorNum(errNum, &errMsg);
      (void )fprintf(stderr,
		     "%s: Failed to allocate value buffer (%s).\n",
		     *argv, errMsg);
    }
  }
  if(ok)
  {
    if(setVal)
    {
      errNum = WlzValueConvertPixel(&pix1, pix0, gVWSp->gType);
      if(errNum == WLZ_ERR_NONE)
      {
	switch(gVWSp->gType)
	{
	  case WLZ_GREY_LONG:
	    *(gVWSp->gPtr[0].lnp) = pix1.v.lnv;
	    break;
	  case WLZ_GREY_INT:
	    *(gVWSp->gPtr[0].inp) = pix1.v.inv;
	    break;
	  case WLZ_GREY_SHORT:
	    *(gVWSp->gPtr[0].shp) = pix1.v.shv;
	    break;
	  case WLZ_GREY_UBYTE:
	    *(gVWSp->gPtr[0].ubp) = pix1.v.ubv;
	    break;
	  case WLZ_GREY_FLOAT:
	    *(gVWSp->gPtr[0].flp) = pix1.v.flv;
	    break;
	  case WLZ_GREY_DOUBLE:
	    *(gVWSp->gPtr[0].dbp) = pix1.v.dbv;
	    break;
	  case WLZ_GREY_RGBA:
	    *(gVWSp->gPtr[0].rgbp) = pix1.v.rgbv;
	    break;
	  default:
	    errNum = WLZ_ERR_GREY_TYPE;
	    break;
	}
      }
    }
    else
    {
      int	idE,
      		idN;

      /* Get values into buffer using background value where neeeded. */
      idN = 0;
      for(idE = 0; idE < nElm; ++idE)
      {
	int	idV;

	if(gVWSp->bkdFlag & (1 << idE))
	{
	  val[idN++] = gVWSp->gVal[idE];
	  for(idV = 1; idV < vpe; ++idV)
	  {
	    val[idN++] = gVWSp->gVal[idE];
	  }
	}
	else
	{
	  (void )WlzGreyValueFromGreyP(val + idN, gVWSp->gPtr[idE],
	                               vpe, gVWSp->gType);
	  idN += vpe;
	}
      }
    }
  }
  if(ok)
  {
    if(setVal == 0)
    {
      if((fP = (strcmp(outFileStr, "-")? fopen(outFileStr, "w"):
		                         stdout)) != NULL)
      {
	int 	idE,
		idN;
	char 	bc =  ' ';

	idN = 0;
	for(idE = 0; idE < nElm; ++idE)
	{
	  int	idV;

	  if(bgdFlag)
	  {
	    bc = ((gVWSp->bkdFlag & (1 << idE)) != 0)? 'b': 'f';
	  }
	  switch(gVWSp->gType)
	  {
	    case WLZ_GREY_LONG:
	      for(idV = 1; idV < vpe; ++idV)
	      {
	        (void )fprintf(fP, "%lld ", val[idN++].lnv);
	      }
	      (void )fprintf(fP, "%lld", val[idN++].lnv);
	      break;
	    case WLZ_GREY_INT:
	      for(idV = 1; idV < vpe; ++idV)
	      {
	        (void )fprintf(fP, "%d ", val[idN++].inv);
	      }
	      (void )fprintf(fP, "%d", val[idN++].inv);
	      break;
	    case WLZ_GREY_SHORT:
	      for(idV = 1; idV < vpe; ++idV)
	      {
	        (void )fprintf(fP, "%d ", val[idN++].shv);
	      }
	      (void )fprintf(fP, "%d", val[idN++].shv);
	      break;
	    case WLZ_GREY_UBYTE:
	      for(idV = 1; idV < vpe; ++idV)
	      {
	        (void )fprintf(fP, "%d ", val[idN++].ubv);
	      }
	      (void )fprintf(fP, "%d", val[idN++].ubv);
	      break;
	    case WLZ_GREY_FLOAT:
	      for(idV = 1; idV < vpe; ++idV)
	      {
	        (void )fprintf(fP, "%g ", val[idN++].flv);
	      }
	      (void )fprintf(fP, "%g", val[idN++].flv);
	      break;
	    case WLZ_GREY_DOUBLE:
	      for(idV = 1; idV < vpe; ++idV)
	      {
	        (void )fprintf(fP, "%lg ", val[idN++].dbv);
	      }
	      (void )fprintf(fP, "%lg", val[idN++].dbv);
	      break;
	    case WLZ_GREY_RGBA:
	      for(idV = 1; idV < vpe; ++idV)
	      {
	        (void )fprintf(fP, "%d,%d,%d,%d ",
			WLZ_RGBA_RED_GET(val[idN].rgbv),
			WLZ_RGBA_GREEN_GET(val[idN].rgbv),
			WLZ_RGBA_BLUE_GET(val[idN].rgbv),
			WLZ_RGBA_ALPHA_GET(val[idN].rgbv));
	        ++idN;
	      }
	      (void )fprintf(fP, "%d,%d,%d,%d",
			 WLZ_RGBA_RED_GET(val[idN].rgbv),
			 WLZ_RGBA_GREEN_GET(val[idN].rgbv),
			 WLZ_RGBA_BLUE_GET(val[idN].rgbv),
			 WLZ_RGBA_ALPHA_GET(val[idN].rgbv));
	      ++idN;
	      break;
	    default:
	      errNum = WLZ_ERR_GREY_TYPE;
	      break;
	  }
	  if(bgdFlag)
	  {
	    (void )fprintf(fP, " %c\n", bc);
	  }
	  else
	  {
	    (void )fprintf(fP, "\n");
	  }
	  if(feof(fP))
	  {
	    errNum = WLZ_ERR_WRITE_INCOMPLETE;
	    break;
	  }
	}
      }
      if(errNum != WLZ_ERR_NONE)
      {
	ok = 0;
	(void )WlzStringFromErrorNum(errNum, &errMsg);
	(void )fprintf(stderr,
		       "%s: Failed to write output file (%s).\n",
		       *argv, errMsg);
      }
    }
    else
    {
      if(((fP = (strcmp(outFileStr, "-")?
		fopen(outFileStr, "w"):
		stdout)) == NULL) ||
	 ((errNum = WlzWriteObj(fP, inObj)) != WLZ_ERR_NONE))
      {
	ok = 0;
	(void )WlzStringFromErrorNum(errNum, &errMsg);
	(void )fprintf(stderr,
		       "%s: Failed to write output object (%s).\n",
		       *argv, errMsg);
      }
    }
    if(fP && strcmp(outFileStr, "-"))
    {
      fclose(fP);
    }
  }
  AlcFree(val);
  if(gVWSp)
  {
    WlzGreyValueFreeWSp(gVWSp);
  }
  if(inObj)
  {
    (void )WlzFreeObj(inObj);
  }
  if(usage)
  {
    (void )fprintf(stderr,
    "Usage: "
    "%s [-b] [-v#] [-x#] [-y#] [-z#] [-h] [<in object>]\n"
    "Either sets or gets a grey value within th given objects values.\n"
    "If the -v flag is given the objects value is set and the modified\n"
    "object is written to the output file, but if the flag is not set\n"
    "then the grey value (or multiple grey values for an object with\n"
    "non-scalar values) is written out in ascii form with a new line\n"
    "character. Only scalar values can currently be set.\n"
    "Version: %s\n"
    "Options:\n"
    "  -b    Indicate background or foreground with b or f following value\n"
    "        written (set to %s).\n"
    "  -c    If getting values get and print connected values.\n"
    "  -v#   Grey value to be set (set to %g).\n"
    "  -x#   Column position (set to %g).\n"
    "  -y#   Line position (set to %g).\n"
    "  -z#   Plane position (set to %g).\n"
    "  -o#   Output file name.\n"
    "  -h    Display this usage information.\n",
    *argv,
    WlzVersion(),
    (bgdFlag)? "true": "false",
    pix0.v.dbv,
    pos.vtX, pos.vtY, pos.vtZ);
  }
  return(!ok);
}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */
