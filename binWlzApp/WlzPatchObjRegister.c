#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _WlzPatchObjRegister_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         binWlzApp/WlzPatchObjRegister.c
* \author       Richard Baldock
* \date         February 1998
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
* \brief 	Registers patch objects and outputs a single object.
* \ingroup	BinWlzApp
*
* \par Binary
* \ref wlzpatchobjregister "WlzPatchObjRegister"
*/

/*!
\ingroup BinWlzApp
\defgroup wlzpatchobjregister WlzPatchObjRegister
\par Name
WlzPatchObjRegister - registers patch objects and outputs a single object.
\par Synopsis
\verbatim
WlzPatchObjRegister  [-h] [-v] [-b] [-d] [-t#[,#]] [-p#] [-g] [-G] [-T#]
                     [<input file>]
\endverbatim
\par Options
<table width="500" border="0">
  <tr> 
    <td><b>-h</b></td>
    <td>Prints usage information.</td>
  </tr>
  <tr> 
    <td><b>-v</b></td>
    <td>Verbose output.</td>
  </tr>
  <tr> 
    <td><b>-b</b></td>
    <td>Ordered breadth-first search, darkest images first, default.</td>
  </tr>
  <tr> 
    <td><b>-d</b></td>
    <td>Unordered depth-first search.</td>
  </tr>
  <tr> 
    <td><b>-t</b></td>
    <td>Set maximum translation values, default 30,30.
        One input value implies equal maximum shift in x & y directions.</td>
  </tr>
  <tr> 
    <td><b>-p</b></td>
    <td>Percent of pixels match, default 10.
        Higher is better but slower.</td>
  </tr>
  <tr> 
    <td><b>-g</b></td>
    <td>Attempt to reset mean grey values to be equal
        within matched regions, default.</td>
  </tr>
  <tr> 
    <td><b>-G</b></td>
    <td>Switch off grey-level matching.</td>
  </tr>
  <tr> 
    <td><b>-T</b></td>
    <td>Threshold the input images e.g. to remove spurious lines
        around the edge.</td>
  </tr>
</table>
\par Description
WlzPatchObjRegister reads a compound woolz object with image patches
(such produced by xmgrab). These patches are then registered and
a single domain object is output.
\par Examples
\verbatim
\endverbatim
\par File
\ref WlzPatchObjRegister.c "WlzPatchObjRegister.c"
\par See Also
\ref BinWlzApp "WlzIntro(1)"
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#endif /* DOXYGEN_SHOULD_SKIP_THIS */
#include <stdio.h>
#include <stdlib.h>

#include <Wlz.h>
#include <Reconstruct.h>

/* externals required by getopt  - not in ANSI C standard */
#ifdef __STDC__ /* [ */
extern int      getopt(int argc, char * const *argv, const char *optstring);
 
extern int 	optind, opterr, optopt;
extern char     *optarg;
#endif /* __STDC__ ] */

static WlzIVertex2	defMaxShift;
static double		matchPercent=10.0;
static int		verboseFlg=0;

static void usage(char *proc_str)
{
  (void )fprintf(stderr,
	  "Usage:\t%s [-b] [-d] [-h] [-v] [-t#[,#]] [-p] [-g] [-G] [-T#]\n"
	  "                            [<input file>]\n"
	  "\tWoolz in a compound woolz object assumed\n"
	  "\tto be output of patch images from xmgrab.\n"
	  "\tRegister the patches and output the single\n"
	  "\tdomain object.\n"
	  "Version: %s\n"
	  "Options:\n"
	  "\t  -b        Ordered breadth-first search, darkest\n"
	  "\t            images first (default)\n"
	  "\t  -d        Unordered depth-first search\n"
	  "\t  -t#[,#]   Set maximum translation values\n"
	  "\t            default (30,30) one input value implies\n"
	  "\t            equal maximum shift in x & y directions\n"
	  "\t  -p#       percent of pixels match default 10 - higher\n"
	  "\t            is better but slower\n"
	  "\t  -g        Attempt to reset mean grey values to be equal\n"
	  "\t            within matched regions (on by default)\n"
	  "\t  -G        Switch off grey-level matching\n"
	  "\t  -T#       Threshold the input images e.g. to remove\n"
	  "\t            spurious lines around the edge\n"
	  "\t  -h        Help - prints this usage message\n"
	  "\t  -v        Verbose operation\n",
	  proc_str,
	  WlzVersion());
  return;
}

int WlzIsAdjacentPatch(
  WlzObject	*obj1,
  WlzObject	*obj2)
{
  int	adjacent=0;

  /* patches are adjacent if either the line bounds  or the column bounds
     are identical and there is some overlap */

  /* test on the columns */
  if((obj1->domain.i->kol1 == obj2->domain.i->kol1) &&
     (obj1->domain.i->lastkl == obj2->domain.i->lastkl)){
    if((obj1->domain.i->lastln < obj2->domain.i->line1) ||
       (obj1->domain.i->line1 > obj2->domain.i->lastln)){
      adjacent = 0;
    }
    else {
      adjacent = 1;
    }
  }
  else if((obj1->domain.i->line1 == obj2->domain.i->line1) &&
	  (obj1->domain.i->lastln == obj2->domain.i->lastln)){
    if((obj1->domain.i->lastkl < obj2->domain.i->kol1) ||
       (obj1->domain.i->kol1 > obj2->domain.i->lastkl)){
      adjacent = 0;
    }
    else {
      adjacent = 1;
    }
  }

  return adjacent;
}

typedef struct _patchTree{
  WlzObject	*obj;
  int		index;
  int		offsetsCalculatedFlag;
  int		depth;
  double	xOff;
  double	yOff;
  struct _patchTree	*children[4];
  int		nchildren;
  double	cost;
} WlzPatchTree;

int sortPatch(
  const void *p1,
  const void *p2)
{
  WlzPatchTree	*patch_1=*((WlzPatchTree **) p1);
  WlzPatchTree	*patch_2=*((WlzPatchTree **) p2);

  if( patch_1->cost < patch_2->cost ){
    return -1;
  }
  if( patch_1->cost > patch_2->cost ){
    return 1;
  }
  return 0;
}


