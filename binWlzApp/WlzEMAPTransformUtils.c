#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _WlzEMAPTransformUtils_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         binWlzApp/WlzEMAPTransformUtils.c
* \author       Richard Baldock
* \date         January 2007
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
* \ingroup      BinWlzApp
* \brief        Utilities to transform between standard EMAP models.
*
* \par Binary
* \ref wlzemaptransformutils "WlzEMAPTransformUtils"
*/

/*!
\ingroup BinWlzApp
\defgroup wlzemaptransformutils WlzEMAPTransformUtils
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <WlzExtFF.h>
#include <WlzEMAP.h>

/* externals required by getopt  - not in ANSI C standard */
#ifdef __STDC__ /* [ */
extern int      getopt(int argc, char * const *argv, const char *optstring);
 
extern int 	optind, opterr, optopt;
extern char     *optarg;
#endif /* __STDC__ ] */

char	*EMAP_WarpTransformsDir="/opt/MouseAtlas/EMAP_WarpTransforms";

static WlzErrorNum WlzSetMeshInverse(
  WlzMeshTransform	*meshTr)
{
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  int		i;

  /* check the mesh transform */
  if( meshTr ){
    if( meshTr->nodes == NULL ){
      errNum = WLZ_ERR_PARAM_NULL;
    }
  }
  else {
    errNum = WLZ_ERR_PARAM_NULL;
  }

  /* loop through nodes,add the displacement then invert displacement */
  if( errNum == WLZ_ERR_NONE ){
    for(i=0; i < meshTr->nNodes; i++){
      meshTr->nodes[i].position.vtX += meshTr->nodes[i].displacement.vtX;
      meshTr->nodes[i].position.vtY += meshTr->nodes[i].displacement.vtY;
      meshTr->nodes[i].displacement.vtX *= -1.0;
      meshTr->nodes[i].displacement.vtY *= -1.0;
    }
  }

  return errNum;
}

static WlzObject *WlzEMAPGetObject(
  char		*dirStr,
  char		*fileStr,
  WlzErrorNum	*dstErr)
{
  char		*fileBuf;
  FILE		*FP;
  WlzObject	*obj=NULL;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  fileBuf = (char *) AlcMalloc((strlen(dirStr) +
				strlen(fileStr) + 8) *
			    sizeof(char));
  
  sprintf(fileBuf, "%s/%s", dirStr, fileStr);
  if((FP = fopen(fileBuf, "rb")) != NULL){
    if((obj = WlzReadObj(FP, &errNum)) != NULL){
      obj = WlzAssignObject(obj, &errNum);
    }
    fclose(FP);
  }
  else {
    errNum = WLZ_ERR_FILE_OPEN;
  }
  AlcFree(fileBuf);

  if( dstErr ){
    *dstErr = errNum;
  }
  return obj;
}

WlzErrorNum WlzEMAPFreeMapping(
  WLZ_EMAP_WarpTransformStruct	*mapping)
{
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  int		i;

  if( mapping ){
    if( mapping->srcModel ){
      AlcFree(mapping->srcModel );
    }

    if( mapping->dstModel ){
      AlcFree(mapping->dstModel );
    }

    if( mapping->srcProj ){
      WlzFree3DViewStruct(mapping->srcProj);
    }

    if( mapping->dstProj ){
      WlzFree3DViewStruct(mapping->dstProj);
    }

    for(i=0; i < 3; i ++){
      if( mapping->srcDoms[i] ){
	errNum = WlzFreeObj(mapping->srcDoms[i]);
      }
      if( mapping->dstDoms[i] ){
	errNum = WlzFreeObj(mapping->dstDoms[i]);
      }
      if( mapping->meshObj[i] ){
	errNum = WlzFreeObj(mapping->meshObj[i]);
      }
    }
    AlcFree(mapping);
  }

  return errNum;
}

