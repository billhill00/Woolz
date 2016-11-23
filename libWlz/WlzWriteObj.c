#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _WlzWriteObj_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         libWlz/WlzWriteObj.c
* \author       Richard Baldock, Bill Hill
* \date         September 2005
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
* \brief	Functions for writing Woolz objects.
* \ingroup	WlzIO
*/

#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <Wlz.h>

/* #define WLZ_DEBUG_WRITEOBJ */

#if defined(_WIN32) && !defined(__x86)
#define __x86
#endif


static WlzErrorNum		WlzWriteIntervalDomain(
				  FILE *fP,
				  WlzIntervalDomain *itvl);
static WlzErrorNum   		WlzWritePlaneDomain(
				  FILE *fP,
				  WlzPlaneDomain *planedm);
static WlzErrorNum		WlzWritePropertyList(
				  FILE *fP,
				  WlzPropertyList *pList);
static WlzErrorNum		WlzWriteProperty(
				  FILE *fP,
				  WlzProperty property);
static WlzErrorNum		WlzWriteValueTable(
				  FILE	*fP,
				  WlzObject *obj);
static WlzErrorNum		WlzWriteVoxelValueTable(
				  FILE *fP,
				  WlzObject *obj);
static WlzErrorNum		WlzWriteTiledValueTable(
				  FILE *fP,
				  WlzObject *obj,
				  int writeTiles);
static WlzErrorNum		WlzWritePolygon(
				  FILE *fP,
				  WlzPolygonDomain *poly);
static WlzErrorNum		WlzWriteBoundList(
				  FILE *fP,
				  WlzBoundList *blist);
static WlzErrorNum		WlzWriteRect(
				  FILE *fP,
				  WlzIRect *rdom);
static WlzErrorNum		WlzWriteHistogramDomain(
				  FILE *fP,
				  WlzHistogramDomain *hist);
static WlzErrorNum		WlzWriteCompoundA(
				  FILE *fP,
				  WlzCompoundArray *c);
static WlzErrorNum		WlzWriteAffineTransform(
				  FILE *fP,
				  WlzAffineTransform *trans);
static WlzErrorNum		WlzWriteWarpTrans(
				  FILE *fP,
				  WlzWarpTrans *obj);
static WlzErrorNum		WlzWriteFMatchObj(
				  FILE *fP,
				  WlzFMatchObj *obj);
static WlzErrorNum		WlzWrite3DWarpTrans(
				  FILE *fP,
				  Wlz3DWarpTrans *obj);
static WlzErrorNum 		WlzWriteContour(
				  FILE *fP,
				  WlzContour *ctr);
static WlzErrorNum 		WlzWriteGMModel(
				  FILE *fP,
				  WlzGMModel *model);
static WlzErrorNum		WlzWriteInt(
				  FILE *fP,
				  int *iP,
				  size_t nI);
static WlzErrorNum		WlzWriteShort(
				  FILE *fP,
				  short *iP,
				  size_t nI);
static WlzErrorNum		WlzWriteUByte(
				  FILE *fP,
				  WlzUByte *iP,
				  size_t nI);
static WlzErrorNum		WlzWriteFloat(
				  FILE *fP,
				  float *iP,
				  size_t nI);
static WlzErrorNum		WlzWriteDouble(
				  FILE *fP,
				  double *iP,
				  size_t nI);
static WlzErrorNum 		WlzWriteVertex2I(
				  FILE *fP,
				  WlzIVertex2 *vP,
				  int nV);
static WlzErrorNum 		WlzWriteVertex2D(
				  FILE *fP,
				  WlzDVertex2 *vP,
				  int nV);
static WlzErrorNum 		WlzWriteVertex3I(
				  FILE *fP,
				  WlzIVertex3 *vP,
				  int nV);
static WlzErrorNum 		WlzWriteVertex3D(
				  FILE *fP,
				  WlzDVertex3 *vP,
				  int nV);
static WlzErrorNum		WlzWriteStr(
				  FILE *fP,
				  char *str);
static WlzErrorNum		WlzWritePixelV(
				  FILE *fP,
				  WlzPixelV *pV,
				  int nPV);
static WlzErrorNum		WlzWriteGreyV(
				  FILE *fP,
				  WlzGreyType type,
				  WlzGreyV *gV,
				  int nGV);
static WlzErrorNum		WlzWriteMeshTransform2D(
				  FILE *fP,
				  WlzMeshTransform *mTrans);
static WlzErrorNum		WlzWriteCMesh2D(
				  FILE *fP,
				  int **dstNodTbl,
				  WlzCMesh2D *mesh);
static WlzErrorNum		WlzWriteCMesh2D5(
				  FILE *fP,
				  int **dstNodTbl,
				  WlzCMesh2D5 *mesh);
static WlzErrorNum		WlzWriteCMesh3D(
				  FILE *fP,
				  int **dstNodTbl,
				  WlzCMesh3D *mesh);
static WlzErrorNum		WlzWriteIndexedValues(
				  FILE *fP,
				  WlzObject *obj);
static WlzErrorNum 		WlzWriteLUTDomain(
				  FILE *fP,
				  WlzLUTDomain *lDom);
static WlzErrorNum 		WlzWriteLUTValues(
				  FILE *fP,
				  WlzObject *obj);
static WlzErrorNum 		WlzWrite3DViewStruct(
				  FILE *fp,
				  WlzThreeDViewStruct *vs);
static WlzErrorNum	 	WlzWritePoints(
				  FILE *fP,
				  WlzPoints *pts);
static WlzErrorNum      	WlzWritePointValues(
				  FILE *fP,
				  WlzObject *obj);
static WlzErrorNum 		WlzWriteConvexHull(
				  FILE *fP,
				  WlzDomain dom);

#ifdef WLZ_UNUSED_FUNCTIONS
static WlzErrorNum 		WlzWriteBox2I(
				  FILE *fP,
				  WlzIBox2 *bP,
				  int nB);
static WlzErrorNum 		WlzWriteBox2D(
				  FILE *fP,
				  WlzDBox2 *bP,
				  int nB);
static WlzErrorNum 		WlzWriteBox3I(
				  FILE *fP,
				  WlzIBox3 *bP,
				  int nB);
static WlzErrorNum 		WlzWriteBox3D(
				  FILE *fP,
				  WlzDBox3 *bP,
				  int nB);
#endif /* WLZ_UNUSED_FUNCTIONS */

/* These macros convert sequences of bytes from the architecture's endianness
 * to the Woolz file format order. */
#if defined (__sparc) || defined (__mips) || defined (__ppc)
#define WLZ_SWAP_OUT_WORD(T,S) \
		(T).ubytes[0] = (S).ubytes[3]; \
		(T).ubytes[1] = (S).ubytes[2]; \
		(T).ubytes[2] = (S).ubytes[1]; \
		(T).ubytes[3] = (S).ubytes[0];
#endif /* __sparc || __mips */
#if defined (__x86) || defined (__alpha)
#define WLZ_SWAP_OUT_WORD(T,S) \
		(T).inv = (S).inv;
#endif /* __x86 || __alpha */

#if defined (__sparc) || defined (__mips) || defined (__ppc)
#define WLZ_SWAP_OUT_SHORT(T,S) \
		(T).ubytes[0] = (S).ubytes[1]; \
		(T).ubytes[1] = (S).ubytes[0];
#endif /* __sparc || __mips */
#if defined (__x86) || defined (__alpha)
#define WLZ_SWAP_OUT_SHORT(T,S) \
		(T).shv = (S).shv;
#endif /* __x86 || __alpha */

#if defined (__sparc) || defined (__mips) || defined (__ppc)
#define WLZ_SWAP_OUT_FLOAT(T,S) \
		(T).ubytes[0] = (S).ubytes[1]; \
		(T).ubytes[1] = (S).ubytes[0] + 1; \
		(T).ubytes[2] = (S).ubytes[3]; \
		(T).ubytes[3] = (S).ubytes[2];
#endif /* __sparc || __mips */
#if defined (__x86) || defined (__alpha)
#define WLZ_SWAP_OUT_FLOAT(T,S) \
		(T).ubytes[3] = (S).ubytes[1]; \
		(T).ubytes[2] = (S).ubytes[0]; \
		(T).ubytes[1] = (S).ubytes[3] + 1; \
		(T).ubytes[0] = (S).ubytes[2];
#endif /* __x86 || __alpha */

#if defined (__sparc) || defined (__mips) || defined (__ppc)
#define WLZ_SWAP_OUT_DOUBLE(T,S) \
		(T).ubytes[0] = (S).ubytes[7]; \
		(T).ubytes[1] = (S).ubytes[6]; \
		(T).ubytes[2] = (S).ubytes[5]; \
		(T).ubytes[3] = (S).ubytes[4]; \
		(T).ubytes[4] = (S).ubytes[3]; \
		(T).ubytes[5] = (S).ubytes[2]; \
		(T).ubytes[6] = (S).ubytes[1]; \
		(T).ubytes[7] = (S).ubytes[0];
#endif /* __sparc || __mips */
#if defined (__x86) || defined (__alpha)
#define WLZ_SWAP_OUT_DOUBLE(T,S) \
                (T).dbv = (S).dbv;
#endif /* __x86 || __alpha */

/*!
* \return	Number of bytes written.
* \ingroup 	WlzIO
* \brief	Writes an integer reordered to DEC VAX(!) format.
* \param	i			Value written.
* \param	fP			Given file.
*/
static int putword(int i, FILE *fP)
{
  WlzGreyV	in,
  		out;

  in.inv = i;
  WLZ_SWAP_OUT_WORD(out, in);
  return((int )fwrite(out.ubytes, sizeof(char), 4, fP));
}

/*!
* \return	Number of bytes written.
* \ingroup	WlzIO
* \brief	Writes a short reordered to DEC VAX(!) format.
* \param	i			Value written.
* \param	fP			Given file.
*/
static int putshort(short i, FILE *fP)
{
  WlzGreyV	in,
  		out;

  in.shv = i;
  WLZ_SWAP_OUT_SHORT(out, in);
  return((int )fwrite(out.ubytes, sizeof(char), 2, fP));
}

/*!
* \return	Number of bytes written.
* \ingroup	WlzIO
* \brief	Writes a float reordered to DEC VAX(!) format.
* \param	f			Value written.
* \param	fP			Given file.
*/
static int putfloat(float f, FILE *fP)
{
  WlzGreyV	in,
  		out;

  in.flv = f;
  WLZ_SWAP_OUT_FLOAT(out, in);
  return((int )fwrite(out.ubytes, sizeof(char), 4, fP));
}

/*!
* \return	Number of bytes written.
* \ingroup	WlzIO
* \brief	Writes a double reordered to DEC VAX(!) format.
* \param	f			Value written.
* \param	fP			Given file.
*/
static int putdouble(double d, FILE *fP)
{
  WlzGreyV	in,
  		out;

  in.dbv = d;
  WLZ_SWAP_OUT_DOUBLE(out, in);
  return((int )fwrite(out.ubytes, sizeof(char), 8, fP));
}

/*!
* \return       Woolz error number code.
* \ingroup      WlzIO
* \brief        Top-level procedure for writing an object to a file stream.
*		For historical reasons most data are written using DEC VAX
*		byte ordering.
*
* \param    	fP			File pointer for output.
* \param    	obj			Ptr to top-level object to be written.
*/
WlzErrorNum	WlzWriteObj(FILE *fP, WlzObject *obj)
{
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(fP == NULL)
  {
    errNum = WLZ_ERR_PARAM_NULL;
  }
#ifdef _WIN32
  else if(_setmode(_fileno(fP), 0x8000) == -1)
  {
    errNum = WLZ_ERR_READ_EOF;
  }
#endif
  else if(obj == NULL)
  {
    if(putc((unsigned int )0, fP) == EOF)
    {
      errNum = WLZ_ERR_WRITE_EOF;
    }
  }
  else if(putc((unsigned int )obj->type, fP) == EOF)
  {
    errNum = WLZ_ERR_WRITE_EOF;
  }
  else if(errNum == WLZ_ERR_NONE)
  {
    switch( obj->type )
    {
      case WLZ_EMPTY_OBJ:
	/* Nothing except the object type needs to be written */
	break;
      case WLZ_2D_DOMAINOBJ:
	errNum = WlzWriteIntervalDomain(fP, obj->domain.i);
	if(errNum == WLZ_ERR_NONE)
        {
	  if((obj->values.core == NULL) ||
	     (WlzGreyTableIsTiled(obj->values.core->type) == 0))
	  {
	    errNum = WlzWriteValueTable(fP, obj);
	  }
	  else
	  {
	    errNum = WlzWriteTiledValueTable(fP, obj, 1);
	  }
	}
	if(errNum == WLZ_ERR_NONE)
        {
	  errNum = WlzWritePropertyList(fP, obj->plist);
	}
	break;
      case WLZ_3D_DOMAINOBJ:
	errNum = WlzWritePlaneDomain(fP, obj->domain.p);
	if(errNum == WLZ_ERR_NONE)
        {
	  if((obj->values.core == NULL) ||
	     (WlzGreyTableIsTiled(obj->values.core->type) == 0))
	  {
	    errNum = WlzWriteVoxelValueTable(fP, obj);
	  }
	  else
	  {
	    errNum = WlzWriteTiledValueTable(fP, obj, 1);
	  }
	}
	if(errNum == WLZ_ERR_NONE)
	{
	  errNum = WlzWritePropertyList(fP, obj->plist);
	}
	break;
      case WLZ_TRANS_OBJ:
	if(((errNum = WlzWriteAffineTransform(fP,
				obj->domain.t)) == WLZ_ERR_NONE) &&
	   ((errNum = WlzWriteObj(fP, obj->values.obj)) == WLZ_ERR_NONE))
	{
	  errNum = WlzWritePropertyList(fP, obj->plist);
	}
	break;
      case WLZ_3D_WARP_TRANS:
	if((errNum = WlzWrite3DWarpTrans(fP,
				(Wlz3DWarpTrans *)obj)) == WLZ_ERR_NONE)
	{
	  errNum = WlzWritePropertyList(fP, ((Wlz3DWarpTrans *)obj)->plist);
	}
	break;
      case WLZ_2D_POLYGON:
	errNum = WlzWritePolygon(fP, obj->domain.poly);
        break;
      case WLZ_BOUNDLIST:
	errNum = WlzWriteBoundList(fP, obj->domain.b);
        break;
      case WLZ_HISTOGRAM:
	errNum = WlzWriteHistogramDomain(fP, obj->domain.hist);
	break;
      case WLZ_CONTOUR:
        errNum = WlzWriteContour(fP, obj->domain.ctr);
	break;
      case WLZ_CMESH_2D:
        if(((errNum = WlzWriteCMesh2D(fP, NULL,
	                              obj->domain.cm2)) == WLZ_ERR_NONE) &&
           ((errNum = WlzWriteIndexedValues(fP, obj)) == WLZ_ERR_NONE))
	{
	  errNum = WlzWritePropertyList(fP, obj->plist);
	}
	break;
      case WLZ_CMESH_2D5:
        if(((errNum = WlzWriteCMesh2D5(fP, NULL,
	                              obj->domain.cm2d5)) == WLZ_ERR_NONE) &&
           ((errNum = WlzWriteIndexedValues(fP, obj)) == WLZ_ERR_NONE))
	{
	  errNum = WlzWritePropertyList(fP, obj->plist);
	}
	break;
      case WLZ_CMESH_3D:
        if(((errNum = WlzWriteCMesh3D(fP, NULL,
	                              obj->domain.cm3)) == WLZ_ERR_NONE) &&
           ((errNum = WlzWriteIndexedValues(fP, obj)) == WLZ_ERR_NONE))
	{
	  errNum = WlzWritePropertyList(fP, obj->plist);
	}
	break;
      case WLZ_RECTANGLE:
	errNum = WlzWriteRect(fP, obj->domain.r);
	break;
      case WLZ_AFFINE_TRANS:
	errNum = WlzWriteAffineTransform(fP, obj->domain.t);
	break;
      case WLZ_3D_VIEW_STRUCT:
	errNum = WlzWrite3DViewStruct(fP, obj->domain.vs3d);
	break;
      case WLZ_WARP_TRANS:
	errNum = WlzWriteWarpTrans(fP, (WlzWarpTrans *)obj);
	break;
      case WLZ_FMATCHOBJ:
	errNum = WlzWriteFMatchObj(fP, (WlzFMatchObj *)obj);
	break;
      case WLZ_COMPOUND_ARR_1: /* FALLTHROUGH */
      case WLZ_COMPOUND_ARR_2:
	errNum = WlzWriteCompoundA(fP, (WlzCompoundArray *)obj);
	break;
      case WLZ_PROPERTY_OBJ:
	errNum = WlzWritePropertyList(fP, obj->plist);
	break;
      case WLZ_CONV_HULL:
#ifdef WLZ_OLDCODE_CONVHULL
	if((errNum = WlzWritePolygon(fP, obj->domain.poly)) == WLZ_ERR_NONE)
	{
	  errNum = WlzWriteConvexHullValues(fP, obj->values.c);
	}
#else
        errNum = WlzWriteConvexHull(fP, obj->domain);
#endif
	break;
      case WLZ_3D_POLYGON:
	errNum = WLZ_ERR_OBJECT_TYPE;
	break;
      case WLZ_MESH_TRANS:
	errNum = WlzWriteMeshTransform2D(fP, obj->domain.mt);
	break;
      case WLZ_LUT:
        if(((errNum = WlzWriteLUTDomain(fP, 
	                                obj->domain.lut)) == WLZ_ERR_NONE) &&
           ((errNum = WlzWriteLUTValues(fP, obj)) == WLZ_ERR_NONE))
	{
	  errNum = WlzWritePropertyList(fP, obj->plist);
	}
	break;
      case WLZ_POINTS:
	if(((errNum = WlzWritePoints(fP,
	                             obj->domain.pts)) == WLZ_ERR_NONE) &&
           ((errNum = WlzWritePointValues(fP, obj)) == WLZ_ERR_NONE))
	{
	  errNum = WlzWritePropertyList(fP, obj->plist);
	}
        break;
      /* Orphans and not yet implemented object types for I/O */
      case WLZ_CONVOLVE_INT:    /* FALLTHROUGH */
      case WLZ_CONVOLVE_FLOAT:  /* FALLTHROUGH */
      case WLZ_TEXT:            /* FALLTHROUGH */
      case WLZ_COMPOUND_LIST_1: /* FALLTHROUGH */
      case WLZ_COMPOUND_LIST_2: /* FALLTHROUGH */
      default:
	errNum = WLZ_ERR_OBJECT_TYPE;
	break;
    }
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzIO
* \brief	Write's the given native int values to the given file
*               as a 4 byte integer.
* \param	fP			Given file.
* \param	iP			Ptr to native ints.
* \param	nI			Number of ints.
*/
static WlzErrorNum WlzWriteInt(FILE *fP, int *iP, size_t nI)
{
  WlzErrorNum 	errNum = WLZ_ERR_NONE;

  while((errNum == WLZ_ERR_NONE) && (nI-- > 0))
  {
    if(!putword(*iP, fP))
    {
      errNum = WLZ_ERR_WRITE_INCOMPLETE;
    }
    ++iP;
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzIO
* \brief	Write's the given native short values to the given file
*               as a 2 byte short integer.
* \param	fP			Given file.
* \param	iP			Ptr to native shorts.
* \param	nI			Number of shorts.
*/
static WlzErrorNum WlzWriteShort(FILE *fP, short *iP, size_t nI)
{
  WlzErrorNum 	errNum = WLZ_ERR_NONE;

  while((errNum == WLZ_ERR_NONE) && (nI-- > 0))
  {
    if(!putshort(*iP, fP))
    {
      errNum = WLZ_ERR_WRITE_INCOMPLETE;
    }
    ++iP;
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzIO
* \brief	Write's the given native unsigned byte values to the given file
*               as a single unsigned byte.
* \param	fP			Given file.
* \param	iP			Ptr to native unsigned bytes.
* \param	nI			Number of bytes.
*/
static WlzErrorNum WlzWriteUByte(FILE *fP, WlzUByte *iP, size_t nI)
{
  WlzErrorNum 	errNum = WLZ_ERR_NONE;

  if(fwrite(iP, sizeof(WlzUByte), nI, fP) != nI)
  {
    errNum = WLZ_ERR_WRITE_INCOMPLETE;
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzIO
* \brief	Write's the given native float values to the given file
*               as a 4 floats.
* \param	fP			Given file.
* \param	iP			Ptr to native floats.
* \param	nI			Number of floats.
*/
static WlzErrorNum WlzWriteFloat(FILE *fP, float *iP, size_t nI)
{
  WlzErrorNum 	errNum = WLZ_ERR_NONE;

  while((errNum == WLZ_ERR_NONE) && (nI-- > 0))
  {
    if(!putfloat(*iP, fP))
    {
      errNum = WLZ_ERR_WRITE_INCOMPLETE;
    }
    ++iP;
  }
  if(fwrite(iP, sizeof(float), nI, fP) != nI)
  {
    errNum = WLZ_ERR_WRITE_INCOMPLETE;
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzIO
* \brief	Write's the given native double values to the given file
*               as a 8 byte doubles.
* \param	fP			Given file.
* \param	iP			Ptr to native doubles.
* \param	nI			Number of doubles.
*/
static WlzErrorNum WlzWriteDouble(FILE *fP, double *iP, size_t nI)
{
  WlzErrorNum 	errNum = WLZ_ERR_NONE;

  while((errNum == WLZ_ERR_NONE) && (nI-- > 0))
  {
    if(!putdouble(*iP, fP))
    {
      errNum = WLZ_ERR_WRITE_INCOMPLETE;
    }
    ++iP;
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup      WlzIO
* \brief	Write's the given 2D integer verticies to the given file.
* \param	fP			Given file.
* \param	vP			Ptr to 2D integer verticies.
* \param	nV			Number of verticies.
*/
static WlzErrorNum WlzWriteVertex2I(FILE *fP, WlzIVertex2 *vP, int nV)
{
  WlzErrorNum 	errNum = WLZ_ERR_NONE;

  while((errNum == WLZ_ERR_NONE) && (nV-- > 0))
  {
    if(!putword(vP->vtY, fP) || !putword(vP->vtX, fP))
    {
      errNum = WLZ_ERR_WRITE_INCOMPLETE;
    }
    ++vP;
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup      WlzIO
* \brief	Write's the given 2D double verticies to the given file.
* \param	fP			Given file.
* \param	vP			Ptr to 2D double verticies.
* \param	nV			Number of verticies.
*/
static WlzErrorNum WlzWriteVertex2D(FILE *fP, WlzDVertex2 *vP, int nV)
{
  WlzErrorNum 	errNum = WLZ_ERR_NONE;

  while((errNum == WLZ_ERR_NONE) && (nV-- > 0))
  {
    if(!putdouble(vP->vtY, fP) || !putdouble(vP->vtX, fP))
    {
      errNum = WLZ_ERR_WRITE_INCOMPLETE;
    }
    ++vP;
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup      WlzIO
* \brief	Write's the given 3D integer verticies to the given file.
* \param	fP			Given file.
* \param	vP			Ptr to 3D integer verticies.
* \param	nV			Number of verticies.
*/
static WlzErrorNum WlzWriteVertex3I(FILE *fP, WlzIVertex3 *vP, int nV)
{
  WlzErrorNum 	errNum = WLZ_ERR_NONE;

  while((errNum == WLZ_ERR_NONE) && (nV-- > 0))
  {
    if(!putword(vP->vtX, fP) || !putword(vP->vtY, fP) ||
       !putword(vP->vtZ, fP))
    {
      errNum = WLZ_ERR_WRITE_INCOMPLETE;
    }
    ++vP;
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup      WlzIO
* \brief	Write's the given 3D double verticies to the given file.
* \param	fP			Given file stream.
* \param	vP			Ptr to 3D double verticies.
* \param	nV			Number of verticies.
*/
static WlzErrorNum WlzWriteVertex3D(FILE *fP, WlzDVertex3 *vP, int nV)
{
  WlzErrorNum 	errNum = WLZ_ERR_NONE;

  while((errNum == WLZ_ERR_NONE) && (nV-- > 0))
  {
    if(!putdouble(vP->vtX, fP) || !putdouble(vP->vtY, fP) ||
       !putdouble(vP->vtZ, fP))
    {
      errNum = WLZ_ERR_WRITE_INCOMPLETE;
    }
    ++vP;
  }
  return(errNum);
}

#ifdef WLZ_UNUSED_FUNCTIONS
/*!
* \return	Woolz error code.
* \ingroup      WlzIO
* \brief	Write's the given 2D integer box to the given file.
* \param	fP			Given file.
* \param	bP			Ptr to 2D integer box.
* \param	nB			Number of bounding boxes.
*/
static WlzErrorNum WlzWriteBox2I(FILE *fP, WlzIBox2 *bP, int nB)
{
  WlzErrorNum 	errNum = WLZ_ERR_NONE;

  while((errNum == WLZ_ERR_NONE) && (nB-- > 0))
  {
    if(!putword(bP->xMin, fP) || !putword(bP->yMin, fP) ||
       !putword(bP->xMax, fP) || !putword(bP->yMax, fP))
    {
      errNum = WLZ_ERR_WRITE_INCOMPLETE;
    }
    ++bP;
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup      WlzIO
* \brief	Write's the given 2D double box to the given file.
* \param	fP			Given file.
* \param	bP			Ptr to 2D double box.
* \param	nB			Number of bounding boxes.
*/
static WlzErrorNum WlzWriteBox2D(FILE *fP, WlzDBox2 *bP, int nB)
{
  WlzErrorNum 	errNum = WLZ_ERR_NONE;

  while((errNum == WLZ_ERR_NONE) && (nB-- > 0))
  {
    if(!putdouble(bP->xMin, fP) || !putdouble(bP->yMin, fP) ||
       !putdouble(bP->xMax, fP) || !putdouble(bP->yMax, fP))
    {
      errNum = WLZ_ERR_WRITE_INCOMPLETE;
    }
    ++bP;
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzIO
* \brief	Write's the given 3D integer box to the given file.
* \param	fP			Given file.
* \param	bP			Ptr to 3D integer box.
* \param	nB			Number of bounding boxes.
*/
static WlzErrorNum WlzWriteBox3I(FILE *fP, WlzIBox3 *bP, int nB)
{
  WlzErrorNum 	errNum = WLZ_ERR_NONE;

  while((errNum == WLZ_ERR_NONE) && (nB-- > 0))
  {
    if(!putword(bP->xMin, fP) || !putword(bP->yMin, fP) ||
       !putword(bP->zMin, fP) ||
       !putword(bP->xMax, fP) || !putword(bP->yMax, fP) ||
       !putword(bP->zMax, fP))
    {
      errNum = WLZ_ERR_WRITE_INCOMPLETE;
    }
    ++bP;
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup      WlzIO
* \brief	Write's the given 3D double box to the given file.
* \param	fP			Given file.
* \param	bP			Ptr to 3D double box.
* \param	nB			Number of bounding boxes.
*/
static WlzErrorNum WlzWriteBox3D(FILE *fP, WlzDBox3 *bP, int nB)
{
  WlzErrorNum 	errNum = WLZ_ERR_NONE;

  while((errNum == WLZ_ERR_NONE) && (nB-- > 0))
  {
    if(!putdouble(bP->xMin, fP) || !putdouble(bP->yMin, fP) ||
       !putdouble(bP->zMin, fP) ||
       !putdouble(bP->xMax, fP) || !putdouble(bP->yMax, fP) ||
       !putdouble(bP->zMax, fP))
    {
      errNum = WLZ_ERR_WRITE_INCOMPLETE;
    }
    ++bP;
  }
  return(errNum);
}
#endif /* WLZ_UNUSED_FUNCTIONS */

/*!
* \return	Woolz error code.
* \ingroup	WlzIO
* \brief	Writes an ASCII string to the given file.
*		\verbatim
		<number of characters> int, 4 bytes
		<characters>
		\endverbatim
* \param	fP			Given file.
* \param	str			The string.
*/
static WlzErrorNum	WlzWriteStr(FILE *fP, char *str)
{
  int		len;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  len = strlen(str);
  if((putword(len, fP) !=  4) || (fwrite(str, 1, len, fP) != len))
  {
    errNum = WLZ_ERR_WRITE_INCOMPLETE;
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzIO
* \brief	Writes the given grey values to the given file. Each
*		of the grey values may be of the same type.
* \param	fP			Given file.
* \param	type			Grey value type.
* \param	gV			The grey values.
* \param	nGV			Number of grey values to write.
*/
static WlzErrorNum	WlzWriteGreyV(FILE *fP, WlzGreyType type,
				      WlzGreyV *gV, int nGV)
{
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  switch(type)
  {
    case WLZ_GREY_INT:
      while((errNum == WLZ_ERR_NONE) && (nGV-- > 0))
      {
	if(putword(gV->inv, fP) != 4)
	{
	  errNum = WLZ_ERR_WRITE_INCOMPLETE;
	}
	++gV;
      }
      break;
    case WLZ_GREY_SHORT:
      while((errNum == WLZ_ERR_NONE) && (nGV-- > 0))
      {
	if(putshort(gV->shv, fP) != 2)
	{
	  errNum = WLZ_ERR_WRITE_INCOMPLETE;
	}
	++gV;
      }
      break;
    case WLZ_GREY_UBYTE:
      while((errNum == WLZ_ERR_NONE) && (nGV-- > 0))
      {
	if(putc(((unsigned int )(gV->ubv)), fP) == EOF)
	{
	  errNum = WLZ_ERR_WRITE_INCOMPLETE;
	}
	++gV;
      }
      break;
    case WLZ_GREY_FLOAT:
      while((errNum == WLZ_ERR_NONE) && (nGV-- > 0))
      {
        if(putfloat(gV->flv, fP) != 4)
	{
	  errNum = WLZ_ERR_WRITE_INCOMPLETE;
	}
	++gV;
      }
      break;
    case WLZ_GREY_DOUBLE:
      while((errNum == WLZ_ERR_NONE) && (nGV-- > 0))
      {
        if(putdouble(gV->dbv, fP) != 8)
	{
	  errNum = WLZ_ERR_WRITE_INCOMPLETE;
	}
	++gV;
      }
      break;
    case WLZ_GREY_RGBA:
      while((errNum == WLZ_ERR_NONE) && (nGV-- > 0))
      {
	if(putword((int )(gV->rgbv), fP) != 4)
	{
	  errNum = WLZ_ERR_WRITE_INCOMPLETE;
	}
	++gV;
      }
      break;
    default:
      errNum = WLZ_ERR_GREY_TYPE;
      break;
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzIO
* \brief	Writes the given pixel values to the given file. Each
*		of the pixel values may be of a different type.
* \param	fP			Given file.
* \param	pV			The pixel values.
* \param	nGV			Number of grey values to write.
*/
static WlzErrorNum	WlzWritePixelV(FILE *fP, WlzPixelV *pV, int nPV)
{
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  while((errNum == WLZ_ERR_NONE) && (nPV-- > 0))
  {
    if(putc((unsigned int)pV->type, fP) == EOF)
    {
      errNum = WLZ_ERR_WRITE_INCOMPLETE;
    }
    else
    {
      errNum = WlzWriteGreyV(fP, pV->type, &(pV->v), 1);
    }
    ++pV;
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup      WlzIO
* \brief	Write's the given Woolz interval domain to the given file.
* \param	fP			Given file.
* \param	itvl			Interval domain.
*/
static WlzErrorNum WlzWriteIntervalDomain(FILE *fP, WlzIntervalDomain *itvl)
{
  int 			i,
  			j,
			nlines;
  WlzIntervalLine	*ivln;
  WlzErrorNum		errNum = WLZ_ERR_NONE;

  if(itvl == NULL)
  {
    if(putc(0,fP) == EOF)
    {
      errNum = WLZ_ERR_WRITE_EOF;
    }
  }
  else
  {
    /* standardise it so no non-standard domains on disk
       - can't do this it conflicts with read-only access
       to object store */
    /*WlzStandardIntervalDomain(itvl);*/

    /* write the type and bounding box */
    if((putc((unsigned int) itvl->type, fP) == EOF) ||
       !putword(itvl->line1, fP) ||
       !putword(itvl->lastln, fP) ||
       !putword(itvl->kol1, fP) ||
       !putword(itvl->lastkl, fP) )
    {
      errNum = WLZ_ERR_WRITE_INCOMPLETE;
    }
    else
    {
      switch(itvl->type)
      {
	case WLZ_INTERVALDOMAIN_INTVL:
	  nlines = itvl->lastln - itvl->line1;
	  for(i = 0; (i <= nlines) && (errNum == WLZ_ERR_NONE); i++)
	  {
	    if(!putword(itvl->intvlines[i].nintvs, fP))
	    {
	      errNum = WLZ_ERR_WRITE_INCOMPLETE;
	    }
	  }
	  if(errNum == WLZ_ERR_NONE)
	  {
	    ivln = itvl->intvlines;
	    for(i = 0; (i <= nlines) && (errNum == WLZ_ERR_NONE); i++)
	    {
	      for(j = 0; (j < ivln->nintvs) && (errNum == WLZ_ERR_NONE); j++)
	      {
		if(!putword(ivln->intvs[j].ileft, fP) ||
		   !putword(ivln->intvs[j].iright, fP))
		{
		  errNum = WLZ_ERR_WRITE_INCOMPLETE;
		}
	      }
	      ivln++;
	    }
	  }
	  break;
	case WLZ_INTERVALDOMAIN_RECT:
	  break;
	default:
	  errNum = WLZ_ERR_DOMAIN_TYPE;
	  break;
      }
    }
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzIO
* \brief	Writes a plane domain to the given file.
* \param	fP			Given file.
* \param	planedm			Palne domain.
*/
static WlzErrorNum WlzWritePlaneDomain(FILE *fP, WlzPlaneDomain *planedm)
{
  int		i,
  		nplanes;
  float		dummy_float = 0.0;
  WlzDomain	*domains;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(planedm == NULL)
  {
    if(putc(0,fP) == EOF)
    {
      errNum = WLZ_ERR_WRITE_EOF;
    }
  }
  else
  {
    if((putc((unsigned int )(planedm->type), fP) == EOF) ||
       !putword(planedm->plane1, fP) ||
       !putword(planedm->lastpl, fP) ||
       !putword(planedm->line1, fP) ||
       !putword(planedm->lastln, fP) ||
       !putword(planedm->kol1, fP) ||
       !putword(planedm->lastkl, fP) ||
       !putfloat((planedm->voxel_size)[0], fP) ||
       !putfloat((planedm->voxel_size)[1], fP) ||
       !putfloat((planedm->voxel_size)[2], fP))
    {
      errNum = WLZ_ERR_WRITE_INCOMPLETE;
    }
    else
    {
      /* write dummy values of plane positions for backward
	 compatibility - should go on file-format revision */
      nplanes = planedm->lastpl - planedm->plane1 + 1;
      for(i = 0; (i < nplanes) && (errNum == WLZ_ERR_NONE); i++)
      {
	if(!putfloat(dummy_float, fP))
	{
	  errNum = WLZ_ERR_WRITE_INCOMPLETE;
	}
      }
    }
    if(errNum == WLZ_ERR_NONE)
    {
      domains = planedm->domains;
      switch(planedm->type)
      {
	case WLZ_PLANEDOMAIN_DOMAIN:
	  for(i = 0; (i < nplanes) && (errNum == WLZ_ERR_NONE); i++, domains++)
	  {
	    errNum = WlzWriteIntervalDomain(fP, (*domains).i);
	  }
	  break;
	case WLZ_PLANEDOMAIN_POLYGON:
	  for(i = 0; (i < nplanes) && (errNum == WLZ_ERR_NONE); i++, domains++)
	  {
	    errNum = WlzWritePolygon(fP, (*domains).poly);
	  }
	  break;
	case WLZ_PLANEDOMAIN_BOUNDLIST:
	  for(i = 0; (i < nplanes) && (errNum == WLZ_ERR_NONE); i++, domains++)
	  {
	    errNum = WlzWriteBoundList(fP, (*domains).b);
	  }
	  break;
	case WLZ_PLANEDOMAIN_HISTOGRAM:
	  for(i = 0; (i < nplanes) && (errNum == WLZ_ERR_NONE); i++, domains++)
	  {
	    errNum = WlzWriteHistogramDomain(fP, (*domains).hist);
	  }
	  break;
	case WLZ_PLANEDOMAIN_AFFINE:
	  for(i = 0; (i < nplanes) && (errNum == WLZ_ERR_NONE); i++, domains++)
	  {
	    errNum = WlzWriteAffineTransform(fP, (*domains).t);
	  }
	  break;
	case WLZ_PLANEDOMAIN_WARP:
	  for(i = 0; (i < nplanes) && (errNum == WLZ_ERR_NONE); i++, domains++)
	  {
	    errNum = WlzWriteWarpTrans(fP, (*domains).wt);
	  }
	  break;
	default:
	  errNum = WLZ_ERR_DOMAIN_TYPE;
	  break;
      }
    }
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzIO
* \brief	Writes a single property to the given file.
* \param	fP			Given file.
* \param	property		Property to be written.
*/
static WlzErrorNum WlzWriteProperty(FILE *fP, WlzProperty property)
{
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(property.core == NULL)
  {
    if(putc(0,fP) == EOF)
    {
      errNum = WLZ_ERR_WRITE_EOF;
    }
  }
  else
  {
    switch( property.core->type ){
    case WLZ_PROPERTY_SIMPLE:
      /* Write the size */
      if((putc((unsigned int) WLZ_PROPERTY_SIMPLE, fP) == EOF) ||
	 !putword(property.simple->size, fP))
      {
	errNum = WLZ_ERR_WRITE_INCOMPLETE;
      }
      else if(!fwrite(property.simple->prop, property.simple->size, 1, fP))
      {
	errNum = WLZ_ERR_WRITE_INCOMPLETE;
      }
      break;

    case  WLZ_PROPERTY_EMAP:
      if((putc((unsigned int) WLZ_PROPERTY_EMAP, fP) == EOF) ||
	 (putc((unsigned int) property.emap->emapType, fP) == EOF) ||
	 !fwrite(property.emap->modelUID,
		 EMAP_PROPERTY_UID_LENGTH, 1, fP) ||
	 !fwrite(property.emap->anatomyUID,
		 EMAP_PROPERTY_UID_LENGTH, 1, fP) ||
	 !fwrite(property.emap->targetUID,
		 EMAP_PROPERTY_UID_LENGTH, 1, fP) ||
	 !fwrite(property.emap->targetVersion,
		 EMAP_PROPERTY_VERSION_LENGTH, 1, fP) ||
	 !fwrite(property.emap->stage,
		 EMAP_PROPERTY_STAGE_LENGTH, 1, fP) ||
	 !fwrite(property.emap->subStage,
		 EMAP_PROPERTY_STAGE_LENGTH, 1, fP) ||
	 !fwrite(property.emap->modelName,
		 EMAP_PROPERTY_MODELNAME_LENGTH, 1, fP) ||
	 !fwrite(property.emap->version,
		 EMAP_PROPERTY_VERSION_LENGTH, 1, fP) ||
	 !putword(property.emap->creationTime, fP) ||
	 !fwrite(property.emap->creationAuthor,
		 EMAP_PROPERTY_AUTHORNAME_LENGTH, 1, fP) ||
	 !fwrite(property.emap->creationMachineName,
		 EMAP_PROPERTY_MACHINENAME_LENGTH, 1, fP) ||
	 !putword(property.emap->modificationTime, fP) ||
	 !fwrite(property.emap->modificationAuthor,
		 EMAP_PROPERTY_AUTHORNAME_LENGTH, 1, fP)){
	errNum = WLZ_ERR_WRITE_INCOMPLETE;
      }
      else {
	if(property.emap->fileName &&
	   (strlen(property.emap->fileName) > 0)){
	  if(!putword(strlen(property.emap->fileName), fP) ||
	     !fwrite(property.emap->fileName, strlen(property.emap->fileName),
		     1, fP)){
	    errNum = WLZ_ERR_WRITE_INCOMPLETE;
	  }
	}
	else {
	  if(!putword(0, fP)){
	    errNum = WLZ_ERR_WRITE_INCOMPLETE;
	  }
	}
	if((errNum == WLZ_ERR_NONE) && property.emap->comment &&
	   (strlen(property.emap->comment))){
	  if(!putword(strlen(property.emap->comment), fP) ||
	     !fwrite(property.emap->comment, strlen(property.emap->comment),
		     1, fP)){
	    errNum = WLZ_ERR_WRITE_INCOMPLETE;
	  }
	}
	else {
	  if(!putword(0, fP)){
	    errNum = WLZ_ERR_WRITE_INCOMPLETE;
	  }
	}
      }
      break;
    case WLZ_PROPERTY_NAME:
      if(putc((unsigned int) WLZ_PROPERTY_NAME, fP) == EOF)
      {
        errNum = WLZ_ERR_WRITE_INCOMPLETE;
      }
      else
      {
        errNum = WlzWriteStr(fP, property.name->name);
      }
      break;
    case WLZ_PROPERTY_GREY:
      if(putc((unsigned int) WLZ_PROPERTY_GREY, fP) == EOF)
      {
        errNum = WLZ_ERR_WRITE_INCOMPLETE;
      }
      else if((errNum = WlzWriteStr(fP, property.greyV->name)) == WLZ_ERR_NONE)
      {
        errNum = WlzWritePixelV(fP, &(property.greyV->value), 1);
      }
      break;
    case WLZ_PROPERTY_TEXT:
      if(putc((unsigned int) WLZ_PROPERTY_TEXT, fP) == EOF)
      {
        errNum = WLZ_ERR_WRITE_INCOMPLETE;
      }
      else if((errNum = WlzWriteStr(fP, property.text->name)) == WLZ_ERR_NONE)
      {
        errNum = WlzWriteStr(fP, property.text->text);
      }
      break;
    default:
      errNum = WLZ_ERR_PROPERTY_TYPE;
      break;
    }
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzIO
* \brief	Writes a property list to the given file.
* \param	fP			Given file.
* \param	pList			Property list.
*/
static WlzErrorNum WlzWritePropertyList(FILE *fP, WlzPropertyList *pList)
{
  AlcDLPItem	*item;
  WlzProperty	property;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if((pList == NULL) || (pList->list == NULL) ||
     (AlcDLPListCount(pList->list, NULL) < 1))
   {
    if(putc(0,fP) == EOF)
    {
      errNum = WLZ_ERR_WRITE_EOF;
    }
  }
  else
  {
    if((putc(2,fP) == EOF) || !putword(AlcDLPListCount(pList->list, NULL), fP))
    {
      errNum = WLZ_ERR_WRITE_INCOMPLETE;
    }
    else
    {
      item = pList->list->head;
      do
      {
	if(item)
	{
	  if(item->entry != NULL)
	  {
	    property.core = (WlzCoreProperty *)(item->entry);
	    errNum = WlzWriteProperty(fP, property);
	  }
	  else
	  {
	    putc(0,fP);
	  }
	  item = item->next;
	}
      } while((errNum == WLZ_ERR_NONE) && (item != pList->list->head));
    }
  }
  return errNum;
}

/*!
* \return	Woolz error code.
* \ingroup	WlzIO
* \brief	Writes the 2D values of a Woolz 2D domain object to the
*		given file.
* \param	fP			Given file.
* \param	obj			Object containing values that
*					are to be written to file.
*/
static WlzErrorNum WlzWriteValueTable(FILE *fP, WlzObject *obj)
{
  WlzIntervalWSpace	iwsp;
  WlzGreyWSpace		gwsp;
  WlzGreyType		gType;
  WlzGreyP		g;
  WlzPixelV		background,
  			min,
			max;
  int 			i;
  WlzGreyType		packing;
  WlzErrorNum		errNum = WLZ_ERR_NONE;

  /* obj == NULL has been checked by WlzWriteObj() */
  if(obj->values.core == NULL)
  {
    if(putc(0,fP) == EOF)
    {
      errNum = WLZ_ERR_WRITE_EOF;
    }
  }
  else
  {
    if(errNum == WLZ_ERR_NONE)
    {
      gType = WlzGreyTableTypeToGreyType(obj->values.core->type, &errNum);
    }
    if(errNum == WLZ_ERR_NONE)
    {
      /* The "type" written to disc only codes the pixel type.
         The shape type on subsequent reading is entirely
         determined by the object domain.
         For the moment, choice is between standard ragged-rectangle
         or true rectangle.  */
      if(putc((unsigned int )gType, fP) == EOF)
      {
	errNum = WLZ_ERR_WRITE_INCOMPLETE;
      }
    }
    background = WlzGetBackground(obj, &errNum);
    if(errNum == WLZ_ERR_NONE)
    {
      switch(gType)
      {
	case WLZ_GREY_INT:
	  /* Calculate packing to minimise disc space */
	  if((errNum = WlzGreyRange(obj, &min, &max)) == WLZ_ERR_NONE)
	  {
	    if((min.v.inv >= 0) && (max.v.inv <= 255))
	    {
	      packing = WLZ_GREY_UBYTE;
	    }
	    else if((min.v.inv >= SHRT_MIN) && (max.v.inv <= SHRT_MAX))
	    {
	      packing = WLZ_GREY_SHORT;
	    }
	    else
	    {
	      packing = WLZ_GREY_INT;
	    }
	    if((putc((unsigned int )packing, fP) == EOF) ||
	       !putword(background.v.inv, fP))
            {
	      errNum = WLZ_ERR_WRITE_INCOMPLETE;
	    }
	    if((errNum = WlzInitGreyScan(obj, &iwsp, &gwsp)) == WLZ_ERR_NONE)
	    {
	      while((errNum == WLZ_ERR_NONE) &&
	            ((errNum = WlzNextGreyInterval(&iwsp)) == WLZ_ERR_NONE))
	      {
		g = gwsp.u_grintptr;
		switch(packing)
		{
		  case WLZ_GREY_INT:
		    for(i = 0; (i < iwsp.colrmn) && (errNum == WLZ_ERR_NONE);
			i++, g.inp++)
		    {
		      if(!putword(*g.inp, fP))
		      {
			errNum = WLZ_ERR_WRITE_INCOMPLETE;
		      }
		    }
		    break;
		  case WLZ_GREY_SHORT:
		    for(i = 0; (i < iwsp.colrmn) && (errNum == WLZ_ERR_NONE);
			i++, g.inp++)
		    {
		      if(!putshort((short) (*g.inp), fP))
		      {
			errNum = WLZ_ERR_WRITE_INCOMPLETE;
		      }
		    }
		    break;
		  case WLZ_GREY_UBYTE:
		    for(i = 0; (i < iwsp.colrmn) && (errNum == WLZ_ERR_NONE);
		    	i++, g.inp++)
		    {
		      if(putc((unsigned int )*g.inp, fP) == EOF)
		      {
			errNum = WLZ_ERR_WRITE_INCOMPLETE;
		      }
		    }
		    break;
		  default:
		    errNum = WLZ_ERR_GREY_TYPE;
		    break;
		}
	      }
	      (void )WlzEndGreyScan(&iwsp, &gwsp);
	      if(errNum == WLZ_ERR_EOO)
	      {
	        errNum = WLZ_ERR_NONE;
	      }
	    }
	  }
	  break;
	case WLZ_GREY_SHORT:
	  /* Calculate packing to minimise disc space */
	  if((errNum = WlzGreyRange(obj, &min, &max)) == WLZ_ERR_NONE)
	  {
	    if((min.v.shv >= 0) && (max.v.shv <= 255))
	    {
	      packing = WLZ_GREY_UBYTE;
	    }
	    else
	    {
	      packing = WLZ_GREY_SHORT;
	    }
	    if((putc((unsigned int )packing, fP) == EOF) ||
	       !putword(background.v.shv, fP))
	    {
	      errNum = WLZ_ERR_WRITE_INCOMPLETE;
	    }
	    if((errNum = WlzInitGreyScan(obj, &iwsp, &gwsp)) == WLZ_ERR_NONE)
	    {
	      while((errNum == WLZ_ERR_NONE) &&
	            ((errNum = WlzNextGreyInterval(&iwsp)) == WLZ_ERR_NONE))
	      {
		g = gwsp.u_grintptr;
		switch(packing)
		{
		  case WLZ_GREY_SHORT:
		    for(i = 0; (i < iwsp.colrmn) && (errNum == WLZ_ERR_NONE);
		    	i++, g.shp++)
		    {
		      if(!putshort(*g.shp, fP))
		      {
			errNum = WLZ_ERR_WRITE_INCOMPLETE;
		      }
		    }
		    break;
		  case WLZ_GREY_UBYTE:
		    for(i = 0; (i < iwsp.colrmn) && (errNum == WLZ_ERR_NONE);
		    	i++, g.shp++)
		    {
		      if(putc((unsigned int) *g.shp, fP) == EOF)
		      {
			errNum = WLZ_ERR_WRITE_INCOMPLETE;
		      }
		    }
		    break;
		  default:
		    errNum = WLZ_ERR_GREY_TYPE;
		    break;
		}
	      }
	      (void )WlzEndGreyScan(&iwsp, &gwsp);
	      if(errNum == WLZ_ERR_EOO)
	      {
	        errNum = WLZ_ERR_NONE;
	      }
	    }
	  }
	  break;
	case WLZ_GREY_UBYTE:
	  packing = WLZ_GREY_UBYTE;
	  if((putc((unsigned int )packing, fP) == EOF) ||
	     !putword(background.v.ubv, fP))
	  {
	    errNum = WLZ_ERR_WRITE_INCOMPLETE;
	  }
	  else
	  {
	    errNum = WlzInitGreyScan(obj, &iwsp, &gwsp);
	  }
	  if(errNum == WLZ_ERR_NONE)
	  {
	    while((errNum == WLZ_ERR_NONE) &&
	           ((errNum = WlzNextGreyInterval(&iwsp)) == WLZ_ERR_NONE))
	    {
	      g = gwsp.u_grintptr;
	      if((int )fwrite(g.ubp, sizeof(WlzUByte),
	      		      iwsp.colrmn, fP) < iwsp.colrmn)
	      {
		errNum = WLZ_ERR_WRITE_INCOMPLETE;
	      }
	    }
	    (void )WlzEndGreyScan(&iwsp, &gwsp);
	    if(errNum == WLZ_ERR_EOO)
	    {
	      errNum = WLZ_ERR_NONE;
	    }
	  }
	  break;
	case WLZ_GREY_FLOAT:
	  packing = WLZ_GREY_FLOAT;
	  if((putc((unsigned int )packing, fP) == EOF) ||
	     !putfloat(background.v.flv, fP))
	  {
	    errNum = WLZ_ERR_WRITE_INCOMPLETE;
	  }
	  else
	  {
	    errNum = WlzInitGreyScan(obj, &iwsp, &gwsp);
	  }
	  if(errNum == WLZ_ERR_NONE)
	  {
	    while((errNum == WLZ_ERR_NONE) &&
	           ((errNum = WlzNextGreyInterval(&iwsp)) == WLZ_ERR_NONE))
	    {
	      g = gwsp.u_grintptr;
	      for(i = 0; (i < iwsp.colrmn) && (errNum == WLZ_ERR_NONE);
		  i++, g.flp++)
	      {
		if(!putfloat(*g.flp, fP))
		{
		  errNum = WLZ_ERR_WRITE_INCOMPLETE;
		}
	      }
	    }
	    (void )WlzEndGreyScan(&iwsp, &gwsp);
	    if(errNum == WLZ_ERR_EOO)
	    {
	      errNum = WLZ_ERR_NONE;
	    }
	  }
	  break;
	case WLZ_GREY_DOUBLE:
	  packing = WLZ_GREY_DOUBLE;
	  if((putc((unsigned int )packing, fP) == EOF) ||
	     !putdouble(background.v.dbv, fP))
	  {
	    errNum = WLZ_ERR_WRITE_INCOMPLETE;
	  }
	  else
	  {
	    errNum = WlzInitGreyScan(obj, &iwsp, &gwsp);
	  }
	  if(errNum == WLZ_ERR_NONE)
	  {
	    while((errNum == WLZ_ERR_NONE) &&
	           ((errNum = WlzNextGreyInterval(&iwsp)) == WLZ_ERR_NONE))
	    {
	      g = gwsp.u_grintptr;
	      for(i = 0; (i < iwsp.colrmn) && (errNum == WLZ_ERR_NONE);
	          i++, g.dbp++)
	      {
		if(!putdouble(*g.dbp, fP))
		{
		  errNum = WLZ_ERR_WRITE_INCOMPLETE;
		}
	      }
	    }
	    (void )WlzEndGreyScan(&iwsp, &gwsp);
	    if(errNum == WLZ_ERR_EOO)
	    {
	      errNum = WLZ_ERR_NONE;
	    }
	  }
	  break;
	case WLZ_GREY_RGBA:
	  packing = WLZ_GREY_RGBA;
	  if((putc((unsigned int )packing, fP) == EOF) ||
	     !putword(background.v.rgbv, fP))
	  {
	    errNum = WLZ_ERR_WRITE_INCOMPLETE;
	  }
	  else
	  {
	    errNum = WlzInitGreyScan(obj, &iwsp, &gwsp);
	  }
	  if(errNum == WLZ_ERR_NONE)
	  {
	    while((errNum == WLZ_ERR_NONE) &&
	           ((errNum = WlzNextGreyInterval(&iwsp)) == WLZ_ERR_NONE))
	    {
	      g = gwsp.u_grintptr;
	      for(i = 0; (i < iwsp.colrmn) && (errNum == WLZ_ERR_NONE);
	          i++, g.rgbp++)
	      {
		if(!putword(*g.rgbp, fP))
		{
		  errNum = WLZ_ERR_WRITE_INCOMPLETE;
		}
	      }
	    }
	    (void )WlzEndGreyScan(&iwsp, &gwsp);
	    if(errNum == WLZ_ERR_EOO)
	    {
	      errNum = WLZ_ERR_NONE;
	    }
	  }
	  break;
	default:
	  errNum = WLZ_ERR_GREY_TYPE;
	  break;
      }
    }
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzIO
* \brief	Writes the voxel values of a Woolz object to the given file.
* \param	fP			Given file.
* \param	obj			Object with values.
*/
static WlzErrorNum WlzWriteVoxelValueTable(FILE *fP, WlzObject *obj)
{
  int			i, nplanes;
  WlzObject		tempobj;
  WlzDomain 		*domains;
  WlzValues		*values;
  WlzVoxelValues	*voxtab;
  WlzPlaneDomain	*planedm;
  WlzErrorNum		errNum = WLZ_ERR_NONE;

  /* check object */
  if(obj->values.core == NULL)
  {
    if(putc(0,fP) == EOF)
    {
      errNum = WLZ_ERR_WRITE_EOF;
    }
  }
  else
  {
    voxtab = (WlzVoxelValues *) obj->values.vox;
    /* note here the background is written without a type and as an
       integer. On read the value is replaced by a value from one of
       the plane valuetables */
    if((putc((unsigned int) voxtab->type, fP) == EOF) ||
	!putword(voxtab->bckgrnd.v.inv, fP))
    {
      errNum = WLZ_ERR_WRITE_INCOMPLETE;
    }
    else
    {
      planedm = obj->domain.p;
      switch(voxtab->type)
      {
	case WLZ_VOXELVALUETABLE_GREY:
	  nplanes = planedm->lastpl - planedm->plane1 + 1;
	  values = voxtab->values;
	  domains = planedm->domains;
	  tempobj.type = WLZ_2D_DOMAINOBJ;
	  tempobj.linkcount = 0;
	  tempobj.plist = NULL;
	  tempobj.assoc = NULL;
	  for(i=0; (i < nplanes) && (errNum == WLZ_ERR_NONE);
	      i++, domains++, values++)
	  {
	    tempobj.domain.i = (*domains).i;
	    tempobj.values.v = (*values).v;
	    errNum = WlzWriteValueTable(fP, &tempobj);
	  }
	  break;
	default:
	  errNum = WLZ_ERR_VALUES_TYPE;
	  break;
      }
    }
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup 	WlzIO
* \brief	Writes a polygon domain to the given file.
* \param	fP			Given file.
* \param	poly			Polygon domain.
*/
static WlzErrorNum WlzWritePolygon(FILE *fP, WlzPolygonDomain *poly)
{
  int		nvertices, i;
  WlzIVertex2	*ivtx;
  WlzFVertex2	*fvtx;
  WlzDVertex2	*dvtx;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(poly == NULL)
  {
    if(putc(0,fP) == EOF)
    {
      errNum = WLZ_ERR_WRITE_EOF;
    }
  }
  else
  {
    /* Write the number of vertices */
    nvertices = poly->nvertices;
    if((putc((unsigned int )poly->type, fP) == EOF) ||
       !putword(nvertices, fP))
    {
      errNum = WLZ_ERR_WRITE_INCOMPLETE;
    }
  }
  if(errNum == WLZ_ERR_NONE && poly)
  {
    switch(poly->type)
    {
      case WLZ_POLYGON_INT:
	ivtx = poly->vtx;
	for(i = 0; (i < nvertices) && (errNum == WLZ_ERR_NONE); i++, ivtx++)
	{
	  if(!putword(ivtx->vtY, fP) || !putword(ivtx->vtX, fP))
	  {
	    errNum = WLZ_ERR_WRITE_INCOMPLETE;
	  }
	}
	break;
      case WLZ_POLYGON_FLOAT:
	fvtx = (WlzFVertex2 *)poly->vtx;
	for(i = 0; (i < nvertices) && (errNum == WLZ_ERR_NONE); i++, fvtx++)
	{
	  if(!putfloat(fvtx->vtY, fP) || !putfloat(fvtx->vtX, fP))
	  {
	    errNum = WLZ_ERR_WRITE_INCOMPLETE;
	  }
	}
	break;
      case WLZ_POLYGON_DOUBLE:
	dvtx = (WlzDVertex2 *)poly->vtx;
	for(i = 0; (i < nvertices) && (errNum == WLZ_ERR_NONE); i++, dvtx++)
	{
	  if(!putdouble(dvtx->vtY, fP) || !putdouble(dvtx->vtX, fP))
	  {
	    errNum = WLZ_ERR_WRITE_INCOMPLETE;
	  }
	}
	break;
      default:
	errNum = WLZ_ERR_POLYGON_TYPE;
	break;
    }
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzIO
* \brief	Writes a boundary list to the given file.
* \param	fP			Given file.
* \param	blist			Boundary list.
*/
static WlzErrorNum WlzWriteBoundList(FILE *fP, WlzBoundList *blist)
{
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(blist == NULL)
  {
    if(putc(0,fP) == EOF)
    {
      errNum = WLZ_ERR_WRITE_EOF;
    }
  }
  else
  {
    if((putc(1, fP) == EOF) ||
       (putc((unsigned int)blist->type, fP) == EOF))
    {
      errNum = WLZ_ERR_WRITE_INCOMPLETE;
    }
    else if(((errNum = WlzWriteBoundList(fP, blist->next)) == WLZ_ERR_NONE) &&
	    ((errNum = WlzWriteBoundList(fP, blist->down)) == WLZ_ERR_NONE))
    {
      if(!putword(blist->wrap, fP))
      {
        errNum = WLZ_ERR_WRITE_INCOMPLETE;
      }
      else
      {
	errNum = WlzWritePolygon(fP, blist->poly);
      }
    }
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzIO
* \brief	Writes an integer rectangle to the given file.
* \param	fP			Given file.
* \param	rdom			Integer rectangle.
*/
static WlzErrorNum WlzWriteRect(FILE *fP, WlzIRect *rdom)
{
  WlzFRect	*frdom;
  int		i;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(rdom == NULL)
  {
    if(putc(0,fP) == EOF)
    {
      errNum = WLZ_ERR_WRITE_EOF;
    }
  }
  else
  {
    if(putc(rdom->type,fP) == EOF)
    {
      errNum = WLZ_ERR_WRITE_INCOMPLETE;
    }
    else
    {
      switch(rdom->type)
      {
	case WLZ_RECTANGLE_DOMAIN_INT:
	  for(i = 0; (i < 4) && (errNum == WLZ_ERR_NONE); ++i)
	  {
	    if(!putword(rdom->irk[i], fP))
	    {
	      errNum = WLZ_ERR_WRITE_INCOMPLETE;
	    }
	  }
	  for(i = 0; (i < 4) && (errNum == WLZ_ERR_NONE); ++i)
	  {
	    if(!putword(rdom->irl[i], fP))
	    {
	      errNum = WLZ_ERR_WRITE_INCOMPLETE;
	    }
	  }
	  if(!putfloat(rdom->rangle, fP))
	  {
	    errNum = WLZ_ERR_WRITE_INCOMPLETE;
	  }
	  break;
	case WLZ_RECTANGLE_DOMAIN_FLOAT:
	  frdom = (WlzFRect *) rdom;
	  for(i = 0; (i < 4) && (errNum == WLZ_ERR_NONE); ++i)
	  {
	    if(!putfloat(frdom->frk[i], fP))
	    {
	      errNum = WLZ_ERR_WRITE_INCOMPLETE;
	    }
	  }
	  for(i = 0; (i < 4) && (errNum == WLZ_ERR_NONE); ++i)
	  {
	    if(!putfloat(frdom->frl[i], fP))
	    {
	      errNum = WLZ_ERR_WRITE_INCOMPLETE;
	    }
	  }
	  if(!putfloat(frdom->rangle, fP))
	  {
	    errNum = WLZ_ERR_WRITE_INCOMPLETE;
	  }
	  break;
	default:
	  errNum = WLZ_ERR_DOMAIN_TYPE;
	  break;
      }
    }
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzIO
* \brief	Writes a histogram domain to the given file.
* \param	fP			Given file.
* \param	hist			Histogram domain.
*/
static WlzErrorNum WlzWriteHistogramDomain(FILE *fP, WlzHistogramDomain *hist)
{
  int		tI0;
  int		*tIP0;
  double	*tDP0;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(hist == NULL)
  {
    if(putc(0, fP) == EOF)
    {
      errNum = WLZ_ERR_WRITE_EOF;
    }
  }
  else
  {
    if((putc(hist->type, fP) == EOF) ||
       (putword((hist->nBins > 0)? (hist->nBins): 0, fP) == 0) ||
       (putdouble(hist->origin, fP) == 0) ||
       (putdouble(hist->binSize, fP) == 0))
    {
      errNum = WLZ_ERR_WRITE_INCOMPLETE;
    }
    else if((hist->type != WLZ_HISTOGRAMDOMAIN_INT) &&
            (hist->type != WLZ_HISTOGRAMDOMAIN_FLOAT))
    {
      errNum = WLZ_ERR_DOMAIN_TYPE;
    }
    else
    {
      if((tI0 = hist->nBins) > 0)
      {
	switch(hist->type)
	{
	  case WLZ_HISTOGRAMDOMAIN_INT:
	    tIP0 = hist->binValues.inp;
	    do
	    {
	      if(!putword(*tIP0++, fP))
	      {
	        errNum = WLZ_ERR_WRITE_INCOMPLETE;
	      }
	    } while((--tI0 > 0) && (errNum == WLZ_ERR_NONE));
	    break;
	  case WLZ_HISTOGRAMDOMAIN_FLOAT:
	    tDP0 = hist->binValues.dbp;
	    do
	    {
	      if(!putdouble(*tDP0++, fP))
	      {
	        errNum = WLZ_ERR_WRITE_INCOMPLETE;
	      }
	    } while((--tI0 > 0) && (errNum == WLZ_ERR_NONE));
	    break;
	  default:
	    break;
	}
      }
    }
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzIO
* \brief	Writes a compound array object to the given file.
* \param	fP			Given file.
* \param	c			Compound array object.
*/
static WlzErrorNum WlzWriteCompoundA(FILE *fP, WlzCompoundArray *c)
{
  int 		i;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  /* No need to check for NULL because checked by WlzWriteObj() */
  if((putc((unsigned int )c->otype, fP) == EOF) || !putword(c->n, fP))
  {
    errNum = WLZ_ERR_WRITE_EOF;
  }
  else
  {
    /* Write the objects, note NULL is a legal value */
    for(i = 0; (i < c->n) && (errNum == WLZ_ERR_NONE); i++)
    {
      if(c->o[i] == NULL)
      {
	if(putc(0, fP) == EOF)
	{
	  errNum = WLZ_ERR_WRITE_INCOMPLETE;
	}
      }
      else
      {
	errNum = WlzWriteObj(fP, c->o[i]);
      }
    }
  }
  if( errNum == WLZ_ERR_NONE )
  {
    errNum = WlzWritePropertyList(fP, c->plist);
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzIO
* \brief	Writes a Woolz affine transform to the given file.
* \param	fP			Given file.
* \param	trans			Affine transform.
*/
static WlzErrorNum WlzWriteAffineTransform(FILE *fP, WlzAffineTransform *trans)
{
  int		i,
  		j;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  /* check for NULL */
  if(trans == NULL)
  {
    if(putc((unsigned int )0, fP) == EOF)
    {
      errNum = WLZ_ERR_WRITE_EOF;
    }
  }
  else
  {
    if(putc((unsigned int) trans->type, fP) == EOF)
    {
      errNum = WLZ_ERR_WRITE_EOF;
    }
    if(errNum == WLZ_ERR_NONE)
    {
      for(i = 0; (i < 4) && (errNum == WLZ_ERR_NONE); i++)
      {
	for(j = 0; (j < 4) && (errNum == WLZ_ERR_NONE); j++)
	{
	  if(!putdouble(trans->mat[i][j], fP))
	  {
	    errNum = WLZ_ERR_WRITE_INCOMPLETE;
	  }
	}
      }
    }
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzIO
* \brief	Writes a FE warp transform to the given file.
* \param	fP			Given file.
* \param	obj			Warp transform.
*/
static WlzErrorNum WlzWriteWarpTrans(FILE *fP, WlzWarpTrans *obj)
{
  int		i,
  		j;
  WlzDVertex2	*dptr;
  WlzTElement	*eptr;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(!putc((unsigned int )obj->type, fP))
  {
    errNum = WLZ_ERR_WRITE_EOF;
  }
  else if(!putword(obj->nelts,fP) ||
	  !putword(obj->nodes,fP) ||
	  !putfloat(obj->imdisp,fP) ||
	  !putfloat(obj->iterdisp,fP))
  {
    errNum = WLZ_ERR_WRITE_INCOMPLETE;
  }
  else
  {
    /* Write out nodal coords */
    dptr = obj->ncoords;
    for(i = 0; (i < obj->nodes) && (errNum == WLZ_ERR_NONE); i++, dptr++)
    {
      if(!putfloat((float) dptr->vtX, fP) ||
	 !putfloat((float) dptr->vtY, fP))
      {
	errNum = WLZ_ERR_WRITE_INCOMPLETE;
      }
    }
  }
  if(errNum == WLZ_ERR_NONE)
  {
    /* Write out nodal displacements */
    dptr = obj->displacements;
    for(i = 0; (i < obj->nodes) && (errNum == WLZ_ERR_NONE); i++, dptr++)
    {
      if(!putfloat((float) dptr->vtX, fP) ||
	 !putfloat((float) dptr->vtY, fP))
      {
	errNum = WLZ_ERR_WRITE_INCOMPLETE;
      }
    }
  }
  if(errNum == WLZ_ERR_NONE)
  {
    /* Write out elements */
    eptr = obj->eltlist;
    for(i = 0; (i < obj->nelts) && (errNum == WLZ_ERR_NONE); i++, eptr++)
    {
      if((putc((unsigned int) eptr->type, fP) == EOF) ||
         !putword(eptr->n, fP))
      {
        errNum = WLZ_ERR_WRITE_INCOMPLETE;
      }
      for(j = 0; (j < 3) && (errNum == WLZ_ERR_NONE); j++)
      {
	if (!putword(eptr->nodes[j], fP))
	{
	  errNum = WLZ_ERR_WRITE_INCOMPLETE;
	}
      }
      for(j = 0; (j < 3) && (errNum == WLZ_ERR_NONE); j++)
      {
	if (!putfloat(eptr->u[j], fP))
	{
	  errNum = WLZ_ERR_WRITE_INCOMPLETE;
	}
      }
      for(j = 0; (j < 3) && (errNum == WLZ_ERR_NONE); j++)
      {
	if (!putfloat(eptr->a[j], fP))
	{
	  errNum = WLZ_ERR_WRITE_INCOMPLETE;
	}
      }
    }
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzIO
* \brief	Writes FE warp match features to the given file.
* \param	fP			Given file.
* \param	obj			Match features.
*/
static WlzErrorNum WlzWriteFMatchObj(FILE *fP, WlzFMatchObj *obj)
{
  int		i,
  		j;
  WlzFMatchPoint *mptr;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(!putword(obj->nopts,fP))
  {
    errNum = WLZ_ERR_WRITE_EOF;
  }
  else
  {
    mptr = obj->matchpts;
    for(i = 0; (i < obj->nopts) && (errNum == WLZ_ERR_NONE); i++, mptr++)
    {
      if(!putword(mptr->type,fP) ||
	 !putword(mptr->node,fP) ||
	 !putfloat(mptr->ptcoords.vtX,fP) ||
	 !putfloat(mptr->ptcoords.vtY,fP))
      {
	errNum = WLZ_ERR_WRITE_INCOMPLETE;
      }
      for(j = 0; (j < WLZ_MAX_NODAL_DEGREE) && (errNum == WLZ_ERR_NONE); j++)
      {
	if (!putword(mptr->elements[j],fP))
	{
	  errNum = WLZ_ERR_WRITE_INCOMPLETE;
	}
      }
    }
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzIO
* \brief	Writes a 3D FE warp transform to the given file.
* \param	fP			Given file.
* \param	obj			Warp transform.
*/
static WlzErrorNum WlzWrite3DWarpTrans(FILE *fP, Wlz3DWarpTrans *obj)
{
  int 		i;
  WlzFMatchObj	**intdoms;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(!putword(obj->iteration,fP))
  {
    errNum = WLZ_ERR_WRITE_EOF;
  }
  else if(!putword(obj->currentplane,fP) ||
           !putfloat(obj->maxdisp,fP))
  {
    errNum = WLZ_ERR_WRITE_INCOMPLETE;
  }
  else if(!WlzWritePlaneDomain(fP, obj->pdom))
  {
    errNum = WLZ_ERR_WRITE_INCOMPLETE;
  }
  else
  {
    intdoms = obj->intptdoms;
    for(i = obj->pdom->plane1;
        (i <= obj->pdom->lastpl) && (errNum == WLZ_ERR_NONE); i++, intdoms++)
    {
      if(!WlzWriteFMatchObj(fP, *intdoms))
      {
	errNum = WLZ_ERR_WRITE_INCOMPLETE;
      }
    }
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzIO
* \brief	Writes either a 2D or 3D contour to the given file.
* \param	fP			Given file.
* \param	ctr			Contour.
*/
static WlzErrorNum WlzWriteContour(FILE *fP, WlzContour *ctr)
{
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(ctr == NULL)
  {
    errNum = WLZ_ERR_DOMAIN_NULL;
  }
  else if(ctr->type != WLZ_CONTOUR)
  {
    errNum = WLZ_ERR_DOMAIN_TYPE;
  }
  else
  {
    if(putc((unsigned int )(ctr->type), fP) == EOF)
    {
      errNum = WLZ_ERR_WRITE_INCOMPLETE;
    }
    else
    {
      errNum = WlzWriteGMModel(fP, ctr->model);
    }
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzIO
* \brief	Write's a geometric model data structure to the given file.
*		The format is:
*		\verbatim
 		  model->type (byte)
 		  {
 		    case WLZ_GMMOD_2I
 		    case WLZ_GMMOD_2D
 		    case WLZ_GMMOD_2N
 		      nEdge (int)
 		    case WLZ_GMMOD_3I
 		    case WLZ_GMMOD_3D
 		    case WLZ_GMMOD_3N
 		      nLoop (int)
 		  }
 		  {
 		    case model->type == WLZ_GMMOD_2I
 		      vertexGU.vg2I->vtx (WlzIVertex2, int * 2)
 		    case model->type == WLZ_GMMOD_2D
 		      vertexGU.vg2D->vtx (WlzDVertex2, double * 2)
 		    case model->type == WLZ_GMMOD_2N
 		      vertexGU.vg2D->vtx (WlzDVertex2, double * 2)
 		      vertexGU.vg2D->nrm (WlzDVertex2, double * 2)
 		    case model->type == WLZ_GMMOD_3I
 		      vertexGU.vg3I->vtx (WlzIVertex2, int * 3)
 		    case model->type == WLZ_GMMOD_3D
 		      vertexGU.vg3D->vtx (WlzDVertex2, double * 3)
 		    case model->type == WLZ_GMMOD_3N
 		      vertexGU.vg3D->vtx (WlzDVertex2, double * 3)
 		      vertexGU.vg3D->nrm (WlzDVertex2, double * 3)
 		  } * nVertex
 		  {
 		    case WLZ_GMMOD_2I
 		    case WLZ_GMMOD_2D
 		    case WLZ_GMMOD_2N
 		    {
 		      2 vertex indicies
 		    } * nEdge
 		    case WLZ_GMMOD_3I
 		    case WLZ_GMMOD_3D
 		    case WLZ_GMMOD_3N
 		    {
 		      3 vertex indicies
 		    } * nLoop
 		  }
		\endverbatim
* \param	fP			Given file.
* \param	model			Geometric model.
*/
static WlzErrorNum WlzWriteGMModel(FILE *fP, WlzGMModel *model)
{

  int		idI,
  		iCnt,
		vCnt,
		encodeMtd = 0;
  int		bufI[3];
  AlcVector	*vec;
  WlzGMEdgeT	*tET;
  WlzGMElemP	eP;
  WlzGMResIdxTb	*resIdxTb = NULL;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(model == NULL)
  {
    errNum = WLZ_ERR_DOMAIN_NULL;
  }
  else
  {
    /* Check the model type. */
    switch(model->type)
    {
      case WLZ_GMMOD_2I: /* FALLTHROUGH */
      case WLZ_GMMOD_2D: /* FALLTHROUGH */
      case WLZ_GMMOD_2N: /* FALLTHROUGH */
      case WLZ_GMMOD_3I: /* FALLTHROUGH */
      case WLZ_GMMOD_3D: /* FALLTHROUGH */
      case WLZ_GMMOD_3N:
        break;
      default:
        errNum = WLZ_ERR_DOMAIN_TYPE;
	break;
    }
  }
  if(errNum == WLZ_ERR_NONE)
  {
    /* Output model type and file encoding method, followed by the number of
     * verticies and the number of simplicies. Currently the file encoding
     * method is just output as '0', but in future new encoding methods
     * may be used, for example strips of simplicies to reduce the file
     * size. */
    switch(model->type)
    {
      case WLZ_GMMOD_2I: /* FALLTHROUGH */
      case WLZ_GMMOD_2D: /* FALLTHROUGH */
      case WLZ_GMMOD_2N:
	if((putc((unsigned int )(model->type), fP) == EOF) ||
	    (putc((unsigned int )encodeMtd, fP) == EOF) ||
	    !putword(model->res.vertex.numElm, fP) ||
	    !putword(model->res.edge.numElm, fP))
	{
	  errNum = WLZ_ERR_WRITE_INCOMPLETE;
	}
	break;
      case WLZ_GMMOD_3I: /* FALLTHROUGH */
      case WLZ_GMMOD_3D: /* FALLTHROUGH */
      case WLZ_GMMOD_3N:
	if((putc((unsigned int )(model->type), fP) == EOF) ||
	    (putc((unsigned int )encodeMtd, fP) == EOF) ||
	    !putword(model->res.vertex.numElm, fP) ||
	    !putword(model->res.face.numElm, fP))
	{
	  errNum = WLZ_ERR_WRITE_INCOMPLETE;
	}
	break;
    }
  }
  if((errNum == WLZ_ERR_NONE) && (model->res.vertex.numElm > 0))
  {
    /* Index the verticies. */
    resIdxTb = WlzGMModelResIdx(model, WLZ_GMELMFLG_VERTEX, &errNum);
    if(errNum == WLZ_ERR_NONE)
    {
      /* Output the vertex geometries. */
      idI = 0;
      vec = model->res.vertex.vec;
      iCnt = model->res.vertex.numIdx;
      vCnt = 0;
      while((errNum == WLZ_ERR_NONE) && (iCnt-- > 0))
      {
	eP.vertex = (WlzGMVertex *)AlcVectorItemGet(vec, idI++);
	if(eP.vertex->idx >= 0)
	{
	  ++vCnt;
	  switch(model->type)
	  {
	    case WLZ_GMMOD_2I:
	      errNum = WlzWriteVertex2I(fP, &(eP.vertex->geo.vg2I->vtx), 1);
	      break;
	    case WLZ_GMMOD_2D:
	      errNum = WlzWriteVertex2D(fP, &(eP.vertex->geo.vg2D->vtx), 1);
	      break;
	    case WLZ_GMMOD_2N:
	      errNum = WlzWriteVertex2D(fP, &(eP.vertex->geo.vg2N->vtx), 1);
	      if(errNum == WLZ_ERR_NONE)
	      {
	        errNum = WlzWriteVertex2D(fP, &(eP.vertex->geo.vg2N->nrm), 1);
	      }
	      break;
	    case WLZ_GMMOD_3I:
	      errNum = WlzWriteVertex3I(fP, &(eP.vertex->geo.vg3I->vtx), 1);
	      break;
	    case WLZ_GMMOD_3D:
	      errNum = WlzWriteVertex3D(fP, &(eP.vertex->geo.vg3D->vtx), 1);
	      break;
	    case WLZ_GMMOD_3N:
	      errNum = WlzWriteVertex3D(fP, &(eP.vertex->geo.vg3N->vtx), 1);
	      if(errNum == WLZ_ERR_NONE)
	      {
	        errNum = WlzWriteVertex3D(fP, &(eP.vertex->geo.vg3N->nrm), 1);
	      }
	      break;
	  }
	}
      }
    }
    if(errNum == WLZ_ERR_NONE)
    {
      /* Output the vertex indicies of the simplicies. */
      idI = 0;
      vCnt = 0;
      switch(model->type)
      {
	case WLZ_GMMOD_2I:
	case WLZ_GMMOD_2D:
	case WLZ_GMMOD_2N:
	  vec = model->res.edge.vec;
	  iCnt = model->res.edge.numIdx;
	  while((errNum == WLZ_ERR_NONE) && (iCnt-- > 0))
	  {
	    ++vCnt;
	    eP.edge = (WlzGMEdge *)AlcVectorItemGet(vec, idI++);
	    if(eP.edge->idx >= 0)
	    {
	      tET = eP.edge->edgeT;
	      bufI[0] = *(resIdxTb->vertex.idxLut +
			  tET->vertexT->diskT->vertex->idx);
	      bufI[1] = *(resIdxTb->vertex.idxLut +
			  tET->opp->vertexT->diskT->vertex->idx);
	      errNum = WlzWriteInt(fP, bufI, 2);
	    }
	  }
	  break;
	case WLZ_GMMOD_3I:
	case WLZ_GMMOD_3D:
	case WLZ_GMMOD_3N:
	  vec = model->res.face.vec;
	  iCnt = model->res.face.numIdx;
	  while((errNum == WLZ_ERR_NONE) && (iCnt-- > 0))
	  {
	    eP.face = (WlzGMFace *)AlcVectorItemGet(vec, idI++);
	    if(eP.face->idx >= 0)
	    {
	      ++vCnt;
	      /* Loop IS a triangle, in 3D nothing else is allowed. */
	      tET = eP.face->loopT->edgeT;
	      bufI[0] = *(resIdxTb->vertex.idxLut +
			  tET->vertexT->diskT->vertex->idx);
	      bufI[1] = *(resIdxTb->vertex.idxLut +
			  tET->next->vertexT->diskT->vertex->idx);
	      bufI[2] = *(resIdxTb->vertex.idxLut +
			  tET->prev->vertexT->diskT->vertex->idx);
	      errNum = WlzWriteInt(fP, bufI, 3);
	    }
	  }
	  break;
      }
    }
  }
  if(resIdxTb)
  {
    WlzGMModelResIdxFree(resIdxTb);
  }
  return(errNum);
}

/* function:     WlzWriteMeshTransform3D    */
/*! 
* \ingroup      WlzIO
* \author	J. Rao.
* \brief        Write a 3D mesh transform to the given file-stream.
*
* \return       Error number.
* \param    fP	Output file-stream pointer
* \param    obj	Mesh transform to be written.
* \par      Source:
*                WlzWriteObj.c
*/
WlzErrorNum WlzWriteMeshTransform3D(
  FILE 			*fP,
  WlzMeshTransform3D 	*obj)
{
  int		i,
  		j;
  WlzMeshNode3D	*dptr;
  WlzMeshElem3D	*eptr;
  WlzErrorNum	errNum = WLZ_ERR_NONE;
  /*
  if( !putc((unsigned int )obj->type, fP) )
  {
    errNum = WLZ_ERR_WRITE_EOF;
  }
  */
  if(     !putword(obj->nElem,  fP) ||
	  !putword(obj->nNodes ,fP) )
  {
    errNum = WLZ_ERR_WRITE_INCOMPLETE;
  }
  else
  {
    /* Write out nodal position and displacement */
    dptr = obj->nodes;
    for(i = 0; (i < obj->nNodes) && (errNum == WLZ_ERR_NONE); i++, dptr++)
    {  /* position */
      if(!putfloat((float) dptr->position.vtX, fP) ||
         !putfloat((float) dptr->position.vtY, fP) ||
	 !putfloat((float) dptr->position.vtZ, fP))
      {
	errNum = WLZ_ERR_WRITE_INCOMPLETE;
      }
      /* displacement */
      if(!putfloat((float) dptr->displacement.vtX, fP) ||
         !putfloat((float) dptr->displacement.vtY, fP) ||
	 !putfloat((float) dptr->displacement.vtZ, fP))
      {
	errNum = WLZ_ERR_WRITE_INCOMPLETE;
      }

    }
  }
  if(errNum == WLZ_ERR_NONE)
  {
    /* Write out elements */
    eptr = obj->elements;
    for(i = 0; (i < obj->nElem) && (errNum == WLZ_ERR_NONE); i++, eptr++)
    {/*
      if(!putc((unsigned int )eptr->type, fP)  )
      {
        errNum = WLZ_ERR_WRITE_INCOMPLETE;
      } */
      /* output the index of this element */
      if(!putword(eptr->idx, fP))
      {
        errNum = WLZ_ERR_WRITE_INCOMPLETE;
      }
      /* output nodes indeces */
      for(j = 0; (j < 4) && (errNum == WLZ_ERR_NONE); j++)
      {
	if (!putword(eptr->nodes[j], fP))
	{
	  errNum = WLZ_ERR_WRITE_INCOMPLETE;
	}
      }
      /* output its neighbours */
      /*
      for(j = 0; (j < 4) && (errNum == WLZ_ERR_NONE); j++)
      {
	if (!putword(eptr->neighbours[j], fP))
	{
	  errNum = WLZ_ERR_WRITE_INCOMPLETE;
	}
      }
      */
    }
  }
    /* now Write out displacement */
    /*
  if(errNum == WLZ_ERR_NONE)
  {
    dptr = obj->nodes;
    for(i = 0; (i < obj->nNodes) && (errNum == WLZ_ERR_NONE); i++, dptr++)
    {
      if(!putfloat((float) dptr->displacement.vtX, fP) ||
         !putfloat((float) dptr->displacement.vtY, fP) ||
	 !putfloat((float) dptr->displacement.vtZ, fP))
      {
	errNum = WLZ_ERR_WRITE_INCOMPLETE;
      }
    }
  }
  */
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzIO
* \brief	Writes a 2D mesh transform to the given file.
* \param	fP			Given file.
* \param	obj			Mesh transform.
*/
WlzErrorNum    WlzWriteMeshTransform2D(
				  FILE *fP,
			          WlzMeshTransform *obj)
{
  int		i,
  		j;
  WlzMeshNode	*dptr;
  WlzMeshElem	*eptr;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(     !putword(obj->nElem,  fP) ||
	  !putword(obj->nNodes ,fP) )
  {
    errNum = WLZ_ERR_WRITE_INCOMPLETE;
  }
  else
  {
    /* Write out nodal position and displacement */
    dptr = obj->nodes;
    for(i = 0; (i < obj->nNodes) && (errNum == WLZ_ERR_NONE); i++, dptr++)
    {  /* position */
      if(!putfloat((float) dptr->position.vtX, fP) ||
         !putfloat((float) dptr->position.vtY, fP))
	{
	errNum = WLZ_ERR_WRITE_INCOMPLETE;
      }
      /* displacement */
      if(!putfloat((float) dptr->displacement.vtX, fP) ||
         !putfloat((float) dptr->displacement.vtY, fP))
	 {
	errNum = WLZ_ERR_WRITE_INCOMPLETE;
      }
    }
  }
  if(errNum == WLZ_ERR_NONE)
  {
    /* Write out elements */
    eptr = obj->elements;
    for(i = 0; (i < obj->nElem) && (errNum == WLZ_ERR_NONE); i++, eptr++)
    {
      /* output the index of this element */
      if(!putword(eptr->idx, fP))
      {
        errNum = WLZ_ERR_WRITE_INCOMPLETE;
      }
      /* output the neighbour flags */
      if(!putword(eptr->flags, fP))
      {
        errNum = WLZ_ERR_WRITE_INCOMPLETE;
      }
      
      /* output nodes indeces */
      for(j = 0; (j < 3) && (errNum == WLZ_ERR_NONE); j++)
      {
	if (!putword(eptr->nodes[j], fP))
	{
	  errNum = WLZ_ERR_WRITE_INCOMPLETE;
	}
      }
      /* output its neighbours */

      for(j = 0; (j < 3) && (errNum == WLZ_ERR_NONE); j++)
      {
	if (!putword(eptr->neighbours[j], fP))
	{
	  errNum = WLZ_ERR_WRITE_INCOMPLETE;
	}
      }
    }
  }
  /* Write out displacement */
  if(errNum == WLZ_ERR_NONE)
  {
    dptr = obj->nodes;
    for(i = 0; (i < obj->nNodes) && (errNum == WLZ_ERR_NONE); i++, dptr++)
    {
      if(!putfloat((float) dptr->displacement.vtX, fP) ||
         !putfloat((float) dptr->displacement.vtY, fP))
	 {
	errNum = WLZ_ERR_WRITE_INCOMPLETE;
      }
    }
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzIO
* \brief	Writes a 2D conforming mesh to the given file.
* \param	fP			Given file.
* \param	dstNodTbl		If non-null the node table used in
* 					squeezing out the non-valid nodes is
* 					returned, otherwise the table is
* 					freed.
* \param	mesh			Conforming mesh (3D).
*/
static WlzErrorNum WlzWriteCMesh2D(FILE *fP, int **dstNodTbl,
				   WlzCMesh2D *mesh)
{
  int		idN,
  		idE,
		nNod,
  		nElm;
  WlzCMeshNod2D	*nod;
  WlzCMeshElm2D	*elm;
  WlzCMeshNod2D	*nodes[3];
  int		*nodTbl = NULL;
  WlzErrorNum errNum = WLZ_ERR_NONE;

  if(mesh == NULL)
  {
    errNum = WLZ_ERR_DOMAIN_NULL;
  }
  else if(mesh->type != WLZ_CMESH_2D)
  {
    errNum = WLZ_ERR_DOMAIN_TYPE;
  }
  /* Generate mesh node index table to avoid deleted nodes and then
   * write the number of nodes followed by the number of elements
   * to the file. */
  if(errNum == WLZ_ERR_NONE)
  {
    if((nodTbl = (int *)AlcMalloc(mesh->res.nod.maxEnt * sizeof(int))) == NULL)
    {
      errNum = WLZ_ERR_MEM_ALLOC;
    }
  }
  if(errNum == WLZ_ERR_NONE)
  {
    nNod = 0;
    for(idN = 0; idN < mesh->res.nod.maxEnt; ++idN)
    {
      nod = (WlzCMeshNod2D *)AlcVectorItemGet(mesh->res.nod.vec, idN);
      if(nod->idx >= 0)
      {
	nodTbl[idN] = nNod++;
      }
    }
    nElm = 0;
    for(idE = 0; idE < mesh->res.elm.maxEnt; ++idE)
    {
      elm = (WlzCMeshElm2D *)AlcVectorItemGet(mesh->res.elm.vec, idE);
      if(elm->idx >= 0)
      {
	nElm++;
      }
    }
    putword(nNod, fP);
    putword(nElm, fP);
#ifdef WLZ_DEBUG_WRITEOBJ
    (void )fprintf(stderr,
		   "WlzWriteCMesh2D() "
		   "%d %d\n",
		   nNod, nElm);
#endif /* WLZ_DEBUG_WRITEOBJ */
    if(feof(fP) != 0)
    {
      errNum = WLZ_ERR_WRITE_INCOMPLETE;
    }
  }
  /* Write node flags then node position (x then y). */
  if(errNum == WLZ_ERR_NONE)
  {
    for(idN = 0; idN < mesh->res.nod.maxEnt; ++idN)
    {
      nod = (WlzCMeshNod2D *)AlcVectorItemGet(mesh->res.nod.vec, idN);
      if(nod->idx >= 0)
      {
	putword(nod->flags, fP);
	putdouble(nod->pos.vtX, fP);
	putdouble(nod->pos.vtY, fP);
#ifdef WLZ_DEBUG_WRITEOBJ
        (void )fprintf(stderr,
	               "WlzWriteCMesh2D() "
		       "% 8d % 8d 0x%08x % 8g % 8g\n",
                       nod->idx, nodTbl[idN], nod->flags,
		       nod->pos.vtX, nod->pos.vtY);
#endif /* WLZ_DEBUG_WRITEOBJ */
	if(feof(fP) != 0)
	{
	  errNum = WLZ_ERR_WRITE_INCOMPLETE;
	  break;
	}
      }
    }
  }
  /* Write element flags and node indices. */
  if(errNum == WLZ_ERR_NONE)
  {
    for(idE = 0; idE < mesh->res.elm.maxEnt; ++idE)
    {
      elm = (WlzCMeshElm2D *)AlcVectorItemGet(mesh->res.elm.vec, idE);
      if(elm->idx >= 0)
      {
	nodes[0] = WLZ_CMESH_ELM2D_GET_NODE_0(elm);
	nodes[1] = WLZ_CMESH_ELM2D_GET_NODE_1(elm);
	nodes[2] = WLZ_CMESH_ELM2D_GET_NODE_2(elm);
	putword(elm->flags, fP);
	putword(nodTbl[nodes[0]->idx], fP);
	putword(nodTbl[nodes[1]->idx], fP);
	putword(nodTbl[nodes[2]->idx], fP);
#ifdef WLZ_DEBUG_WRITEOBJ
        (void )fprintf(stderr,
	               "WlzWriteCMesh2D() "
                       "% 8d 0x%08x % 8d % 8d % 8d\n",
		       elm->idx, elm->flags,
		       nodes[0]->idx, nodes[1]->idx,
		       nodes[2]->idx);
#endif /* WLZ_DEBUG_WRITEOBJ */
	if(feof(fP) != 0)
	{
	  errNum = WLZ_ERR_WRITE_INCOMPLETE;
	  break;
	}
      }
    }
  }
  if(dstNodTbl)
  {
    *dstNodTbl = nodTbl;
  }
  else
  {
    AlcFree(nodTbl);
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzIO
* \brief	Writes a 2D5 conforming mesh to the given file.
* \param	fP			Given file.
* \param	dstNodTbl		If non-null the node table used in
* 					squeezing out the non-valid nodes is
* 					returned, otherwise the table is
* 					freed.
* \param	mesh			Conforming mesh (3D).
*/
static WlzErrorNum WlzWriteCMesh2D5(FILE *fP, int **dstNodTbl,
				   WlzCMesh2D5 *mesh)
{
  int		idN,
  		idE,
		nNod,
  		nElm;
  WlzCMeshNod2D5 *nod;
  WlzCMeshElm2D5 *elm;
  WlzCMeshNod2D5 *nodes[3];
  int		*nodTbl = NULL;
  WlzErrorNum errNum = WLZ_ERR_NONE;

  if(mesh == NULL)
  {
    errNum = WLZ_ERR_DOMAIN_NULL;
  }
  else if(mesh->type != WLZ_CMESH_2D5)
  {
    errNum = WLZ_ERR_DOMAIN_TYPE;
  }
  /* Generate mesh node index table to avoid deleted nodes and then
   * write the number of nodes followed by the number of elements
   * to the file. */
  if(errNum == WLZ_ERR_NONE)
  {
    if((nodTbl = (int *)AlcMalloc(mesh->res.nod.maxEnt * sizeof(int))) == NULL)
    {
      errNum = WLZ_ERR_MEM_ALLOC;
    }
  }
  if(errNum == WLZ_ERR_NONE)
  {
    nNod = 0;
    for(idN = 0; idN < mesh->res.nod.maxEnt; ++idN)
    {
      nod = (WlzCMeshNod2D5 *)AlcVectorItemGet(mesh->res.nod.vec, idN);
      if(nod->idx >= 0)
      {
	nodTbl[idN] = nNod++;
      }
    }
    nElm = 0;
    for(idE = 0; idE < mesh->res.elm.maxEnt; ++idE)
    {
      elm = (WlzCMeshElm2D5 *)AlcVectorItemGet(mesh->res.elm.vec, idE);
      if(elm->idx >= 0)
      {
	nElm++;
      }
    }
    putword(nNod, fP);
    putword(nElm, fP);
#ifdef WLZ_DEBUG_WRITEOBJ
    (void )fprintf(stderr,
		   "WlzWriteCMesh2D() "
		   "%d %d\n",
		   nNod, nElm);
#endif /* WLZ_DEBUG_WRITEOBJ */
    if(feof(fP) != 0)
    {
      errNum = WLZ_ERR_WRITE_INCOMPLETE;
    }
  }
  /* Write node flags then node position (x then y). */
  if(errNum == WLZ_ERR_NONE)
  {
    for(idN = 0; idN < mesh->res.nod.maxEnt; ++idN)
    {
      nod = (WlzCMeshNod2D5 *)AlcVectorItemGet(mesh->res.nod.vec, idN);
      if(nod->idx >= 0)
      {
	putword(nod->flags, fP);
	putdouble(nod->pos.vtX, fP);
	putdouble(nod->pos.vtY, fP);
	putdouble(nod->pos.vtZ, fP);
#ifdef WLZ_DEBUG_WRITEOBJ
        (void )fprintf(stderr,
	               "WlzWriteCMesh2D5() "
		       "% 8d % 8d 0x%08x % 8g % 8g % 8g\n",
                       nod->idx, nodTbl[idN], nod->flags,
		       nod->pos.vtX, nod->pos.vtY, nod->pos.vtZ);
#endif /* WLZ_DEBUG_WRITEOBJ */
	if(feof(fP) != 0)
	{
	  errNum = WLZ_ERR_WRITE_INCOMPLETE;
	  break;
	}
      }
    }
  }
  /* Write element flags and node indices. */
  if(errNum == WLZ_ERR_NONE)
  {
    for(idE = 0; idE < mesh->res.elm.maxEnt; ++idE)
    {
      elm = (WlzCMeshElm2D5 *)AlcVectorItemGet(mesh->res.elm.vec, idE);
      if(elm->idx >= 0)
      {
	nodes[0] = WLZ_CMESH_ELM2D5_GET_NODE_0(elm);
	nodes[1] = WLZ_CMESH_ELM2D5_GET_NODE_1(elm);
	nodes[2] = WLZ_CMESH_ELM2D5_GET_NODE_2(elm);
	putword(elm->flags, fP);
	putword(nodTbl[nodes[0]->idx], fP);
	putword(nodTbl[nodes[1]->idx], fP);
	putword(nodTbl[nodes[2]->idx], fP);
#ifdef WLZ_DEBUG_WRITEOBJ
        (void )fprintf(stderr,
	               "WlzWriteCMesh2D5() "
                       "% 8d 0x%08x % 8d % 8d % 8d\n",
		       elm->idx, elm->flags,
		       nodes[0]->idx, nodes[1]->idx,
		       nodes[2]->idx);
#endif /* WLZ_DEBUG_WRITEOBJ */
	if(feof(fP) != 0)
	{
	  errNum = WLZ_ERR_WRITE_INCOMPLETE;
	  break;
	}
      }
    }
  }
  if(dstNodTbl)
  {
    *dstNodTbl = nodTbl;
  }
  else
  {
    AlcFree(nodTbl);
  }
  return(errNum);
}


/*!
* \return	Woolz error code.
* \ingroup	WlzIO
* \brief	Writes a 3D conforming mesh to the given file.
* \param	fP			Given file.
* \param	dstNodTbl		If non-null the node table used in
* 					squeezing out the non-valid nodes is
* 					returned, otherwise the table is
* 					freed.
* \param	mesh			Conforming mesh (3D).
*/
static WlzErrorNum WlzWriteCMesh3D(FILE *fP, int **dstNodTbl,
				   WlzCMesh3D *mesh)
{
  int		idN,
  		idE,
		nNod,
  		nElm;
  WlzCMeshNod3D	*nod;
  WlzCMeshElm3D	*elm;
  WlzCMeshNod3D	*nodes[4];
  int		*nodTbl = NULL;
  WlzErrorNum errNum = WLZ_ERR_NONE;

  if(mesh == NULL)
  {
    errNum = WLZ_ERR_DOMAIN_NULL;
  }
  else if(mesh->type != WLZ_CMESH_3D)
  {
    errNum = WLZ_ERR_DOMAIN_TYPE;
  }
  /* Generate mesh node index table to avoid deleted nodes and then
   * write the number of nodes followed by the number of elements
   * to the file. */
  if(errNum == WLZ_ERR_NONE)
  {
    if((nodTbl = (int *)AlcMalloc(mesh->res.nod.maxEnt * sizeof(int))) == NULL)
    {
      errNum = WLZ_ERR_MEM_ALLOC;
    }
  }
  if(errNum == WLZ_ERR_NONE)
  {
    nNod = 0;
    for(idN = 0; idN < mesh->res.nod.maxEnt; ++idN)
    {
      nod = (WlzCMeshNod3D *)AlcVectorItemGet(mesh->res.nod.vec, idN);
      if(nod->idx >= 0)
      {
	nodTbl[idN] = nNod++;
      }
    }
    nElm = 0;
    for(idE = 0; idE < mesh->res.elm.maxEnt; ++idE)
    {
      elm = (WlzCMeshElm3D *)AlcVectorItemGet(mesh->res.elm.vec, idE);
      if(elm->idx >= 0)
      {
	nElm++;
      }
    }
    putword(nNod, fP);
    putword(nElm, fP);
#ifdef WLZ_DEBUG_WRITEOBJ
    (void )fprintf(stderr,
		   "WlzWriteCMesh3D() "
		   "%d %d\n",
		   nNod, nElm);
#endif /* WLZ_DEBUG_WRITEOBJ */
    if(feof(fP) != 0)
    {
      errNum = WLZ_ERR_WRITE_INCOMPLETE;
    }
  }
  /* Write node flags then node position (x then y). */
  if(errNum == WLZ_ERR_NONE)
  {
    for(idN = 0; idN < mesh->res.nod.maxEnt; ++idN)
    {
      nod = (WlzCMeshNod3D *)AlcVectorItemGet(mesh->res.nod.vec, idN);
      if(nod->idx >= 0)
      {
	putword(nod->flags, fP);
	putdouble(nod->pos.vtX, fP);
	putdouble(nod->pos.vtY, fP);
	putdouble(nod->pos.vtZ, fP);
#ifdef WLZ_DEBUG_WRITEOBJ
        (void )fprintf(stderr,
	               "WlzWriteCMesh2D() "
		       "% 8d % 8d 0x%08x % 8g % 8g % 8g\n",
                       nod->idx, nodTbl[idN], nod->flags,
		       nod->pos.vtX, nod->pos.vtY, nod->pos.vtZ);
#endif /* WLZ_DEBUG_WRITEOBJ */
	if(feof(fP) != 0)
	{
	  errNum = WLZ_ERR_WRITE_INCOMPLETE;
	  break;
	}
      }
    }
  }
  /* Write element flags and node indices. */
  if(errNum == WLZ_ERR_NONE)
  {
    for(idE = 0; idE < mesh->res.elm.maxEnt; ++idE)
    {
      elm = (WlzCMeshElm3D *)AlcVectorItemGet(mesh->res.elm.vec, idE);
      if(elm->idx >= 0)
      {
	nodes[0] = WLZ_CMESH_ELM3D_GET_NODE_0(elm);
	nodes[1] = WLZ_CMESH_ELM3D_GET_NODE_1(elm);
	nodes[2] = WLZ_CMESH_ELM3D_GET_NODE_2(elm);
	nodes[3] = WLZ_CMESH_ELM3D_GET_NODE_3(elm);
	putword(elm->flags, fP);
	putword(nodTbl[nodes[0]->idx], fP);
	putword(nodTbl[nodes[1]->idx], fP);
	putword(nodTbl[nodes[2]->idx], fP);
	putword(nodTbl[nodes[3]->idx], fP);
#ifdef WLZ_DEBUG_WRITEOBJ
        (void )fprintf(stderr,
	               "WlzWriteCMesh3D() "
                       "% 8d 0x%08x % 8d % 8d % 8d % 8d\n",
		       elm->idx, elm->flags,
		       nodes[0]->idx, nodes[1]->idx,
		       nodes[2]->idx, nodes[3]->idx);
#endif /* WLZ_DEBUG_WRITEOBJ */
	if(feof(fP) != 0)
	{
	  errNum = WLZ_ERR_WRITE_INCOMPLETE;
	  break;
	}
      }
    }
  }
  if(dstNodTbl)
  {
    *dstNodTbl = nodTbl;
  }
  else
  {
    AlcFree(nodTbl);
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzIO
* \brief	Writes an indexed value table to the given file.
* \param	fP			Given file pointer.
* \param	obj			Object with an indexed value table
* 					that's to be written to the file.
*/
static WlzErrorNum WlzWriteIndexedValues(FILE *fP, WlzObject *obj)
{
  int		idX,
		idV,
		vCount,
  		nValues = 0;
  WlzGreyP	gP;
  WlzCMeshP	mesh;
  WlzIndexedValues *ixv;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(obj == NULL)
  {
    errNum = WLZ_ERR_OBJECT_NULL;
  }
  else if(obj->domain.core == NULL)
  {
    errNum = WLZ_ERR_DOMAIN_NULL;
  }
  else if(obj->values.core == NULL)
  {
    if(putc(0,fP) == EOF)
    {
      errNum = WLZ_ERR_WRITE_EOF;
    }
  }
  else
  {
    ixv = obj->values.x;
    if(ixv->type != (WlzObjectType )WLZ_INDEXED_VALUES)
    {
      errNum = WLZ_ERR_VALUES_TYPE;
    }
    else
    {
      switch(obj->domain.core->type)
      {
	case WLZ_CMESH_2D:
	  mesh.m2 = obj->domain.cm2;
	  switch(ixv->attach)
	  {
	    case WLZ_VALUE_ATTACH_NOD:
	      nValues = mesh.m2->res.nod.numEnt;
	      break;
	    case WLZ_VALUE_ATTACH_ELM:
	      nValues = mesh.m2->res.elm.numEnt;
	      break;
	    default:
	      errNum = WLZ_ERR_VALUES_TYPE;
	      break;
	  }
	  break;
	case WLZ_CMESH_2D5:
	  mesh.m2d5 = obj->domain.cm2d5;
	  switch(ixv->attach)
	  {
	    case WLZ_VALUE_ATTACH_NOD:
	      nValues = mesh.m2d5->res.nod.numEnt;
	      break;
	    case WLZ_VALUE_ATTACH_ELM:
	      nValues = mesh.m2d5->res.elm.numEnt;
	      break;
	    default:
	      errNum = WLZ_ERR_VALUES_TYPE;
	      break;
	  }
	  break;
	case WLZ_CMESH_3D:
	  mesh.m3 = obj->domain.cm3;
	  switch(ixv->attach)
	  {
	    case WLZ_VALUE_ATTACH_NOD:
	      nValues = mesh.m3->res.nod.numEnt;
	      break;
	    case WLZ_VALUE_ATTACH_ELM:
	      nValues = mesh.m3->res.elm.numEnt;
	      break;
	    default:
	      errNum = WLZ_ERR_VALUES_TYPE;
	      break;
	  }
	  break;
	default:
	  errNum = WLZ_ERR_DOMAIN_TYPE;
	  break;

      }
    }
    if(errNum == WLZ_ERR_NONE)
    {
      if(putc((unsigned int )(ixv->type), fP) == 0)
      {
	errNum = WLZ_ERR_WRITE_INCOMPLETE;
      }
    }
    if(errNum == WLZ_ERR_NONE)
    {
      (void )putword(ixv->rank, fP);
      for(idX = 0; idX < ixv->rank; ++idX)
      {
	(void )putword(ixv->dim[idX], fP);
      }
      (void )putc((unsigned int )(ixv->vType), fP);
      (void )putc((unsigned int )(ixv->attach), fP);
      (void )putword(nValues, fP);
      if(feof(fP))
      {
	errNum = WLZ_ERR_WRITE_INCOMPLETE;
      }
    }
    if(errNum == WLZ_ERR_NONE)
    {
      vCount = 1;
      if(ixv->rank > 0)
      {
	for(idX = 0; idX < ixv->rank; ++idX)
	{
	  vCount *= ixv->dim[idX];
	}
      }
      for(idX = 0; idX < nValues; ++idX)
      {
	gP.v = WlzIndexedValueGet(ixv, idX);
	for(idV = 0; idV < vCount; ++idV)
	{
	  switch(ixv->vType)
	  {
	    case WLZ_GREY_INT:
	      (void )putword(gP.inp[idV], fP);
	      break;
	    case WLZ_GREY_SHORT:
	      (void )putshort(gP.shp[idV], fP);
	      break;
	    case WLZ_GREY_UBYTE:
	      (void )putc(gP.ubp[idV], fP);
	      break;
	    case WLZ_GREY_FLOAT:
	      (void )putfloat(gP.flp[idV], fP);
	      break;
	    case WLZ_GREY_DOUBLE:
	      (void )putdouble(gP.dbp[idV], fP);
	      break;
	    case WLZ_GREY_RGBA:
	      (void )putword(gP.rgbp[idV], fP);
	      break;
	    default:
	      errNum = WLZ_ERR_GREY_TYPE;
	      break;
	  }
	}
	if(feof(fP))
	{
	  errNum = WLZ_ERR_WRITE_INCOMPLETE;
	  break;
	}
      }
    }
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzIO
* \brief	Writes an tiled value table to the given file.
* \param	fP			Given file pointer.
* \param	obj			Object with an tiled value table
* 					that's to be written to the file.
* \param	writeTiles		Write tiles even if no tiles are
* 					allocated for the valuetable.
*/
static WlzErrorNum WlzWriteTiledValueTable(FILE *fP, WlzObject *obj,
					   int writeTiles)
{
  long		tMrk;
  size_t	vSz = 1;
  WlzGreyType   gType;
  WlzTiledValues *tVal = NULL;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  tVal = obj->values.t;
  gType = WlzGreyTableTypeToGreyType(tVal->type, &errNum);
  if(errNum == WLZ_ERR_NONE)
  {
    if(putc((unsigned int )(tVal->type), fP) == EOF)
    {
      errNum = WLZ_ERR_WRITE_INCOMPLETE;
    }
  }
  if(errNum == WLZ_ERR_NONE)
  {
    putc(tVal->dim, fP);
    putword(tVal->kol1, fP);
    putword(tVal->lastkl, fP);
    putword(tVal->line1, fP);
    putword(tVal->lastln, fP);
    putword(tVal->plane1, fP);
    putword(tVal->lastpl, fP);
    errNum = WlzWritePixelV(fP, &(tVal->bckgrnd), 1);
  }
  if(errNum == WLZ_ERR_NONE)
  {
    if(tVal->vRank > 0)
    {
      int	idx;

      (void )putword(tVal->vRank, fP);
      for(idx = 0; idx < tVal->vRank; ++idx)
      {
	vSz *= tVal->vDim[idx];
	(void )putword(tVal->vDim[idx], fP);
      }
    }
  }
  if(errNum == WLZ_ERR_NONE)
  {
    int         nIdx;

    putword(tVal->tileSz, fP);
    putword(tVal->tileWidth, fP);
    putword(tVal->numTiles, fP);
    putword(tVal->nIdx[0], fP);
    putword(tVal->nIdx[1], fP);
    nIdx = tVal->nIdx[0] * tVal->nIdx[1];
    if(tVal->dim > 2)
    {
      putword(tVal->nIdx[2], fP);
      nIdx *= tVal->nIdx[2];
    }
    errNum = WlzWriteInt(fP, (int *)(tVal->indices), nIdx);
  }
  if(errNum == WLZ_ERR_NONE)
  {
    long	blks;
    WlzLong     off[2];

    tMrk = ftell(fP) + (2 * sizeof(unsigned int ));
    blks = (tMrk + tVal->tileSz - 1) / tVal->tileSz;
    tMrk = blks * tVal->tileSz;
    off[0] = tMrk & 0xffffffff;
    off[1] = (sizeof(long) > 4)? tMrk >> 32: 0;
    putword((unsigned int )(off[0]), fP);
    putword((unsigned int )(off[1]), fP);
    if(fseek(fP, tMrk, SEEK_SET) != 0)
    {
      errNum = WLZ_ERR_WRITE_INCOMPLETE;
    }
  }
  if(errNum == WLZ_ERR_NONE)
  {
    size_t      gSz,
    		tSz;

    gSz = WlzGreySize(gType);
    tSz = tVal->numTiles * tVal->tileSz;
    if(tVal->tiles.v != NULL)
    {
      if(fwrite(tVal->tiles.v, gSz, tSz * vSz, fP) != tSz)
      {
        errNum = WLZ_ERR_WRITE_INCOMPLETE;
      }
    }
    else if(writeTiles != 0)
    {
      /* No tile data so reserve tile space in the file by seeking
       * to the end of the tiles and writing a byte. */
      if((fseek(fP, (gSz * tSz * vSz) - 1, SEEK_CUR) != 0) ||
         (fwrite("", 1, 1, fP) != 1))
      {
	errNum = WLZ_ERR_WRITE_INCOMPLETE;
      }
    }
  }
#ifdef WLZ_DEBUG_WRITEOBJ
  if(tVal == NULL)
  {
    (void )fprintf(stderr, "WlzReadTiledValues() tVal == NULL\n");
  }
  else
  {
    (void )fprintf(stderr, "WlzWriteTiledValueTable() tVal\n");
    (void )fprintf(stderr, "WlzWriteTiledValueTable() tVal->type = %d\n",
                   (int )(tVal->type));
    (void )fprintf(stderr, "WlzWriteTiledValueTable() tVal->linkcount = %d\n",
                   tVal->linkcount);
    (void )fprintf(stderr, "WlzWriteTiledValueTable() tVal->dim = %d\n",
                   tVal->dim);
    (void )fprintf(stderr, "WlzWriteTiledValueTable() tVal->kol1 = %d\n",
                   tVal->kol1);
    (void )fprintf(stderr, "WlzWriteTiledValueTable() tVal->lastkl = %d\n",
                   tVal->lastkl);
    (void )fprintf(stderr, "WlzWriteTiledValueTable() tVal->line1 = %d\n",
                   tVal->line1);
    (void )fprintf(stderr, "WlzWriteTiledValueTable() tVal->lastln = %d\n",
                   tVal->lastln);
    (void )fprintf(stderr, "WlzWriteTiledValueTable() tVal->plane1 = %d\n",
                   tVal->plane1);
    (void )fprintf(stderr, "WlzWriteTiledValueTable() tVal->lastpl = %d\n",
                   tVal->lastpl);
    (void )fprintf(stderr, "WlzWriteTiledValueTable() tVal->bckgrnd.type = %d\n",
                   (int )(tVal->bckgrnd.type));
    (void )fprintf(stderr, "WlzWriteTiledValueTable() tVal->tileSz = %ld\n",
                   (long )(tVal->tileSz));
    (void )fprintf(stderr, "WlzWriteTiledValueTable() tVal->tileWidth = %ld\n",
                   (long )tVal->tileWidth);
    (void )fprintf(stderr, "WlzWriteTiledValueTable() tVal->numTiles = %ld\n",
                   (long )tVal->numTiles);
    (void )fprintf(stderr, "WlzWriteTiledValueTable() tVal->nIdx = %d, %d, %d\n",
                   tVal->nIdx[0], tVal->nIdx[1],
		   (tVal->dim == 2)? 0: tVal->nIdx[2]);
    (void )fprintf(stderr, "WlzWriteTiledValueTable() tVal->indices = %p\n",
                   tVal->indices);
    (void )fprintf(stderr, "WlzWriteTiledValueTable() tVal->fd = %d\n",
                   tVal->fd);
    (void )fprintf(stderr, "WlzWriteTiledValueTable() tVal->tileOffset = %ld\n",
                   tVal->tileOffset);
    (void )fprintf(stderr, "WlzWriteTiledValueTable() tVal->tiles.v = %p\n",
                   tVal->tiles.v);
    (void )fprintf(stderr, "WlzWriteTiledValueTable() tMrk = %ld\n",
                   tMrk);
    (void )fprintf(stderr, "WlzWriteTiledValueTable() errno = %s\n",
                   strerror(errno));
  }
#endif /* WLZ_DEBUG_WRITEOBJ */
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzIO
* \brief	Writes a look up table domain to the given file.
* \param	fP			Given file.
* \param	lDom			Look up table domain.
*/
static WlzErrorNum WlzWriteLUTDomain(FILE *fP, WlzLUTDomain *lDom)
{
  const WlzUByte version = 1;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(lDom == NULL)
  {
    if(putc(0, fP) == EOF)
    {
      errNum = WLZ_ERR_WRITE_EOF;
    }
  }
  else
  {
    if((putc(lDom->type, fP) == EOF) ||
       (putc(version, fP) == EOF) ||
       (putword(lDom->bin1, fP) == 0) ||
       (putword(lDom->lastbin, fP) == 0))
    {
      errNum = WLZ_ERR_WRITE_INCOMPLETE;
    }
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzIO
* \brief	Writes look up table values to the given file.
* \param	fP			Given file.
* \param	obj			Look up table object.
*/
static WlzErrorNum WlzWriteLUTValues(FILE *fP, WlzObject *obj)
{
  const WlzUByte version = 1;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(obj == NULL)
  {
    errNum = WLZ_ERR_OBJECT_NULL;
  }
  else if(obj->domain.core == NULL)
  {
    errNum = WLZ_ERR_DOMAIN_NULL;
  }
  else if(obj->values.core == NULL)
  {
    if(putc(0,fP) == EOF)
    {
      errNum = WLZ_ERR_WRITE_EOF;
    }
  }
  else
  {
    WlzLUTDomain *lutDom;
    WlzLUTValues *lutVal;

    lutDom = obj->domain.lut;
    lutVal = obj->values.lut;
    if(lutVal->type != WLZ_LUT)
    {
      errNum = WLZ_ERR_VALUES_TYPE;
    }
    if(errNum == WLZ_ERR_NONE)
    {
      if(putc((unsigned int )(lutVal->type), fP) == 0)
      {
	errNum = WLZ_ERR_WRITE_INCOMPLETE;
      }
    }
    if(errNum == WLZ_ERR_NONE)
    {
      int	i,
      		n;
      WlzGreyP	gP;

      gP = lutVal->val;
      (void )putc(version, fP);
      (void )putc(lutVal->vType, fP);
      n = lutDom->lastbin - lutDom->bin1 + 1;
      (void )putword(n, fP);
      switch(lutVal->vType)
      {
	case WLZ_GREY_INT:
	  for(i = 0; i < n; ++i)
	  {
	    (void )putword(gP.inp[i], fP);
	  }
	  break;
	case WLZ_GREY_SHORT:
	  for(i = 0; i < n; ++i)
	  {
	    (void )putshort(gP.shp[i], fP);
	  }
	  break;
	case WLZ_GREY_UBYTE:
	  for(i = 0; i < n; ++i)
	  {
	    (void )putc(gP.ubp[i], fP);
	  }
	  break;
	case WLZ_GREY_FLOAT:
	  for(i = 0; i < n; ++i)
	  {
	    (void )putfloat(gP.flp[i], fP);
	  }
	  break;
	case WLZ_GREY_DOUBLE:
	  for(i = 0; i < n; ++i)
	  {
	    (void )putdouble(gP.dbp[i], fP);
	  }
	  break;
	case WLZ_GREY_RGBA:
	  for(i = 0; i < n; ++i)
	  {
	    (void )putword(gP.rgbp[i], fP);
	  }
	  break;
	default:
	  break;
      }
      if(feof(fP))
      {
	errNum = WLZ_ERR_WRITE_INCOMPLETE;
      }
    }
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup      WlzIO
* \brief	Writes a 3D section structure to the given open file.
* \param	fP			Given file.
* \param	vs			Given view.
*/
static WlzErrorNum WlzWrite3DViewStruct(FILE *fP, WlzThreeDViewStruct *vs)
{
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  /* check for NULL */
  if(vs == NULL)
  {
    if(putc((unsigned int )0, fP) == EOF)
    {
      errNum = WLZ_ERR_WRITE_EOF;
    }
  }
  else
  {
    if(putc((unsigned int) vs->type, fP) == EOF)
    {
      errNum = WLZ_ERR_WRITE_EOF;
    }
    if(errNum == WLZ_ERR_NONE)
    {
      errNum = WlzWriteVertex3D(fP, &(vs->fixed), 1);
    }
    if(errNum == WLZ_ERR_NONE)
    {
      if(!putdouble(vs->theta, fP) ||
         !putdouble(vs->phi, fP) ||
         !putdouble(vs->zeta, fP) ||
         !putdouble(vs->dist, fP) ||
         !putdouble(vs->scale, fP) ||
         !putdouble(vs->voxelSize[0], fP) ||
         !putdouble(vs->voxelSize[1], fP) ||
         !putdouble(vs->voxelSize[2], fP) ||
         !putword(vs->voxelRescaleFlg, fP) ||
         (putc((unsigned int)vs->interp, fP) == EOF) ||
         (putc((unsigned int)vs->view_mode, fP) == EOF))
      {
	errNum = WLZ_ERR_WRITE_INCOMPLETE;
      }
    }
    if(errNum == WLZ_ERR_NONE)
    {
      errNum = WlzWriteVertex3D(fP, &(vs->up), 1);
    }
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup      WlzIO
* \brief	Writes a points domain to the given open file.
* \param	fP			Given file.
* \param	pts			Given points domain.
*/
static WlzErrorNum	WlzWritePoints(FILE *fP, WlzPoints *pts)
{
  const WlzUByte version = 1;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(pts == NULL)
  {
    if(putc(0, fP) == EOF)
    {
      errNum = WLZ_ERR_WRITE_EOF;
    }
  }
  else
  {
    if((putc(pts->type, fP) == EOF) ||
       (putc(version, fP) == EOF) ||
       (putword(pts->nPoints, fP) == 0))
    {
      errNum = WLZ_ERR_WRITE_INCOMPLETE;
    }
    else
    {
      switch(pts->type)
      {
        case WLZ_POINTS_2I:
          errNum = WlzWriteVertex2I(fP, pts->points.i2, pts->nPoints);
	  break;
	case WLZ_POINTS_2D:
          errNum = WlzWriteVertex2D(fP, pts->points.d2, pts->nPoints);
	  break;
	case WLZ_POINTS_3I:
          errNum = WlzWriteVertex3I(fP, pts->points.i3, pts->nPoints);
	  break;
	case WLZ_POINTS_3D:
          errNum = WlzWriteVertex3D(fP, pts->points.d3, pts->nPoints);
	  break;
        default:
	  errNum = WLZ_ERR_DOMAIN_TYPE;
	  break;
      }
    }
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzIO
* \brief	Writes point values to the given file.
* \param	fP			Given file.
* \param	obj			Points object.
*/
static WlzErrorNum      WlzWritePointValues(FILE *fP, WlzObject *obj)
{
  size_t	vCount = 1;
  const WlzUByte version = 1;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(obj == NULL)
  {
    errNum = WLZ_ERR_OBJECT_NULL;
  }
  else if(obj->domain.core == NULL)
  {
    errNum = WLZ_ERR_DOMAIN_NULL;
  }
  else if(obj->values.core == NULL)
  {
    if(putc(0,fP) == EOF)
    {
      errNum = WLZ_ERR_WRITE_EOF;
    }
  }
  else
  {
    WlzPoints *pDom;
    WlzPointValues *pVal;

    pDom = obj->domain.pts;
    pVal = obj->values.pts;
    if(pVal->type != (WlzObjectType )WLZ_POINT_VALUES)
    {
      errNum = WLZ_ERR_VALUES_TYPE;
    }
    if(errNum == WLZ_ERR_NONE)
    {
      if(putc((unsigned int )(pVal->type), fP) == 0)
      {
	errNum = WLZ_ERR_WRITE_INCOMPLETE;
      }
    }
    if(errNum == WLZ_ERR_NONE)
    {
      int	idx;

      (void )putc(version, fP);
      (void )putword(pVal->rank, fP);
      for(idx = 0; idx < pVal->rank; ++idx)
      {
	vCount *= pVal->dim[idx];
	(void )putword(pVal->dim[idx], fP);
      }
      (void )putc((unsigned int )(pVal->vType), fP);
      (void )putword(pDom->nPoints, fP);
      if(feof(fP))
      {
	errNum = WLZ_ERR_WRITE_INCOMPLETE;
      }
    }
    if(errNum == WLZ_ERR_NONE)
    {
      WlzGreyP gP;

      vCount *= pDom->nPoints;
      gP = pVal->values;
      switch(pVal->vType)
      {
	case WLZ_GREY_INT:
	  errNum = WlzWriteInt(fP, gP.inp, vCount);
	  break;
	case WLZ_GREY_SHORT:
	  errNum = WlzWriteShort(fP, gP.shp, vCount);
	  break;
	case WLZ_GREY_UBYTE:
	  errNum = WlzWriteUByte(fP, gP.ubp, vCount);
	  break;
	case WLZ_GREY_FLOAT:
	  errNum = WlzWriteFloat(fP, gP.flp, vCount);
	  break;
	case WLZ_GREY_DOUBLE:
	  errNum = WlzWriteDouble(fP, gP.dbp, vCount);
	  break;
	case WLZ_GREY_RGBA:
	  errNum = WlzWriteInt(fP, (int *)(gP.rgbp), vCount);
	  break;
	default:
	  errNum = WLZ_ERR_GREY_TYPE;
	  break;
      }
    }
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzIO
* \brief	Writes either a 2D or 3D convex hull to the given file.
* \param	fP			Given file.
* \param	dom			Domain with either 2 or 3D convex hull.
*/
static WlzErrorNum WlzWriteConvexHull(FILE *fP, WlzDomain dom)
{
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(dom.core == NULL)
  {
    if(putc(0,fP) == EOF)
    {
      errNum = WLZ_ERR_WRITE_EOF;
    }
  }
  else if(dom.core->type == WLZ_CONVHULL_DOMAIN_2D)
  {
    WlzConvHullDomain2 *cvh;

    cvh = dom.cvh2;
    if((cvh->vtxType != WLZ_VERTEX_I2) && (cvh->vtxType != WLZ_VERTEX_D2))
    {
      errNum = WLZ_ERR_PARAM_TYPE;
    }
    else
    {
      if((putc((unsigned int )cvh->type, fP) == EOF) ||
	 (putc((unsigned int )cvh->vtxType, fP) == EOF) ||
	 !putword(cvh->nVertices, fP))
      {
	errNum = WLZ_ERR_WRITE_INCOMPLETE;
      }
      else
      {
        errNum = (cvh->vtxType == WLZ_VERTEX_I2)?
	         WlzWriteVertex2I(fP, &(cvh->centroid.i2), 1):
		 WlzWriteVertex2D(fP, &(cvh->centroid.d2), 1);
      }
      if(errNum == WLZ_ERR_NONE)
      {
        errNum = (cvh->vtxType == WLZ_VERTEX_I2)?
	         WlzWriteVertex2I(fP, cvh->vertices.i2, cvh->nVertices):
		 WlzWriteVertex2D(fP, cvh->vertices.d2, cvh->nVertices);
      }
    }
  }
  else if(dom.core->type == WLZ_CONVHULL_DOMAIN_3D)
  {
    WlzConvHullDomain3 *cvh;

    cvh = dom.cvh3;
    if((cvh->vtxType != WLZ_VERTEX_I3) && (cvh->vtxType != WLZ_VERTEX_D3))
    {
      errNum = WLZ_ERR_PARAM_TYPE;
    }
    else
    {
      if((putc((unsigned int )cvh->type, fP) == EOF) ||
	 (putc((unsigned int )cvh->vtxType, fP) == EOF) ||
	 !putword(cvh->nVertices, fP) ||
	 !putword(cvh->nFaces, fP))
      {
	errNum = WLZ_ERR_WRITE_INCOMPLETE;
      }
      else
      {
        errNum = (cvh->vtxType == WLZ_VERTEX_I3)?
	         WlzWriteVertex3I(fP, &(cvh->centroid.i3), 1):
		 WlzWriteVertex3D(fP, &(cvh->centroid.d3), 1);
      }
      if(errNum == WLZ_ERR_NONE)
      {
        errNum = (cvh->vtxType == WLZ_VERTEX_I3)?
	         WlzWriteVertex3I(fP, cvh->vertices.i3, cvh->nVertices):
		 WlzWriteVertex3D(fP, cvh->vertices.d3, cvh->nVertices);
      }
      if(errNum == WLZ_ERR_NONE)
      {
        errNum = WlzWriteInt(fP, cvh->faces, 3 * cvh->nFaces);
      }
    }
  }
  else
  {
    errNum = WLZ_ERR_DOMAIN_TYPE;
  }
  return(errNum);
}