WlzPatchTree *WlzMakePatchTree(
  WlzObject	*obj,
  int		depth,
  double	cost)
{
  WlzPatchTree	*patchTree;

  /* allocate space and initialise */
  patchTree = (WlzPatchTree *) AlcCalloc(1, sizeof(WlzPatchTree));
  patchTree->obj = WlzAssignObject(obj, NULL);
  patchTree->depth = depth;
  patchTree->cost = cost;

  return patchTree;
}

WlzErrorNum	WlzGetPatchTreeToDepth(
  WlzObject	**objs,
  int		nobjs,
  WlzPatchTree	*patchTree,
  int		depth)
{
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  int		i;
  WlzGreyType	dstGType;
  double	dstMin, dstMax, dstSum, dstSumSq;
  double	dstMean, dstStdDev;
  WlzObject	*obj1;

  if((patchTree == NULL) ||
     (patchTree->depth >= depth)){
    return errNum;
  }

  if( patchTree->depth == (depth - 1) ){
    for(i=0; (i < nobjs) && (patchTree->nchildren < 4); i++){
      if( objs[i] && WlzIsAdjacentPatch(patchTree->obj, objs[i]) ){
	if( WlzGreyTypeFromObj(objs[i], &errNum) == WLZ_GREY_RGBA ){
	  obj1 = WlzRGBAToModulus(objs[i], &errNum);
	  WlzGreyStats(obj1, &dstGType, &dstMin, &dstMax,
		       &dstSum, &dstSumSq, &dstMean, &dstStdDev,
		       NULL);
	  WlzFreeObj(obj1);
	}
	else {
	  WlzGreyStats(objs[i], &dstGType, &dstMin, &dstMax,
		       &dstSum, &dstSumSq, &dstMean, &dstStdDev,
		       NULL);
	}
	patchTree->children[patchTree->nchildren] =
	  WlzMakePatchTree(objs[i], depth, dstMean);
	patchTree->children[patchTree->nchildren]->index = i;
	WlzFreeObj(objs[i]);
	objs[i] = NULL;
	patchTree->nchildren++;
      }
    }
    /* sort the children */
    AlgSort(patchTree->children, patchTree->nchildren,
	    sizeof(WlzPatchTree *), sortPatch);
  }
  else {
    for(i=0; i < patchTree->nchildren; i++){
      errNum = WlzGetPatchTreeToDepth(objs, nobjs,
				      patchTree->children[i], depth);
    }
  }

  return errNum;
}

static int numObjsLeft(
  WlzObject	**objs, 
  int		nobjs)
{
  int i, n;

  for(i=0, n=0; i < nobjs; i++){
    if( objs[i] ){
      n++;
    }
  }
  return n;
}

WlzPatchTree *WlzGetPatchTree(
  WlzObject	*obj,
  WlzObject	**objs,
  int		nobjs)
{
  int		i;
  int		nPatches;
  WlzPatchTree	*patchTree;
  WlzObject	*adjObj;

  /* allocate space and initialise */
  patchTree = (WlzPatchTree *) AlcCalloc(1, sizeof(WlzPatchTree));
  patchTree->obj = WlzAssignObject(obj, NULL);

  /* check for non-NULL adjacent objects  - depth first search */
  for(i=0, nPatches=0; (i < nobjs) && (nPatches < 4); i++){
    if( objs[i] && WlzIsAdjacentPatch(obj, objs[i]) ){
      adjObj = objs[i];
      objs[i] = NULL;
      patchTree->children[nPatches] = WlzGetPatchTree(adjObj, objs, nobjs);
      WlzFreeObj(adjObj);
      nPatches++;
    }
  }

  patchTree->offsetsCalculatedFlag = 0;
  patchTree->nchildren = nPatches;

  return patchTree;
}

WlzErrorNum WlzFreePatchTree(
  WlzPatchTree	*patchTree)
{
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  int		i;

  /* free sub trees */
  for(i=0; (i < patchTree->nchildren) && (errNum == WLZ_ERR_NONE); i++){
    errNum = WlzFreePatchTree(patchTree->children[i]);
  }

  /* free the object */
  if( errNum == WLZ_ERR_NONE ){
    errNum = WlzFreeObj(patchTree->obj);
  }

  /* free this branch */
  AlcFree((void *) patchTree);

  return errNum;
}

double WlzMass(
  WlzObject *obj,
  WlzErrorNum *dstErr)
{
  double		mass;
  WlzIntervalWSpace 	iwsp;
  WlzGreyWSpace		gwsp;
  WlzGreyP		gptr;
  int			i;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* check object */
  if( obj == (WlzObject *) NULL ){
    errNum = WLZ_ERR_OBJECT_NULL;
    mass = -1;
  }

  if( errNum == WLZ_ERR_NONE ){
    switch( obj->type ){

    case WLZ_2D_DOMAINOBJ:
      if( obj->domain.core == NULL ){
	errNum = WLZ_ERR_DOMAIN_NULL;
	mass = -1;
      }
      break;
    
    case WLZ_EMPTY_OBJ:
      mass = 0;
      break;

    default:
      errNum = WLZ_ERR_OBJECT_TYPE;
      mass = -1;
      break;

    }
  }

  /* calculate mass */
  if( errNum == WLZ_ERR_NONE ){
    mass = 0.0;
    errNum = WlzInitGreyScan(obj, &iwsp, &gwsp);
    if( errNum == WLZ_ERR_NONE ){
      while( (errNum = WlzNextGreyInterval(&iwsp)) == WLZ_ERR_NONE ){
	gptr = gwsp.u_grintptr;
	switch (gwsp.pixeltype) {

	case WLZ_GREY_INT:
	  for (i=0; i<iwsp.colrmn; i++, gptr.inp++){
	    if( *gptr.inp < 0 ){
	      mass -= *gptr.inp;
	    }
	    else {
	      mass += *gptr.inp;
	    }
	  }
	  break;

	case WLZ_GREY_SHORT:
	  for (i=0; i<iwsp.colrmn; i++, gptr.shp++){
	    if( *gptr.shp < 0 ){
	      mass -= *gptr.shp;
	    }
	    else {
	      mass += *gptr.shp;
	    }
	  }
	  break;

	case WLZ_GREY_UBYTE:
	  for (i=0; i<iwsp.colrmn; i++, gptr.ubp++){
	    mass += *gptr.ubp;
	  }
	  break;

	case WLZ_GREY_FLOAT:
	  for (i=0; i<iwsp.colrmn; i++, gptr.flp++){
	    if( *gptr.flp < 0 ){
	      mass -= *gptr.flp;
	    }
	    else {
	      mass += *gptr.flp;
	    }
	  }
	  break;

	case WLZ_GREY_DOUBLE:
	  for (i=0; i<iwsp.colrmn; i++, gptr.dbp++){
	    if( *gptr.dbp < 0 ){
	      mass -= *gptr.dbp;
	    }
	    else {
	      mass += *gptr.dbp;
	    }
	  }
	  break;

	default:
	  errNum = WLZ_ERR_GREY_TYPE;
	  break;
	}
      }
      if(errNum == WLZ_ERR_EOO)	  /* Reset error from end of intervals */ 
      {
	errNum = WLZ_ERR_NONE;
      }
    }
  }

  if( dstErr ){
    *dstErr = errNum;
  }
  return mass;
}