WLZ_EMAP_WarpTransformStruct *WlzEMAPGetMapping(
  char		*srcModel,
  char		*dstModel,
  char		*transformDir,
  WlzErrorNum	*dstErr)
{
  WLZ_EMAP_WarpTransformStruct	*mapping=NULL;
  char		*mappingDirStrF=NULL;
  char		*mappingDirStrB=NULL;
  char		*file;
  FILE		*FP;
  char		*transDir;
  struct stat	statBuf;
  WlzThreeDViewStruct	*viewStr;
  BibFileRecord	*bibfileRecord;
  BibFileError	bibFileErr;
  char		*errMsg;
  WlzErrorNum	errNum=WLZ_ERR_NONE;


  
  /* special knowledge here, check against mapping directories,
     currently we ignore version number */
  if( transformDir ){
    transDir = transformDir;
  }
  else {
    transDir = EMAP_WarpTransformsDir;
  }
  mappingDirStrF = AlcMalloc((strlen(transDir) +
			      80) * sizeof(char));
  sprintf(mappingDirStrF,
	  "%s/mapping_%s_to_%s",
	  transDir, srcModel, dstModel);

  mappingDirStrB = AlcMalloc((strlen(transDir) +
			      80) * sizeof(char));
  sprintf(mappingDirStrB,
	  "%s/mapping_%s_to_%s",
	  transDir, dstModel, srcModel);
  file = AlcMalloc((strlen(transDir) +
		    128) * sizeof(char));

  /* check both forwards and backwards directory paths */
  if( stat(mappingDirStrF, &statBuf) == 0 ){
    if( S_ISDIR(statBuf.st_mode) ){

      /* the mapping is forwards */
      mapping = AlcCalloc(sizeof(WLZ_EMAP_WarpTransformStruct), 1);
      mapping->srcModel = AlcStrDup(srcModel);
      mapping->dstModel = AlcStrDup(dstModel);
      mapping->mtime = statBuf.st_mtime;

      /* src domains */
      mapping->srcDoms[0] = WlzEMAPGetObject(mappingDirStrF, "source_bb.wlz",
					     &errNum);
      mapping->srcDoms[1] = WlzEMAPGetObject(mappingDirStrF, "source_bt.wlz",
					     &errNum);
      mapping->srcDoms[2] = WlzEMAPGetObject(mappingDirStrF, "source_tt.wlz",
					     &errNum);

      /* src projection */
      viewStr = WlzMake3DViewStruct(WLZ_3D_VIEW_STRUCT, &errNum);
      sprintf(file, "%s/source_projection.bib", mappingDirStrF);
      if((FP = fopen(file, "r")) != NULL){
	bibFileErr = BibFileRecordRead(&bibfileRecord, &errMsg, FP);
	while((bibFileErr == BIBFILE_ER_NONE) &&
	      (strncmp(bibfileRecord->name, "Wlz3DSectionViewParams", 22))){
	  BibFileRecordFree(&bibfileRecord);
	  bibFileErr = BibFileRecordRead(&bibfileRecord, &errMsg, FP);
	}
	fclose( FP );
	if( bibFileErr != BIBFILE_ER_NONE ){
	  errNum = WLZ_ERR_PARAM_DATA;
	}
	else {
	  WlzEffBibParse3DSectionViewParamsRecord(bibfileRecord, viewStr);
	  mapping->srcProj = viewStr;
	}
	BibFileRecordFree(&bibfileRecord);
      }

      /* dst domains */
      mapping->dstDoms[0] = WlzEMAPGetObject(mappingDirStrF, "target_bb.wlz",
					     &errNum);
      mapping->dstDoms[1] = WlzEMAPGetObject(mappingDirStrF, "target_bt.wlz",
					     &errNum);
      mapping->dstDoms[2] = WlzEMAPGetObject(mappingDirStrF, "target_tt.wlz",
					     &errNum);

      /* dst projection */
      viewStr = WlzMake3DViewStruct(WLZ_3D_VIEW_STRUCT, &errNum);
      sprintf(file, "%s/target_projection.bib", mappingDirStrF);
      if((FP = fopen(file, "r")) != NULL){
	bibFileErr = BibFileRecordRead(&bibfileRecord, &errMsg, FP);
	while((bibFileErr == BIBFILE_ER_NONE) &&
	      (strncmp(bibfileRecord->name, "Wlz3DSectionViewParams", 22))){
	  BibFileRecordFree(&bibfileRecord);
	  bibFileErr = BibFileRecordRead(&bibfileRecord, &errMsg, FP);
	}
	fclose( FP );
	if( bibFileErr != BIBFILE_ER_NONE ){
	  errNum = WLZ_ERR_PARAM_DATA;
	}
	else {
	  WlzEffBibParse3DSectionViewParamsRecord(bibfileRecord, viewStr);
	  mapping->dstProj = viewStr;
	}
	BibFileRecordFree(&bibfileRecord);
      }

      /* now the mesh transforms */
      mapping->meshObj[0] = WlzEMAPGetObject(mappingDirStrF,
					     "source_to_target_bb.wlz",
					     &errNum);
      mapping->meshObj[1] = WlzEMAPGetObject(mappingDirStrF,
					     "source_to_target_bt.wlz",
					     &errNum);
      mapping->meshObj[2] = WlzEMAPGetObject(mappingDirStrF,
					     "source_to_target_tt.wlz",
					     &errNum);
    }
    else {
      errNum = WLZ_ERR_PARAM_DATA;
    }
  }
  else if( stat(mappingDirStrB, &statBuf) == 0 ){
    if( S_ISDIR(statBuf.st_mode) ){

      /* the mapping is backwards */
      mapping = AlcCalloc(sizeof(WLZ_EMAP_WarpTransformStruct), 1);
      mapping->srcModel = AlcStrDup(srcModel);
      mapping->dstModel = AlcStrDup(dstModel);
      mapping->mtime = statBuf.st_mtime;

      /* src domains */
      mapping->srcDoms[0] = WlzEMAPGetObject(mappingDirStrB, "target_bb.wlz",
					     &errNum);
      mapping->srcDoms[1] = WlzEMAPGetObject(mappingDirStrB, "target_bt.wlz",
					     &errNum);
      mapping->srcDoms[2] = WlzEMAPGetObject(mappingDirStrB, "target_tt.wlz",
					     &errNum);

      /* src projection */
      viewStr = WlzMake3DViewStruct(WLZ_3D_VIEW_STRUCT, &errNum);
      sprintf(file, "%s/target_projection.bib", mappingDirStrB);
      if((FP = fopen(file, "r")) != NULL){
	bibFileErr = BibFileRecordRead(&bibfileRecord, &errMsg, FP);
	while((bibFileErr == BIBFILE_ER_NONE) &&
	      (strncmp(bibfileRecord->name, "Wlz3DSectionViewParams", 22))){
	  BibFileRecordFree(&bibfileRecord);
	  bibFileErr = BibFileRecordRead(&bibfileRecord, &errMsg, FP);
	}
	fclose( FP );
	if( bibFileErr != BIBFILE_ER_NONE ){
	  errNum = WLZ_ERR_PARAM_DATA;
	}
	else {
	  WlzEffBibParse3DSectionViewParamsRecord(bibfileRecord, viewStr);
	  mapping->srcProj = viewStr;
	}
	BibFileRecordFree(&bibfileRecord);
      }

      /* dst domains */
      mapping->dstDoms[0] = WlzEMAPGetObject(mappingDirStrB, "source_bb.wlz",
					     &errNum);
      mapping->dstDoms[1] = WlzEMAPGetObject(mappingDirStrB, "source_bt.wlz",
					     &errNum);
      mapping->dstDoms[2] = WlzEMAPGetObject(mappingDirStrB, "source_tt.wlz",
					     &errNum);

      /* dst projection */
      viewStr = WlzMake3DViewStruct(WLZ_3D_VIEW_STRUCT, &errNum);
      sprintf(file, "%s/source_projection.bib", mappingDirStrB);
      if((FP = fopen(file, "r")) != NULL){
	bibFileErr = BibFileRecordRead(&bibfileRecord, &errMsg, FP);
	while((bibFileErr == BIBFILE_ER_NONE) &&
	      (strncmp(bibfileRecord->name, "Wlz3DSectionViewParams", 22))){
	  BibFileRecordFree(&bibfileRecord);
	  bibFileErr = BibFileRecordRead(&bibfileRecord, &errMsg, FP);
	}
	fclose( FP );
	if( bibFileErr != BIBFILE_ER_NONE ){
	  errNum = WLZ_ERR_PARAM_DATA;
	}
	else {
	  WlzEffBibParse3DSectionViewParamsRecord(bibfileRecord, viewStr);
	  mapping->dstProj = viewStr;
	}
	BibFileRecordFree(&bibfileRecord);
      }

      /* now the mesh transforms */
      if((mapping->meshObj[0] = WlzEMAPGetObject(mappingDirStrB,
						 "source_to_target_bb.wlz",
						 &errNum)) != NULL){
	WlzSetMeshInverse(mapping->meshObj[0]->domain.mt);
      }
      if((mapping->meshObj[1] = WlzEMAPGetObject(mappingDirStrB,
						 "source_to_target_bt.wlz",
						 &errNum)) != NULL){
	WlzSetMeshInverse(mapping->meshObj[1]->domain.mt);
      }
      if((mapping->meshObj[2] = WlzEMAPGetObject(mappingDirStrB,
						 "source_to_target_tt.wlz",
						 &errNum)) != NULL){
	WlzSetMeshInverse(mapping->meshObj[2]->domain.mt);
      }
    }
    else {
      errNum = WLZ_ERR_PARAM_DATA;
    }
  }
  else {
    errNum = WLZ_ERR_PARAM_DATA;
  }

  /* clean space */
  AlcFree(mappingDirStrF);
  AlcFree(mappingDirStrB);

  if( dstErr ){
    *dstErr = errNum;
  }
  return mapping;
}

