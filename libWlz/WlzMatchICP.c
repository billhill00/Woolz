#pragma ident "MRC HGU $Id$"
/*!
* \file        	WlzMatchICP.c
* \author       Bill Hill
* \date         August 2001
* \version	$Id$
* \note
*		Copyright:
*		2001 Medical Research Council, UK.
*		All rights reserved.
* \par  Address:
*		MRC Human Genetics Unit,
*		Western General Hospital,
*		Edinburgh, EH4 2XU, UK.
* \brief	Functions to match objects, providing a set of
*		corresponding points, using ICP based registration.
* \ingroup	WlzTransform
* \todo         
* \bug
*/
#include <stdio.h>
#include <limits.h>
#include <float.h>
#include <Wlz.h>
#include <strings.h>

typedef struct _WlzMatchICPShell
{
  int				size;
  WlzGMShell			*shell;
  WlzAffineTransform 		*tr;
} WlzMatchICPShell;

typedef struct _WlzMatchICPShellListElm
{
  struct _WlzMatchICPShell	mShell;
  struct _WlzMatchICPShellListElm *next;
  struct _WlzMatchICPShellListElm *prev;
} WlzMatchICPShellListElm;

typedef struct _WlzMatchICPShellList
{
  struct _WlzMatchICPShellListElm *head;
  struct _WlzMatchICPShellListElm *tail;
} WlzMatchICPShellList;

typedef struct _WlzMatchICPCbData
{
  int		nNS;
  int		nFS;
  WlzErrorNum	errNum;
  struct _WlzMatchICPShellList *list;
} WlzMatchICPCbData;

static WlzErrorNum  		WlzMatchICPCtr(
				  WlzContour *tCtr,
				  WlzContour *sCtr,
				  WlzAffineTransform *initTr,
				  int maxItr,
				  int minSpx,
				  int *dstNMatch,
				  WlzVertexP *dstTMatch,
				  WlzVertexP *dstSMatch,
				  int brkFlg, int dbgFlg);
static WlzErrorNum		WlzMatchICPRegShellLst(
				  AlcKDTTree *tTree,
				  WlzGMModel *sGM,
				  WlzMatchICPShellList *sLst,
				  WlzTransformType trType,
				  WlzVertexType vType,
				  int nTV,
				  WlzVertexP tVx,
				  WlzVertexP tNr,
				  int nSV,
				  WlzVertexP sVx,
				  WlzVertexP sNr,
				  int *iBuf,
				  WlzVertexP tVBuf,
				  WlzVertexP sVBuf,
				  double *wBuf,
				  int maxItr,
				  int minSpx,
				  WlzAffineTransform *initTr);
static void			WlzMatchICPShellListInsertList(
				  WlzMatchICPShellList *list0,
				  WlzMatchICPShellList *list1);
static WlzAffineTransform 	*WlzMatchICPRegModel(
				  AlcKDTTree *tTree,
				  WlzTransformType trType,
				  WlzVertexType vType,
				  int nTV,
				  WlzVertexP tVx,
				  WlzVertexP tNr,
				  int nSV,
				  WlzVertexP sVx,
				  WlzVertexP sNr,
				  int *iBuf,
				  WlzVertexP tVBuf,
				  WlzVertexP sVBuf,
				  double *wBuf,
				  int maxItr,
				  WlzAffineTransform *initTr,
				  int *dstConv,
				  WlzErrorNum *dstErr);
static WlzAffineTransform 	*WlzMatchICPRegShell(
				  AlcKDTTree *tTree,
				  WlzGMShell *sS,
				  WlzTransformType trType,
				  WlzVertexType vType,
				  int nTV,
				  WlzVertexP tVx,
				  WlzVertexP tNr,
				  int nSV,
				  WlzVertexP sVx,
				  WlzVertexP sNr,
				  int *iBuf,
				  WlzVertexP tVBuf,
				  WlzVertexP sVBuf,
				  double *wBuf,
				  int maxItr,
				  WlzAffineTransform *initTr,
				  int *dstConv,
				  WlzErrorNum *dstErr);
static int			WlzMatchICPGetPoints(
				  AlcKDTTree *tTree,
				  WlzMatchICPShell *mS,
				  WlzVertexType vType,
				  WlzVertexP tVx,
				  WlzVertexP tNr,
				  WlzVertexP sNr,
				  WlzVertexP tMatch,
				  WlzVertexP sMatch,
				  int offset,
				  WlzErrorNum *dstErr);
static int			WlzMatchICPGetPoints2D(
				  AlcKDTTree *tTree,
				  WlzMatchICPShell *mS,
				  WlzDVertex2 *tVx,
				  WlzDVertex2 *tNr,
				  WlzDVertex2 *sNr,
				  WlzDVertex2 *tMatch,
				  WlzDVertex2 *sMatch);
static WlzErrorNum		WlzMatchICPBreakShellCon(
				  WlzMatchICPCbData *cbData,
				  WlzGMModel *sGM,
				  WlzGMShell *sS,
				  int *iBuf);
static WlzErrorNum 		WlzMatchICPBreakShellCon2D(
				  WlzMatchICPCbData *cbData,
				  WlzGMModel *sGM,
				  WlzGMShell *sS,
				  int *iBuf);
static WlzErrorNum		WlzMatchICPBreakShellMid(
				  WlzMatchICPCbData *cbData,
				  WlzGMModel *sGM,
				  WlzMatchICPShell *sMS,
				  WlzVertexP sNr,
				  int *iBuf,
				  int minSpx);
static WlzErrorNum		WlzMatchICPBreakShellLoopTs(
				  WlzGMModel *sGM,
				  WlzGMShell *sS,
				  WlzVertexP sNr,
				  int minSpx,
				  int maxFlg);
static WlzErrorNum		WlzMatchICPBreakShellLop2D(
				  WlzGMModel *sGM,
				  WlzGMShell *sS,
				  WlzDVertex2 *sNr,
				  int minSpx,
				  int maxFlg);
static WlzErrorNum 		WlzMatchICPBreakShellCur2D(
				  WlzMatchICPCbData *cbData,
				  WlzGMModel *sGM,
				  WlzMatchICPShell *sMS,
				  WlzDVertex2 *sNr,
				  int *iBuf,
				  int minSpx);
static WlzGMVertex 		*WlzMatchICPLoopTMaxMinCurv2D(
				  WlzGMLoopT *gLT,
				  WlzDVertex2 *gNr,
				  int minEdg,
				  int maxFlg,
				  double *dstAngle);
static WlzErrorNum 		WlzMatchICPRemoveVerticies(
				  WlzMatchICPCbData *cbData,
				  WlzGMModel *sGM,
				  int *iBuf,
				  int nD);
static int			WlzMatchICPShellSzCmp(
				  const void *val0,
				  const void *val1);
static void			WlzMatchICPShellCb(
				  WlzGMModel *model,
				  WlzGMElemP elm,
				  WlzGMCbReason reason,
				  void *data);
static WlzMatchICPShellList *	WlzMatchICPShellListNew(
				  void);
static WlzMatchICPShellListElm *WlzMatchICPShellListElmNew(
				  void);
static void			WlzMatchICPShellListElmFree(
				  WlzMatchICPShellListElm *elm);
static void			WlzMatchICPShellListFree(
				  WlzMatchICPShellList *list);
static void			WlzMatchICPShellListElmInsert(
				  WlzMatchICPShellList *list,
				  WlzMatchICPShellListElm *nElm);
static void			WlzMatchICPShellListElmAppend(
				  WlzMatchICPShellList *list,
				  WlzMatchICPShellListElm *nElm);
static void			WlzMatchICPShellListElmUnlink(
				  WlzMatchICPShellList *list,
				  WlzMatchICPShellListElm *elm);

/*!
* \return				Error code.
* \ingroup	WlzTransform
* \brief	Finds matching points in two objects using an ICP
*		based matching algorithm.
*		If either the target or source object is a contour
*		then it's model is modified in place. If this isn't
*		wanted then use WlzCopyObj() to copy the contours
*		before calling this function.
* \param	tObj			The target object.
* \param	sObj			The source object to be
*					matched with target object.
* \param	initTr			Initial affine transform
*					to be applied to the source
*					object prior to matching. May
*					be NULL.
* \param	dstNMatch		Destination pointer for the number
*					of match points found. Required.
* \param	dstTMatch		Destination pointer for the target
*					match points found. Required.
* \param	dstSMatch		Destination pointer for the source
*					match points found. Required.
* \param	maxItr			Maximum number of iterations.
* \param	minSpx			Minimum number of simplicies in
*					a contour shell for matching.
* \param	brkFlg			Controls the breaking of the source
*					shells. Possible values are:
*					  - 0
*					    Whole model registration, source
*					    shells are never broken.
*					  - 1
*					    Whole source shells are
*					    registered to the target model.
*					    Source shells are never broken.
*					  - 2
*					    Source shells are only ever
*					    broken by connectivity.
*					  - > 2
*					    Source shells are broken by
*					    connectivity and then near
*					    there midpoints brkFlg - 1
*					    times.
* \param	dbgFlg			Aid debugging by transforming the
*					(possibly) broken source shells if 
*					non-zero.
*/
WlzErrorNum	WlzMatchICPObjs(WlzObject *tObj, WlzObject *sObj,
				WlzAffineTransform *initTr,
				int *dstNMatch, WlzVertexP *dstTMatch,
				WlzVertexP *dstSMatch, int maxItr, int minSpx,
				int brkFlg, int dbgFlg)
{
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if((tObj == NULL) | (sObj == NULL))
  {
    errNum = WLZ_ERR_OBJECT_NULL;
  }
  else if (tObj->type != sObj->type)
  {
    errNum = WLZ_ERR_OBJECT_TYPE;
  }
  else if((tObj->domain.core == NULL) || (sObj->domain.core == NULL))
  {
    errNum = WLZ_ERR_DOMAIN_NULL;
  }
  else if((dstNMatch == NULL) || (dstTMatch == NULL) || (dstSMatch == NULL))
  {
    errNum = WLZ_ERR_PARAM_NULL;
  }
  else
  {
    switch(tObj->type)
    {
      case WLZ_CONTOUR:
	if((tObj->domain.ctr->model == NULL) ||
	   (sObj->domain.ctr->model == NULL))
	{
	  errNum = WLZ_ERR_DOMAIN_NULL;
	}
	else if(tObj->domain.ctr->model->type !=
		sObj->domain.ctr->model->type)
	{
	  errNum = WLZ_ERR_DOMAIN_TYPE;
	}
	else
	{
	  switch(tObj->domain.ctr->model->type)
	  {
	    case WLZ_GMMOD_2I: /* FALLTHROUGH */
	    case WLZ_GMMOD_2D: /* FALLTHROUGH */
	    case WLZ_GMMOD_3I: /* FALLTHROUGH */
	    case WLZ_GMMOD_3D:
	      errNum = WlzMatchICPCtr(tObj->domain.ctr, sObj->domain.ctr,
	      			      initTr, maxItr, minSpx,
				      dstNMatch, dstTMatch, dstSMatch,
				      brkFlg, dbgFlg);
	      break;
	    default:
	      errNum = WLZ_ERR_DOMAIN_TYPE;
	      break;
	  }
	}
        break;
      default:
        break;
    }
  }
  return(errNum);
}

