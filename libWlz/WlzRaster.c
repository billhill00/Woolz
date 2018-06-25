#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _WlzRaster_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         libWlz/WlzRaster.c
* \author       Bill Hill
* \date         March 2001
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
* \brief	Functions to rasterize geometric Woolz objects into 2D
* 		or 3D domain objects.
* \ingroup	WlzRaster
* \todo		Function WlzRasterGM2D() has yet to be implemented.
*/

#include <Wlz.h>
#include <limits.h>

static WlzObject 		*WlzRasterCtr(
				  WlzContour *ctr,
				  WlzIBox3 bBox,
			          WlzErrorNum *dstErr);
static WlzObject 		*WlzRasterGM(
				  WlzGMModel *model,
				  WlzIBox3 bBox,
				  WlzErrorNum *dstErr);
static WlzObject 		*WlzRasterGM2D(
				  WlzGMModel *model,
				  WlzIBox2 bBox,
			          WlzErrorNum *dstErr);
static WlzObject 		*WlzRasterGM3D(
				  WlzGMModel *model,
				  WlzIBox3 bBox,
			          WlzErrorNum *dstErr);
static WlzErrorNum 		WlzRasterAddSimplex3I(
				  WlzUByte ***bMsk,
				  WlzIVertex3 sz,
				  WlzIVertex3 org,
				  WlzIVertex3 *simplex);
static int 			WlzRasterVtxCmp3I(
				  const void *ptr0,
				  const void *ptr1);
static void			WlzRasterLine3I(
				  WlzUByte ***bMsk,
				  WlzIVertex3 *seg);
static void			WlzRasterSetVoxel(
				  WlzUByte ***bMsk,
				  WlzIVertex3 pos);

/*!
* \return	Woolz domain object, NULL on error.
* \ingroup	WlzRaster
* \brief	Rasterizes the given geometric object, creating a new
*               domain object.
* \param	gObj			Given geometric object.
* \param	dstErr			Destination ptr for error
*                                       code, may be NULL.
*/
WlzObject 	*WlzRasterObj(WlzObject *gObj, WlzErrorNum *dstErr)
{
  WlzObject	*dObj = NULL;
  WlzIBox3	bBox;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(gObj == NULL)
  {
    errNum = WLZ_ERR_OBJECT_NULL;
  }
  else if(gObj->domain.core == NULL)
  {
    errNum = WLZ_ERR_DOMAIN_NULL;
  }
  if(errNum == WLZ_ERR_NONE)
  {
    switch(gObj->type)
    {
      case WLZ_EMPTY_OBJ:
        dObj = WlzMakeEmpty(&errNum);
	break;
      case WLZ_CONTOUR:
	bBox = WlzBoundingBox3I(gObj, &errNum);
	if(errNum == WLZ_ERR_NONE)
	{
	  dObj = WlzRasterCtr(gObj->domain.ctr, bBox, &errNum);
	}
        break;
      default:
        errNum = WLZ_ERR_OBJECT_TYPE;
	break;
    }
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(dObj);
}

/*!
* \ingroup	WlzRaster
* \brief        Given an interval domain which has a single interval per
*               line, which covers the line range of the two given vertices,
*               this function updates the intervals by setting the left/right
*               fields for a line drawn between the two given vertices.
*               It may be used to set the intervals for any convex
*               polygon simply by initialising the intervals to always
*               false values (eg min = max of object and max = min of object)
*               then calling this function for each ordered pair of boundary
*               vertices.
*               Both vertices must be relative to idom->(kol1,line1).
* \param        iDom            Interval domain for which intervals
*                               are to be set.
* \param        v0              First vertex.
* \param        v1              Second vertex.
*/
void            WlzRasterLineSetItv2D(WlzIntervalDomain *iDom,
                                      WlzIVertex2 v0, WlzIVertex2 v1)
{
  int           i,
                e,
                steep = 1,
                t;
  WlzIVertex2   del,
                pos,
                step;
  WlzInterval   *itv;

#ifdef WLZ_RASTER_LINE_DEBUG
    (void )fprintf(stderr, "\n");
#endif
  del.vtX = v1.vtX - v0.vtX;
  del.vtY = v1.vtY - v0.vtY;
  step.vtX = ((del.vtX > 0) << 1) - 1;
  step.vtY = ((del.vtY > 0) << 1) - 1;
  del.vtX = abs(del.vtX);
  del.vtY = abs(del.vtY);
  if(del.vtY > del.vtX)
  {
    steep = 0;
    t = v0.vtX; v0.vtX = v0.vtY; v0.vtY = t;
    t = del.vtX; del.vtX = del.vtY; del.vtY = t;
    t = step.vtX; step.vtX = step.vtY; step.vtY = t;
  }
  e = (del.vtY << 1) - del.vtX;
  for(i = 0; i < del.vtX; i++)
  {
    if(steep)
    {
      pos = v0;
    }
    else
    {
      pos.vtX = v0.vtY;
      pos.vtY = v0.vtX;
    }
    itv = (iDom->intvlines + pos.vtY)->intvs;
    if(itv->ileft > pos.vtX)
    {
      itv->ileft = pos.vtX;
    }
    if(itv->iright < pos.vtX)
    {
      itv->iright = pos.vtX;
    }
#ifdef WLZ_RASTER_LINE_DEBUG
    (void )fprintf(stderr, "%d %d\n", pos.vtX, pos.vtY);
#endif
    while(e >= 0)
    {
      v0.vtY += step.vtY;
      e -= (del.vtX << 1);
    }
    v0.vtX += step.vtX;
    e += (del.vtY << 1);
  }
}

/*!
* \return	Woolz domain object, NULL on error.
* \ingroup	WlzRaster
* \brief	Rasterizes the given contour object, creating a new
*               domain object.
* \param	ctr			Given contour.
* \param	bBox			Bounding box of the model.
* \param	dstErr			Destination ptr for error code, may be
* 					NULL.
*/
static WlzObject *WlzRasterCtr(WlzContour *ctr,
			       WlzIBox3 bBox, WlzErrorNum *dstErr)
{
  WlzObject	*dObj = NULL;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(ctr->model == NULL)
  {
    errNum = WLZ_ERR_DOMAIN_DATA;
  }
  else
  {
    dObj = WlzRasterGM(ctr->model, bBox, &errNum);
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(dObj);
}

/*!
* \return	Woolz domain object, NULL on error.
* \ingroup	WlzRaster
* \brief	Rasterizes the given geometric model, creating a new
*               domain object.
* \param	model			Given geometric model.
* \param	bBox			Bounding box of the model.
* \param	dstErr			Destination ptr for error
*                                       code, may be NULL.
*/
static WlzObject *WlzRasterGM(WlzGMModel *model, 
			      WlzIBox3 bBox, WlzErrorNum *dstErr)
{
  WlzObject	*dObj = NULL;
  WlzErrorNum	errNum = WLZ_ERR_NONE;
  WlzIBox2	bBox2;

  if(model->child == NULL)
  {
    dObj = WlzMakeEmpty(&errNum);
  }
  else
  {
    switch(model->type)
    {
      case WLZ_GMMOD_2I: /* FALLTHROUGH */
      case WLZ_GMMOD_2D: /* FALLTHROUGH */
      case WLZ_GMMOD_2N:
       bBox2.xMin = bBox.xMin;
       bBox2.yMin = bBox.yMin;
       bBox2.xMax = bBox.xMax;
       bBox2.yMax = bBox.yMax;
       dObj = WlzRasterGM2D(model, bBox2, &errNum);
       break;
      case WLZ_GMMOD_3I: /* FALLTHROUGH */
      case WLZ_GMMOD_3D: /* FALLTHROUGH */
      case WLZ_GMMOD_3N:
       dObj = WlzRasterGM3D(model, bBox, &errNum);
       break;
      default:
        errNum = WLZ_ERR_DOMAIN_TYPE;
	break;
    }
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(dObj);
}
/*!
* \return	Woolz domain object, NULL on error.
* \ingroup	WlzRaster
* \brief	Rasterizes the given 2D geometric model, creating a new
*               domain object.
* \param	model			Given 2D geometric model.
* \param	bBox			Bounding box of the model.
* \param	dstErr			Destination ptr for error
*                                       code, may be NULL.
*/
static WlzObject *WlzRasterGM2D(WlzGMModel *model,
			        WlzIBox2 bBox, WlzErrorNum *dstErr)
{
  WlzObject	*dObj = NULL;
  WlzErrorNum	errNum = WLZ_ERR_UNIMPLEMENTED;

  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(dObj);
}

/*!
* \return	Woolz domain object, NULL on error.
* \ingroup	WlzRaster
* \brief	Rasterizes the given 3D geometric model, creating a new
*               domain object.
* \param	model			Given 3D geometric model.
* \param	bBox			Bounding box of the model.
* \param	dstErr			Destination ptr for error
*                                       code, may be NULL.
*/
static WlzObject *WlzRasterGM3D(WlzGMModel *model,
			        WlzIBox3 bBox, WlzErrorNum *dstErr)
{
  int		vIdx,
		fETIdx,
  		fLTIdx;
  WlzIVertex3	org,
		sz;
  WlzGMEdgeT	*tET;
  WlzGMLoopT	*tLT;
  WlzGMShell	*lS,
  		*nS,
		*tS;
  char		*lFlg = NULL;
  WlzObject	*dObj = NULL;
  WlzUByte	***dAry = NULL;
  WlzDVertex3	*vDP;
  WlzIVertex3	vBuf[3];
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  org.vtX = bBox.xMin; org.vtY = bBox.yMin; org.vtZ = bBox.zMin; 
  sz.vtX = bBox.xMax - bBox.xMin + 1;
  sz.vtY = bBox.yMax - bBox.yMin + 1;
  sz.vtZ = bBox.zMax - bBox.zMin + 1;
  if(((lFlg = (char *)AlcCalloc(model->res.face.numIdx,
  				sizeof(char))) == NULL) ||
     (AlcBit3Calloc(&dAry, sz.vtZ, sz.vtY, sz.vtX) != ALC_ER_NONE))
  {
    errNum = WLZ_ERR_MEM_ALLOC;
  }
  if(errNum == WLZ_ERR_NONE)
  {
    /* For each shell. */
    tS = lS = model->child;
    nS = tS->next;
    do
    {
      tS = nS;
      nS = nS->next;
      tLT = tS->child;
      fLTIdx = tLT->idx;
      /* For each loopT. */
      do
      {
	tLT = tLT->next;
	/* For each face, there are two loopT's per face make sure we only
	 * rasterize each face once. */
        if(*(lFlg + tLT->face->idx) == 0)
	{
	  /* For each edgeT of the loopT add it's vertex to the simplex
	   * vertex buffer. */
	  vIdx = 0;
	  tET = tLT->edgeT;
	  fETIdx = tET->idx;
	  if(model->type == WLZ_GMMOD_3I)
	  {
	    do
	    {
	      vBuf[vIdx] = tET->vertexT->diskT->vertex->geo.vg3I->vtx;
	      tET = tET->next;
	    } while((tET->idx != fETIdx) && (++vIdx <= 2));
	  }
	  else /* model->type == WLZ_GMMOD_3D || WLZ_GMMOD_3N */
	  {
	    do
	    {
	      vDP = &(tET->vertexT->diskT->vertex->geo.vg3D->vtx);
	      vBuf[vIdx].vtX = WLZ_NINT(vDP->vtX);
	      vBuf[vIdx].vtY = WLZ_NINT(vDP->vtY);
	      vBuf[vIdx].vtZ = WLZ_NINT(vDP->vtZ);
	      tET = tET->next;
	    } while((tET->idx != fETIdx) && (++vIdx <= 2));
	  }
	  if(vIdx > 2)
	  {
	    /* Just incase something other than a simplex get's used! */
	    errNum = WLZ_ERR_DOMAIN_DATA;
	  }
	  else
	  {
	    /* Rasterize the simplex. */
	    errNum = WlzRasterAddSimplex3I(dAry, sz, org, vBuf);
	  }
	}
      } while((errNum == WLZ_ERR_NONE) && (tLT->idx != fLTIdx));
    } while((errNum == WLZ_ERR_NONE) && (tS->idx != lS->idx));
  }
  if(errNum == WLZ_ERR_NONE)
  {
    dObj = WlzFromArray3D((void ***)dAry, sz, org, WLZ_GREY_BIT, WLZ_GREY_BIT,
    			  0.0, 1.0, 0, 0, &errNum);
  }
  if(dAry)
  {
    Alc3Free((void ***)dAry);
  }
  if(lFlg)
  {
    AlcFree(lFlg);
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(dObj);
}

/*!
* \return	+ve, -ve or zero.
* \ingroup	WlzRaster
* \brief	Compares two 3D integral verticies for AlgSort().
* \param	ptr0			First pointer.
* \param	ptr1			Second pointer.
*/
static int 	WlzRasterVtxCmp3I(const void *ptr0, const void *ptr1)
{
  int		cmp;
  WlzIVertex3	*vtx0,
  		*vtx1;
  
  vtx0 = (WlzIVertex3 *)ptr0;
  vtx1 = (WlzIVertex3 *)ptr1;
  if((cmp = vtx1->vtZ - vtx0->vtZ) == 0)
  {
    if((cmp = vtx1->vtY - vtx0->vtY) == 0)
    {
      cmp = vtx1->vtX - vtx0->vtX;
    }
  }
  return(cmp);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzRaster
* \brief	Rasterizes the given 3D simplex into the given 3D
*               bit array by setting bits on the face of the simplex.
* \param	bMsk			The 3D bitmask to be written to.
* \param	sz			Size of the bit map array.
* \param	org			Origin of the bitmap array.
* \param	simplex			The coordinates of the verticies
*                                       of the simplex.
*/
static WlzErrorNum WlzRasterAddSimplex3I(WlzUByte ***bMsk, WlzIVertex3 sz,
					 WlzIVertex3 org, WlzIVertex3 *simplex)
{
  int		idN,
  		fstSeg;
  int		cnt[2],
  		errXY[2],
		errXZ[2],
		errZY[2];
  WlzIVertex3	seg[2],
  		abs[2],
		del[2],
		sgn[2],
		inc[2];
  WlzErrorNum	errNum = WLZ_ERR_NONE;

#ifdef WLZ_RASTER_DEBUG
  fprintf(stderr, "simplex (%d %d %d), (%d %d %d), (%d %d %d)\n",
          (simplex + 0)->vtX, (simplex + 0)->vtY, (simplex + 0)->vtZ,
          (simplex + 1)->vtX, (simplex + 1)->vtY, (simplex + 1)->vtZ,
          (simplex + 2)->vtX, (simplex + 2)->vtY, (simplex + 2)->vtZ);
#endif /* WLZ_RASTER_DEBUG */
  /* Make the simplex relative to the origin. */
  for(idN = 0; idN < 3; ++idN)
  {
    WLZ_VTX_3_SUB(*(simplex + idN), *(simplex + idN), org);
  }
  /* Sort the verticies of the simplex by plane then line, so that the first
   * vertex has the lowest plane/line coordinate. */
  AlgSort((void *)simplex, 3, sizeof(WlzIVertex3), WlzRasterVtxCmp3I);
  /* Initialize variables for drawing a line from *(simplex + 0) to
   * *(simplex + 2) and from *(simplex + 0) to *(simplex + 1) and
   * then on to *(simplex + 2). At each step draw a line between
   * the two lines being constructed. */
  fstSeg = 1;
  seg[0] = seg[1] = *(simplex + 0);
  WLZ_VTX_3_SUB(del[0], *(simplex + 2), *(simplex + 0));
  WLZ_VTX_3_SUB(del[1], *(simplex + 1), *(simplex + 0));
  for(idN = 0; idN < 2; ++idN)
  {
    WLZ_VTX_3_SIGN(sgn[idN], del[idN]);
    WLZ_VTX_3_ABS(abs[idN], del[idN]);
    inc[idN].vtX = 2 * abs[idN].vtX;
    inc[idN].vtY = 2 * abs[idN].vtY;
    inc[idN].vtZ = 2 * abs[idN].vtZ;
    errXY[idN] = abs[idN].vtY - abs[idN].vtX; 
    errXZ[idN] = abs[idN].vtZ - abs[idN].vtX; 
    errZY[idN] = abs[idN].vtY - abs[idN].vtZ;
    cnt[idN] = abs[idN].vtX + abs[idN].vtY + abs[idN].vtZ;
  }
  while((cnt[0] > 0) || (cnt[1] > 0))
  {
    /* Set a line of voxels from seg[0] to seg[1]. */
    WlzRasterLine3I(bMsk, seg);
    for(idN = 0; idN < 2; ++idN)
    {
      if(cnt[idN] > 0)
      {
	/* Update line segment N from *(simplex + 0) to *(simplex + 2)
	 * if N == 0, or from *(simplex + 0) to *(simplex + 1) if N == 1. */
	if(errXY[idN] < 0 )
	{
	  if(errXZ[idN] < 0)
	  {
	    seg[idN].vtX += sgn[idN].vtX;
	    errXY[idN] += inc[idN].vtY; errXZ[idN] += inc[idN].vtZ;
	  }
	  else
	  {
	    seg[idN].vtZ += sgn[idN].vtZ;
	    errXZ[idN] -= inc[idN].vtX; errZY[idN] += inc[idN].vtY;
	  }
	}
	else
	{
	  if(errZY[idN] < 0)
	  {
	    seg[idN].vtZ += sgn[idN].vtZ;
	    errXZ[idN] -= inc[idN].vtX; errZY[idN] += inc[idN].vtY;
	  }
	  else
	  {
	    seg[idN].vtY += sgn[idN].vtY;
	    errXY[idN] -= inc[idN].vtX; errZY[idN] -= inc[idN].vtZ;
	  }
	}
	--cnt[idN];
      }
    }
    if(fstSeg && (cnt[1] == 0))
    {
      fstSeg = 0;
      seg[1] = *(simplex + 1);
      WLZ_VTX_3_SUB(del[1], *(simplex + 2), *(simplex + 1));
      WLZ_VTX_3_SIGN(sgn[1], del[1]);
      WLZ_VTX_3_ABS(abs[1], del[1]);
      inc[1].vtX = 2 * abs[1].vtX;
      inc[1].vtY = 2 * abs[1].vtY;
      inc[1].vtZ = 2 * abs[1].vtZ;
      errXY[1] = abs[1].vtY - abs[1].vtX; 
      errXZ[1] = abs[1].vtZ - abs[1].vtX; 
      errZY[1] = abs[1].vtY - abs[1].vtZ;
      cnt[1] = abs[1].vtX + abs[1].vtY + abs[1].vtZ;
    }
  }
  WlzRasterLine3I(bMsk, seg);
  return(errNum);
}

/*!
* \return	void
* \ingroup	WlzRaster
* \brief	Rasterizes the given 3D line segmant into the given 3D
*               bit array.
* \param	bMsk			The 3D bitmask to be written to.
* \param	seg			The coordinates of the line
*                                       segment's end points.
*/
static void	WlzRasterLine3I(WlzUByte ***bMsk, WlzIVertex3 *seg)
{
  int		cnt,
  		errXY,
		errXZ,
		errZY;
  WlzIVertex3   pos,
		abs,
  		del,
		sgn,
		inc;

  pos = *(seg + 0);
  WLZ_VTX_3_SUB(del, *(seg + 1), *(seg + 0));
  WLZ_VTX_3_SIGN(sgn, del);
  WLZ_VTX_3_ABS(abs, del);
  inc.vtX = 2 * abs.vtX;
  inc.vtY = 2 * abs.vtY;
  inc.vtZ = 2 * abs.vtZ;
  errXY = abs.vtY - abs.vtX;
  errXZ = abs.vtZ - abs.vtX;
  errZY = abs.vtY - abs.vtZ;
  if((cnt = abs.vtX + abs.vtY + abs.vtZ) > 0)
  {
    while(cnt-- > 0)
    {
      WlzRasterSetVoxel(bMsk, pos);
      if(errXY < 0 )
      {
	if(errXZ < 0)
	{
	  pos.vtX += sgn.vtX;
	  errXY += inc.vtY; errXZ += inc.vtZ;
	}
	else
	{
	  pos.vtZ += sgn.vtZ;
	  errXZ -= inc.vtX; errZY += inc.vtY;
	}
      }
      else
      {
	if(errZY < 0)
	{
	  pos.vtZ += sgn.vtZ;
	  errXZ -= inc.vtX; errZY += inc.vtY;
	}
	else
	{
	  pos.vtY += sgn.vtY;
	  errXY -= inc.vtX; errZY -= inc.vtZ;
	}
      }
    }
    WlzRasterSetVoxel(bMsk, pos);
  }
}

/*!
* \return	void
* \ingroup	WlzRaster
* \brief	Sets a single voxel in the given 3D bit array.
* \param	bMsk			The 3D bitmask to be written to.
* \param	pos			The coordinates of the line
*                                       voxel to set.
*/
static void	WlzRasterSetVoxel(WlzUByte ***bMsk, WlzIVertex3 pos)
{
  *(*(*(bMsk + pos.vtZ) + pos.vtY) + (pos.vtX / 8)) |= 1 << (pos.vtX % 8);
}

#ifdef WLZ_RASTER_TEST
/* Test main() for WlzrasterObj(). */

extern int	getopt(int argc, char * const *argv, const char *optstring);

extern char	*optarg;
extern int	optind,
		opterr,
		optopt;

int             main(int argc, char *argv[])
{
  int           option,
  		ok = 1,
		usage = 0;
  FILE		*fP = NULL;
  char		*inObjFileStr,
  		*outObjFileStr;
  WlzObject     *inObj = NULL,
  		*outObj = NULL;
  WlzErrorNum   errNum = WLZ_ERR_NONE;
  static char	optList[] = "ho:";
  const char	*errMsgStr;
  const char	outObjFileStrDef[] = "-",
  		inObjFileStrDef[] = "-";

  outObjFileStr = (char *)outObjFileStrDef;
  inObjFileStr = (char *)inObjFileStrDef;
  while(ok && ((option = getopt(argc, argv, optList)) != -1))
  {
    switch(option)
    {
      case 'h':
        usage = 1;
	ok = 0;
	break;
      case 'o':
        outObjFileStr = optarg;
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
       (outObjFileStr == NULL) || (*outObjFileStr == '\0'))
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
  }
  if(ok)
  {
    if((inObjFileStr == NULL) ||
       (*inObjFileStr == '\0') ||
       ((fP = (strcmp(inObjFileStr, "-")?
	      fopen(inObjFileStr, "r"): stdin)) == NULL) ||
       ((inObj= WlzAssignObject(WlzReadObj(fP, &errNum), NULL)) == NULL) ||
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
    }

  }
  if(ok)
  {
    outObj = WlzRasterObj(inObj, &errNum);
    if(errNum != WLZ_ERR_NONE)
    {
      ok = 0;
      (void )fprintf(stderr, "%s Failed to scan convert object.\n",
      	     	     argv[0]);
    }
  }
  if(inObj)
  {
    (void )WlzFreeObj(inObj);
  }
  if(ok)
  {
  if((fP = (strcmp(outObjFileStr, "-")?
	   fopen(outObjFileStr, "w"): stdout)) == NULL)
    {
      ok = 0;
      (void )fprintf(stderr, "%s Failed to open output file %s.\n",
      	      	     argv[0], outObjFileStr);
    }
  }
  if(ok)
  {
    errNum = WlzWriteObj(fP, outObj);
    if(errNum != WLZ_ERR_NONE)
    {
      ok = 0;
      (void )WlzStringFromErrorNum(errNum, &errMsgStr);
      (void )fprintf(stderr,
                     "%s: Failed to write output object (%s).\n",
                     argv[0], errMsgStr);
    }
  }
  if(fP && strcmp(outObjFileStr, "-"))
  {
    (void )fclose(fP);
  }
  if(outObj)
  {
    (void )WlzFreeObj(outObj);
  }
  if(usage)
  {
      (void )fprintf(stderr,
      "Usage: %s%sExample: %s%s",
      *argv,
      " [-o<output object>] [-h] [<input object>]\n"
      "Options:\n"
      "  -h  Prints this usage information.\n"
      "  -o  Output object file name.\n"
      "Scan converts the given object.\n"
      "The input object is read from stdin and the output output is written\n"
      "to stdout unless filenames are given.\n",
      *argv,
      " in.wlz\n"
      "The input Woolz object is read from in.wlz, and the scan converted\n"
      "object is written to stdout.\n");
  }
  return(!ok);
}

#endif /* WLZ_RASTER_TEST */