int WlzEMAPIsMapping(
  char		*srcModel,
  char		*dstModel,
  char		*transformDir,
  WlzErrorNum	*dstErr)
{
  char		*mappingDirStrF=NULL;
  char		*mappingDirStrB=NULL;
  char		*transDir;
  struct stat	statBuf;
  int		rtnVal;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  /* special knowledge here, check against mapping directories,
     currently we ignore version number */
  if( transformDir ){
    transDir = transformDir;
  }
  else {
    transDir = EMAP_WarpTransformsDir;
  }
  mappingDirStrF = AlcMalloc((strlen(transDir) +
			      80) * sizeof(char));
  sprintf(mappingDirStrF,
	  "%s/mapping_%s_to_%s",
	  transDir, srcModel, dstModel);

  mappingDirStrB = AlcMalloc((strlen(transDir) +
			      80) * sizeof(char));
  sprintf(mappingDirStrB,
	  "%s/mapping_%s_to_%s",
	  transDir, dstModel, srcModel);

  if( stat(mappingDirStrF, &statBuf) == 0 ){
    if( S_ISDIR(statBuf.st_mode) ){
      rtnVal = 1;
    }
    else {
      rtnVal = 0;
      errNum = WLZ_ERR_PARAM_DATA;
    }
  }
  else if( stat(mappingDirStrB, &statBuf) == 0 ){
    if( S_ISDIR(statBuf.st_mode) ){
      rtnVal = 1;
    }
    else {
      rtnVal = 0;
      errNum = WLZ_ERR_PARAM_DATA;
    }
  }
  else {
    rtnVal = 0;
    errNum = WLZ_ERR_PARAM_DATA;
  }

  /* clean space */
  AlcFree(mappingDirStrF);
  AlcFree(mappingDirStrB);

  if( dstErr ){
    *dstErr = errNum;
  }
  return rtnVal;
}