WlzObject *WlzGreyShift(
  WlzObject	*obj,
  double	delta,
  WlzErrorNum	*dstErr)
{
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  WlzObject	*rtnObj=NULL;
  WlzIntervalWSpace	iwsp;
  WlzGreyWSpace		gwsp;
  WlzGreyP		gptr;
  int			i, idelta;

  /* check the object */
  /* 2D only for now to test the idea */
  if( obj ){
    switch( obj->type ){
    case WLZ_2D_DOMAINOBJ:
      if( obj->domain.core ){
	if(obj->domain.core->type == WLZ_EMPTY_DOMAIN){
	  rtnObj = WlzMakeEmpty(&errNum);
	}
	else {
	  if(!obj->values.core ){
	    errNum = WLZ_ERR_VALUES_NULL;
	  }
	}
      }
      else {
	errNum = WLZ_ERR_DOMAIN_NULL;
      }
      break;

    case WLZ_EMPTY_OBJ:
      rtnObj = WlzMakeEmpty(&errNum);
      break;

    case WLZ_3D_DOMAINOBJ:
    case WLZ_TRANS_OBJ:
    default:
      errNum = WLZ_ERR_OBJECT_TYPE;
      break;
    }
  }
  else {
    errNum = WLZ_ERR_OBJECT_NULL;
  }

  /* 2D case */
  if( (errNum == WLZ_ERR_NONE) && (rtnObj == NULL) ){
    if((rtnObj = WlzCopyObject(obj, &errNum)) != NULL){
      if((errNum = WlzInitGreyScan(rtnObj, &iwsp, &gwsp)) == WLZ_ERR_NONE){
	while((errNum == WLZ_ERR_NONE) &&
	      ((errNum = WlzNextGreyInterval(&iwsp)) == WLZ_ERR_NONE)){
	  gptr = gwsp.u_grintptr;
	  switch (gwsp.pixeltype) {
	  case WLZ_GREY_INT:
	    for(i=0; i<iwsp.colrmn; i++, gptr.inp++){
	      if( delta > 0.0 ){
		idelta = (int) (delta + ((double) (rand()&0xffff))/0xffff);
	      }
	      else if( delta < 0.0 ){
		idelta = (int) (delta - ((double) (rand()&0xffff))/0xffff);
	      }
	      else {
		idelta = 0;
	      }
	      *gptr.inp += idelta;
	    }
	    break;

	  case WLZ_GREY_SHORT:
	    for(i=0; i<iwsp.colrmn; i++, gptr.shp++){
	      if( delta > 0.0 ){
		idelta = (int) (delta + ((double) (rand()&0xffff))/0xffff);
	      }
	      else if( delta < 0.0 ){
		idelta = (int) (delta - ((double) (rand()&0xffff))/0xffff);
	      }
	      else {
		idelta = 0;
	      }
	      *gptr.shp += idelta;
	    }
	    break;

	  case WLZ_GREY_UBYTE:
	    for(i=0; i<iwsp.colrmn; i++, gptr.ubp++){
	      if( delta > 0.0 ){
		idelta = (int) (delta + ((double) (rand()&0xffff))/0xffff);
	      }
	      else if( delta < 0.0 ){
		idelta = (int) (delta - ((double) (rand()&0xffff))/0xffff);
	      }
	      else {
		idelta = 0;
	      }
	      idelta += *gptr.ubp;
	      *gptr.ubp = WLZ_CLAMP(idelta, 0, 255);
	    }
	    break;

	  case WLZ_GREY_FLOAT:
	    for(i=0; i<iwsp.colrmn; i++, gptr.flp++){
	      *gptr.flp += delta;
	    }
	    break;

	  case WLZ_GREY_DOUBLE:
	    for(i=0; i<iwsp.colrmn; i++, gptr.dbp++){
	      *gptr.dbp += delta;
	    }
	    break;
	  default:
	    errNum = WLZ_ERR_GREY_TYPE;
	    break;
	  }
	}
	if( errNum == WLZ_ERR_EOO ){
	  errNum = WLZ_ERR_NONE;
	}
	if( errNum != WLZ_ERR_NONE ){
	  WlzFreeObj(rtnObj);
	  rtnObj = NULL;
	}
      }
      else {
	WlzFreeObj(rtnObj);
	rtnObj = NULL;
      }
    }
  }

  /* set return error */
  if( dstErr ){
    *dstErr = errNum;
  }
  return rtnObj;
}