/*!
* \return				Error code.
* \ingroup	WlzTransform
* \brief	Establishes matching points in two 2D contours using ICP
*		based registration algorithm. The contours have been
*		checked so that their models are neither NULL or
*		of different types.
*		The source contour's model is modified in place.
* \param	tCtr			The target contour.
* \param	sCtr			The source contour to be
*					matched with target contour.
* \param	initTr			Initial affine transform
*					to be applied to the source
*					object prior to matching. May
*					be NULL.
* \param	maxItr			Maximum number of iterations.
* \param	minSpx			Minimum number of simplicies in
*					a contour shell for matching.
* \param	dstNMatch		Destination pointer for the number
*					of match points found.
* \param	dstTMatch		Destination pointer for the target
*					match points found.
* \param	dstSMatch		Destination pointer for the source
*					match points found.
* \param	brkFlg			Controls the breaking of the source
*					shells. Possible values are:
*					  - 0
*					    Whole model registration, source
*					    shells are never broken.
*					  - 1
*					    Whole source shells are
*					    registered to the target model.
*					    Source shells are never broken.
*					  - 2
*					    Source shells are only ever
*					    broken by connectivity.
*					  - > 2
*					    Source shells are broken by
*					    connectivity and then near
*					    there midpoints brkFlg - 1
*					    times.
* \param	dbgFlg			Aid debugging by transforming the
*					(possibly) broken source shells if 
*					non-zero.
*/
static WlzErrorNum  WlzMatchICPCtr(WlzContour *tCtr, WlzContour *sCtr,
				   WlzAffineTransform *initTr,
				   int maxItr, int minSpx,
				   int *dstNMatch, WlzVertexP *dstTMatch,
				   WlzVertexP *dstSMatch,
				   int brkFlg, int dbgFlg)
{
  int		idS,
		n0,
		n1,
  		nTV,
  		nSV,
		nOSS,
		maxVI,
		maxTVI,
		maxSVI,
		vSz,
		brkIdx,
		convFlg,
		nMatch = 0;
  int		*vIBuf = NULL;
  double	*wBuf = NULL;
  WlzGMModel	*tGM,
  		*sGM;
  WlzVertexP	tVx,
		tNr,
  		sVx,
		sNr,
		tVBuf,
		sVBuf;
  WlzVertexType	vType,
  		tVType;
  WlzTransformType trType;
  AlcKDTTree	*tTree = NULL;
  WlzAffineTransform *tTr,
  		*globTr = NULL;
  WlzGMShell 	*cSS;
  WlzMatchICPShell *sMS,
  		*sMSBuf = NULL;
  WlzMatchICPShellListElm *lElm0,
  		*lElm1;
  WlzMatchICPShellList *bSList = NULL,
		*rSList = NULL,
		*vSList = NULL;
  WlzMatchICPCbData cbData;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  tVBuf.v = sVBuf.v = NULL;
  tVx.v = tNr.v = sVx.v = sNr.v = NULL;
  /* Get the geometric models from the contours. */
  tGM = tCtr->model;
  sGM = sCtr->model;
  /* Remove small shells from the models. */
  errNum = WlzGMFilterRmSmShells(tGM, minSpx);
  if(errNum == WLZ_ERR_NONE)
  {
    errNum = WlzGMFilterRmSmShells(sGM, minSpx);
  }
  /* Get the verticies and normals from the target and source models. */
  if(errNum == WLZ_ERR_NONE)
  {
    tVx = WlzVerticiesFromCtr(tCtr, &tNr, NULL, &nTV, &tVType, &errNum);
  }
  if(errNum == WLZ_ERR_NONE)
  {
    sVx = WlzVerticiesFromCtr(sCtr, &sNr, NULL, &nSV, &vType, &errNum);
    if(tVType != vType)
    {
      errNum = WLZ_ERR_DOMAIN_TYPE;
    }
    else if((nTV <= 0) || (nSV <= 0))
    {
      errNum = WLZ_ERR_DOMAIN_DATA;
    }
    else
    {
      switch(vType)
      {
        case WLZ_VERTEX_D2:
	  vSz = sizeof(WlzDVertex2);
	  trType = WLZ_TRANSFORM_2D_AFFINE;
	  break;
	case WLZ_VERTEX_D3:
	  vSz = sizeof(WlzDVertex3);
	  trType = WLZ_TRANSFORM_3D_AFFINE;
	  break;
        default:
	  errNum = WLZ_ERR_DOMAIN_DATA;
	  break;
      }
    }
  }
  /* Create some buffers.
   * vIBuf:	A source vertex index buffer to allow registration to be
   *            between the target (using the kd-tree) and the indexed
   *		source verticies.
   * tVBuf:	Temporary buffer for target verticies.
   * sVBuf:	Temporary buffer for source verticies.
   * wBuf:	Temporary for nearest neighbour vertex weights.
   * sMSBuf:	Buffer with source shell pointers, shell sizes and affine
   *            transforms.
   */
  if(errNum == WLZ_ERR_NONE)
  {
    nOSS = sGM->res.shell.numElm;
    maxTVI = tGM->res.vertex.numIdx;
    maxSVI = sGM->res.vertex.numIdx;
    maxVI = WLZ_MAX(maxSVI, maxTVI);
    if(((vIBuf = (int *)AlcMalloc(maxVI * sizeof(int))) == NULL) ||
       ((tVBuf.v = AlcMalloc(maxVI * vSz)) == NULL) ||
       ((sVBuf.v = AlcMalloc(maxVI * vSz)) == NULL) ||
       ((wBuf = (double *)AlcMalloc(maxVI * sizeof(double))) == NULL) ||
       ((sMSBuf = (WlzMatchICPShell *)
       		  AlcCalloc(nOSS, sizeof(WlzMatchICPShell))) == NULL))
    {
      errNum = WLZ_ERR_MEM_ALLOC;
    }
  }
  /* Create empty lists.
   * bSList:	List of shells broken from one of the original source shells.
   * rSList:	List of broken source shells registered with the target model.
   * vSList:    Temporary list of broken shells.
   */
  if(errNum == WLZ_ERR_NONE)
  {
    if(((bSList = WlzMatchICPShellListNew()) == NULL) ||
       ((rSList = WlzMatchICPShellListNew()) == NULL) ||
       ((vSList = WlzMatchICPShellListNew()) == NULL))
    {
      errNum = WLZ_ERR_MEM_ALLOC;
    }
  }
  /* Build a kD-tree from the verticies of the the model. */
  if(errNum == WLZ_ERR_NONE)
  {
    tTree = WlzVerticiesBuildTree(vType, nTV, tVx, vIBuf, &errNum);
  }
  /* Register the verticies of the source model to those of the target. */
  if(errNum == WLZ_ERR_NONE)
  {
    globTr = WlzAssignAffineTransform(
    	     WlzMatchICPRegModel(tTree, trType, vType, nTV, tVx, tNr,
				 nSV, sVx, sNr, vIBuf, tVBuf, sVBuf, wBuf,
				 maxItr, initTr, &convFlg, &errNum), NULL);
    if((errNum == WLZ_ERR_NONE) && (convFlg == 0))
    {
      errNum = WLZ_ERR_ALG_CONVERGENCE;
    }
  }
  /* Get the source shells and their sizes. Then sort by size, the sort is
   * currently a debuging aid but may be used in future for constraining the
   * matching of small shells. */
  if((errNum == WLZ_ERR_NONE) && (brkFlg > 0))
  {
    sMS = sMSBuf;
    cSS = sGM->child;
    do
    {
      sMS->shell = cSS;
      sMS->size = WlzGMShellSimplexCnt(cSS);
      ++sMS;
      cSS = cSS->next;
    } while(cSS != sGM->child);
    idS = 0;
    qsort(sMSBuf, nOSS, sizeof(WlzMatchICPShell), WlzMatchICPShellSzCmp);
    /* Match each source shell to the target model starting with the largest
     * shell. */
    sMS = sMSBuf;
    while((errNum == WLZ_ERR_NONE) && (idS < nOSS))
    {
      tTr = WlzAssignAffineTransform(
            WlzMatchICPRegShell(tTree, sMS->shell, trType, vType,
			        nTV, tVx, tNr, nSV, sVx, sNr,
			        vIBuf, tVBuf, sVBuf, wBuf,
			        maxItr, globTr, &convFlg, &errNum), NULL);
      if((errNum == WLZ_ERR_NONE) && (convFlg == 1))
      {
	sMS->tr = tTr;
      }
      else
      {
	/* Whole source shell failed to register with the target model. */
	sMS->tr = NULL;
	WlzFreeAffineTransform(tTr);
      }
      ++sMS;
      ++idS;
    }
  }
  /* Remove all high connectivity verticies from the shells and then
   * register each of the child shells, with size above the threshold
   * to the target model. The list of new shells is built in  a list:
   * This is bSList if brkFlg > 2 or rSList if brkFlg == 2. */
  if((errNum == WLZ_ERR_NONE) && (brkFlg > 1))
  {
    cbData.nNS = cbData.nFS = 0;
    cbData.errNum = WLZ_ERR_NONE;
    cbData.list = vSList;
    errNum = WlzGMModelAddResCb(sGM, (WlzGMCbFn )WlzMatchICPShellCb,
				&cbData);
  }
  if((errNum == WLZ_ERR_NONE) && (brkFlg > 1))
  {
    idS = 0;
    sMS = sMSBuf;
    while((errNum == WLZ_ERR_NONE) && (idS < nOSS))
    {
      if(sMS->tr != NULL)
      {
	if(errNum == WLZ_ERR_NONE)
	{
	  errNum = WlzMatchICPBreakShellCon(&cbData, sGM, sMS->shell, vIBuf);
	}
	if(errNum == WLZ_ERR_NONE)
	{
	  if((lElm0 = WlzMatchICPShellListElmNew()) == NULL)
	  {
	    errNum = WLZ_ERR_MEM_ALLOC;
	  }
	  else
	  {
	    lElm0->mShell.size = sMS->size;
	    lElm0->mShell.shell = sMS->shell;
	    lElm0->mShell.tr = WlzAssignAffineTransform(sMS->tr, NULL);
	    WlzMatchICPShellListElmInsert(vSList, lElm0);
	    if(vSList->head->next != NULL)
	    {
	      /* Shell contained high connectivity verticies, so pass through
	       * the list, registering shells above the size threshold to the
	       * target model and removing the list elements of any small
	       * shells or shells that do not register. */
	      errNum = WlzMatchICPRegShellLst(tTree, sGM, vSList,
		  trType, vType, nTV, tVx, tNr, nSV, sVx, sNr,
		  vIBuf, tVBuf, sVBuf, wBuf, maxItr, minSpx,
		  sMS->tr);
	    }
	  }
	}
	if(errNum == WLZ_ERR_NONE)
	{
	  WlzMatchICPShellListInsertList(bSList, vSList);
	  vSList->head = vSList->tail = NULL;
	}
      }
      else
      {
	/* Whole source shell failed to register with the target model. */
	WlzFreeAffineTransform(tTr);
      }
      ++sMS;
      ++idS;
    }
    WlzGMModelRemResCb(sGM, WlzMatchICPShellCb, &cbData);
  }
  /* Make sure that all simplex topology elements are connected through
   * next/prev. There's no need to re-register the shell. */
  if((errNum == WLZ_ERR_NONE) && (brkFlg > 1))
  {
    lElm0 = bSList->head;
    while(lElm0 && (errNum == WLZ_ERR_NONE))
    {
      cSS = lElm0->mShell.shell;
      if(cSS->child != cSS->child->next)
      {
        errNum = WlzMatchICPBreakShellLoopTs(sGM, cSS, sNr, minSpx, 0);
      }
      lElm0 = lElm0->next;
    }
  }
  /* Now have a list of shells each of which has: Greater than the threshold
   * number of simplicies, is affine registered to the target model and has
   * no regions of high connectivity.
   * Repeatedly sweep through the broken shell list (bSList) further breaking
   * the shells. When a shell has less than the treshold number of simplicies
   * or it fails to register with the target model then it is removed
   * from the list. When a shell is not broken by breaking at points of
   * low curvature which are at least the threshold size away from a
   * boundary then it is added to the resgistered shell list (rSList). */
  if(errNum == WLZ_ERR_NONE)
  {
    if(brkFlg > 2)
    {
      if(errNum == WLZ_ERR_NONE)
      {
	errNum = WlzGMModelAddResCb(sGM, (WlzGMCbFn )WlzMatchICPShellCb,
				    &cbData);
      }
      if(errNum == WLZ_ERR_NONE)
      {
	n0 = -1;
	brkIdx = 2;
	do
	{
	  n1 = n0;
	  n0 = 0;
	  lElm0 = bSList->head;
	  while((errNum == WLZ_ERR_NONE) && (lElm0 != NULL))
	  {
	    lElm1 = lElm0->next;
	    /* Unlink the list element from the others in the list. */
	    WlzMatchICPShellListElmUnlink(bSList, lElm0);
	    /* Break the list element's shell. */
	    cbData.nNS = cbData.nFS = 0;
	    cbData.errNum = WLZ_ERR_NONE;
	    cbData.list = vSList;
	    errNum = WlzMatchICPBreakShellMid(&cbData, sGM, &(lElm0->mShell),
					      sNr, vIBuf, minSpx);
	    n0 += cbData.nNS;
	    if(errNum == WLZ_ERR_NONE)
	    {
	      if(vSList->head == NULL)		                  /* n0 == 0 */
	      {
		/* Shell was not broken so move it to the registered broken
		 * shells list. */
		WlzMatchICPShellListElmAppend(rSList, lElm0);
	      }
	      else
	      {
		/* Shell was broken into many shells, so pass through the
		 * list, registering shells above the size threshold to the
		 * target model and removing the list elements of any small
		 shells or shells that do not register. */
		WlzMatchICPShellListElmInsert(vSList, lElm0);
		errNum = WlzMatchICPRegShellLst(tTree, sGM, vSList,
				    trType, vType,
				    nTV, tVx, tNr, nSV, sVx, sNr,
				    vIBuf, tVBuf, sVBuf, wBuf, maxItr, minSpx,
				    lElm0->mShell.tr);
		if(errNum == WLZ_ERR_NONE)
		{
		  /* Merge the new list with the existing broken shell list
		   * taking care that the new list elements are before any
		   * of the existing list elements. */
		  WlzMatchICPShellListInsertList(bSList, vSList);
		  vSList->head = vSList->tail = NULL;
		}
	      }
	    }
	    lElm0 = lElm1;
	  }
	} while((errNum == WLZ_ERR_NONE) && (n1 != n0) && (++brkIdx < brkFlg));
	WlzGMModelRemResCb(sGM, WlzMatchICPShellCb, &cbData);
      }
    }
    else if(brkFlg > 0)
    {
      if(brkFlg == 1)
      {
	idS = 0;
	sMS = sMSBuf;
	while((errNum == WLZ_ERR_NONE) && (idS < nOSS))
	{
	  if(sMS->tr)
	  {
	    if((lElm0 = WlzMatchICPShellListElmNew()) == NULL)
	    {
	      errNum = WLZ_ERR_MEM_ALLOC;
	    }
	    else
	    {
	      WlzMatchICPShellListElmInsert(bSList, lElm0);
	      lElm0->mShell.size = sMS->size;
	      lElm0->mShell.shell = sMS->shell;
	      lElm0->mShell.tr = WlzAssignAffineTransform(sMS->tr, NULL);
	    }
	  }
	  ++sMS;
	  ++idS;
	}
      }
      if(errNum == WLZ_ERR_NONE)
      {
	WlzMatchICPShellListInsertList(rSList, bSList);
	bSList = NULL;
      }
    }
  }
  /* Now have list of broken shells which are registered to the target model.
   * Find match points on broken shells and target model. */
  if((errNum == WLZ_ERR_NONE) && (brkFlg > 1))
  {
    n0 = 0;
    lElm0 = rSList->head;
    while((errNum == WLZ_ERR_NONE) && (lElm0 != NULL)) 
    {
      if(lElm0->mShell.shell && (lElm0->mShell.size > minSpx) &&
         lElm0->mShell.tr)
      {
	n1 = WlzMatchICPGetPoints(tTree, &(lElm0->mShell), vType, tVx,
				  tNr, sNr, tVBuf, sVBuf, n0, &errNum);
	n0 += n1;
      }
      lElm0 = lElm0->next;
    }
    nMatch = n0;
  }
  /* This stuff is just for debugging. */
  if((errNum == WLZ_ERR_NONE) && dbgFlg)
  {
    /* Remove unregistered source shells. */
    if((brkFlg > 2) && (n1 == n0))
    {
      lElm0 = bSList->head;
      while(lElm0 != NULL) 
      {
	WlzGMModelDeleteS(sGM, lElm0->mShell.shell);
	lElm0 = lElm0->next;
      }
    }
    else if(brkFlg > 1)
    {
      WlzMatchICPShellListInsertList(rSList, bSList);
      bSList->head = bSList->tail = NULL;
    }
    /* Transform the registered source shells. */
    if(brkFlg > 0)
    {
      lElm0 = rSList->head;
      while((errNum == WLZ_ERR_NONE) && (lElm0 != NULL)) 
      {
	if(lElm0->mShell.size < minSpx)
	{
	  WlzGMModelDeleteS(sGM, lElm0->mShell.shell);
	}
	else
	{
	  WlzAffineTransformGMShell(lElm0->mShell.shell, lElm0->mShell.tr);
	}
	lElm0 = lElm0->next;
      }
    }
    else
    {
      (void )WlzAffineTransformGMModel(sGM, globTr, 0, &errNum);
    }
  }
  /* Free the temporary storage that's been allocated. */
  if((errNum == WLZ_ERR_NONE) && (nMatch > 0))
  {
    /* Reallocate the match points. */
    if(((sVBuf.v = AlcRealloc(sVBuf.v, nMatch * vSz)) == NULL) ||
       ((tVBuf.v = AlcRealloc(tVBuf.v, nMatch * vSz)) == NULL))
    {
      errNum = WLZ_ERR_MEM_ALLOC;
    }
  }
  if((errNum != WLZ_ERR_NONE) || (nMatch <= 0))
  {
    AlcFree(tVBuf.v);
    AlcFree(sVBuf.v);
    sVBuf.v = NULL;
    tVBuf.v = NULL;
  }
  if(errNum == WLZ_ERR_NONE)
  {
    if(dstNMatch)
    {
      *dstNMatch = nMatch;
    }
    if(dstSMatch)
    {
      *dstSMatch = sVBuf;
    }
    if(dstTMatch)
    {
      *dstTMatch = tVBuf;
    }
  }
  if(bSList)
  {
    lElm0 = bSList->head;
    while(lElm0)
    {
      lElm1 = lElm0->next;
      (void )WlzFreeAffineTransform(lElm0->mShell.tr);
      lElm0 = lElm1;
    }
    WlzMatchICPShellListFree(bSList);
  }
  if(rSList)
  {
    lElm0 = rSList->head;
    while(lElm0)
    {
      lElm1 = lElm0->next;
      (void )WlzFreeAffineTransform(lElm0->mShell.tr);
      lElm0 = lElm1;
    }
    WlzMatchICPShellListFree(rSList);
  }
  AlcFree(sMSBuf);
  AlcFree(vIBuf);
  AlcFree(wBuf);
  AlcFree(tVx.v);
  AlcFree(tNr.v);
  AlcFree(sVx.v);
  AlcFree(sNr.v);
  WlzFreeAffineTransform(globTr);
  return(errNum);
}

/*!
* \return				Error code.
* \ingroup	WlzTransform
* \brief	Pass through the given shell list, computing and setting
*		shell sizes within the list, registering shells all
*		above the size threshold to the target model and removing
*		the list elements of any small shells or shells that do not
*		register.
* \param	tTree			Given kD-tree populated by the
*					target verticies such that the
*					nodes of the tree have the same
*					indicies as the given target verticies
*					and normals.
* \param	sGM			Source geometric model.
* \param	sLst			The given list of shells.
* \param	trType			The required type of transform,
*					must be either WLZ_TRANSFORM_2D_REG,
*					or WLZ_TRANSFORM_2D_AFFINE.
* \param	vType			Type of vertices.
* \param	nTV			Number of target verticies.
* \param	tVx			The target verticies.
* \param	tNr			The target normals.
* \param        nSV			Number of source verticies.
* \param        sVx 			The source verticies.
* \param	sNr			The source normals.
*		iBuf			Buffer with at least nSV ints.
* \param	tVBuf			A buffer with room for at least
*					nS verticies.
*		sVBuf			A buffer with room for at least
*					nS verticies.
* \param	wBuf			A buffer with room for at least
*					nS doubles.
* \param	maxItr			Maximum number of iterations.
* \param	minSpx			Minimum number of simplicies within a
* 					shell.
* \param	initTr			Initial affine transform, may be NULL.
*/
static WlzErrorNum	WlzMatchICPRegShellLst(AlcKDTTree *tTree,
				WlzGMModel *sGM,
				WlzMatchICPShellList *sLst,
				WlzTransformType trType, WlzVertexType vType,
				int nTV, WlzVertexP tVx, WlzVertexP tNr,
				int nSV, WlzVertexP sVx, WlzVertexP sNr,
				int *iBuf, WlzVertexP tVBuf, WlzVertexP sVBuf,
				double *wBuf, int maxItr, int minSpx,
				WlzAffineTransform *initTr)
{
  int		convFlg,
  		remFlg;
  WlzMatchICPShellListElm *lElm0,
  		*lElm1;
  WlzAffineTransform *tTr;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  lElm0 = sLst->head;
  while((errNum == WLZ_ERR_NONE) && (lElm0 != NULL))
  {
    tTr = NULL;
    remFlg = 1;
    lElm0->mShell.size = WlzGMShellSimplexCnt(lElm0->mShell.shell);
    if(lElm0->mShell.size >= minSpx)
    {
      tTr = WlzAssignAffineTransform(
	    WlzMatchICPRegShell(tTree, lElm0->mShell.shell,
			        trType, vType,
			        nTV, tVx, tNr, nSV, sVx, sNr,
			        iBuf, tVBuf, sVBuf, wBuf,
			        maxItr, initTr, &convFlg, &errNum),
	    NULL);
      remFlg = !convFlg;
    }
    if(errNum == WLZ_ERR_NONE)
    {
      lElm1 = lElm0->next;
      if(remFlg)
      {
	/* Either this shell is too small or it has failed to
	 * register with the target model: Remove the element from
	 * the list. */
	WlzMatchICPShellListElmUnlink(sLst, lElm0);
	WlzGMModelDeleteS(sGM, lElm0->mShell.shell);
	WlzMatchICPShellListElmFree(lElm0);
	WlzFreeAffineTransform(tTr);
      }
      else
      {
	/* Shell is larger than threshold size and has been
	 * registered with the target model. */
	lElm0->mShell.tr = tTr;
      }
      lElm0 = lElm1;
    }
  }
  return(errNum);
}

/*!
* \return				void
* \ingroup	WlzTransform
* \brief	Inserts list1 at the head of list0.
* \param	list0			Pointer to the list of shells.
* \param 	list1			List of shells
*/
static void	WlzMatchICPShellListInsertList(WlzMatchICPShellList *list0,
					       WlzMatchICPShellList *list1)
{
  if(list0 && list1)
  {
    if(list0->head == NULL)
    {
      list0->head = list1->head;
      list0->tail = list1->tail;
    }
    else if(list1->tail)
    {
      list0->head->prev = list1->tail;
      list1->tail->next = list0->head;
      list0->head = list1->head;
    }
  }
}

/*!
* \return				Affine transform found, NULL
*					on error.
* \ingroup	WlzTransform
* \brief	Establishes an affine transfrom matching the source
*		model to the target model. The target model's verticies
*		are in an already populated kD-tree in the workspace.
* \param	tTree			Given kD-tree populated by the
*					target verticies such that the
*					nodes of the tree have the same
*					indicies as the given target verticies
*					and normals.
* \param	trType			The required type of transform,
*					must be either WLZ_TRANSFORM_2D_REG,
*					or WLZ_TRANSFORM_2D_AFFINE.
* \param	vType			Type of vertices.
* \param	nTV			Number of target verticies.
* \param	tVx			The target verticies.
* \param	tNr			The target normals.
* \param        nSV			Number of source verticies.
* \param        sVx 			The source verticies.
* \param	sNr			The source normals.
*		iBuf			Buffer with at least nSV ints.
* \param	tVBuf			A buffer with room for at least
*					nS verticies.
*		sVBuf			A buffer with room for at least
*					nS verticies.
* \param	wBuf			A buffer with room for at least
*					nS doubles.
* \param	maxItr			Maximum number of iterations.
* \param	initTr			Initial affine transform, may be NULL.
* \param	dstConv			Destination pointer for a
*					convergence flag which is set to
*					a non zero value if the registration
*					converges.
* \param	dstErr			Destination error pointer,
*					may be NULL.
*/
static WlzAffineTransform *WlzMatchICPRegModel(AlcKDTTree *tTree,
				WlzTransformType trType, WlzVertexType vType,
				int nTV, WlzVertexP tVx, WlzVertexP tNr,
				int nSV, WlzVertexP sVx, WlzVertexP sNr,
				int *iBuf, WlzVertexP tVBuf, WlzVertexP sVBuf,
				double *wBuf, int maxItr, 
				WlzAffineTransform *initTr, int *dstConv,
				WlzErrorNum *dstErr)
{
  int		idV,
  		conv = 0;
  WlzAffineTransform *tr = NULL;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  for(idV = 0; idV < nSV; ++idV)
  {
    *(iBuf + idV) = idV;
  }
  tr = WlzRegICPTreeAndVerticies(tTree, trType, vType,
  			     nTV, tVx, tNr, nSV, iBuf, sVx, sNr,
			     tVBuf, sVBuf, wBuf, maxItr, initTr,
			     &conv, &errNum);
  if(dstConv)
  {
    *dstConv = conv;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(tr);
}

/*!
* \return				Affine transform found, NULL
*					on error.
* \ingroup	WlzTransform
* \brief	Establishes an affine transfrom matching the source
*		shell to the target model. The target model's verticies
*		are in an already populated kD-tree.
* \param	tTree			Given kD-tree populated by the
*					target verticies such that the
*					nodes of the tree have the same
*					indicies as the given target verticies
*					and normals.
* \param	sS			Source shell.
* \param	trType			The required type of transform,
*					must be either WLZ_TRANSFORM_2D_REG,
*					or WLZ_TRANSFORM_2D_AFFINE.
* \param	vType			Type of vertices.
* \param	nTV			Number of target verticies.
* \param	tVx			The target verticies.
* \param	tNr			The target normals.
* \param        nSV			Number of source verticies.
* \param        sVx 			The source verticies.
* \param	sNr			The source normals.
*		iBuf			Buffer with at least nSV ints.
* \param	tVBuf			A buffer with room for at least
*					nS verticies.
*		sVBuf			A buffer with room for at least
*					nS verticies.
* \param	wBuf			A buffer with room for at least
*					nS doubles.
* \param	maxItr			Maximum number of iterations.
* \param	initTr			Initial affine transform, may be NULL.
* \param	dstConv			Destination pointer for a
*					convergence flag which is set to
*					a non zero value if the registration
*					converges.
* \param	dstErr			Destination error pointer,
*					may be NULL.
*/
static WlzAffineTransform *WlzMatchICPRegShell(AlcKDTTree *tTree,
				WlzGMShell *sS,
				WlzTransformType trType, WlzVertexType vType,
				int nTV, WlzVertexP tVx, WlzVertexP tNr,
				int nSV, WlzVertexP sVx, WlzVertexP sNr,
				int *iBuf, WlzVertexP tVBuf, WlzVertexP sVBuf,
				double *wBuf, int maxItr, 
				WlzAffineTransform *initTr, int *dstConv,
				WlzErrorNum *dstErr)
{
  int		nSSV,
  		conv = 0;
  WlzGMVertex	*v0;
  WlzGMVertexT	*vT0;
  WlzGMDiskT	*dT0;
  WlzGMEdgeT	*cET,
  		*fET;
  WlzGMLoopT	*cLT,
  		*fLT;
  WlzAffineTransform *tr = NULL;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  /* Find the indicies of the source shell's verticies. */
  nSSV = 0;
  cLT = fLT = sS->child;
  do
  {
    cET = fET = cLT->edgeT;
    do
    {
      vT0 = cET->vertexT;
      dT0 = vT0->diskT;
      v0 = dT0->vertex;
      if((v0->diskT == dT0) && (dT0->vertexT == vT0))
      {
	*(iBuf + nSSV++) = v0->idx;
      }
      cET = cET->next;
    } while(cET != fET);
    cLT = cLT->next;
  } while(cLT != fLT);
  /* Register the source shell's verticies with the target model using the
   * existing kD-tree. */
  tr = WlzRegICPTreeAndVerticies(tTree, trType, vType,
  			  nTV, tVx, tNr, nSSV, iBuf, sVx, sNr,
			  tVBuf, sVBuf, wBuf,
			  maxItr, initTr, &conv, &errNum);
  if(dstConv)
  {
    *dstConv = conv;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(tr);
}

/*!
* \return				Number of matched points.
* \ingroup	WlzTransform
* \brief	Computes matching points from the broken source shells
*		and their affine transforms.
* \param	tTree			kD-tree populated by the target
*					verticies.
* \param	mS			Match shell.
* \param	vType			Type of vertices.
* \param	tVx			Target vertex geometries.
* \param 	tNr			Target normals.
* \param 	sNr			Source normals.
* \param	tMatch			Allocated space for the target
*					match verticies.
* \param	sMatch			Allocated space for the source
*					match verticies.
* \param	offset			Offset into tMatch and smatch.
* \param	dstErr			Destination error pointer,
*					may be NULL.
*/
static int	WlzMatchICPGetPoints(AlcKDTTree *tTree, WlzMatchICPShell *mS,
				     WlzVertexType vType, WlzVertexP tVx,
				     WlzVertexP tNr, WlzVertexP sNr,
				     WlzVertexP tMatch, WlzVertexP sMatch,
				     int offset, WlzErrorNum *dstErr)
{
  int		nMatch = 0;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  switch(vType)
  {
    case WLZ_VERTEX_D2:
      nMatch = WlzMatchICPGetPoints2D(tTree, mS, tVx.d2, tNr.d2, sNr.d2,
      				      tMatch.d2 + offset, sMatch.d2 + offset);
      break;
    case WLZ_VERTEX_D3: /* FALLTHROUGH */
    default:
      errNum = WLZ_ERR_UNIMPLEMENTED;
      break;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(nMatch);
}

/*!
* \return				Number of matched points.
* \ingroup	WlzTransform
* \brief	Computes matching points from the broken source shell
*		it's assosiated affine transform and the target kD-tree.
* 		Finds the 2D vertex at the given shell's midpoint, applies
*		the affine transform associated with the shell in the match
*		shell data structure to the verticies geometry then finds the
*		closest point in the target model to the transformed vertex
*		using the target kD-tree.
* \param	tTree			kD-tree populated by the target
*					verticies.
* \param	mS			Match shell.
* \param	tVx			Target vertex geometries.
* \param 	tNr			Target normals.
* \param 	sNr			Source normals.
* \param	tMatch			Allocated space for the target
*					match vertex.
* \param	sMatch			Allocated space for the source
*					match vertex.
*/
static int	WlzMatchICPGetPoints2D(AlcKDTTree *tTree, WlzMatchICPShell *mS,
				       WlzDVertex2 *tVx, 
				       WlzDVertex2 *tNr, WlzDVertex2 *sNr,
				       WlzDVertex2 *tMatch, WlzDVertex2 *sMatch)
{
  int		nMatch = 0;
  WlzGMShell	*sS;
  WlzGMVertex	*cV;
  WlzDVertex2	sV,
		tV,
  		tSV;
  double	cDist;
  double	vxD2[2];
  AlcKDTNode	*tNode;
  const int	minSpx = 5; /* Avoid end regions of shells. */
  const double	maxDist = 5.0; /* TODO There's some experimentation to be
  				* done in choosing the value. */

  if(mS && ((sS = mS->shell) != NULL) && (sS->child == sS->child->next))
  {
    cV = WlzMatchICPLoopTMaxMinCurv2D(sS->child, sNr, minSpx, 1, NULL);
    (void )WlzGMVertexGetG2D(cV, &sV);
    tSV = WlzAffineTransformVertexD2(mS->tr, sV, NULL);
    vxD2[0] = tSV.vtX;
    vxD2[1] = tSV.vtY;
    tNode = AlcKDTGetNN(tTree, vxD2, maxDist, &cDist, NULL);
    if(tNode && (cDist <= maxDist))
    {
      tV = *(tVx + tNode->idx);
      *(sMatch) = sV;
      *(tMatch) = tV;
      nMatch = 1;
    }
  }
  return(nMatch);
}

/*!
* \return				Error code.
* \ingroup	WlzTransform
* \brief	Find all elements within the given shell which have
*		high connectivity and remove them, while at the same
*		time building a list of the shells that are derived from
*		the given shell.
* \param	cbData			Callback data structure in which
*					the list of new shells is being built.
* \param	sGM			Source model.
* \param	sS			Source shell.
* \param	iBuf			Buffer containing the indicies of
*					verticies to be removed from the
*					model.
*/
static WlzErrorNum	WlzMatchICPBreakShellCon(WlzMatchICPCbData *cbData,
				WlzGMModel *sGM, WlzGMShell *sS, int *iBuf)
{
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  switch(sGM->type)
  {
    case WLZ_GMMOD_2I: /* FALLTHROUGH */
    case WLZ_GMMOD_2D:
      errNum = WlzMatchICPBreakShellCon2D(cbData, sGM, sS, iBuf);
      break;
    case WLZ_GMMOD_3I: /* FALLTHROUGH */
    case WLZ_GMMOD_3D:
      errNum = WLZ_ERR_UNIMPLEMENTED;
      break;
    default:
      errNum = WLZ_ERR_DOMAIN_TYPE;
      break;
  }
  return(errNum);
}

/*!
* \return				Error code.
* \ingroup	WlzTransform
* \brief	Find all verticies within the given shell which have
*		high connectivity and remove them, while at the same
*		time building a list of the shells that are derived from
*		the given shell.
* \param	cbData			Callback data structure in which
*					the list of new shells is being built.
* \param	sGM			Source model.
* \param	sS			Source shell.
* \param	iBuf			Buffer containing the indicies of
*					verticies to be removed from the
*					model.
*/
static WlzErrorNum WlzMatchICPBreakShellCon2D(WlzMatchICPCbData *cbData,
				WlzGMModel *sGM, WlzGMShell *sS, int *iBuf)
{
  int		nHCV;
  WlzGMVertex	*v0;
  WlzGMVertexT	*vT0;
  WlzGMDiskT	*dT0;
  WlzGMEdgeT	*cET,
  		*fET;
  WlzGMLoopT	*cLT,
  		*fLT;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  /* Find all verticies with high connectivity that are in the given
   * shell. */
  nHCV = 0;
  cLT = fLT = sS->child;
  do
  {
    cET = fET = cLT->edgeT;
    do
    {
      vT0 = cET->vertexT;
      /* Test for high connectivity */
      if((vT0 != vT0->next) && (vT0 != vT0->next->next))
      {
	dT0 = vT0->diskT;
	v0 = dT0->vertex;
	/* Only delete a vertex once. */
	if((v0->diskT == dT0) && (dT0->vertexT == vT0))
	{
	  *(iBuf + nHCV++) = v0->idx;
	}
      }
      cET = cET->next;
    } while(cET != fET);
    cLT = cLT->next;
  } while(cLT != fLT);
  /* Remove all the high connectivity that were found. */
  if(nHCV > 0)
  {
    errNum = WlzMatchICPRemoveVerticies(cbData, sGM, iBuf, nHCV);
  }
  return(errNum);
}

/*!
* \return				Error code.
* \ingroup	WlzTransform
* \brief	Break the given shell by removing verticies near the
*		midpoint of the shell. . The given shell is known to only
*		have simple connectivity, eg a shell with simple connectivity
*		in a 2D model will no more than two edges using a single
*		vertex.
* \param	cbData			Callback data structure in which
*					the list of new shells is being built.
* \param	sGM			Source model.
* \param	sMS			The match shell.
* \param	sNr			The source shell normals.
* \param	iBuf			Buffer containing the indicies of
*					verticies to be removed from the
*					model.
* \param	minSpx			Minimum number of simplicies to
*					leave in a shell.
*/
static WlzErrorNum	WlzMatchICPBreakShellMid(WlzMatchICPCbData *cbData,
				WlzGMModel *sGM, WlzMatchICPShell *sMS,
				WlzVertexP sNr, int *iBuf, int minSpx)
{
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  switch(sGM->type)
  {
    case WLZ_GMMOD_2I: /* FALLTHROUGH */
    case WLZ_GMMOD_2D:
      /* If the shell has more than one loop topology element then it is
       * a closed loop so this needs to be broken into a simple non-cyclic
       * chain of edge segments before it can be broken by removing low
       * curvature verticies. */
      errNum = WlzMatchICPBreakShellCur2D(cbData, sGM, sMS, sNr.d2,
      					  iBuf, minSpx);
      break;
    case WLZ_GMMOD_3I: /* FALLTHROUGH */
    case WLZ_GMMOD_3D:
      errNum = WLZ_ERR_UNIMPLEMENTED;
      break;
    default:
      errNum = WLZ_ERR_DOMAIN_TYPE;
      break;
  }
  return(errNum);
}

/*!
* \return				Error code.
* \ingroup	WlzTransform
* \brief	Make sure that all simplex topology elements are connected
*		through the next and prev pointers without recourse to the
*		opp pointer for connectivity.
* \param	sGM			Source model.
* \param	sS			Given shell.
* \param	sNr			Source normals.
* \param	minSpx			Minimum number of simplicies to
*					leave in a shell.
* \param	maxFlg			Search is for maximum curvature
*					if non-zero.
*/
static WlzErrorNum	WlzMatchICPBreakShellLoopTs(WlzGMModel *sGM,
					WlzGMShell *sS, WlzVertexP sNr,
					int minSpx, int maxFlg)
{
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  switch(sGM->type)
  {
    case WLZ_GMMOD_2I: /* FALLTHROUGH */
    case WLZ_GMMOD_2D:
      errNum = WlzMatchICPBreakShellLop2D(sGM, sS, sNr.d2, minSpx, maxFlg);
      break;
    case WLZ_GMMOD_3I: /* FALLTHROUGH */
    case WLZ_GMMOD_3D:
      errNum = WLZ_ERR_UNIMPLEMENTED;
      break;
    default:
      errNum = WLZ_ERR_DOMAIN_TYPE;
      break;
  }
  return(errNum);

}

/*!
* \return				Woolz error code.
* \ingroup	WlzTransform
* \brief	Make sure that all simplex topology elements are connected
*		through the next and prev pointers without recourse to the
*		opp pointer for connectivity. In 2D this means that a shell
*		with no high connectivity verticies has two opposite loop
*		topology elements. This is fixed by removing the lowest
*		curvature vertex.
* \param	sGM			Source model.
* \param	sS			Given shell.
* \param	sNr			Source normals.
* \param	minSpx			Minimum number of simplicies to
*					leave in a shell.
* \param	maxFlg			Search is for maximum curvature
*					if non-zero.
*/
static WlzErrorNum	WlzMatchICPBreakShellLop2D(WlzGMModel *sGM,
					WlzGMShell *sS, WlzDVertex2 *sNr,
					int minSpx, int maxFlg)
{
  WlzGMVertex	*sV;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  /* Find the vertex which is at the point of the lowest curvature in a loop
   * topology element of the shell. */
  sV = WlzMatchICPLoopTMaxMinCurv2D(sS->child, sNr, minSpx, maxFlg, NULL);
  errNum = WlzGMModelDeleteV(sGM, sV);
  return(errNum);
}

/*!
* \return				Error code.
* \ingroup	WlzTransform
* \brief	Look for the vertex at which the curvature is localy maximal
*		for each of the shells. Remove all the verticies found from
*		the 2D source model.
* \param	cbData			Callback data structure in which
*					the list of new shells is being built.
* \param	sGM			Source model.
* \param	sMS			The match shell with pointers to the
*					shell and it's number of simplicies.
* \param	sNr			Source normals.
* \param	iBuf			Buffer containing the indicies of
*					verticies to be removed from the
*					model.
* \param	minSpx			Minimum number of simplicies to
*					leave in a shell.
*/
static WlzErrorNum WlzMatchICPBreakShellCur2D(WlzMatchICPCbData *cbData,
				WlzGMModel *sGM, WlzMatchICPShell *sMS,
				WlzDVertex2 *sNr, int *iBuf, int minSpx)
{
  int		idD;
  WlzGMVertex	*sV;
  WlzGMLoopT	*sLT;
  WlzGMShell	*sS;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  /* Find the vertex which has the minimum weighted curvature,
   * where the weighting is biased towards the centre of an open
   * edge chain. */
  idD = 0;
  sS = sMS->shell;
  sLT = sS->child;
  do
  {
    if((sMS->size > (minSpx + 1) * 2) &&
       ((sV = WlzMatchICPLoopTMaxMinCurv2D(sLT, sNr, minSpx,
       				           0, NULL)) != NULL))
    {
      *(iBuf + idD++) = sV->idx;
    }
    sLT = sLT->next;
  } while(sLT != sS->child);
  if(idD > 0)
  {
    errNum = WlzMatchICPRemoveVerticies(cbData, sGM, iBuf, idD);
  }
  return(errNum);
}

/*!
* \return				Vertex at point of weighted
*					minimum or maximum curvature.
* \brief	Given a loop topology element and an array of normals
*		for the verticies of the loop topology element. Finds
*		the vertex at the point of maximum or minimum curvature
*		weighted towards the centre of the loop.
* 		The actual curvature is computed using the given
*		normals, with two consecutive vertecies
*		\f$mathbf{v_0}\f$ and \f$\mathbf{v_1}\f$ having normals 
*		\f$mathbf{n_0}\f$ and \f$\mathbf{n_1}\f$, their
*		curvature is given by:
*		  \f[c = \frac{1}{2}{(1 + cos\theta)}\f]
*		where:
*		  \f[cos\theta = \frac{\mathbf{n_0} \cdot \mathbf{n_1}}
                                      {|\mathbf{n_0}| |\mathbf{n_1}|}\f]
*		Because the normals are all unit normals this simplifies
*		to:
*		\f[cos\theta = \mathbf{n_0} \cdot \mathbf{n_1}\f]
*		For maximum curvature look for low values of \f$cos\theta)\f$
*		and for minimum curvature look for high values.
*		The weighting towards the centre of an open chain is
*		done using a simple quadratic:
*		  \f[w(x) = 4\frac{x}{l}(1 - frac{x}{l})\f]
* \param	gLT			Given loop topology element.
* \param	gNr			Given vertex normals.
* \param	minEdg			Minimum number of edges to
*					skip at the terminal ends of an
*					loop topology element that is
*					an open edge chain.
* \param	maxFlg			Search is for maximum curvature
*					if non-zero.
* \param	dstAngle		Destination pointer for maximum
*					of minimum angle found.
*/
static WlzGMVertex *WlzMatchICPLoopTMaxMinCurv2D(WlzGMLoopT *gLT,
					WlzDVertex2 *gNr, int minEdg,
                                        int maxFlg, double *dstAngle)
{
  int		idN,
  		idP,
		len = 0,
		wxFlg,
		okFlg = 1;
  double	tD0,
		wx,
		wA,
		wB,
		wC,
		cur,
		mSgn,
		mCur,
  		cosAng;
  WlzGMEdgeT	*fET,
		*lET,
  		*tET0,
		*tET1,
		*mET,
		*nET,
		*pET;
  WlzGMVertex	*mV = NULL;
  WlzDVertex2	n0,
  		n1;


  /* Find the first and last edge topology elements between which to search
   * for the vertex at the centre of a maximum or minimum curvature region. */
  if(gLT != gLT->next)
  {
    wxFlg = 0;           /* Don't use weighting if the edge chain is closed. */
    fET = gLT->edgeT;
    /* Count the edge topology elements. */
    tET0 = fET;
    while(((nET = tET0->next) != tET0->opp) && (nET != fET))
    {
      ++len;
      tET0 = nET;
    }
    if(len <= (2 * minEdg))
    {
      okFlg = 0;
    }
  }
  else
  {
    wxFlg = 1; 		       /* Use weighting if the edge chain is closed. */
    /* The given loop topology element is the only one in the shell, find
     * it's end point. */
    tET0 = gLT->edgeT;
    while(((pET = tET0->prev) != tET0->opp) && (pET != gLT->edgeT))
    {
      tET0 = pET;
    }
    /* Go forwards minEdg edge topology elements to find the first
     * edge topology element. */
    idN = minEdg;
    while(((nET = tET0->next) != tET0->opp) && (idN-- > 0))
    {
      tET0 = nET;
    }
    if(idN == 0)
    {
      okFlg = 0;
    }
    else
    {
      fET = tET0;
      /* Find the other end point, counting the edge topology elements while
       * traveling. */
      while(((nET = tET0->next) != tET0->opp) && (nET != pET))
      {
	++len;
	tET0 = nET;
      }
      if(nET == pET)
      {
	okFlg = 0;
      }
    }
  }
  if(okFlg)
  {
    /* Go back minEdg edge topology elements to find the last edge topology
     * element. */
    len -= minEdg;
    idN = minEdg;
    while(((pET = tET0->prev) != fET) && (idN-- > 0))
    {
      tET0 = pET;
    }
    if(idN == 0)
    {
      okFlg = 0;
    }
  }
  if(okFlg)
  {
    lET = tET0;
  }
  /* Having found the first and last edge topology elements between which to
   * search, now do the search for the vertex at the point of minimal or
   * maximal curvature. With the curvature defined by cur = mSgn * cosAng
   * and mSgn = 1.0 for maximal curvature and -1.0 for minimal curvature.
   * A quadratic weighting function is used (when the contour is an open
   * edge chain). This has the form \f$ w(x) = ax^2 + bx + c \f$, with
   * \f$ a = -0.4/N^2, b = 0.4/ N, c = 0.9 \f$.
   */
  if(okFlg)
  {
    mSgn = maxFlg? -1.0: 1.0;
    wA = (mSgn * -0.10) / (len  * len);
    wB = (mSgn * 0.10) / len;
    wC = 0.90;
    idN = 0;
    mCur = -2.0;
    tET1 = mET = fET;
    n1 = *(gNr + tET1->vertexT->diskT->vertex->idx);
    while((nET = tET1->next) != lET)
    {
      tET0 = tET1;
      n0 = n1;
      tET1 = nET;
      n1 = *(gNr + tET1->vertexT->diskT->vertex->idx);
      cosAng = fabs(WLZ_VTX_2_DOT(n0, n1));
      cur = mSgn * cosAng;
      if(wxFlg)
      {
	wx = (wA * idN * idN) + (wB * idN) + wC;
	cur *= wx;
      }
      if(cur > mCur)
      {
	mCur = cur;
	mET = tET0;
      }
      ++idN;
    }
    /* Having found the maximal or minimal curvature, now compute the
     * angle. */
    mV = mET->vertexT->diskT->vertex;
    n0 = *(gNr + mV->idx);
    n1 = *(gNr + mET->next->vertexT->diskT->vertex->idx);
    cosAng = WLZ_VTX_2_DOT(n0, n1);
    if(dstAngle)
    {
      if(cosAng < -1.0)
      {
        *dstAngle = ALG_M_PI;
      }
      else if(cosAng > 1.0)
      {
        *dstAngle = 0.0;
      }
      else
      {
        *dstAngle = acos(cosAng);
      }
    }
  }
  return(mV);
}

/*!
* \return				Error code.
* \ingroup	WlzTransform
* \brief	Removes all verticies in the workspace's vertex flags
*		buffer from the source model.
* \param	cbData			Callback data structure in which
*					the list of new shells is being built.
* \param	sGM			Source model.
* \param	iBuf			Buffer containing the indicies of
*					verticies to be removed from the
*					model.
* \param	nD			Number of verticies to be removed.
*/
static WlzErrorNum WlzMatchICPRemoveVerticies(WlzMatchICPCbData *cbData,
				WlzGMModel *sGM, int *iBuf, int nD)
{
  int		idD;
  AlcVector   	*vec;
  WlzGMVertex	*sV;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  /* Remove all these high connectivity verticies from the model creating
   * new shells. Make sure that the transform of each of these new shells
   * is a copy of the transform of the shell with the high connectivity
   * vertex. */
  idD = 0;
  vec = sGM->res.vertex.vec;
  while((errNum == WLZ_ERR_NONE) && (idD < nD))
  {
    sV = (WlzGMVertex *)AlcVectorItemGet(vec, *(iBuf + idD));
    if(sV && (sV->idx >= 0))
    {
      errNum = WlzGMModelDeleteV(sGM, sV);
      if(errNum == WLZ_ERR_NONE)
      {
	errNum = cbData->errNum;
      }
    }
    ++idD;
  }
  return(errNum);
}

/*!
* \return				Comparison value.
* \ingroup	WlzTransform
* \brief	Compare match shell sizes for qsort. Sorted shells will
*		have the largest shells first and the smallest shells
*		last.
* \param	val0			Used to pass first match shell.
* \param	val1			Used to pass second match shell.
*/
static int	WlzMatchICPShellSzCmp(const void *val0, const void *val1)
{
  int 		cmp;

  cmp = ((WlzMatchICPShell *)val1)->size - ((WlzMatchICPShell *)val0)->size;
  return(cmp);
}

/*!
* \return	<void>
* \ingroup	WlzTransform
* \brief	Callback function which is called by the Woolz geometric model
*		functions when a new element has been created or is about to
*		be free'd. This function builds a list of new shells and
*		keeps count of both the number of new and freed shells.
* \param	model			The geometric model.
* \param	elm			The element that's been created or
*					is about to be destroyed.
* \param	reason			The reason for the callback.
* \param	data			Used to pass the callback data
*					structure with the list of new shells.
*/
static void	WlzMatchICPShellCb(WlzGMModel *model, WlzGMElemP elm,
			           WlzGMCbReason reason, void *data)
{
  WlzMatchICPShellListElm *tLElm,
  			  *nLElm;
  WlzMatchICPCbData *nSCb;

  if(elm.core && (elm.core->type == WLZ_GMELM_SHELL))
  {
    nSCb = (WlzMatchICPCbData *)data;
    if(reason == WLZ_GMCB_NEW)
    {
      if((nLElm = WlzMatchICPShellListElmNew()) == NULL)
      {
        nSCb->errNum = WLZ_ERR_MEM_ALLOC;
      }
      else
      {
	++(nSCb->nNS);
	nLElm->mShell.shell = elm.shell;
	WlzMatchICPShellListElmAppend(nSCb->list, nLElm);
      }
    }
    else
    {
      /* Check that the shell that's about to be deleted isn't in the list
       * that's being built. If it is then remove it before it's deleted. */
      tLElm = nSCb->list->head;
      while(tLElm)
      {
        nLElm = tLElm->next;
	if(elm.shell == tLElm->mShell.shell)
	{
	  /* Need to remove the list entry before the shell is deleted. */
          WlzMatchICPShellListElmUnlink(nSCb->list, tLElm);
	  WlzMatchICPShellListElmFree(tLElm);
	}
	tLElm = nLElm;
      }
      ++(nSCb->nFS);
    }
  }
}

/*!
* \return				New shell match list or NULL on error.
* \ingroup	WlzTransform
* \brief	Allocate a new match list.
* \param	void
*/
static WlzMatchICPShellList *WlzMatchICPShellListNew(void)
{
  WlzMatchICPShellList *nList;

  nList  = (WlzMatchICPShellList *)
           AlcCalloc(1, sizeof(WlzMatchICPShellList));
  return(nList);
}

/*!
* \return				New shell match list element or NULL
*					on error.
* \ingroup	WlzTransform
* \brief	Allocate a new match list element.
* \param	void
*/
static WlzMatchICPShellListElm *WlzMatchICPShellListElmNew(void)
{
  WlzMatchICPShellListElm *nLElm;

  nLElm  = (WlzMatchICPShellListElm *)
           AlcCalloc(1, sizeof(WlzMatchICPShellListElm));
  return(nLElm);
}

/*!
* \return				New shell match list element or NULL
*					on error.
* \ingroup	WlzTransform
* \brief	Free's a unlinked match list element.
* \param	void
*/
static void	WlzMatchICPShellListElmFree(WlzMatchICPShellListElm *elm)
{
  AlcFree(elm);
}

/*!
* \return				New shell match list element or NULL
*					on error.
* \ingroup	WlzTransform
* \brief	Free's a whole list of match list elements along with the
*		list itself. Transforms and shells held in the list are NOT
*		freed.
* \param	void
*/
static void	WlzMatchICPShellListFree(WlzMatchICPShellList *list)
{
  WlzMatchICPShellListElm *elm0,
  		*elm1;

  elm0 = list->head;
  while(elm0)
  {
    elm1 = elm0->next;
    AlcFree(elm0);
    elm0 = elm1;
  }
  AlcFree(list);
}


/*!
* \return	void
* \ingroup	WlzTransform
* \brief	Inserts a new element onto the head of the match shell list.
* \param	list			Match shell list.
* \param	nElm			New match shell list element.
*/
static void	WlzMatchICPShellListElmInsert(WlzMatchICPShellList *list,
					WlzMatchICPShellListElm *nElm)
{
  if(list && nElm)
  {
    if(list->head)
    {
      list->head->prev = nElm;
      nElm->next = list->head;
      nElm->prev = NULL;
      list->head = nElm;
    }
    else
    {
      list->head = list->tail = nElm;
    }
  }
}

/*!
* \return	void
* \ingroup	WlzTransform
* \brief	Appends a new element onto the end of the match shell list.
* \param	list			Match shell list.
* \param	nElm			New match shell list element.
*/
static void	WlzMatchICPShellListElmAppend(WlzMatchICPShellList *list,
					WlzMatchICPShellListElm *nElm)
{
  if(list && nElm)
  {
    if(list->tail)
    {
      list->tail->next = nElm;
      nElm->prev = list->tail;
      nElm->next = NULL;
      list->tail = nElm;
    }
    else
    {
      list->head = list->tail = nElm;
    }
  }
}

/*!
* \return	void
* \ingroup	WlzTransform
* \brief	Unlinks the given element from the list of shells.
* \param	list			List of match shells.
* \param 	elm			Element of the list of match shells
*					to be unlinked.
*/
static void	WlzMatchICPShellListElmUnlink(WlzMatchICPShellList *list,
					  WlzMatchICPShellListElm *elm)
{
  if(list && elm)
  {
    if(elm->prev)
    {
      elm->prev->next = elm->next;
    }
    else
    {
      list->head = elm->next;
    }
    if(elm->next)
    {
      elm->next->prev = elm->prev;
    }
    else
    {
      list->tail = elm->prev;
    }
    elm->prev = elm->next = NULL;
  }
}

/* #define WLZ_MATCHICP_TEST 1 */

#if WLZ_MATCHICP_TEST == 1
/* Test main() for WlzMatchICPObjs().
 * Match input objects, find match points and output decomposed source
 * contour.
 */

extern int	getopt(int argc, char * const *argv, const char *optstring);

extern char	*optarg;
extern int	optind,
		opterr,
		optopt;

int             main(int argc, char *argv[])
{
  int           idx,
		brkFlg = INT_MAX,
		dbgFlg = 0,
		nMatch,
		maxItr = 200,
		minSpx = 100,
  		option,
  		ok = 1,
		usage = 0;
  FILE		*fP = NULL;
  char		*inTrFileStr,
  		*outFileStr,
		*outDbgFileStr;
  char		*inObjFileStr[2];
  WlzObject	*inTrObj;
  WlzObject	*inObj[2];
  WlzVertexP	matchTP,
  		matchSP;
  WlzErrorNum	errNum = WLZ_ERR_NONE;
  const char	*errMsg;
  static char	optList[] = "aghrb:d:D:o:t:i:s:";
  const char	outFileStrDef[] = "-",
  		inObjFileStrDef[] = "-";

  inTrObj = NULL;
  inObj[0] = inObj[1] = NULL;
  outFileStr = (char *)outFileStrDef;
  outDbgFileStr = NULL;
  inTrFileStr = NULL;
  inObjFileStr[0] = inObjFileStr[1] = (char *)inObjFileStrDef;
  matchTP.v = matchSP.v = NULL;
  while(ok && ((option = getopt(argc, argv, optList)) != -1))
  {
    switch(option)
    {
      case 'b':
        if((sscanf(optarg, "%d", &brkFlg) != 1) || (brkFlg < 0))
	{
	  usage = 1;
	  ok = 0;
	}
	break;
      case 'd':
	dbgFlg = 1;
        outDbgFileStr = optarg;
	break;
      case 'D':
        if((sscanf(optarg, "%d", &dbgFlg) != 1) ||
	   (dbgFlg < 0) || (dbgFlg > 2))
        {
	  usage = 1;
	  ok = 0;
	}
	break;
      case 'o':
        outFileStr = optarg;
	break;
      case 't':
        inTrFileStr = optarg;
	break;
      case 'i':
        if((sscanf(optarg, "%d", &maxItr) != 1) || (maxItr <= 0))
	{
	  usage = 1;
	  ok = 0;
	}
	break;
      case 's':
        if((sscanf(optarg, "%d", &minSpx) != 1) || (minSpx <= 0))
	{
	  usage = 1;
	  ok = 0;
	}
	break;
      case 'h':
        usage = 1;
	ok = 0;
	break;
      default:
        usage = 1;
	ok = 0;
	break;
    }
  }
  if(ok)
  {
    if((inObjFileStr == NULL) || (*inObjFileStr == '\0') ||
       (outFileStr == NULL) || (*outFileStr == '\0'))
    {
      ok = 0;
      usage = 1;
    }
    if(ok && (optind < argc))
    {
      if((optind + 2) != argc)
      {
        usage = 1;
	ok = 0;
      }
      else
      {
        inObjFileStr[0] = *(argv + optind);
        inObjFileStr[1] = *(argv + optind + 1);
      }
    }
  }
  if(ok)
  {
    idx = 0;
    while(ok && (idx < 2))
    {
      if((inObjFileStr[idx] == NULL) ||
	  (*inObjFileStr[idx] == '\0') ||
	  ((fP = (strcmp(inObjFileStr[idx], "-")?
		  fopen(inObjFileStr[idx], "r"): stdin)) == NULL) ||
	  ((inObj[idx] = WlzAssignObject(WlzReadObj(fP, &errNum),
	  				 NULL)) == NULL) ||
	  (errNum != WLZ_ERR_NONE))
      {
	ok = 0;
	(void )fprintf(stderr,
		       "%s: failed to read object from file %s\n",
		       *argv, inObjFileStr[idx]);
      }
      if(fP && strcmp(inObjFileStr[idx], "-"))
      {
	fclose(fP);
        fP = NULL;
      }
      ++idx;
    }
  }
  if(ok && inTrFileStr && (*inTrFileStr != '\0'))
  {
    if(((fP = (strcmp(inTrFileStr, "-")?
    	       fopen(inTrFileStr, "r"): stdin)) == NULL) ||
       ((inTrObj = WlzAssignObject(WlzReadObj(fP, &errNum),
       				   NULL)) == NULL) ||
       (errNum != WLZ_ERR_NONE))
    {
      ok = 0;
      (void )fprintf(stderr,
      		     "%s Failed to read the initial affine transform from\n"
      		     "file %s.\n",
		     *argv, inTrFileStr);
    }
    if(fP && strcmp(inTrFileStr, "-"))
    {
      fclose(fP);
      fP = NULL;
    }
    if(inTrObj &&
       ((inTrObj->type != WLZ_AFFINE_TRANS) || (inTrObj->domain.core == NULL)))
    {
      ok = 0;
      (void )fprintf(stderr,
      		     "%s: Initial affine transform object invalid type\n",
		     *argv);
    }
  }
  if(ok)
  {
    errNum = WlzMatchICPObjs(inObj[0], inObj[1],
    			     (inTrObj)? inTrObj->domain.t: NULL,
			     &nMatch, &matchTP, &matchSP,
			     maxItr, minSpx, brkFlg, dbgFlg > 1);
    if(errNum != WLZ_ERR_NONE)
    {
      ok = 0;
      (void )fprintf(stderr, "%s Failed to match contours.\n",
      		     argv[0]);
    }
  }
  if(ok)
  {
    if(outDbgFileStr && (dbgFlg > 0))
    {
      errNum = WLZ_ERR_WRITE_EOF;
      if(((fP = (strcmp(outDbgFileStr, "-")?
      	        fopen(outDbgFileStr, "w"):
		stdout)) == NULL))
      {
	ok = 0;
	(void )WlzStringFromErrorNum(errNum, &errMsg);
	(void )fprintf(stderr,
		       "%s: failed to open debug object "
		       "file %s (%s).\n",
		       *argv, outDbgFileStr, errMsg);
      }
      else if((errNum = WlzWriteObj(fP, inObj[1])) != WLZ_ERR_NONE)
      {
	ok = 0;
	(void )WlzStringFromErrorNum(errNum, &errMsg);
	(void )fprintf(stderr,
		       "%s: failed to write debug object "
		       "to file %s (%s).\n",
		       *argv, outDbgFileStr, errMsg);
      }
      if(fP && strcmp(outDbgFileStr, "-"))
      {
        (void )fclose(fP);
      }
    }
  }
  if(ok)
  {
    errNum = WLZ_ERR_WRITE_EOF;
    if(((fP = (strcmp(outFileStr, "-")?
              fopen(outFileStr, "w"):
	      stdout)) == NULL))
    {
      ok = 0;
      (void )WlzStringFromErrorNum(errNum, &errMsg);
      (void )fprintf(stderr,
      		     "%s: failed to open correspondences "
		     "file %s (%s).\n",
		     *argv, outFileStr, errMsg);
    }
    else
    {
      /* Output the correspondces: Assumed to be WlzDVertex2. */
      for(idx = 0; idx <nMatch; ++idx)
      {
        fprintf(fP, "%g %g %g %g\n",
		(matchSP.d2 + idx)->vtX, (matchSP.d2 + idx)->vtY,
		(matchTP.d2 + idx)->vtX - (matchSP.d2 + idx)->vtX,
		(matchTP.d2 + idx)->vtY - (matchSP.d2 + idx)->vtY);
      }
    }
    if(fP && strcmp(outFileStr, "-"))
    {
      (void )fclose(fP);
    }
  }
  AlcFree(matchTP.v);
  AlcFree(matchSP.v);
  (void )WlzFreeObj(inTrObj);
  (void )WlzFreeObj(inObj[0]);
  (void )WlzFreeObj(inObj[1]);
  if(usage)
  {
      (void )fprintf(stderr,
      "Usage: %s%s",
      *argv,
      " [-b#] [-d#] [-D#] [-o#] [-t#] [-i#] [-h]\n"
      "          [<input object 0>] [<input object 1>]\n"
      "Options:\n"
      "  -b  Shell breaking control value.\n"
      "  -d  Debug output file.\n"
      "  -D  Debug flag:\n"
      "        0  no debug output.\n"
      "        1  untransformed decomposed source model.\n"
      "        2  transformed decomposed source model.\n"
      "  -o  Output file name.\n"
      "  -t  Initial affine transform.\n"
      "  -i  Maximum number of iterations.\n"
      "  -s  Miimum number of simplicies.\n"
      "  -h  Prints this usage information.\n"
      "Reads a pair of contours and computes a set of correspondence points"
      "using an ICP based matching algorithm.\n");
  }
  return(!ok);
}
#endif /* WLZ_MATCHICP_TEST == 1 */

#if WLZ_MATCHICP_TEST == 2
/* Test main() for WlzMatchICPObjs().
 * Decompose the input contour. Each shell of the given contour is
 * decomposed.
 */

extern int	getopt(int argc, char * const *argv, const char *optstring);

extern char	*optarg;
extern int	optind,
		opterr,
		optopt;

int             main(int argc, char *argv[])
{
  int           idx,
  		sCount,
		maxCrv = 1,
  		minSpx = 100,
  		option,
  		ok = 1,
		usage = 0;
  FILE		*fP = NULL;
  char		*inObjFileStr,
		*outObjFileStr;
  WlzObject	*obj = NULL;
  WlzGMModel 	*model;
  WlzGMShell 	*shell;
  WlzGMShell	**allShells;
  WlzVertexP	sVx,
  		sNr;
  WlzErrorNum	errNum = WLZ_ERR_NONE;
  const char	*errMsg;
  static char	optList[] = "hnxo:s:";
  const char	inObjFileStrDef[] = "-",
  		outObjFileStrDef[] = "-";

  sVx.v = NULL;
  sNr.v = NULL;
  inObjFileStr = (char *)inObjFileStrDef;
  outObjFileStr = (char *)outObjFileStrDef;
  while(ok && ((option = getopt(argc, argv, optList)) != -1))
  {
    switch(option)
    {
      case 'o':
        outObjFileStr = optarg;
	break;
      case 's':
        if((sscanf(optarg, "%d", &minSpx) != 1) || (minSpx <= 0))
	{
	  usage = 1;
	  ok = 0;
	}
	break;
      case 'n':
        maxCrv = 0;
	break;
      case 'x':
        maxCrv = 1;
	break;
      case 'h':
        usage = 1;
	ok = 0;
	break;
      default:
        usage = 1;
	ok = 0;
	break;
    }
  }
  if(ok)
  {
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
  }
  if(ok)
  {
    if((inObjFileStr == NULL) || (*inObjFileStr == '\0') ||
       ((fP = (strcmp(inObjFileStr, "-")?
	       fopen(inObjFileStr, "r"): stdin)) == NULL) ||
       ((obj = WlzAssignObject(WlzReadObj(fP, &errNum), NULL)) == NULL) ||
       (errNum != WLZ_ERR_NONE))
    {
      ok = 0;
      (void )fprintf(stderr,
		     "%s: failed to read object from file %s\n",
		     *argv, inObjFileStr);
    }
    if(fP && strcmp(inObjFileStr, "-"))
    {
      fclose(fP);
      fP = NULL;
    }
  }
  if(ok)
  {
    if(obj->type != WLZ_CONTOUR)
    {
      errNum = WLZ_ERR_OBJECT_TYPE;
    }
    else if((obj->domain.core == NULL) ||
            ((model = obj->domain.ctr->model) == NULL))
    {
      errNum = WLZ_ERR_DOMAIN_NULL;
    }
    else
    {
      sVx = WlzVerticiesFromCtr(obj->domain.ctr, &sNr, NULL, NULL, NULL,
      				&errNum);
      if((errNum == WLZ_ERR_NONE) && (sVx.v == NULL))
      {
        errNum = WLZ_ERR_MEM_ALLOC;
      }
    }
    if(errNum == WLZ_ERR_NONE)
    {
      /* Make an  array of the existing shells. */
      sCount = 0;
      if((shell = model->child) != NULL)
      {
	do
	{
	  ++sCount;
	  shell = shell->next;
	} while(shell != model->child);
      }
      /* Fill the array and then decompose all the shells. */
      if(sCount > 0)
      {
        if((allShells = (WlzGMShell **)
		        AlcMalloc(sizeof(WlzGMShell *) * sCount)) == NULL)
	{
	  errNum = WLZ_ERR_MEM_ALLOC;
	}
	else
	{ 
	  idx = 0;
	  do
	  {
	    allShells[idx++] = shell;
	    shell = shell->next;
	  } while(shell != model->child);
	  idx = 0;
	  while((errNum == WLZ_ERR_NONE) && (idx < sCount))
	  {
	    errNum = WlzMatchICPBreakShellLoopTs(model, allShells[idx++], sNr,
	    				         minSpx, maxCrv);
	  }
	  AlcFree(allShells);
	}
      }
    }
    if(errNum != WLZ_ERR_NONE)
    {
      ok = 0;
      (void )fprintf(stderr, "%s Failed to decompose contour.\n",
      		     argv[0]);
    }
  }
  if(ok)
  {
    if((fP = (strcmp(outObjFileStr, "-")?
	      fopen(outObjFileStr, "w"): stdout)) == NULL)
    {
      ok = 0;
      errNum = WLZ_ERR_WRITE_EOF;
      (void )fprintf(stderr, "%s Failed to open output file %s.\n",
      		     argv[0], outObjFileStr);
    }
  }
  if(ok)
  {
    if((errNum = WlzWriteObj(fP, obj)) != WLZ_ERR_NONE)
    {
      ok = 0;
      (void )WlzStringFromErrorNum(errNum, &errMsg);
      (void )fprintf(stderr,
		     "%s: failed to write objec to file %s (%s).\n",
		     *argv, outObjFileStr, errMsg);
    }
    if(fP && strcmp(outObjFileStr, "-"))
    {
      (void )fclose(fP);
    }
  }
  (void )WlzFreeObj(obj);
  if(usage)
  {
      (void )fprintf(stderr,
      "Usage: %s%s",
      *argv,
      "  [-o#] [-s#] [-h] [-n] [-x] [<input object>]\n"
      "Options:\n"
      "  -o  Output file name.\n"
      "  -s  Minimum number of simplicies.\n"
      "  -h  Prints this usage information.\n"
      "  -n  Break at minimum curvature.\n"
      "  -x  Break at maximum curvature.\n"
      "Reads a contours and then decomposes each of it's shells.\n");
  }
  AlcFree(sVx.v);
  AlcFree(sNr.v);
  return(!ok);
}
#endif /* WLZ_MATCHICP_TEST == 2 */