WlzObject *WlzEMAPDomainTransform(
  char		*srcModel,
  char		*dstModel,
  WlzObject	*obj,
  WlzErrorNum	*dstErr)
{
  WlzObject	*rtnObj=NULL;
  WLZ_EMAP_WarpTransformStruct	*mapping;
  int		i;
  WlzObject	*obj1, *obj2, *obj3;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  /* check input models and domain */
  if( WlzEMAPIsMapping(srcModel, dstModel, NULL, &errNum) ){
    if( obj ){
      switch( obj->type ){

      case WLZ_2D_DOMAINOBJ:
	break;

      case WLZ_3D_DOMAINOBJ:
	break;

      case WLZ_EMPTY_OBJ:
	return WlzMakeEmpty(dstErr);

      default:
	errNum = WLZ_ERR_OBJECT_TYPE;
	break;
      }
    }
    else {
      errNum = WLZ_ERR_OBJECT_NULL;
    }
  }

  /* Get the mapping structure and apply */
  if(errNum == WLZ_ERR_NONE){
    if((mapping = WlzEMAPGetMapping(srcModel, dstModel, NULL,
				    &errNum)) != NULL){
      if((errNum = WlzInit3DViewStruct(mapping->srcProj,
                                       obj)) == WLZ_ERR_NONE ){
	for(i=0; i < 3; i++){
	  if( mapping->srcDoms[i] ){
	    if((obj1 = WlzIntersect2(obj, mapping->srcDoms[i],
	                             &errNum)) != NULL){
	      if((obj2 = WlzGetSectionFromObject(obj1,
						 mapping->srcProj,
						 WLZ_INTERPOLATION_NEAREST,
						 &errNum)) != NULL){
		if((obj3 = WlzMeshTransformObj(obj2,
		                               mapping->meshObj[i]->domain.mt,
					       WLZ_INTERPOLATION_NEAREST,
					       &errNum)) != NULL){
		  WlzFreeObj(obj1);
		  WlzFreeObj(obj2);
		  if( rtnObj ){
		    obj2 = WlzUnion2(obj3, rtnObj, NULL);
		    WlzFreeObj(obj3);
		    WlzFreeObj(rtnObj);
		    rtnObj = obj2;
		  }
		  else {
		    rtnObj = obj3;
		  }
		}
	      }
	    }
	  }
	}
      }
      if((errNum == WLZ_ERR_NONE) && rtnObj ){
	if((errNum = WlzInit3DViewStruct(mapping->dstProj,
	                                 rtnObj)) == WLZ_ERR_NONE){
	  if((obj1 = Wlz3DViewTransformObj(rtnObj, mapping->dstProj,
	                                   &errNum)) != NULL){
	    WlzFreeObj(rtnObj);
	    rtnObj = obj1;
	  }
	  else {
	    WlzFreeObj(rtnObj);
	    rtnObj = NULL;
	  }
	}
	else {
	  WlzFreeObj(rtnObj);
	  rtnObj = NULL;
	}
      }
      WlzEMAPFreeMapping(mapping);
    }
  }

  if( dstErr ){
    *dstErr = errNum;
  }
  return rtnObj;
}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */
