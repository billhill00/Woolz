#pragma ident "MRC HGU $Id$"
/*!
* \file         WlzSepTrans.c
* \author       richard <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Mon May 26 08:16:16 2003
* \version      MRC HGU $Id$
*               $Revision$
*               $Name$
* \par Copyright:
*               1994-2002 Medical Research Council, UK.
*               All rights reserved.
* \par Address:
*               MRC Human Genetics Unit,
*               Western General Hospital,
*               Edinburgh, EH4 2XU, UK.
* \ingroup      WlzValuesFilters
* \brief        Execute a separable transform on a 2D Woolz domain object.
 It is the user responsibility to ensure that the grey-0value type is
 appropraite for the resultant.
*               
* \todo         -
* \bug          None known
*
* Maintenance log with most recent changes at top of list.
*/

#include <stdlib.h>
#include <Wlz.h>

/* function:     WlzSepTrans    */
/*! 
* \ingroup      WlzValuesFilters
* \brief        Perform 2D seperable transform on a 2D grey-level image.
*
* \return       Pointer to transformed object
* \param    obj	Input object pointer
* \param    x_fun	Convolution function to be applied in the x-direction (along the rows).
* \param    x_params	Parameter pointer to be passed to the x-function.
* \param    y_fun	Convolution function to be applied in the y-direction (down the columns).
* \param    y_params	Parameter pointer to be passed to the y-function.
* \param    dstErr	error return.
* \par      Source:
*                WlzSepTrans.c
*/
WlzObject *WlzSepTrans(
  WlzObject		*obj,
  WlzIntervalConvFunc	x_fun,
  void			*x_params,
  WlzIntervalConvFunc	y_fun,
  void			*y_params,
  WlzErrorNum		*dstErr)
{
  WlzIntervalWSpace	iwspace;
  WlzGreyWSpace		gwspace;
  WlzSepTransWSpace	stwspc;
  WlzValues		values;
  WlzObject		*obj1, *obj2;
  int			i, width, height;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* check object pointers and type */
  obj2 = NULL;
  stwspc.outbuf.p.dbp = NULL;
  if( obj == NULL ){
    errNum = WLZ_ERR_OBJECT_NULL;
  }
  else {
    switch( obj->type ){

    case WLZ_2D_DOMAINOBJ:
      /* check domain and valuetable */
      if( obj->domain.core == NULL ){
	errNum = WLZ_ERR_DOMAIN_NULL;
	break;
      }
      if( obj->values.core == NULL ){
	errNum = WLZ_ERR_VALUES_NULL;
	break;
      }
      break;

    default:
    case WLZ_3D_DOMAINOBJ:
      errNum = WLZ_ERR_OBJECT_TYPE;
      break;

    case WLZ_TRANS_OBJ:
      if( obj1 = WlzSepTrans(obj->values.obj,
			     x_fun, x_params, y_fun, y_params, &errNum) ){
	values.obj = obj1;
	return WlzMakeMain(obj->type, obj->domain, values,
			   NULL, obj, dstErr);
      }
      break;

    case WLZ_EMPTY_OBJ:
      return WlzMakeEmpty(dstErr);
    }
  }

  /* make space for a calculation buffer - assume worst case of doubles */
  if( errNum == WLZ_ERR_NONE ){
    width  = obj->domain.i->lastkl - obj->domain.i->kol1 + 1;
    height = obj->domain.i->lastln - obj->domain.i->line1 + 1;
    stwspc.inbuf.type = WlzGreyTableTypeToGreyType(obj->values.core->type,
						   NULL);
    stwspc.bckgrnd = WlzGetBackground(obj, NULL);

    switch( stwspc.inbuf.type ){
    case WLZ_GREY_INT:
    case WLZ_GREY_SHORT:
    case WLZ_GREY_UBYTE:
      stwspc.outbuf.type = WLZ_GREY_INT;
      break;
    default:
      stwspc.outbuf.type = stwspc.inbuf.type;
      break;
    }

    if( (stwspc.outbuf.p.dbp = (double *) 
	 AlcMalloc(sizeof(double)*(width>height?width:height))) == NULL){
      errNum = WLZ_ERR_MEM_ALLOC;
    }
  }

  /* tranpose the object - interchange x & y coordinates */
  if( errNum == WLZ_ERR_NONE ){
    obj1 = WlzTransposeObj(obj, &errNum);
  }

  /* perform the y convolution */
  if((errNum == WLZ_ERR_NONE) &&
     ((errNum = WlzInitGreyScan(obj1, &iwspace, &gwspace)) == WLZ_ERR_NONE))
  {
    while( (errNum = WlzNextGreyInterval(&iwspace)) == WLZ_ERR_NONE ){
      stwspc.inbuf.p.inp = gwspace.u_grintptr.inp;
      stwspc.len = iwspace.rgtpos - iwspace.lftpos + 1;
      if( (errNum = (*y_fun)(&stwspc, y_params)) == WLZ_ERR_NONE ){
	switch(  stwspc.inbuf.type ){
	case WLZ_GREY_INT:
	  for(i=0; i < stwspc.len; i++, stwspc.inbuf.p.inp++)
	    *stwspc.inbuf.p.inp = stwspc.outbuf.p.inp[i];
	  break;
	case WLZ_GREY_SHORT:
	  for(i=0; i < stwspc.len; i++, stwspc.inbuf.p.shp++)
	    *stwspc.inbuf.p.shp = stwspc.outbuf.p.inp[i];
	  break;
	case WLZ_GREY_UBYTE:
	  for(i=0; i < stwspc.len; i++, stwspc.inbuf.p.ubp++)
	    *stwspc.inbuf.p.ubp = stwspc.outbuf.p.inp[i];
	  break;
	case WLZ_GREY_FLOAT:
	  for(i=0; i < stwspc.len; i++, stwspc.inbuf.p.flp++)
	    *stwspc.inbuf.p.flp = stwspc.outbuf.p.flp[i];
	  break;
	case WLZ_GREY_DOUBLE:
	  for(i=0; i < stwspc.len; i++, stwspc.inbuf.p.dbp++)
	    *stwspc.inbuf.p.dbp = stwspc.outbuf.p.dbp[i];
	  break;
	case WLZ_GREY_RGBA:
	  for(i=0; i < stwspc.len; i++, stwspc.inbuf.p.rgbp++)
	    *stwspc.inbuf.p.rgbp = stwspc.outbuf.p.rgbp[i];
	  break;
	}
      }
      else {
	break; /* break from the while loop */
      }
    }
    if( errNum == WLZ_ERR_EOO ){
      errNum = WLZ_ERR_NONE;
    }
  }

  /* rotate back and free temporary object */
  if( errNum == WLZ_ERR_NONE ){
    obj2 = WlzTransposeObj(obj1, &errNum);
    WlzFreeObj(obj1);
  }

  /* perform x convolution */
  if((errNum == WLZ_ERR_NONE) &&
     ((errNum = WlzInitGreyScan(obj2, &iwspace, &gwspace)) == WLZ_ERR_NONE))
  {
    while( (errNum = WlzNextGreyInterval(&iwspace)) == WLZ_ERR_NONE ){
      stwspc.inbuf.p.inp = gwspace.u_grintptr.inp;
      stwspc.len = iwspace.rgtpos - iwspace.lftpos + 1;
      if( (errNum = (*x_fun)(&stwspc, x_params)) == WLZ_ERR_NONE ){
	switch(  gwspace.pixeltype ){
	case WLZ_GREY_INT:
	  for(i=0; i < stwspc.len; i++, stwspc.inbuf.p.inp++)
	    *stwspc.inbuf.p.inp = stwspc.outbuf.p.inp[i];
	  break;
	case WLZ_GREY_SHORT:
	  for(i=0; i < stwspc.len; i++, stwspc.inbuf.p.shp++)
	    *stwspc.inbuf.p.shp = stwspc.outbuf.p.inp[i];
	  break;
	case WLZ_GREY_UBYTE:
	  for(i=0; i < stwspc.len; i++, stwspc.inbuf.p.ubp++)
	    *stwspc.inbuf.p.ubp = stwspc.outbuf.p.inp[i];
	  break;
	case WLZ_GREY_FLOAT:
	  for(i=0; i < stwspc.len; i++, stwspc.inbuf.p.flp++)
	    *stwspc.inbuf.p.flp = stwspc.outbuf.p.flp[i];
	  break;
	case WLZ_GREY_DOUBLE:
	  for(i=0; i < stwspc.len; i++, stwspc.inbuf.p.dbp++)
	    *stwspc.inbuf.p.dbp = stwspc.outbuf.p.dbp[i];
	  break;
	case WLZ_GREY_RGBA:
	  for(i=0; i < stwspc.len; i++, stwspc.inbuf.p.rgbp++)
	    *stwspc.inbuf.p.rgbp = stwspc.outbuf.p.rgbp[i];
	  break;
	}
      }
      else {
	break; /* break from the while loop */
      }
    }
  }
  if( errNum == WLZ_ERR_EOO ){
    errNum = WLZ_ERR_NONE;
  }

  /* free the buffer and return transformed object */
  if( stwspc.outbuf.p.dbp ){
    AlcFree((void *) stwspc.outbuf.p.dbp);
  }

  if( dstErr ){
    *dstErr = errNum;
  }
  return obj2;
}
