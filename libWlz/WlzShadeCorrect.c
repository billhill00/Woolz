#pragma ident "MRC HGU $Id$"
#pragma ident "MRC HGU $Id$"
/*!
* \file         WlzShadeCorrect.c
* \author       Bill Hill
* \date         January 2001
* \version      $Id$
* \note
*               Copyright
*               2002 Medical Research Council, UK.
*               All rights reserved.
*               All rights reserved.
* \par Address:
*               MRC Human Genetics Unit,
*               Western General Hospital,
*               Edinburgh, EH4 2XU, UK.
* \brief	
* \ingroup	WlzValueFilters
* \todo         -
* \bug          None known.
*/
#include <stdio.h>
#include <float.h>
#include <limits.h>
#include <string.h>
#include <Wlz.h>

static WlzObject 		*WlzShadeCorrect2DG(
				  WlzObject *srcObj,
				  WlzObject *shdObj,
				  double nrmVal,
				  int inPlace,
				  WlzErrorNum *dstErr);
static WlzObject 		*WlzShadeCorrect2D(
				  WlzObject *srcObj,
				  WlzObject *shdObj,
				  double nrmVal,
				  int inPlace,
				  WlzErrorNum *dstErr);

/*!
* \return	Shade corrected object or NULL on error.
* \ingroup	WlzValueFilters
* \brief	Shade corrects the given domain object with grey
*               values.
*		\f[
		  p_i = \frac{n  o_i}{s_i}
		\f]
*               The shade corrected image P with values \f$p_i\f$ is
*		created by applying a correction factor to each image
*		value of the given image O with values \f$o_i\f$.
* \param	srcObj			Given object to be shade corrected.
* \param	shdObj			Given bright field object.
* \param	nrmVal			Normalization value.
* \param	inPlace			Modify the grey values of the given
*					object in place if non-zero.
* \param	dstErr			Destination error pointer, may
*                                       be null.
*/
WlzObject	*WlzShadeCorrect(WlzObject *srcObj, WlzObject *shdObj,
			         double nrmVal, int inPlace,
			         WlzErrorNum *dstErr)
{
  WlzObject	*rtnObj = NULL;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if((srcObj == NULL) || (shdObj == NULL))
  {
    errNum = WLZ_ERR_OBJECT_NULL;
  }
  else if((srcObj->domain.core == NULL) || (shdObj->domain.core == NULL))
  {
    errNum = WLZ_ERR_DOMAIN_NULL;
  }
  else if((srcObj->values.core == NULL) || (shdObj->values.core == NULL))
  {
    errNum = WLZ_ERR_VALUES_NULL;
  }
  else if(srcObj->type != shdObj->type)
  {
    errNum = WLZ_ERR_OBJECT_TYPE;
  }
  else
  {
    switch(srcObj->type)
    {
      case WLZ_2D_DOMAINOBJ:
	rtnObj = WlzShadeCorrect2D(srcObj, shdObj, nrmVal, inPlace, &errNum);
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
  return(rtnObj);
}

/*!
* \return	Shade corrected object or NULL on error.
* \ingroup	WlzValueFilters
* \brief	Shade corrects the given 2D domain object with grey
*               values. This function just checks that the given and
*               shade objects have the same grey types, it then calls
*               WlzShadeCorrect2DG() to shade correct the given object.
* \param	srcObj			Given object to be shade corrected.
* \param	shdObj			Given bright field object.
* \param	nrmVal			Normalization value.
* \param	inPlace			Modify the grey values of the
*                                       given object if non-zero.
* \param	dstErr			Destination error pointer, may
*                                       be null.
*/
static WlzObject *WlzShadeCorrect2D(WlzObject *srcObj, WlzObject *shdObj,
				    double nrmVal, int inPlace,
				    WlzErrorNum *dstErr)
{
  WlzGreyType	srcG,
  		shdG;
  WlzObject	*rtnObj = NULL;
  WlzErrorNum 	errNum = WLZ_ERR_NONE;

  srcG = WlzGreyTableTypeToGreyType(srcObj->values.v->type, &errNum);
  if(errNum == WLZ_ERR_NONE)
  {
    shdG = WlzGreyTableTypeToGreyType(shdObj->values.v->type, &errNum);
  }
  if(errNum == WLZ_ERR_NONE)
  {
    if(srcG != shdG)
    {
      errNum = WLZ_ERR_GREY_TYPE;
    }
    else
    {
      rtnObj = WlzShadeCorrect2DG(srcObj, shdObj, nrmVal, inPlace, &errNum);
    }
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(rtnObj);
}

/*!
* \return	Shade corrected object or NULL on error.
* \ingroup	WlzValueFilters
* \brief	Shade corrects the given 2D domain object with grey
*               values. Grey value types known to be the same.
* \param	srcObj			Given object to be shade
*                                       corrected.
* \param	shdObj			Given bright field object.
* \param	nrmVal			Normalization value.
* \param	inPlace			Modify the grey values of the
*                                       given object if non-zero.
* \param	dstErr			Destination error pointer, may
*                                       be null.
*/
static WlzObject *WlzShadeCorrect2DG(WlzObject *srcObj, WlzObject *shdObj,
				     double nrmVal, int inPlace,
				     WlzErrorNum *dstErr)
{
  int		tI0,
  		iCnt;
  double	tD0;
  WlzObject	*uObj = NULL,
		*uSrcObj = NULL,
		*uShdObj = NULL,
  		*rtnObj = NULL;
  WlzGreyP	srcPix,
  		shdPix,
		rtnPix;
  WlzValues	newVal;
  WlzIntervalWSpace srcIWSp,
  		shdIWSp,
		rtnIWSp;
  WlzGreyWSpace	srcGWSp,
  		shdGWSp,
		rtnGWSp;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  /* Find union of the given and shade objects. */
  uObj = WlzUnion2(srcObj, shdObj, &errNum);
  /* Make new objects with the values of the given and shade objects
   * but the domain of their intersection. */
  if(errNum == WLZ_ERR_NONE)
  {
    uSrcObj = WlzMakeMain(WLZ_2D_DOMAINOBJ, uObj->domain, srcObj->values,
    			  NULL, NULL, &errNum);
  }
  if(errNum == WLZ_ERR_NONE)
  {
    uShdObj = WlzMakeMain(WLZ_2D_DOMAINOBJ, uObj->domain, shdObj->values,
    			  NULL, NULL, &errNum);
  }
  /* Make a new object, again using the union for the domain, but this time
   * either sharing the given objects values or creating a new value table. */
  if(errNum == WLZ_ERR_NONE)
  {
    if(inPlace)
    {
      rtnObj = WlzMakeMain(WLZ_2D_DOMAINOBJ, uObj->domain, srcObj->values,
      			   NULL, NULL, &errNum);
    }
    else
    {
      newVal.v = WlzNewValueTb(uObj, srcObj->values.core->type,
      			       WlzGetBackground(srcObj, NULL), &errNum);
      if(errNum == WLZ_ERR_NONE)
      {
        if((rtnObj = WlzMakeMain(WLZ_2D_DOMAINOBJ, uObj->domain, newVal,
			         NULL, NULL, &errNum)) == NULL)
        {
	  (void )WlzFreeValueTb(newVal.v);
	}
      }
    }
  }
  /* Work through the intervals setting the grey values. */
  if(errNum == WLZ_ERR_NONE)
  {
    if(((errNum = WlzInitGreyScan(uSrcObj, &srcIWSp,
    				  &srcGWSp)) == WLZ_ERR_NONE) &&
       ((errNum = WlzInitGreyScan(uShdObj, &shdIWSp,
    				  &shdGWSp)) == WLZ_ERR_NONE) &&
       ((errNum = WlzInitGreyScan(rtnObj, &rtnIWSp,
    				  &rtnGWSp)) == WLZ_ERR_NONE))
    {
      while(((errNum = WlzNextGreyInterval(&srcIWSp)) == WLZ_ERR_NONE) &&
            ((errNum = WlzNextGreyInterval(&shdIWSp)) == WLZ_ERR_NONE) &&
            ((errNum = WlzNextGreyInterval(&rtnIWSp)) == WLZ_ERR_NONE))
      {
	srcPix = srcGWSp.u_grintptr;
	shdPix = shdGWSp.u_grintptr;
	rtnPix = rtnGWSp.u_grintptr;
	iCnt = rtnIWSp.rgtpos - rtnIWSp.lftpos + 1;
        switch(rtnGWSp.pixeltype)
	{
	  case WLZ_GREY_INT:
	    while(iCnt-- > 0)
	    {
	      tD0 = (*(srcPix.inp)++ * nrmVal) / (*(shdPix.inp)++ + 1.0);
	      *(rtnPix.inp)++ = WLZ_NINT(tD0);
	    }
	    break;
	  case WLZ_GREY_SHORT:
	    while(iCnt-- > 0)
	    {
	      tD0 = (*(srcPix.shp)++ * nrmVal) / (*(shdPix.shp)++ + 1.0);
	      tI0 = WLZ_NINT(tD0);
	      *(rtnPix.shp)++ = WLZ_CLAMP(tI0, SHRT_MIN, SHRT_MAX);
	    }
	    break;
	  case WLZ_GREY_UBYTE:
	    while(iCnt-- > 0)
	    {
	      tD0 = (*(srcPix.ubp)++ * nrmVal) / (*(shdPix.ubp)++ + 1.0);
	      tI0 = WLZ_NINT(tD0);
	      *(rtnPix.ubp)++ = WLZ_CLAMP(tI0, 0, 255);
	    }
	    break;
	  case WLZ_GREY_FLOAT:
	    while(iCnt-- > 0)
	    {
	      tD0 = (*(srcPix.flp)++ * nrmVal) / (*(shdPix.flp)++ + 1.0);
	      *(rtnPix.flp)++ = tD0;
	    }
	    break;
	  case WLZ_GREY_DOUBLE:
	    while(iCnt-- > 0)
	    {
	      tD0 = (*(srcPix.dbp)++ * nrmVal) / (*(shdPix.dbp)++ + 1.0);
	      *(rtnPix.dbp)++ = tD0;
	    }
	    break;
	}
      }
      if(errNum == WLZ_ERR_EOO)         /* Reset error from end of intervals */
      {
        errNum = WLZ_ERR_NONE;
      }
    }
  }
  (void )WlzFreeObj(uObj);
  (void )WlzFreeObj(uSrcObj);
  (void )WlzFreeObj(uShdObj);
  if(errNum != WLZ_ERR_NONE)
  {
    (void )WlzFreeObj(rtnObj);
    rtnObj = NULL;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(rtnObj);
}