WlzObject *WlzGreyScale(
  WlzObject	*obj,
  double	scale,
  WlzErrorNum	*dstErr)
{
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  WlzObject	*rtnObj=NULL;
  WlzIntervalWSpace	iwsp;
  WlzGreyWSpace		gwsp;
  WlzGreyP		gptr;
  int			i, idelta;

  /* check the object */
  /* 2D only for now to test the idea */
  if( obj ){
    switch( obj->type ){
    case WLZ_2D_DOMAINOBJ:
      if( obj->domain.core ){
	if(obj->domain.core->type == WLZ_EMPTY_DOMAIN){
	  rtnObj = WlzMakeEmpty(&errNum);
	}
	else {
	  if(!obj->values.core ){
	    errNum = WLZ_ERR_VALUES_NULL;
	  }
	}
      }
      else {
	errNum = WLZ_ERR_DOMAIN_NULL;
      }
      break;

    case WLZ_EMPTY_OBJ:
      rtnObj = WlzMakeEmpty(&errNum);
      break;

    case WLZ_3D_DOMAINOBJ:
    case WLZ_TRANS_OBJ:
    default:
      errNum = WLZ_ERR_OBJECT_TYPE;
      break;
    }
  }
  else {
    errNum = WLZ_ERR_OBJECT_NULL;
  }

  /* 2D case */
  if( (errNum == WLZ_ERR_NONE) && (rtnObj == NULL) ){
    if((rtnObj = WlzCopyObject(obj, &errNum)) != NULL){
      if((scale != 1.0) && 
	 (errNum = WlzInitGreyScan(rtnObj, &iwsp, &gwsp)) == WLZ_ERR_NONE ){
	while( (errNum == WLZ_ERR_NONE) &&
	       (errNum = WlzNextGreyInterval(&iwsp)) == WLZ_ERR_NONE ){
	  gptr = gwsp.u_grintptr;
	  switch (gwsp.pixeltype) {
	  case WLZ_GREY_INT:
	    for(i=0; i<iwsp.colrmn; i++, gptr.inp++){
	      idelta = (int) (scale * (*gptr.inp)
			      + ((double) (rand()&0xffff))/0xffff);
	      *gptr.inp = idelta;
	    }
	    break;

	  case WLZ_GREY_SHORT:
	    for(i=0; i<iwsp.colrmn; i++, gptr.shp++){
	      idelta = (int) (scale * (*gptr.shp)
			      + ((double) (rand()&0xffff))/0xffff);
	      *gptr.shp = idelta;
	    }
	    break;

	  case WLZ_GREY_UBYTE:
	    for(i=0; i<iwsp.colrmn; i++, gptr.ubp++){
	      idelta = (int) (scale * (*gptr.ubp)
			      + ((double) (rand()&0xffff))/0xffff);
	      *gptr.ubp = WLZ_CLAMP(idelta, 0, 255);
	    }
	    break;

	  case WLZ_GREY_FLOAT:
	    for(i=0; i<iwsp.colrmn; i++, gptr.flp++){
	      *gptr.flp *= scale;
	    }
	    break;

	  case WLZ_GREY_DOUBLE:
	    for(i=0; i<iwsp.colrmn; i++, gptr.dbp++){
	      *gptr.dbp += scale;
	    }
	    break;
	  default:
	    errNum = WLZ_ERR_GREY_TYPE;
	    break;
	  }
	}
	if( errNum == WLZ_ERR_EOO ){
	  errNum = WLZ_ERR_NONE;
	}
	if( errNum != WLZ_ERR_NONE ){
	  WlzFreeObj(rtnObj);
	  rtnObj = NULL;
	}
      }
      else {
	WlzFreeObj(rtnObj);
	rtnObj = NULL;
      }
    }
  }

  /* set return error */
  if( dstErr ){
    *dstErr = errNum;
  }
  return rtnObj;
}

WlzObject *WlzRGBAGreyScale(
  WlzObject	*obj,
  double	*scale,
  WlzErrorNum	*dstErr)
{
  WlzObject	*rtnObj=NULL, *objs[4];
  WlzCompoundArray	*cmpnd, *rtnCmpnd;
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  int		i;

  /* check objects */
  if( obj == NULL ){
    errNum = WLZ_ERR_OBJECT_NULL;
  }
  else if( WlzGreyTypeFromObj(obj, &errNum) != WLZ_GREY_RGBA ){
    errNum = WLZ_ERR_VALUES_TYPE;
  }
  else {
    switch( obj->type ){
    case WLZ_2D_DOMAINOBJ:	/* only flavour for now */
      if((cmpnd = WlzRGBAToCompound(obj, WLZ_RGBA_SPACE_RGB,
                                    &errNum)) != NULL){
	/* apply re-scaling to each channel */
	for(i=0; i < 3; i++){
	  objs[i] = WlzGreyScale(cmpnd->o[i], scale[i], &errNum);
	}
	objs[3] = cmpnd->o[3];
	rtnCmpnd = WlzMakeCompoundArray(WLZ_COMPOUND_ARR_1, 3, 4, &(objs[0]),
					objs[0]->type, &errNum);
	rtnObj = WlzCompoundToRGBA(rtnCmpnd, WLZ_RGBA_SPACE_RGB, &errNum);
	WlzFreeObj((WlzObject *) rtnCmpnd);
      }
      break;

    default:
      errNum = WLZ_ERR_OBJECT_TYPE;
      break;
    }
  }

  if( dstErr ){
    *dstErr = errNum;
  }
  return rtnObj;
}


double WlzGreyMeanDifference(
  WlzObject	*obj1,
  WlzObject	*obj2,
  double	samplePercent,
  WlzErrorNum	*dstErr)
{
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  double	rtnDiff=10000.0;
  int		idiff, idiffIncr;
  WlzObject	*o1, *o2;
  int		countedArea;
  WlzIntervalWSpace 	iwsp1, iwsp2;
  WlzGreyWSpace		gwsp1, gwsp2;
  WlzGreyP		gptr1, gptr2;
  int			i;
  int			pixelIncr;
  
  /* get intersection */
  o1 = WlzIntersect2(obj1, obj2, NULL);
  o2 = WlzMakeMain(o1->type, o1->domain, o1->values, NULL, NULL, NULL);
  o1->values = WlzAssignValues(obj1->values, NULL);
  o2->values = WlzAssignValues(obj2->values, NULL);

  /* loop through grey tables checking differences */
  WlzInitGreyScan(o1, &iwsp1, &gwsp1);
  WlzInitGreyScan(o2, &iwsp2, &gwsp2);
  idiff = 0;
  countedArea = 0;
  pixelIncr = (int) (100.0 / samplePercent);
  while((errNum == WLZ_ERR_NONE) &&
        (errNum = WlzNextGreyInterval(&iwsp1)) == WLZ_ERR_NONE ){
    (void) WlzNextGreyInterval(&iwsp2);
    gptr1 = gwsp1.u_grintptr;
    gptr2 = gwsp2.u_grintptr;
    switch (gwsp1.pixeltype) {
    case WLZ_GREY_INT:
      for (i=0; i<iwsp1.colrmn; i += pixelIncr,
	     gptr1.inp += pixelIncr, gptr2.inp += pixelIncr){
	idiffIncr = *gptr1.inp - *gptr2.inp;
	idiff += (idiffIncr < 0)?-idiffIncr:idiffIncr;
	countedArea++;
      }
      break;
    case WLZ_GREY_SHORT:
      for (i=0; i<iwsp1.colrmn; i += pixelIncr,
	     gptr1.shp += pixelIncr, gptr2.shp += pixelIncr){
	idiffIncr = *gptr1.shp - *gptr2.shp;
	idiff += (idiffIncr < 0)?-idiffIncr:idiffIncr;
	countedArea++;
      }
      break;
    case WLZ_GREY_UBYTE:
      for (i=0; i<iwsp1.colrmn; i += pixelIncr,
	     gptr1.ubp += pixelIncr, gptr2.ubp += pixelIncr){
	idiffIncr = *gptr1.ubp - *gptr2.ubp;
	idiff += (idiffIncr < 0)?-idiffIncr:idiffIncr;
	countedArea++;
      }
      break;
    case WLZ_GREY_FLOAT:
      for (i=0; i<iwsp1.colrmn; i += pixelIncr,
	     gptr1.flp += pixelIncr, gptr2.flp += pixelIncr){
	idiffIncr = *gptr1.flp - *gptr2.flp;
	idiff += (idiffIncr < 0)?-idiffIncr:idiffIncr;
	countedArea++;
      }
      break;
    case WLZ_GREY_DOUBLE:
      for (i=0; i<iwsp1.colrmn; i += pixelIncr,
	     gptr1.dbp += pixelIncr, gptr2.dbp += pixelIncr){
	idiffIncr = *gptr1.dbp - *gptr2.dbp;
	idiff += (idiffIncr < 0)?-idiffIncr:idiffIncr;
	countedArea++;
      }
      break;
    case WLZ_GREY_RGBA:
      for (i=0; i<iwsp1.colrmn; i += pixelIncr,
	     gptr1.rgbp += pixelIncr, gptr2.rgbp += pixelIncr){
	idiffIncr = WLZ_RGBA_RED_GET(*gptr1.rgbp)
	  - WLZ_RGBA_RED_GET(*gptr2.rgbp);
	idiff += (idiffIncr < 0)?-idiffIncr:idiffIncr;
	idiffIncr = WLZ_RGBA_GREEN_GET(*gptr1.rgbp)
	  - WLZ_RGBA_GREEN_GET(*gptr2.rgbp);
	idiff += (idiffIncr < 0)?-idiffIncr:idiffIncr;
	idiffIncr = WLZ_RGBA_BLUE_GET(*gptr1.rgbp)
	  - WLZ_RGBA_BLUE_GET(*gptr2.rgbp);
	idiff += (idiffIncr < 0)?-idiffIncr:idiffIncr;
	countedArea++;
      }
      break;
    default:
      errNum = WLZ_ERR_GREY_TYPE;
      break;
    }
  } 
 
  rtnDiff = (double) idiff / countedArea;
  WlzFreeObj(o1);
  WlzFreeObj(o2);

  if( dstErr ){
    *dstErr = errNum;
  }
  return rtnDiff;
}

WlzErrorNum DumbRegMatch(
  WlzDVertex2	*shift,
  double	*matchVal,
  WlzObject	*obj1,
  WlzObject	*obj2,
  WlzIVertex2	maxShift)
{
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  WlzObject	*o2;
  double       	meanDiff, minMeanDiff;
  int		i, j;
  WlzIVertex2	rtnShift;

  /* no checks just go for it */
  
  minMeanDiff = 10000.0;
  rtnShift.vtX = 0;
  rtnShift.vtY = 0;
  for(i = - (int) maxShift.vtX; i <= (int) maxShift.vtX; i++){
    for(j = - (int) maxShift.vtY; j <= (int) maxShift.vtY; j++){
      o2 = WlzShiftObject(obj2, i, j, 0, NULL);
      meanDiff = WlzGreyMeanDifference(obj1, o2, matchPercent, NULL);
      if( meanDiff < minMeanDiff ){
	minMeanDiff = meanDiff;
	rtnShift.vtX = i;
	rtnShift.vtY = j;
      }
      WlzFreeObj(o2);
    }
  }

  shift->vtX = rtnShift.vtX;
  shift->vtY = rtnShift.vtY;
  *matchVal = minMeanDiff;
  return errNum;
}

WlzPatchTree *WlzPatchTreeUnalignedChild(
  WlzPatchTree	*patchTree,
  int		depth,
  WlzDVertex2	*shift)
{
  WlzPatchTree	*child=NULL;
  int		i;

  if( patchTree ){
    shift->vtX += patchTree->xOff;
    shift->vtY += patchTree->yOff;
    if(patchTree->depth < depth){
      for(i=0; i < patchTree->nchildren; i++){
	if((child = WlzPatchTreeUnalignedChild(patchTree->children[i],
					       depth, shift)) != NULL){
	  break;
	}
      }
    }
    else if( patchTree->depth == depth ){
      if( !patchTree->offsetsCalculatedFlag ){
	child = patchTree;
      }
    }
    if( !child ){
      shift->vtX -= patchTree->xOff;
      shift->vtY -= patchTree->yOff;
    }
  }

  return child;
}

int WlzPatchMaxDepth(
  WlzPatchTree	*patchTree)
{
  int i, d, depth=0;

  if( patchTree ){
    if( patchTree->nchildren ){
      depth = WlzPatchMaxDepth(patchTree->children[0]);
      for(i=1; i < patchTree->nchildren; i++){
	d = WlzPatchMaxDepth(patchTree->children[i]);
	if( d > depth ){
	  depth = d;
	}
      }
    }
    else {
      depth = patchTree->depth;
    }
  }

  return depth;
}
  
WlzErrorNum WlzRegisterPatchTreeBF(
  WlzPatchTree	*patchTree)
{
  WlzObject	*obj, *obj1, *obj2, *objs[2];
  WlzPatchTree	*child;
  int 		depth;
  WlzDVertex2	shift;
  double	ccVal;
  WlzIVertex2	maxShift;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  /* assume nothing registered, get image to depth 0 */
  obj = WlzAssignObject(patchTree->obj, NULL);
  patchTree->offsetsCalculatedFlag = 1;
  depth = patchTree->depth + 1;

  /* get children to current depth, align and merge */
  maxShift.vtX = defMaxShift.vtX;
  maxShift.vtY = defMaxShift.vtY;
  shift.vtX = 0;
  shift.vtY = 0;
  while( depth <= WlzPatchMaxDepth(patchTree) ){
    while((child = WlzPatchTreeUnalignedChild(patchTree, depth,
                                              &shift)) != NULL){
      obj1 = WlzShiftObject(child->obj, shift.vtX, shift.vtY, 0, NULL);
      (void) DumbRegMatch(&shift, &ccVal, obj, obj1, maxShift);
      child->offsetsCalculatedFlag = 1;
      child->xOff = shift.vtX;
      child->yOff = shift.vtY;
      objs[1] = WlzShiftObject(obj1, shift.vtX, shift.vtY, 0, NULL);
      WlzFreeObj(obj1);
      objs[0] = obj;
      obj2 = WlzUnionN(2, objs, 1, NULL);
      WlzFreeObj(obj);
      WlzFreeObj(objs[1]);
      obj = WlzAssignObject(obj2, NULL);
      shift.vtX = 0;
      shift.vtY = 0;
    }
    depth++;
  }

  WlzFreeObj(obj);

  return errNum;
}

WlzErrorNum WlzRegisterPatchTreeDF(
  WlzPatchTree	*patchTree)
{
  int 		i;
  /* RecError	recErr=REC_ERR_NONE; */
  WlzDVertex2	shift;
  double	ccVal;
  WlzIVertex2	maxShift;
  /* RecPPControl	ppCtrl; */
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  /* calculate the offsets for each child then
     register each child tree */
  maxShift.vtX = defMaxShift.vtX;
  maxShift.vtY = defMaxShift.vtY;
  /*
  ppCtrl.method = REC_PP_WINDOW;
  ppCtrl.window.function = WLZ_WINDOWFN_RECTANGLE;
  ppCtrl.window.size.vtX = 90;
  ppCtrl.window.size.vtY = 90;
  ppCtrl.window.offset.vtX = 0;
  ppCtrl.window.offset.vtY = 0;
  ppCtrl.sample.function = (WlzSampleFn) NULL;
  ppCtrl.sample.factor = 0;
  ppCtrl.erode = 0;
  */
  for(i=0; i < patchTree->nchildren; i++){
    WlzStandardIntervalDomain(patchTree->obj->domain.i);
    WlzStandardIntervalDomain(patchTree->children[i]->obj->domain.i);

    (void) DumbRegMatch(&shift, &ccVal, patchTree->obj,
			patchTree->children[i]->obj, maxShift);
/*    recErr = RecTranMatch(&shift, &ccVal, patchTree->obj,
			  patchTree->children[i]->obj, maxShift, &ppCtrl);*/
    patchTree->children[i]->offsetsCalculatedFlag = 1;
    patchTree->children[i]->xOff = shift.vtX;
    patchTree->children[i]->yOff = shift.vtY;
    WlzRegisterPatchTreeDF(patchTree->children[i]);
  }

  return errNum;
}

WlzObject *WlzPatchTreeToObject(
  WlzPatchTree	*patchTree,
  int		alignGreysFlg)
{
  WlzObject	*rtnObj;
  WlzObject	*obj1, *objs[2];
  int		i, j;

  /* get the patch children adding as required */
  objs[0] = WlzAssignObject(WlzMakeMain(patchTree->obj->type,
					patchTree->obj->domain,
					patchTree->obj->values,
					NULL, NULL, NULL), NULL);
  for(i=0; i < patchTree->nchildren; i++){
    /* get object so far and remove overlap with the current object */
    obj1 = WlzPatchTreeToObject(patchTree->children[i], alignGreysFlg);
    objs[1] = WlzAssignObject(WlzDiffDomain(obj1, objs[0], NULL), NULL);

    /* reset the grey values if necessary  - approximate as a shift */
    if( alignGreysFlg ){
      if( WlzGreyTypeFromObj(obj1, NULL) == WLZ_GREY_RGBA ){
	WlzObject 	*obj2;
	double 		min1[4], max1[4], sum1[4], sumSq1[4], mean1[4], stdDev1[4];
	double 		min2[4], max2[4], sum2[4], sumSq2[4], mean2[4], stdDev2[4];
	WlzGreyType	gType;

	if((obj2 = WlzIntersect2(obj1, objs[0], NULL)) != NULL){
	  obj2->values.core = obj1->values.core;
	  WlzRGBAGreyStats(obj2, WLZ_RGBA_SPACE_RGB, &gType, min1, max1,
			   sum1, sumSq1, mean1, stdDev1, NULL);
	  obj2->values.core = objs[0]->values.core;
	  WlzRGBAGreyStats(obj2, WLZ_RGBA_SPACE_RGB, &gType, min2, max2,
			   sum2, sumSq2, mean2, stdDev2, NULL);
	  obj2->values.core = NULL;
	  WlzFreeObj(obj2);
	  for(j=0; j < 3; j++){
	    mean1[j] /= mean2[j];
	  }
	  obj2 = WlzAssignObject
	    (WlzRGBAGreyScale(objs[0], mean1, NULL), NULL);
	  WlzFreeObj(objs[0]);
	  objs[0] = obj2;
	}
      }
      else {
	WlzObject 	*obj2;
	double 		min1, max1, sum1, sumSq1, mean1, stdDev1;
	double 		min2, max2, sum2, sumSq2, mean2, stdDev2;
	WlzGreyType	gType;

	if((obj2 = WlzIntersect2(obj1, objs[0], NULL)) != NULL){
	  obj2->values.core = obj1->values.core;
	  WlzGreyStats(obj2, &gType, &min1, &max1,
		       &sum1, &sumSq1, &mean1, &stdDev1, NULL);
	  obj2->values.core = objs[0]->values.core;
	  WlzGreyStats(obj2, &gType, &min2, &max2,
		       &sum2, &sumSq2, &mean2, &stdDev2, NULL);
	  obj2->values.core = NULL;
	  WlzFreeObj(obj2);
	  obj2 = WlzAssignObject(WlzGreyScale(objs[0], mean1 / mean2, NULL), NULL);
	  WlzFreeObj(objs[0]);
	  objs[0] = obj2;
	}
      }
    }

    WlzFreeObj(obj1);
    obj1 = WlzUnionN(2, objs, 1, NULL);
    /* WlzStandardIntervalDomain(obj1->domain.i); */
    WlzFreeObj(objs[0]);
    WlzFreeObj(objs[1]);
    objs[0] = obj1;
  }

  /* now shift the given object */
  rtnObj = WlzShiftObject(objs[0], (int) patchTree->xOff,
			  (int) patchTree->yOff, 0, NULL);
  WlzFreeObj(objs[0]);

  return rtnObj;
}

int WlzPatchHitBuffers(
  WlzPatchTree	*patchTree)
{
  int i;

  if((abs(patchTree->xOff) >= defMaxShift.vtX) ||
     (abs(patchTree->yOff) >= defMaxShift.vtY)){
    return 1;
  }

  for(i=0; i < patchTree->nchildren; i++){
    if( WlzPatchHitBuffers(patchTree->children[i]) ){
      return 1;
    }
  }

  return 0;
}
  
     

WlzErrorNum WlzPrintPatchTree(
  WlzPatchTree	*patchTree,
  FILE		*fp,
  int		depth)
{
  int	i;
  char	*depthStr;

  /* set depth string */
  depthStr = (char *) AlcMalloc(sizeof(char) * (depth + 2)*2);
  depthStr[0] = '\0';
  for(i=0; i < depth; i++){
    sprintf(depthStr, "%s  ", depthStr);
  }

  /* print this node detail */
  fprintf(fp,
	  "%sObject Index: %d, depth: %d, children: %d, cost: %f\n",
	  depthStr, patchTree->index, patchTree->depth,
	  patchTree->nchildren, patchTree->cost);
  
  /* print children facts */
  for(i=0; i < patchTree->nchildren; i++){
    WlzPrintPatchTree(patchTree->children[i], fp, depth+1);
  }

  return WLZ_ERR_NONE;
}

WlzErrorNum WlzPatchFacts(
  WlzPatchTree	*patchTree,
  FILE		*fp,
  char		**dstStr,
  int 		verbose)
{
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  int		i;

  /* print offsets */
  fprintf(fp,
	  "WlzPatchFacts: offsets\n"
	  "==============\n"
	  " object index = %d, offsetsCalculatedFlag = %d\n"
	  " x-offset = %f, y-offset = %f\n",
	  patchTree->index,
	  patchTree->offsetsCalculatedFlag,
	  patchTree->xOff, patchTree->yOff);
  
  /* print object facts */
  if( verbose ){
    fprintf(fp,
	    "WlzPatchFacts: object detail\n"
	    "==============\n");
    WlzObjectFacts(patchTree->obj, fp, dstStr, 0);
    fprintf(fp,
	    "===================================================\n");
  }

  /* print children facts */
  for(i=0; i < patchTree->nchildren; i++){
    WlzPatchFacts(patchTree->children[i], fp, dstStr, verbose);
  }

  return errNum;
}

 
int main(int	argc,
	 char	**argv)
{

  WlzObject	*obj, *nobj, **objs, *obj1;
  WlzPatchTree	*patchTree;
  WlzCompoundArray	*cobj;
  FILE		*inFile;
  char 		optList[] = "bdgGhvp:t:T:";
  int		option;
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  int		i, startIndx, depth;
  int		alignGreysFlg=1;
  WlzGreyType	gType, dstGType;
  double	dstMin, dstMax, dstSum, dstSumSq;
  double	dstMean, dstStdDev, extMean;
  int		breadthFirstFlg=1;
  WlzPixelV	threshV;
  int		val, threshFlg=0;

  /* set the default maximum shift */
  defMaxShift.vtX = 30;
  defMaxShift.vtY = 30;

  /* read the argument list and check for an input file */
  opterr = 0;
  while( (option = getopt(argc, argv, optList)) != EOF ){
    switch( option ){
    case 'b':
      breadthFirstFlg = 1;
      break;

    case 'd':
      breadthFirstFlg = 0;
      break;

    case 'g':
      alignGreysFlg = 1;
      break;

    case 'G':
      alignGreysFlg = 0;
      break;

    case 'p':
      if( sscanf(optarg, "%lf", &matchPercent) == 1 ){
	if( matchPercent > 100.0 ){
	  matchPercent = 100.0;
	}
	else if( matchPercent < 2.0 ){
	  matchPercent = 2.0;
	}
      }
      else {
	matchPercent = 10.0;
      }
      break;

    case 't':
      switch( sscanf(optarg, "%d,%d",
		     &(defMaxShift.vtX), &(defMaxShift.vtY)) ){

      default:
      case 0:
	fprintf(stderr, "%s: no search width set\n", argv[0]);
        usage(argv[0]);
        return 1;

      case 1:
	defMaxShift.vtY = defMaxShift.vtX;
	break;
 
      case 2:
	break;

      }
      break;

    case 'T':
      if( sscanf(optarg, "%d", &val) < 1 ){
	val = 1;
      }
      threshFlg = 1;
      break;

    case 'v':
      verboseFlg = 1;
      break;

    case 'h':
    default:
      usage(argv[0]);
      return WLZ_ERR_UNSPECIFIED;
    }
  }

  /* verbose output - parameter summary */
  if( verboseFlg ){
    fprintf(stderr,
	    "%s: search - %s, align pixel-values - %s, threshold - %s\n"
	    "\tmatch percentage - %.1f, max shift - (%d,%d)\n",
	    argv[0], breadthFirstFlg?"breadth first":"depth first",
	    alignGreysFlg?"True":"False", threshFlg?"True":"False",
	    matchPercent, defMaxShift.vtX, defMaxShift.vtY);
  }

  inFile = stdin;
  if( optind < argc ){
    if( (inFile = fopen(*(argv+optind), "r")) == NULL ){
      fprintf(stderr, "%s: can't open file %s\n", argv[0], *(argv+optind));
      usage(argv[0]);
      return WLZ_ERR_UNSPECIFIED;
    }
  }

  /* verbose output */
  if( verboseFlg ){
    if( inFile == stdin ){
      fprintf(stderr, "%s: patch object read from stdin\n", argv[0]);
    }
    else {
      fprintf(stderr, "%s: patch object read from %s\n", argv[0],
	      *(argv+optind));
    }
  }

  /* read objects and threshold if possible */
  while( (obj = WlzAssignObject(WlzReadObj(inFile, &errNum), NULL)) != NULL) 
  {
    switch( obj->type )
    {
    case WLZ_COMPOUND_ARR_1:
    case WLZ_COMPOUND_ARR_2:
      cobj = (WlzCompoundArray *) obj;
      if( cobj->n < 1 ){
	nobj = WlzMakeEmpty(&errNum);
      }
      else if( cobj->n == 1 ){
	nobj = WlzAssignObject(cobj->o[0], &errNum);
      }
      else {
	/* copy the object list so the patch tree can be built 
	   threshold at 1 to remove funny black lines */
	objs = (WlzObject **) AlcMalloc(sizeof(WlzObject *) * cobj->n);
	threshV.type = WLZ_GREY_INT;
	threshV.v.inv = 1;
	for(i=0; i < cobj->n; i++){
	  if( threshFlg ){
	    objs[i] = 
	      WlzAssignObject(WlzThreshold(cobj->o[i], threshV,
					   WLZ_THRESH_HIGH, NULL), NULL);
	    if( verboseFlg ){
	      fprintf(stderr, "%s: object index %d thresholded\n", argv[0], i);
	    }
	  }
	  else {
	    objs[i] = 
	      WlzAssignObject(cobj->o[i], NULL);
	  }
	}

	/* find the most dense object - assumed to have the
	   bulk of the foreground therefore a suitable starting
	   point for matching. */
	startIndx = 0;
	gType = WlzGreyTypeFromObj(objs[0], &errNum);
	if( gType == WLZ_GREY_RGBA ){
	  obj1 = WlzRGBAToModulus(objs[0], &errNum);
	  WlzGreyStats(obj1, &dstGType, &dstMin, &dstMax,
		       &dstSum, &dstSumSq, &extMean, &dstStdDev,
		       NULL);
	  WlzFreeObj(obj1);
	}
	else {
	  WlzGreyStats(objs[0], &dstGType, &dstMin, &dstMax,
		       &dstSum, &dstSumSq, &extMean, &dstStdDev,
		       NULL);
	}
	for(i=1; i < cobj->n; i++){
	  if( gType == WLZ_GREY_RGBA ){
	    obj1 = WlzRGBAToModulus(objs[i], &errNum);
	    WlzGreyStats(obj1, &dstGType, &dstMin, &dstMax,
			 &dstSum, &dstSumSq, &dstMean, &dstStdDev,
			 NULL);
	    WlzFreeObj(obj1);
	  }
	  else {
	    WlzGreyStats(objs[i], &dstGType, &dstMin, &dstMax,
			 &dstSum, &dstSumSq, &dstMean, &dstStdDev,
			 NULL);
	  }
	  if( dstMean < extMean ){
	    extMean = dstMean;
	    startIndx = i;
	  }
	}
	if( verboseFlg ){
	  fprintf(stderr, "%s: object index %d at top of patch tree\n",
		 argv[0], startIndx);
	}

	if( breadthFirstFlg ){
	  /* generate the patch tree - breadth first search */
	  depth = 0;
	  patchTree = WlzMakePatchTree(objs[startIndx], depth, extMean);
	  patchTree->index = startIndx;
	  objs[startIndx] = NULL;
	  while( numObjsLeft(objs, cobj->n) && (depth <= cobj->n) ){
	    depth++;
	    WlzGetPatchTreeToDepth(objs, cobj->n, patchTree, depth);
	  }
	}
	else {
	  /* generate the patch tree - depth first search */
	  obj1 = objs[startIndx];
	  objs[startIndx] = NULL;
	  patchTree = WlzGetPatchTree(obj1, objs, cobj->n);
	  WlzFreeObj(obj1);
	}

	/* verbose output */
	if( verboseFlg ){
	  WlzPrintPatchTree(patchTree, stderr, 0);
	}     

	/* calculate the offsets */
	/* don't know why depth-first was disabled - put back for now RAB */
	if( breadthFirstFlg ){
	  errNum = WlzRegisterPatchTreeBF(patchTree);
	}
	else {
	  errNum = WlzRegisterPatchTreeDF(patchTree);
	}

	/* check if hit the buffers */
	if( WlzPatchHitBuffers(patchTree) ){
	  fprintf(stderr,
		  "\007\007 %s: hit the shift limit somewhere, use the -v\n"
		  "flag to print out the offsets. Try extending the limits\n"
		  "with the -t flag or remove the non-matching image\n\007",
		  argv[0]);
	}

	/* print out the patch data and offsets */
	if( verboseFlg ){
	  WlzPatchFacts(patchTree, stderr, NULL, 0);
	}

	/* get the patch object */
	nobj = WlzPatchTreeToObject(patchTree, alignGreysFlg);
	(void) WlzFreePatchTree(patchTree);
	(void) AlcFree((void *) objs);
      }
      if( errNum == WLZ_ERR_NONE ){
	errNum = WlzWriteObj(stdout, nobj);
      }
      if( nobj ){
	WlzFreeObj(nobj);
      }
      break;
      
    default:
      errNum = WlzWriteObj(stdout, obj);
      break;
    }

    WlzFreeObj(obj);
  }

  /* trap the WLZ_ERR_READ_EOF since this is a legal way of indicating
     the end of objects in a file */
  if( errNum == WLZ_ERR_READ_EOF ){
    errNum = WLZ_ERR_NONE;
  }

  return errNum;
}
