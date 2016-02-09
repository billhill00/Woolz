#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _WlzFixedPlaneAlign_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         binWlzApp/WlzFixedPlaneAlign.c
* \author	Richard Baldock
* \date         May 1999
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
* \brief	Reset the transforms in a bibfile so that fixed
* 		planes are fixed.
* \ingroup	BinWlzApp

* \par Binary
* \ref wlzfixedplanealign "WlzFixedPlaneAlign"
*/

/*!
\ingroup BinWlzApp
\defgroup wlzfixedplanealign WlzFixedPlaneAlign
\par Name
WlzFixedPlaneAlign
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include <time.h>

#include <Wlz.h>
#include <bibFile.h>
#include <Reconstruct.h>

/* externals required but not in all headers */
#ifdef __STDC__ /* [ */
extern int	finite(double);

extern int      getopt(int argc, char * const *argv, const char *optstring);
 
extern int 	optind, opterr, optopt;
extern char     *optarg;
#endif /* __STDC__ ] */

static void usage(char *proc_str)
{
  (void )fprintf(stderr,
	  "Usage:\t%s [-1] [-2] [-ffilename1] [-Ffilename2] [-h]\n"
	  "                           [<bibfile1>] [<bibfile2>]\n"
	  "\tReset the transforms in bibfile2 so that the planes that\n"
	  "\tmatch plane in bibfile1 - the fixed planes - are put back\n"
	  "\tto the fixed positions defined by bibfile1. Planes\n"
	  "\tbetween the fixed planes are shifted assuming linear\n"
	  "\tinterpolation defined by the corrective transforms at\n"
	  "\teach end of the range (i.e. at the fixed planes). Planes\n"
	  "\tnot between two fixed planes are transformed as per the\n"
          "\tthe nearest fixed plane. All bibfiles are assumed to be\n"
	  "\tfrom Reconstruct. The modified bibfile2 goes to stdout\n"
	  "Version: %s\n"
	  "Options:\n"
	  "\t  -1                transform 1 is absolute (def: relative)\n"
	  "\t  -2                transform 2 is absolute (def: relative)\n"
	  "\t  -h                Help - prints this usage message\n",
	  proc_str,
	  WlzVersion());
  return;
}

void checkTrans(
  WlzAffineTransform *trans)
{
  WlzAffineTransformPrim prim;
  double	tol = 1.0e-6;

  if( trans == NULL ){
    return;
  }
  if(WlzAffineTransformPrimGet(trans, &prim) != WLZ_ERR_NONE){
    return;
  }

  if( !finite(prim.tx) || (fabs(prim.tx) <= tol) ){
    prim.tx = 0.0;
  }
  if( !finite(prim.ty) || (fabs(prim.ty) <= tol) ){
    prim.ty = 0.0;
  }
  if( !finite(prim.tz) || (fabs(prim.tz) <= tol) ){
    prim.tz = 0.0;
  }
  if( !finite(prim.scale) || (fabs(prim.scale - 1.0) <= tol) ){
    prim.scale = 1.0;
  }
  if( !finite(prim.theta) || (fabs(prim.theta) <= tol) ){
    prim.theta = 0.0;
  }
  if( !finite(prim.phi) || (fabs(prim.phi) <= tol) ){
    prim.phi = 0.0;
  }
  if( !finite(prim.alpha) || (fabs(prim.alpha) <= tol) ){
    prim.alpha = 0.0;
  }
  if( !finite(prim.psi) || (fabs(prim.psi) <= tol) ){
    prim.psi = 0.0;
  }
  if( !finite(prim.xsi) || (fabs(prim.xsi) <= tol) ){
    prim.xsi = 0.0;
  }

  WlzAffineTransformPrimSet(trans, prim);
  return;
}

WlzObject *SecListToTransforms(
  RecSectionList	*secList,
  int		relFlg,
  WlzErrorNum	*dstErr)
{
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  WlzObject	*rtnObj=NULL;
  int		i, p, pMin, pMax;
  WlzDomain	domain;
  WlzValues	values;
  RecSection	*sec;
  HGUDlpListItem	*listItem;
  WlzAffineTransform	*recTrans, *tmpTrans;

  /* sort the section list and find the section range */
  RecSecListSort(secList->list, REC_SECMSK_INDEX);

  /* calculate the cumulative transforms */
  listItem = HGUDlpListTail(secList->list);
  if( relFlg ){
    RecSecCumTransfSet(secList->list, listItem);
  }

  /* define the reconstruct transform */
  recTrans = WlzAffineTransformFromPrimVal(WLZ_TRANSFORM_2D_AFFINE,
					   0.0, 0.0, 0.0,
					   secList->reconstruction.scale.vtX,
					   0.0, 0.0, 0.0, 0.0,
					   0.0, 0, NULL);

  /* create the transforms object */
  listItem = HGUDlpListHead(secList->list);
  sec = (RecSection *) HGUDlpListEntryGet(secList->list, listItem);
  pMin = sec->index;
  listItem = HGUDlpListTail(secList->list);
  sec = (RecSection *) HGUDlpListEntryGet(secList->list, listItem);
  pMax = sec->index;
  if( errNum == WLZ_ERR_NONE ){
    if((domain.p = WlzMakePlaneDomain(WLZ_PLANEDOMAIN_AFFINE,
				      pMin, pMax, 0, 0, 0, 0,
				      &errNum)) != NULL){
      values.core = NULL;
      rtnObj = WlzMakeMain(WLZ_3D_DOMAINOBJ,
			   domain, values, NULL, NULL, &errNum);
    }
  }

  /* now put in the transforms */
  listItem = HGUDlpListHead(secList->list);
  while( listItem ){
    sec = (RecSection *) HGUDlpListEntryGet(secList->list, listItem);
    p = sec->index;
    i = p - rtnObj->domain.p->plane1;
    if( relFlg ){
      if( sec->cumTransform == NULL ){
	rtnObj->domain.p->domains[i].t =
	  WlzAffineTransformFromPrimVal(WLZ_TRANSFORM_2D_AFFINE,
				        0.0, 0.0, 0.0, 1.0,
				        0.0, 0.0, 0.0, 0.0,
				        0.0, 0, NULL);
      }
      else {
	rtnObj->domain.p->domains[i].t =
	  WlzAssignAffineTransform(sec->cumTransform, NULL);
      }
    }
    else {
      rtnObj->domain.p->domains[i].t =
	WlzAssignAffineTransform(sec->transform, NULL);
    }

    /* apply the reconstruct transform */
    tmpTrans = WlzAffineTransformProduct(rtnObj->domain.p->domains[i].t,
					 recTrans, NULL);
    WlzFreeAffineTransform(rtnObj->domain.p->domains[i].t);
    rtnObj->domain.p->domains[i].t = WlzAssignAffineTransform(tmpTrans, NULL);

    listItem = HGUDlpListNext(secList->list, listItem);
  }

  WlzFreeAffineTransform(recTrans);

  if( dstErr ){
    *dstErr = errNum;
  }
  if( (errNum != WLZ_ERR_NONE) && rtnObj ){
    WlzFreeObj(rtnObj);
    rtnObj = NULL;
  }
  return rtnObj;
}

int main(int	argc,
	 char	**argv)
{
  FILE		*inFile1=NULL, *inFile2=NULL;
  char 		optList[] = "f:F:h";
  int		option;
  RecSectionList	recSecList1, recSecList2;
  RecSectionList	*secList1=&recSecList1, *secList2=&recSecList2;
  HGUDlpListItem	*listItem1, *listItem2;
  RecSection	*sec1, *sec2;
  char		*errMsg = NULL;
  RecError	errFlg = REC_ERR_NONE;
  int		numSec1, numSec2;
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  WlzObject	*transformsObj1, *transformsObj2;
  int		p, i, ip;
  WlzAffineTransform	*newTrans, *tmpTrans, *reconTrans;
  int		pPrev, pPost, iPrev, iPost;
  double	tx, txPrev, txPost;
  double	ty, tyPrev, tyPost;
  double	theta, thetaPrev, thetaPost;
  WlzAffineTransformPrim prevPrim, postPrim;

  /* parse the command line */
  opterr = 0;
  while( (option = getopt(argc, argv, optList)) != EOF ){
    switch( option ){

    case 'f':
      if( (optarg != NULL) && (strlen(optarg) > 0) ){
	if( (inFile1 = fopen(optarg, "r")) == NULL ){
	  fprintf(stderr, "%s: failed to open input file %s\n",
		  argv[0], optarg);
	  usage(argv[0]);
	  return 1;
	}
      }
      else {
	usage(argv[0]);
	return 1;
      }
      break;

    case 'F':
      if( (optarg != NULL) && (strlen(optarg) > 0) ){
	if( (inFile2 = fopen(optarg, "r")) == NULL ){
	  fprintf(stderr, "%s: failed to open input file %s\n",
		  argv[0], optarg);
	  usage(argv[0]);
	  return 1;
	}
      }
      else {
	usage(argv[0]);
	return 1;
      }
      break;

    case 'h':
    default:
      usage(argv[0]);
      return 1;

    }
  }

  /* check for files on the command line */
  if( (inFile1 == NULL) && (optind < argc) ){
    if( (inFile1 = fopen(*(argv+optind), "r")) == NULL ){
      fprintf(stderr, "%s: can't open file %s\n", argv[0], *(argv+optind));
      usage(argv[0]);
      return 1;
    }
    optind++;
  }
  if( (inFile2 == NULL) && (optind < argc) ){
    if( (inFile2 = fopen(*(argv+optind), "r")) == NULL ){
      fprintf(stderr, "%s: can't open file %s\n", argv[0], *(argv+optind));
      usage(argv[0]);
      return 1;
    }
    optind++;
  }

  /* read the bibfiles */
  if( inFile1 != NULL ){
    errFlg = RecFileSecListRead(secList1, &numSec1, inFile1, &errMsg);
    if( errFlg == REC_ERR_NONE ){
      transformsObj1 = SecListToTransforms(secList1, 1, &errNum);
    }
    else {
      fprintf(stderr, "%s: failed to read transform1: %s\n", argv[0], errMsg);
      usage(argv[0]);
      return 1;
    }
  }
  else {
    fprintf(stderr, "%s: a file for transform 1 must be defined\n", argv[0]);
    usage(argv[0]);
    return 1;
  }

  if( inFile2 != NULL ){
    errFlg = RecFileSecListRead(secList2, &numSec2, inFile2, &errMsg);
    if( errFlg == REC_ERR_NONE ){
      transformsObj2 = SecListToTransforms(secList2, 1, &errNum);
    }
    else {
      fprintf(stderr, "%s: failed to read transform2: %s\n", argv[0], errMsg);
      usage(argv[0]);
      return 1;
    }
  }
  else {
    fprintf(stderr, "%s: a file for transform 2 must be defined\n", argv[0]);
    usage(argv[0]);
    return 1;
  }

  /* establish the plane indices of the fixed planes */
  listItem1 = HGUDlpListHead(secList1->list);
  while( listItem1 ){
    sec1 = (RecSection *) HGUDlpListEntryGet(secList1->list, listItem1);
    if( strncmp(sec1->imageFile, "empty", 5) == 0 ){
      i = sec1->index - transformsObj1->domain.p->plane1;
      if( transformsObj1->domain.p->domains[i].t ){
	WlzFreeAffineTransform(transformsObj1->domain.p->domains[i].t);
      }
      transformsObj1->domain.p->domains[i].t = NULL;
    }
    listItem1 = HGUDlpListNext(secList1->list, listItem1);
  }

  /* set the corrective transform if matching transforms are in the
     second bibfile */
  for(p=transformsObj1->domain.p->plane1, i=0;
      p <= transformsObj1->domain.p->lastpl; p++, i++){
    if( transformsObj1->domain.p->domains[i].t ){
      if((p < transformsObj2->domain.p->plane1) ||
	 (p > transformsObj2->domain.p->lastpl) ){
	WlzFreeAffineTransform(transformsObj1->domain.p->domains[i].t);
	transformsObj1->domain.p->domains[i].t = NULL;
      }
      else {
	ip = p - transformsObj2->domain.p->plane1;
	tmpTrans =
	  WlzAffineTransformInverse(transformsObj2->domain.p->domains[ip].t,
				    NULL);
	newTrans =
	  WlzAffineTransformProduct(tmpTrans,
				    transformsObj1->domain.p->domains[i].t,
				    NULL);
	WlzFreeAffineTransform(tmpTrans);
	WlzFreeAffineTransform(transformsObj1->domain.p->domains[i].t);
	transformsObj1->domain.p->domains[i].t =
	  WlzAssignAffineTransform(newTrans, NULL);
      }
    }
  }

  /*
    Pass through the second transforms object, if between two fixed
    planes then apply the corrective transform calculated by
    interpolating the bounding fixed plane transforms.
  */
  for(p=transformsObj2->domain.p->plane1, ip=0;
      p <= transformsObj2->domain.p->lastpl; p++, ip++){
    if((p < transformsObj1->domain.p->plane1) ||
       (p > transformsObj1->domain.p->lastpl)){
      continue;
    }
    i = p - transformsObj1->domain.p->plane1;
    /* check if a fixed plane */
    if( transformsObj1->domain.p->domains[i].t ){
      newTrans =
	  WlzAffineTransformProduct(transformsObj2->domain.p->domains[ip].t,
				    transformsObj1->domain.p->domains[i].t,
				    NULL);
      WlzFreeAffineTransform(transformsObj2->domain.p->domains[ip].t);
      transformsObj2->domain.p->domains[ip].t =
	WlzAssignAffineTransform(newTrans, NULL);
    }
    else {
      /* search for previous fixed plane */
      pPrev = p;
      iPrev = i;
      while((pPrev >= transformsObj1->domain.p->plane1) &&
	    (transformsObj1->domain.p->domains[iPrev].t == NULL)
	){
	iPrev--;
	pPrev--;
      }
      
      /* search for following fixed plane */
      pPost = p;
      iPost = i;
      while((pPost <= transformsObj1->domain.p->lastpl) &&
	    (transformsObj1->domain.p->domains[iPost].t == NULL)
	){
	iPost++;
	pPost++;
      }

      /* check for no fixed planes */
      if((pPrev < transformsObj1->domain.p->plane1) &&
	 (pPost > transformsObj1->domain.p->lastpl)
	){
	fprintf(stderr, "%s: something screwy here, no fixed planes\n",
		argv[0]);
	return 1;
      }

      /* calculate and apply corrective transform */
      if( pPost > transformsObj1->domain.p->lastpl ){
	tmpTrans = transformsObj1->domain.p->domains[iPrev].t;
      }
      else if( pPrev < transformsObj1->domain.p->plane1 ){
	tmpTrans = transformsObj1->domain.p->domains[iPost].t;
      }
      else {
	(void)
	WlzAffineTransformPrimGet(transformsObj1->domain.p->domains[iPrev].t,
				   &prevPrim);
	(void )
	WlzAffineTransformPrimGet(transformsObj1->domain.p->domains[iPost].t,
				   &postPrim);
	txPrev = prevPrim.tx;
	txPost = postPrim.tx;
	tyPrev = prevPrim.ty;
	tyPost = postPrim.ty;
	thetaPrev = prevPrim.theta;
	thetaPost = postPrim.theta;
	tx = ((pPost-p) * txPrev + (p-pPrev) * txPost) / (pPost-pPrev);
	ty = ((pPost-p) * tyPrev + (p-pPrev) * tyPost) / (pPost-pPrev);
	theta = ((pPost-p) * thetaPrev + (p-pPrev) * thetaPost) /
	  (pPost-pPrev);
	tmpTrans = WlzAffineTransformFromPrimVal(WLZ_TRANSFORM_2D_AFFINE,
					         tx, ty, 0.0, 1.0,
					         theta, 0.0, 0.0, 0.0,
					         0.0, 0, NULL);
      }
      newTrans =
	WlzAffineTransformProduct(transformsObj2->domain.p->domains[ip].t,
				  tmpTrans, NULL);
      
      WlzFreeAffineTransform(transformsObj2->domain.p->domains[ip].t);
      WlzFreeAffineTransform(tmpTrans);
      transformsObj2->domain.p->domains[ip].t =
	WlzAssignAffineTransform(newTrans, NULL);
    }
  }

  /*  take out the reconstruct transform */
  if( secList2 ){
    /* take out the reconstruct transform first */
    reconTrans = WlzAffineTransformFromPrimVal(WLZ_TRANSFORM_2D_AFFINE,
	      0.0, 0.0, 0.0, 1.0 / secList2->reconstruction.scale.vtX,
	      0.0, 0.0, 0.0, 0.0, 0.0, 0, NULL);
    listItem2 = HGUDlpListHead(secList2->list);
    while( listItem2 ){
      sec2 = (RecSection *) HGUDlpListEntryGet(secList2->list, listItem2);
      if( sec2 ){
	p = sec2->index;
	i = p - transformsObj2->domain.p->plane1;
	if( transformsObj2->domain.p->domains[i].t ){
	  tmpTrans = WlzAffineTransformProduct
	    (transformsObj2->domain.p->domains[i].t, reconTrans, NULL);
	  WlzFreeAffineTransform(transformsObj2->domain.p->domains[i].t);
	  transformsObj2->domain.p->domains[i].t =
	    WlzAssignAffineTransform(tmpTrans, NULL);
	}
      }
      listItem2 = HGUDlpListNext(secList2->list, listItem2);
    }

  }

  /* convert to relative */
  listItem2 = HGUDlpListTail(secList2->list);
  while( listItem2 != HGUDlpListHead(secList2->list) ){
    sec2 = (RecSection *) HGUDlpListEntryGet(secList2->list, listItem2);
    if( sec2 ){
      p = sec2->index;
      i = p - transformsObj2->domain.p->plane1;
      if( strncmp(sec2->imageFile, "empty", 5) == 0 ){
	WlzFreeAffineTransform(transformsObj2->domain.p->domains[i].t);
	transformsObj2->domain.p->domains[i].t =
	  WlzAffineTransformFromPrimVal(WLZ_TRANSFORM_2D_AFFINE,
				        0.0, 0.0, 0.0, 1.0,
				        0.0, 0.0, 0.0, 0.0,
				        0.0, 0, NULL);
      }
      else {
	/* walk backwards to previous non-empty section */
	listItem1 = HGUDlpListPrev(secList2->list, listItem2);
	while( listItem1 != HGUDlpListHead(secList2->list) ){
	  sec1 = (RecSection *) HGUDlpListEntryGet(secList2->list, listItem1);
	  if( strncmp(sec1->imageFile, "empty", 5) ){
	    break;
	  }
	  listItem1 = HGUDlpListPrev(secList2->list, listItem1);
	}
	sec1 = (RecSection *) HGUDlpListEntryGet(secList2->list, listItem1);
	ip = sec1->index - transformsObj2->domain.p->plane1;
	tmpTrans =
	  WlzAffineTransformInverse(transformsObj2->domain.p->domains[ip].t,
				    NULL);
	newTrans = WlzAffineTransformProduct(
	  transformsObj2->domain.p->domains[i].t,
	  tmpTrans, NULL);
	checkTrans(newTrans);
	WlzFreeAffineTransform(transformsObj2->domain.p->domains[i].t);
	WlzFreeAffineTransform(tmpTrans);
	transformsObj2->domain.p->domains[i].t = newTrans;
      }
    }
    listItem2 = HGUDlpListPrev(secList2->list, listItem2);
  }
  
  /* put back the transforms and write to stdout */
  listItem2 = HGUDlpListHead(secList2->list);
  while( listItem2 ){
    sec2 = (RecSection *) HGUDlpListEntryGet(secList2->list, listItem2);
    p = sec2->index;
    i = p - transformsObj2->domain.p->plane1;
    sec2->transform = 
      transformsObj2->domain.p->domains[i].t;
    listItem2 = HGUDlpListNext(secList2->list, listItem2);
  }
  RecFileSecListWrite(stdout, secList2, numSec2, &errMsg);

  return 0;
}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */
