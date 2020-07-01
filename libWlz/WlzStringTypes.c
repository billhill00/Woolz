#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _WlzStringTypes_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         libWlz/WlzStringTypes.c
* \author       Bill Hill
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
* \brief	Functions for converting between Woolz data types and
* 		string representations (names) of the types.
* \ingroup	WlzStrings
*/

#include <stdlib.h>
#include <string.h>
#include <Wlz.h>

/*!
* \return	Read only string or NULL on error.
* \ingroup 	WlzStrings
* \brief	Finds a string for the given object's type.
* \param	obj			Given object.
* \param	dstErr			Destination error pointer, may be null.
*/
const char	*WlzStringFromObjType(WlzObject *obj,
				      WlzErrorNum *dstErr)
{
  WlzErrorNum	errNum = WLZ_ERR_NONE;
  const char	*oTypeStr = NULL;

  if(obj == NULL)
  {
    errNum = WLZ_ERR_OBJECT_NULL;
  }
  else
  {
    oTypeStr = WlzStringFromObjTypeValue(obj->type, &errNum);
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(oTypeStr);
}

/*!
* \return	Read only string or NULL on error.
* \ingroup	WlzStrings
* \brief	Finds a string for the given object type.
* \param	objType			Given object type.
* \param	dstErr			Destination error pointer, may be null.
*/
const char	*WlzStringFromObjTypeValue(WlzObjectType objType,
					WlzErrorNum *dstErr)
{
  WlzErrorNum	errNum = WLZ_ERR_NONE;
  const char	*oTypeStr = NULL;

  switch(objType)
  {
    case WLZ_2D_DOMAINOBJ:
      oTypeStr = "WLZ_2D_DOMAINOBJ";
      break;
    case WLZ_3D_DOMAINOBJ:
      oTypeStr = "WLZ_3D_DOMAINOBJ";
      break;
    case WLZ_TRANS_OBJ:
      oTypeStr = "WLZ_TRANS_OBJ";
      break;
    case WLZ_3D_WARP_TRANS:
      oTypeStr = "WLZ_3D_WARP_TRANS";
      break;
    case WLZ_2D_POLYGON:
      oTypeStr = "WLZ_2D_POLYGON";
      break;
    case WLZ_BOUNDLIST:
      oTypeStr = "WLZ_BOUNDLIST";
      break;
    case WLZ_CONV_HULL:
      oTypeStr = "WLZ_CONV_HULL";
      break;
    case WLZ_HISTOGRAM:
      oTypeStr = "WLZ_HISTOGRAM";
      break;
    case WLZ_3D_POLYGON:
      oTypeStr = "WLZ_3D_POLYGON";
      break;
    case WLZ_CONTOUR:
      oTypeStr = "WLZ_CONTOUR";
      break;
    case WLZ_CMESH_2D:
      oTypeStr = "WLZ_CMESH_2D";
      break;
    case WLZ_CMESH_2D5:
      oTypeStr = "WLZ_CMESH_2D5";
      break;
    case WLZ_CMESH_3D:
      oTypeStr = "WLZ_CMESH_3D";
      break;
    case WLZ_RECTANGLE:
      oTypeStr = "WLZ_RECTANGLE";
      break;
    case WLZ_CONVOLVE_INT:
      oTypeStr = "WLZ_CONVOLVE_INT";
      break;
    case WLZ_CONVOLVE_FLOAT:
      oTypeStr = "WLZ_CONVOLVE_FLOAT";
      break;
    case WLZ_AFFINE_TRANS:
      oTypeStr = "WLZ_AFFINE_TRANS";
      break;
    case WLZ_WARP_TRANS:
      oTypeStr = "WLZ_WARP_TRANS";
      break;
    case WLZ_FMATCHOBJ:
      oTypeStr = "WLZ_FMATCHOBJ";
      break;
    case WLZ_TEXT:
      oTypeStr = "WLZ_TEXT";
      break;
    case WLZ_COMPOUND_ARR_1:
      oTypeStr = "WLZ_COMPOUND_ARR_1";
      break;
    case WLZ_COMPOUND_ARR_2:
      oTypeStr = "WLZ_COMPOUND_ARR_2";
      break;
    case WLZ_COMPOUND_LIST_1:
      oTypeStr = "WLZ_COMPOUND_LIST_1";
      break;
    case WLZ_COMPOUND_LIST_2:
      oTypeStr = "WLZ_COMPOUND_LIST_2";
      break;
    case WLZ_PROPERTY_OBJ:
      oTypeStr = "WLZ_PROPERTY_OBJ";
      break;
    case WLZ_EMPTY_OBJ:
      oTypeStr = "WLZ_EMPTY_OBJ";
      break;
    case WLZ_MESH_TRANS:
      oTypeStr = "WLZ_MESH_TRANS";
      break;
    case WLZ_CMESH_TRANS:
      oTypeStr = "WLZ_CMESH_TRANS";
      break;
    case WLZ_LUT:
      oTypeStr = "WLZ_LUT";
      break;
    case WLZ_3D_VIEW_STRUCT:
      oTypeStr = "WLZ_3D_VIEW_STRUCT";
      break;
    case WLZ_POINTS:
      oTypeStr = "WLZ_POINTS";
      break;
    case WLZ_SPLINE:
      oTypeStr = "WLZ_SPLINE";
      break;
    default:
      errNum = WLZ_ERR_OBJECT_TYPE;
      break;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(oTypeStr);
}

/*!
* \return	Woolz object type or WLZ_NULL on error.
* \ingroup	WlzStrings
* \brief	Finds an enumerated type for the given object type string.
* \param	oTypeStr		Given object type string.
* \param	dstErr			Destination error pointer, may be null.
*/
WlzObjectType	WlzStringToObjType(const char *oTypeStr,
				   WlzErrorNum *dstErr)
{
  int		tI0;
  WlzErrorNum	errNum = WLZ_ERR_OBJECT_TYPE;
  WlzObjectType	oType = WLZ_NULL;

  if(WlzStringMatchValue(&tI0, oTypeStr,
		"WLZ_2D_DOMAINOBJ", WLZ_2D_DOMAINOBJ,
		"WLZ_3D_DOMAINOBJ", WLZ_3D_DOMAINOBJ,
		"WLZ_TRANS_OBJ", WLZ_TRANS_OBJ,
		"WLZ_3D_WARP_TRANS", WLZ_3D_WARP_TRANS,
		"WLZ_2D_POLYGON", WLZ_2D_POLYGON,
		"WLZ_BOUNDLIST", WLZ_BOUNDLIST,
		"WLZ_CONV_HULL", WLZ_CONV_HULL,
		"WLZ_CONTOUR", WLZ_CONTOUR,
		"WLZ_CMESH_2D", WLZ_CMESH_2D,
		"WLZ_CMESH_2D5", WLZ_CMESH_2D5,
		"WLZ_CMESH_3D", WLZ_CMESH_3D,
		"WLZ_HISTOGRAM", WLZ_HISTOGRAM,
		"WLZ_3D_POLYGON", WLZ_3D_POLYGON,
		"WLZ_RECTANGLE", WLZ_RECTANGLE,
		"WLZ_CONVOLVE_INT", WLZ_CONVOLVE_INT,
		"WLZ_CONVOLVE_FLOAT", WLZ_CONVOLVE_FLOAT,
		"WLZ_AFFINE_TRANS", WLZ_AFFINE_TRANS,
		"WLZ_WARP_TRANS", WLZ_WARP_TRANS,
		"WLZ_FMATCHOBJ", WLZ_FMATCHOBJ,
		"WLZ_TEXT", WLZ_TEXT,
		"WLZ_COMPOUND_ARR_1", WLZ_COMPOUND_ARR_1,
		"WLZ_COMPOUND_ARR_2", WLZ_COMPOUND_ARR_2,
		"WLZ_COMPOUND_LIST_1", WLZ_COMPOUND_LIST_1,
		"WLZ_COMPOUND_LIST_2", WLZ_COMPOUND_LIST_2,
		"WLZ_PROPERTY_OBJ", WLZ_PROPERTY_OBJ,
		"WLZ_EMPTY_OBJ", WLZ_EMPTY_OBJ,
		"WLZ_MESH_TRANS", WLZ_MESH_TRANS,
		"WLZ_CMESH_TRANS", WLZ_CMESH_TRANS,
		"WLZ_LUT", WLZ_LUT,
		"WLZ_3D_VIEW_STRUCT", WLZ_3D_VIEW_STRUCT,
		"WLZ_POINTS", WLZ_POINTS,
		"WLZ_SPLINE", WLZ_SPLINE,
		NULL))
  {
    oType = (WlzObjectType )tI0;
    errNum = WLZ_ERR_NONE;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(oType);
}

/*!
* \return	Pointer to read only string or NULL on error.
* \ingroup	WlzStrings
* \brief	Finds a string for the given object's domain type.
* \param	obj			Given object.
* \param	dstErr			Destination error pointer, may be null.
*/
const char	*WlzStringFromObjDomainType(WlzObject *obj,
					    WlzErrorNum *dstErr)
{
  WlzErrorNum	errNum = WLZ_ERR_NONE;
  const char	*oDomTypeStr = NULL;

  if(obj == NULL)
  {
    errNum = WLZ_ERR_OBJECT_NULL;
  }
  else if(obj->domain.core == NULL)
  {
    errNum = WLZ_ERR_DOMAIN_NULL;
  }
  else
  {
    switch(obj->type)
    {
      case WLZ_2D_DOMAINOBJ:
	switch(obj->domain.core->type)
	{
          case WLZ_INTERVALDOMAIN_INTVL:
	    oDomTypeStr = "WLZ_INTERVALDOMAIN_INTVL";
	    break;
          case WLZ_INTERVALDOMAIN_RECT:
	    oDomTypeStr = "WLZ_INTERVALDOMAIN_RECT";
	    break;
	  default:
	    errNum = WLZ_ERR_DOMAIN_TYPE;
	    break;
        }
	break;
      case WLZ_3D_DOMAINOBJ:
	switch(obj->domain.core->type)
	{
          case WLZ_PLANEDOMAIN_DOMAIN:
            oDomTypeStr = "WLZ_PLANEDOMAIN_DOMAIN";
	    break;
          case WLZ_PLANEDOMAIN_POLYGON:
            oDomTypeStr = "WLZ_PLANEDOMAIN_POLYGON";
	    break;
          case WLZ_PLANEDOMAIN_BOUNDLIST:
            oDomTypeStr = "WLZ_PLANEDOMAIN_BOUNDLIST";
	    break;
          case WLZ_PLANEDOMAIN_HISTOGRAM:
            oDomTypeStr = "WLZ_PLANEDOMAIN_HISTOGRAM";
	    break;
          case WLZ_PLANEDOMAIN_AFFINE:
            oDomTypeStr = "WLZ_PLANEDOMAIN_AFFINE";
	    break;
          case WLZ_PLANEDOMAIN_WARP:
            oDomTypeStr = "WLZ_PLANEDOMAIN_WARP";
	    break;
	  default:
	    errNum = WLZ_ERR_DOMAIN_TYPE;
	    break;
        }
	break;
      case WLZ_TRANS_OBJ:
	switch(obj->domain.core->type)
	{
	  case WLZ_TRANSFORM_2D_AFFINE:
	    oDomTypeStr = "WLZ_TRANSFORM_2D_AFFINE";
	    break;
	  case WLZ_TRANSFORM_3D_AFFINE:
	    oDomTypeStr = "WLZ_TRANSFORM_3D_AFFINE";
	    break;
	  default:
	    errNum = WLZ_ERR_DOMAIN_TYPE;
	    break;
	}
        break;
      case WLZ_3D_WARP_TRANS:
	switch(obj->domain.core->type)
	{
	  case WLZ_WARP_TRANS:
	    oDomTypeStr = "WLZ_WARP_TRANS";
	    break;
	  default:
	    errNum = WLZ_ERR_DOMAIN_TYPE;
	    break;
	}
        break;
      case WLZ_2D_POLYGON:
	switch(obj->domain.core->type)
	{
	  case WLZ_POLYGON_INT:
	    oDomTypeStr = "WLZ_POLYGON_INT";
	    break;
	  case WLZ_POLYGON_FLOAT:
	    oDomTypeStr = "WLZ_POLYGON_FLOAT";
	    break;
	  case WLZ_POLYGON_DOUBLE:
	    oDomTypeStr = "WLZ_POLYGON_DOUBLE";
	    break;
	  default:
	    errNum = WLZ_ERR_DOMAIN_TYPE;
	    break;
	}
        break;
      case WLZ_BOUNDLIST:
	switch(obj->domain.core->type)
	{
	  case WLZ_BOUNDLIST_PIECE:
	    oDomTypeStr = "WLZ_BOUNDLIST_PIECE";
	    break;
	  case WLZ_BOUNDLIST_HOLE:
	    oDomTypeStr = "WLZ_BOUNDLIST_HOLE";
	    break;
	  default:
	    errNum = WLZ_ERR_DOMAIN_TYPE;
	    break;
	}
        break;
      case WLZ_CONV_HULL:
        switch(obj->domain.core->type)
	{
	  case WLZ_CONVHULL_DOMAIN_2D:
	    oDomTypeStr = "WLZ_CONVHULL_DOMAIN_2D";
	    break;
	  case WLZ_CONVHULL_DOMAIN_3D:
	    oDomTypeStr = "WLZ_CONVHULL_DOMAIN_3D";
	    break;
	  default:
	    errNum = WLZ_ERR_DOMAIN_TYPE;
	    break;
	}
        break;
      case WLZ_CONTOUR:
        oDomTypeStr = "WLZ_CONTOUR";
	break;
      case WLZ_CMESH_2D:
        oDomTypeStr = "WLZ_CMESH_2D";
	break;
      case WLZ_CMESH_2D5:
        oDomTypeStr = "WLZ_CMESH_2D5";
	break;
      case WLZ_CMESH_3D:
        oDomTypeStr = "WLZ_CMESH_3D";
	break;
      case WLZ_HISTOGRAM:
	switch(obj->domain.core->type)
	{
	  case WLZ_HISTOGRAMDOMAIN_INT:
	    oDomTypeStr = "WLZ_HISTOGRAMDOMAIN_INT";
	    break;
	  case WLZ_HISTOGRAMDOMAIN_FLOAT:
	    oDomTypeStr = "WLZ_HISTOGRAMDOMAIN_FLOAT";
	    break;
	  default:
	    errNum = WLZ_ERR_DOMAIN_TYPE;
	    break;
	}
        break;
      case WLZ_RECTANGLE:
	switch(obj->domain.core->type)
	{
	  case WLZ_RECTANGLE_DOMAIN_INT:
	    oDomTypeStr = "WLZ_RECTANGLE_DOMAIN_INT";
	    break;
	  case WLZ_RECTANGLE_DOMAIN_FLOAT:
	    oDomTypeStr = "WLZ_RECTANGLE_DOMAIN_FLOAT";
	    break;
	  default:
	    errNum = WLZ_ERR_DOMAIN_TYPE;
	    break;
	}
        break;
      case WLZ_LUT:
        oDomTypeStr = "WLZ_LUT";
	break;
      case WLZ_3D_VIEW_STRUCT:
	oDomTypeStr = "WLZ_3D_VIEW_STRUCT";
	break;
      case WLZ_POINTS:
        switch(obj->domain.core->type)
	{
	  case WLZ_POINTS_2I:
	    oDomTypeStr = "WLZ_POINTS_2I";
	    break;
	  case WLZ_POINTS_2D:
	    oDomTypeStr = "WLZ_POINTS_2D";
	    break;
	  case WLZ_POINTS_3I:
	    oDomTypeStr = "WLZ_POINTS_3I";
	    break;
	  case WLZ_POINTS_3D:
	    oDomTypeStr = "WLZ_POINTS_3D";
	    break;
	  default:
	    errNum = WLZ_ERR_DOMAIN_TYPE;
	    break;
        }
	break;
      case WLZ_SPLINE:
        switch(obj->domain.core->type)
        {
	  case WLZ_BSPLINE_C2D:
	    oDomTypeStr = "WLZ_BSPLINE_C2D";
            break;
	  case WLZ_BSPLINE_C3D:
	    oDomTypeStr = "WLZ_BSPLINE_C3D";
            break;
	  default:
	    errNum = WLZ_ERR_DOMAIN_TYPE;
	    break;
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
  return(oDomTypeStr);
}

/*!
* \return	Woolz object type or WLZ_NULL on error.
* \ingroup	WlzStrings
* \brief	Finds an enumerated type for the given object domain
* 		type string.
* \param	oDomTypeStr		Given object domain type string.	
* \param	dstErr			Destination error pointer, may be null.
*/
WlzObjectType	WlzStringToObjDomainType(const char *oDomTypeStr,
				         WlzErrorNum *dstErr)
{
  int		tI0;
  WlzErrorNum	errNum = WLZ_ERR_OBJECT_TYPE;
  WlzObjectType	oDomType = WLZ_NULL;

  if(WlzStringMatchValue(&tI0, oDomTypeStr,
		"WLZ_INTERVALDOMAIN_INTVL", WLZ_INTERVALDOMAIN_INTVL,
		"WLZ_INTERVALDOMAIN_RECT", WLZ_INTERVALDOMAIN_RECT,
		"WLZ_PLANEDOMAIN_DOMAIN", WLZ_PLANEDOMAIN_DOMAIN,
		"WLZ_PLANEDOMAIN_POLYGON", WLZ_PLANEDOMAIN_POLYGON,
		"WLZ_PLANEDOMAIN_BOUNDLIST", WLZ_PLANEDOMAIN_BOUNDLIST,
		"WLZ_PLANEDOMAIN_HISTOGRAM", WLZ_PLANEDOMAIN_HISTOGRAM,
		"WLZ_PLANEDOMAIN_AFFINE", WLZ_PLANEDOMAIN_AFFINE,
		"WLZ_PLANEDOMAIN_WARP", WLZ_PLANEDOMAIN_WARP,
		"WLZ_TRANSFORM_2D_AFFINE", WLZ_TRANSFORM_2D_AFFINE,
		"WLZ_TRANSFORM_3D_AFFINE", WLZ_TRANSFORM_3D_AFFINE,
		"WLZ_WARP_TRANS", WLZ_WARP_TRANS,
		"WLZ_POLYGON_INT", WLZ_POLYGON_INT,
		"WLZ_POLYGON_FLOAT", WLZ_POLYGON_FLOAT,
		"WLZ_POLYGON_DOUBLE", WLZ_POLYGON_DOUBLE,
		"WLZ_BOUNDLIST_PIECE", WLZ_BOUNDLIST_PIECE,
		"WLZ_BOUNDLIST_HOLE", WLZ_BOUNDLIST_HOLE,
		"WLZ_CMESH_2D", WLZ_CMESH_2D,
		"WLZ_CMESH_2D5", WLZ_CMESH_2D5,
		"WLZ_CMESH_3D", WLZ_CMESH_3D,
		"WLZ_HISTOGRAMDOMAIN_INT", WLZ_HISTOGRAMDOMAIN_INT,
		"WLZ_HISTOGRAMDOMAIN_FLOAT", WLZ_HISTOGRAMDOMAIN_FLOAT,
		"WLZ_RECTANGLE_DOMAIN_INT", WLZ_RECTANGLE_DOMAIN_INT,
		"WLZ_RECTANGLE_DOMAIN_FLOAT", WLZ_RECTANGLE_DOMAIN_FLOAT,
		"WLZ_LUT", WLZ_LUT,
		"WLZ_POINTS_2I", WLZ_POINTS_2I,
		"WLZ_POINTS_2D", WLZ_POINTS_2D,
		"WLZ_POINTS_3I", WLZ_POINTS_3I,
		"WLZ_POINTS_3D", WLZ_POINTS_3D,
		"WLZ_BSPLINE_C2D", WLZ_BSPLINE_C2D,
		"WLZ_BSPLINE_C3D", WLZ_BSPLINE_C3D,
		NULL))
  {
    oDomType = (WlzObjectType )tI0;
    errNum = WLZ_ERR_NONE;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(oDomType);
}

/*!
* \return	Pointer to read only string or NULL on error.
* \ingroup      WlzStrings
* \brief	Finds a string for the given object's values type.
* \param	obj			Given object.
* \param	dstErr			Destination error pointer, may be null.
*/
const char	*WlzStringFromObjValuesType(WlzObject *obj, WlzErrorNum *dstErr)
{
  WlzObjectType	valType;
  WlzErrorNum	errNum = WLZ_ERR_NONE;
  const char	*oValTypeStr = NULL;

  if(obj == NULL)
  {
    errNum = WLZ_ERR_OBJECT_NULL;
  }
  else if(obj->values.core == NULL)
  {
    errNum = WLZ_ERR_VALUES_NULL;
  }
  else
  {
    switch(obj->values.core->type)
    {
      case WLZ_LUT:
        oValTypeStr = "WLZ_LUT";
	break;
      case WLZ_INDEXED_VALUES:
	oValTypeStr = "WLZ_INDEXED_VALUES";
        break;
      default:
        /* Other values are a composite of the table and grey type. */
	if(errNum == WLZ_ERR_NONE)
	{
	  switch(obj->type)
	  {
	    case WLZ_2D_DOMAINOBJ:
	      valType = WlzGreyTableTypeToTableType(obj->values.core->type,
	                                            &errNum);
	      switch(valType)
	      {
		case WLZ_GREY_TAB_RAGR:
		  oValTypeStr = "WLZ_GREY_TAB_RAGR";
		  break;
		case WLZ_GREY_TAB_RECT:
		  oValTypeStr = "WLZ_GREY_TAB_RECT";
		  break;
		case WLZ_GREY_TAB_INTL:
		  oValTypeStr = "WLZ_GREY_TAB_INTL";
		  break;
		case WLZ_GREY_TAB_TILED:
		  oValTypeStr = "WLZ_GREY_TAB_TILED";
		  break;
		default:
		  errNum = WLZ_ERR_VALUES_TYPE;
		  break;
	      }
	      break;
	    case WLZ_3D_DOMAINOBJ:
	      switch(obj->values.core->type)
	      {
  		case WLZ_VALUETABLE_TILED_INT:        /* FALLTHROUGH */
  		case WLZ_VALUETABLE_TILED_SHORT:      /* FALLTHROUGH */
  		case WLZ_VALUETABLE_TILED_UBYTE:      /* FALLTHROUGH */
  		case WLZ_VALUETABLE_TILED_FLOAT:      /* FALLTHROUGH */
  		case WLZ_VALUETABLE_TILED_DOUBLE:     /* FALLTHROUGH */
  		case WLZ_VALUETABLE_TILED_BIT:        /* FALLTHROUGH */
  		case WLZ_VALUETABLE_TILED_RGBA:       /* FALLTHROUGH */
  		case WLZ_VALUETABLE_TILED_ARY_INT:    /* FALLTHROUGH */
  		case WLZ_VALUETABLE_TILED_ARY_SHORT:  /* FALLTHROUGH */
  		case WLZ_VALUETABLE_TILED_ARY_UBYTE:  /* FALLTHROUGH */
  		case WLZ_VALUETABLE_TILED_ARY_FLOAT:  /* FALLTHROUGH */
  		case WLZ_VALUETABLE_TILED_ARY_DOUBLE: /* FALLTHROUGH */
  		case WLZ_VALUETABLE_TILED_ARY_BIT:    /* FALLTHROUGH */
  		case WLZ_VALUETABLE_TILED_ARY_RGBA:
		  oValTypeStr = "WLZ_GREY_TAB_TILED";
		  break;
		case WLZ_VOXELVALUETABLE_GREY:
		  oValTypeStr = "WLZ_VOXELVALUETABLE_GREY";
		  break;
		default:
		  errNum = WLZ_ERR_VOXELVALUES_TYPE;
		  break;
	      }
	      break;
	    case WLZ_TRANS_OBJ:
	      oValTypeStr = WlzStringFromObjType(obj->values.obj, &errNum);
	      break;
	    case WLZ_POINTS:
	      valType = obj->values.core->type;
	      switch(valType)                
	      {
		case WLZ_POINT_VALUES:
		  oValTypeStr = "WLZ_POINT_VALUES";
		  break;
	        default:
		  errNum = WLZ_ERR_VOXELVALUES_TYPE;
		  break;
	      }
	      break;
	    default:
	      errNum = WLZ_ERR_OBJECT_TYPE;
	      break;
	  }
	}
	break;
    }
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(oValTypeStr);
}

/*!
* \return	Woolz object type or WLZ_NULL on error.
* \ingroup      WlzStrings
* \brief	Finds an enumerated type for the given object values type
*		string.
* \param	oValTypeStr		Given object values type string.
* \param	dstErr			Destination error pointer, may be null.
*/
WlzObjectType	WlzStringToObjValuesType(const char *oValTypeStr,
				         WlzErrorNum *dstErr)
{
  int		tI0;
  WlzErrorNum	errNum = WLZ_ERR_OBJECT_TYPE;
  WlzObjectType	oValType = WLZ_NULL;

  if(WlzStringMatchValue(&tI0, oValTypeStr,
		"WLZ_GREY_TAB_RAGR", WLZ_GREY_TAB_RAGR,
		"WLZ_GREY_TAB_RECT", WLZ_GREY_TAB_RECT,
		"WLZ_GREY_TAB_INTL", WLZ_GREY_TAB_INTL,
		"WLZ_VOXELVALUETABLE_GREY", WLZ_VOXELVALUETABLE_GREY,
		"WLZ_LUT", WLZ_LUT,
		"WLZ_POINT_VALUES", WLZ_POINT_VALUES,
		NULL))
  {
    oValType = (WlzObjectType )tI0;
    errNum = WLZ_ERR_NONE;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(oValType);
}

/*!
* \return	Read only string or NULL on error.
* \ingroup 	WlzStrings
* \brief	Finds a string for the given scalar feature type.
* \param	fType			Given scalar feature type.
* \param	dstErr			Destination error pointer, may be null.
*/
const char	*WlzStringFromScalarFeatureType(WlzScalarFeatureType fType,
					WlzErrorNum *dstErr)
{
  WlzErrorNum	errNum = WLZ_ERR_NONE;
  const char	*fTypeStr = NULL;

  switch(fType)
  {
    case WLZ_SCALARFEATURE_VALUE:
      fTypeStr = "WLZ_SCALARFEATURE_VALUE";
      break;
    case WLZ_SCALARFEATURE_GRADIENT:
      fTypeStr = "WLZ_SCALARFEATURE_GRADIENT";
      break;
    default:
      errNum = WLZ_ERR_PARAM_DATA;
      break;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(fTypeStr);
}

/*!
* \return	Feature type.
* \brief	Finds an enumerated type for the given scalar feature type.
* \param	fTypeStr		Given scalar feature type string.
* \param	dstErr			Destination error pointer, may be null.
*/
WlzScalarFeatureType WlzStringToScalarFeatureType(const char *fTypeStr,
					WlzErrorNum *dstErr)
{
  int		tI0;
  WlzErrorNum	errNum = WLZ_ERR_PARAM_TYPE;
  WlzScalarFeatureType fType = WLZ_SCALARFEATURE_VALUE;

  if(WlzStringMatchValue(&tI0, fTypeStr,
  		"WLZ_SCALARFEATURE_VALUE", WLZ_SCALARFEATURE_VALUE,
		"WLZ_SCALARFEATURE_GRADIENT", WLZ_SCALARFEATURE_GRADIENT,
		NULL))
  {
    fType = (WlzScalarFeatureType )tI0;
    errNum = WLZ_ERR_NONE;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(fType);
}

/*!
* \return	Read only string or NULL on error.
* \ingroup 	WlzStrings
* \brief	Finds a string for the given property's type.
* \param	prop			Given property.
* \param	dstErr			Destination error pointer, may be null.
*/
const char	*WlzStringFromPropertyType(WlzProperty prop,
				      WlzErrorNum *dstErr)
{
  WlzErrorNum	errNum = WLZ_ERR_NONE;
  const char	*pTypeStr = NULL;

  if(prop.core == NULL)
  {
    errNum = WLZ_ERR_OBJECT_NULL;
  }
  else
  {
    switch(prop.core->type)
    {
      case WLZ_PROPERTY_SIMPLE:
        pTypeStr = "WLZ_PROPERTY_SIMPLE";
	break;
      case WLZ_PROPERTY_EMAP:
        pTypeStr = "WLZ_PROPERTY_EMAP";
	break;
      case WLZ_PROPERTY_NAME:
        pTypeStr = "WLZ_PROPERTY_NAME";
	break;
      case WLZ_PROPERTY_GREY:
        pTypeStr = "WLZ_PROPERTY_GREY";
	break;
      case WLZ_PROPERTY_TEXT:
        pTypeStr = "WLZ_PROPERTY_TEXT";
	break;
      default:
        errNum = WLZ_ERR_PROPERTY_TYPE;
	break;
    }
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(pTypeStr);
}

/*!
* \return	Woolz property type which will be WLZ_NULL if no matching
*		type is found..
* \ingroup      WlzStrings
* \brief	Finds an enumerated type for the given property
*               type string.
* \param	pStr			Given property type string.
* \param	dstErr			Destination error pointer, may be null.
*/
WlzObjectType WlzStringToPropertyType(const char *pStr, WlzErrorNum *dstErr)
{
  int		tI0;
  WlzObjectType	pType = WLZ_NULL;
  WlzErrorNum	errNum = WLZ_ERR_PROPERTY_TYPE;

  if(WlzStringMatchValue(&tI0, pStr,
  		         "WLZ_PROPERTY_SIMPLE", WLZ_PROPERTY_SIMPLE,
			 "WLZ_PROPERTY_EMAP", WLZ_PROPERTY_EMAP,
			 "WLZ_PROPERTY_NAME", WLZ_PROPERTY_NAME,
			 "WLZ_PROPERTY_GREY", WLZ_PROPERTY_GREY,
			 "WLZ_PROPERTY_TEXT", WLZ_PROPERTY_TEXT,
			 NULL))
  {
    pType = (WlzObjectType )tI0;
    errNum = WLZ_ERR_NONE;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(pType);
}


/*!
* \return	Read only string or NULL on error.
* \ingroup 	WlzStrings
* \brief	Finds a string for the given EMAP property's type.
* \param	eProp			Given EMAP property.
* \param	dstErr			Destination error pointer, may be null.
*/
const char	*WlzStringFromEMAPPropertyType(WlzEMAPProperty *eProp,
				      	WlzErrorNum *dstErr)
{
  WlzErrorNum	errNum = WLZ_ERR_NONE;
  const char	*pTypeStr = NULL;

  if(eProp == NULL)
  {
    errNum = WLZ_ERR_OBJECT_NULL;
  }
  else
  {
    switch(eProp->emapType)
    {
      case WLZ_EMAP_PROPERTY_GREY_MODEL:
        pTypeStr = "WLZ_EMAP_PROPERTY_GREY_MODEL";
	break;
      case WLZ_EMAP_PROPERTY_GREY_OTHER:
        pTypeStr = "WLZ_EMAP_PROPERTY_GREY_OTHER";
	break;
      case WLZ_EMAP_PROPERTY_DOMAIN_ANATOMY:
        pTypeStr = "WLZ_EMAP_PROPERTY_DOMAIN_ANATOMY";
	break;
      case WLZ_EMAP_PROPERTY_DOMAIN_OTHER:
        pTypeStr = "WLZ_EMAP_PROPERTY_DOMAIN_OTHER";
	break;
      case WLZ_EMAP_PROPERTY_TRANSFORM:
        pTypeStr = "WLZ_EMAP_PROPERTY_TRANSFORM";
	break;
      case WLZ_EMAP_PROPERTY_DUMMY:
        pTypeStr = "WLZ_EMAP_PROPERTY_DUMMY";
	break;
      default:
        errNum = WLZ_ERR_PROPERTY_TYPE;
	break;
    }
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(pTypeStr);
}

/*!
* \return	EMAP property type which will be WLZ_EMAP_PROPERTY_DUMMY
*		if no matching type is found..
* \ingroup      WlzStrings
* \brief	Finds an enumerated type for the given EMAP property
*               type string.
* \param	pStr			Given property type string.
* \param	dstErr			Destination error pointer, may be null.
*/
WlzEMAPPropertyType WlzStringToEMAPPropertyType(const char *pStr,
					WlzErrorNum *dstErr)
{
  int		tI0;
  WlzEMAPPropertyType	pType = WLZ_EMAP_PROPERTY_DUMMY;
  WlzErrorNum	errNum = WLZ_ERR_PROPERTY_TYPE;

  if(WlzStringMatchValue(&tI0, pStr,
       "WLZ_EMAP_PROPERTY_GREY_MODEL", WLZ_EMAP_PROPERTY_GREY_MODEL,
       "WLZ_EMAP_PROPERTY_GREY_OTHER", WLZ_EMAP_PROPERTY_GREY_OTHER,
       "WLZ_EMAP_PROPERTY_ANATOMY_DOMAIN", WLZ_EMAP_PROPERTY_DOMAIN_ANATOMY,
       "WLZ_EMAP_PROPERTY_OTHER_DOMAIN", WLZ_EMAP_PROPERTY_DOMAIN_OTHER,
       "WLZ_EMAP_PROPERTY_TRANSFORM", WLZ_EMAP_PROPERTY_TRANSFORM,
       "WLZ_EMAP_PROPERTY_DUMMY", WLZ_EMAP_PROPERTY_DUMMY,
       NULL))
  {
    pType = (WlzEMAPPropertyType )tI0;
    errNum = WLZ_ERR_NONE;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(pType);
}

/*!
* \return	Pointer to read only string or NULL on error.
* \ingroup      WlzStrings
* \brief	Finds a string for the given transform type.
* \param	tType			Given transform type.
* \param	dstErr			Destination error pointer, may be null.
*/
const char	*WlzStringFromTransformType(WlzTransformType tType,
				            WlzErrorNum *dstErr)
{
  const char	*tStr = NULL;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  switch(tType)
  {
    case WLZ_TRANSFORM_EMPTY:
      tStr = "WLZ_TRANSFORM_EMPTY";
      break;
    case WLZ_TRANSFORM_2D_AFFINE:
      tStr = "WLZ_TRANSFORM_2D_AFFINE";
      break;
    case WLZ_TRANSFORM_2D_REG:
      tStr = "WLZ_TRANSFORM_2D_REG";
      break;
    case WLZ_TRANSFORM_2D_TRANS:
      tStr = "WLZ_TRANSFORM_2D_TRANS";
      break;
    case WLZ_TRANSFORM_2D_NOSHEAR:
      tStr = "WLZ_TRANSFORM_2D_NOSHEAR";
      break;
    case WLZ_TRANSFORM_3D_AFFINE:
      tStr = "WLZ_TRANSFORM_3D_AFFINE";
      break;
    case WLZ_TRANSFORM_3D_REG:
      tStr = "WLZ_TRANSFORM_3D_REG";
      break;
    case WLZ_TRANSFORM_3D_TRANS:
      tStr = "WLZ_TRANSFORM_3D_TRANS";
      break;
    case WLZ_TRANSFORM_3D_NOSHEAR:
      tStr = "WLZ_TRANSFORM_3D_NOSHEAR";
      break;
    case WLZ_TRANSFORM_2D_BASISFN:
      tStr = "WLZ_TRANSFORM_2D_BASISFN";
      break;
    case WLZ_TRANSFORM_2D5_BASISFN:
      tStr = "WLZ_TRANSFORM_2D5_BASISFN";
      break;
    case WLZ_TRANSFORM_3D_BASISFN:
      tStr = "WLZ_TRANSFORM_3D_BASISFN";
      break;
    case WLZ_TRANSFORM_2D_MESH:
      tStr = "WLZ_TRANSFORM_2D_MESH";
      break;
    case WLZ_TRANSFORM_2D5_MESH:
      tStr = "WLZ_TRANSFORM_2D5_MESH";
      break;
    case WLZ_TRANSFORM_3D_MESH:
      tStr = "WLZ_TRANSFORM_3D_MESH";
      break;
    case WLZ_TRANSFORM_2D_CMESH:
      tStr = "WLZ_TRANSFORM_2D_CMESH";
      break;
    case WLZ_TRANSFORM_3D_CMESH:
      tStr = "WLZ_TRANSFORM_3D_CMESH";
      break;
    default:
      errNum = WLZ_ERR_TRANSFORM_TYPE;
      break;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(tStr);
}

/*!
* \return	Woolz transform type.
* \ingroup      WlzStrings
* \brief	Finds an enumerated type for the given transform
*               type string.
* \param	tStr			Given transform type string.
* \param	dstErr			Destination error pointer, may be null.
*/
WlzTransformType WlzStringToTransformType(const char *tStr,
				          WlzErrorNum *dstErr)
{
  int		tI0;
  WlzTransformType	tType = WLZ_TRANSFORM_2D_AFFINE;
  WlzErrorNum	errNum = WLZ_ERR_TRANSFORM_TYPE;

  if(WlzStringMatchValue(&tI0, tStr,
		      "WLZ_TRANSFORM_EMPTY", WLZ_TRANSFORM_EMPTY,
		      "WLZ_TRANSFORM_2D_AFFINE", WLZ_TRANSFORM_2D_AFFINE,
		      "WLZ_TRANSFORM_2D_REG", WLZ_TRANSFORM_2D_REG,
		      "WLZ_TRANSFORM_2D_TRANS", WLZ_TRANSFORM_2D_TRANS,
		      "WLZ_TRANSFORM_2D_NOSHEAR", WLZ_TRANSFORM_2D_NOSHEAR,
		      "WLZ_TRANSFORM_3D_AFFINE", WLZ_TRANSFORM_3D_AFFINE,
		      "WLZ_TRANSFORM_3D_REG", WLZ_TRANSFORM_3D_REG,
		      "WLZ_TRANSFORM_3D_TRANS", WLZ_TRANSFORM_3D_TRANS,
		      "WLZ_TRANSFORM_3D_NOSHEAR", WLZ_TRANSFORM_3D_NOSHEAR,
		      "WLZ_TRANSFORM_2D_BASISFN", WLZ_TRANSFORM_2D_BASISFN,
		      "WLZ_TRANSFORM_2D5_BASISFN", WLZ_TRANSFORM_2D5_BASISFN,
		      "WLZ_TRANSFORM_3D_BASISFN", WLZ_TRANSFORM_3D_BASISFN,
		      "WLZ_TRANSFORM_2D_MESH", WLZ_TRANSFORM_2D_MESH,
		      "WLZ_TRANSFORM_2D5_MESH", WLZ_TRANSFORM_2D5_MESH,
		      "WLZ_TRANSFORM_3D_MESH", WLZ_TRANSFORM_3D_MESH,
		      "WLZ_TRANSFORM_2D_CMESH", WLZ_TRANSFORM_2D_CMESH,
		      "WLZ_TRANSFORM_3D_CMESH", WLZ_TRANSFORM_3D_CMESH,
		      NULL))
  {
    tType = (WlzTransformType )tI0;
    errNum = WLZ_ERR_NONE;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(tType);
}

/*!
* \return	Pointer to read only string or NULL on error.
* \ingroup      WlzStrings
* \brief	Finds a string for the given mesh generation method.
* \param	mtd			Given mesh generation method.
* \param	dstErr			Destination error pointer, may
*                                       be null.
*/
const char	*WlzStringFromMeshGenMethod(WlzMeshGenMethod mtd,
				            WlzErrorNum *dstErr)
{
  const char	*tStr = NULL;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  switch(mtd)
  {
    case WLZ_MESH_GENMETHOD_BLOCK:
      tStr = "WLZ_MESH_GENMETHOD_BLOCK";
      break;
    case WLZ_MESH_GENMETHOD_GRADIENT:
      tStr = "WLZ_MESH_GENMETHOD_GRADIENT";
      break;
    case WLZ_MESH_GENMETHOD_CONFORM:
      tStr = "WLZ_MESH_GENMETHOD_CONFORM";
      break;
    default:
      errNum = WLZ_ERR_PARAM_DATA;
      break;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(tStr);
}

/*!
* \return	Woolz mesh generation method.
* \ingroup      WlzStrings
* \brief	Finds an enumerated type for the given mesh
*               generation method.
* \param	tStr			Given mesh generation string.
* \param	dstErr			Destination error pointer, may be null.
*/
WlzMeshGenMethod WlzStringToMeshGenMethod(const char *tStr,
					  WlzErrorNum *dstErr)
{
  int		tI0;
  WlzMeshGenMethod  mtd = WLZ_MESH_GENMETHOD_BLOCK;
  WlzErrorNum	errNum = WLZ_ERR_TRANSFORM_TYPE;

  if(WlzStringMatchValue(&tI0, tStr,
		    "WLZ_MESH_GENMETHOD_BLOCK", WLZ_MESH_GENMETHOD_BLOCK,
		    "WLZ_MESH_GENMETHOD_GRADIENT", WLZ_MESH_GENMETHOD_GRADIENT,
		    "WLZ_MESH_GENMETHOD_CONFORM", WLZ_MESH_GENMETHOD_CONFORM,
		    NULL))
  {
    mtd = (WlzMeshGenMethod )tI0;
    errNum = WLZ_ERR_NONE;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(mtd);
}

/*!
* \return	Woolz function type.
* \ingroup      WlzStrings
* \brief	Finds an enumerated type for the given function type.
* \param	tStr			Given function type string.
* \param	dstErr			Destination error pointer, may be null.
*/
WlzFnType	WlzStringToFnType(const char *tStr,
				  WlzErrorNum *dstErr)
{
  int		tI0;
  WlzFnType  fn = WLZ_FN_BASIS_2DGAUSS;
  WlzErrorNum	errNum = WLZ_ERR_TRANSFORM_TYPE;

  if(WlzStringMatchValue(&tI0, tStr,
		       "WLZ_FN_BASIS_2DGAUSS", WLZ_FN_BASIS_2DGAUSS,
		       "WLZ_FN_BASIS_3DGAUSS", WLZ_FN_BASIS_3DGAUSS,
		       "WLZ_FN_BASIS_2DIMQ", WLZ_FN_BASIS_2DIMQ,
		       "WLZ_FN_BASIS_3DIMQ", WLZ_FN_BASIS_3DIMQ,
		       "WLZ_FN_BASIS_2DPOLY", WLZ_FN_BASIS_2DPOLY,
		       "WLZ_FN_BASIS_3DPOLY", WLZ_FN_BASIS_3DPOLY,
		       "WLZ_FN_BASIS_2DMQ", WLZ_FN_BASIS_2DMQ,
		       "WLZ_FN_BASIS_3DMQ", WLZ_FN_BASIS_3DMQ,
		       "WLZ_FN_BASIS_2DTPS", WLZ_FN_BASIS_2DTPS,
		       "WLZ_FN_BASIS_3DTPS", WLZ_FN_BASIS_3DTPS,
		       "WLZ_FN_BASIS_2DCONF_POLY", WLZ_FN_BASIS_2DCONF_POLY,
		       "WLZ_FN_BASIS_3DCONF_POLY", WLZ_FN_BASIS_3DCONF_POLY,
		       "WLZ_FN_BASIS_3DMOS", WLZ_FN_BASIS_3DMOS,
		       "WLZ_FN_BASIS_SCALAR_3DMOS", WLZ_FN_BASIS_SCALAR_3DMOS,
		       NULL))
  {
    fn = (WlzFnType )tI0;
    errNum = WLZ_ERR_NONE;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(fn);
}

/*!
* \return	Pointer to read only string or NULL on error.
* \ingroup      WlzStrings
* \brief	Finds a string for the given function type.
* \param	fn			Given function type.
* \param	dstErr			Destination error pointer, may
*                                       be null.
*/
const char	*WlzStringFromFnType(WlzFnType fn,
				     WlzErrorNum *dstErr)
{
  const char	*tStr = NULL;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  switch(fn)
  {
    case WLZ_FN_BASIS_2DGAUSS:
      tStr = "WLZ_FN_BASIS_2DGAUSS";
      break;
    case WLZ_FN_BASIS_3DGAUSS:
      tStr = "WLZ_FN_BASIS_3DGAUSS";
      break;
    case WLZ_FN_BASIS_2DIMQ:
      tStr = "WLZ_FN_BASIS_2DIMQ";
      break;
    case WLZ_FN_BASIS_3DIMQ:
      tStr = "WLZ_FN_BASIS_3DIMQ";
      break;
    case WLZ_FN_BASIS_2DPOLY:
      tStr = "WLZ_FN_BASIS_2DPOLY";
      break;
    case WLZ_FN_BASIS_3DPOLY:
      tStr = "WLZ_FN_BASIS_3DPOLY";
      break;
    case WLZ_FN_BASIS_2DMQ:
      tStr = "WLZ_FN_BASIS_2DMQ";
      break;
    case WLZ_FN_BASIS_3DMQ:
      tStr = "WLZ_FN_BASIS_3DMQ";
      break;
    case WLZ_FN_BASIS_2DTPS:
      tStr = "WLZ_FN_BASIS_2DTPS";
      break;
    case WLZ_FN_BASIS_3DTPS:
      tStr = "WLZ_FN_BASIS_3DTPS";
      break;
    case WLZ_FN_BASIS_2DCONF_POLY:
      tStr = "WLZ_FN_BASIS_2DCONF_POLY";
      break;
    case WLZ_FN_BASIS_3DCONF_POLY:
      tStr = "WLZ_FN_BASIS_3DCONF_POLY";
      break;
    case WLZ_FN_BASIS_3DMOS:
      tStr = "WLZ_FN_BASIS_3DMOS";
      break;
    case WLZ_FN_BASIS_SCALAR_3DMOS:
      tStr = "WLZ_FN_BASIS_SCALAR_3DMOS";
      break;
    default:
      errNum = WLZ_ERR_PARAM_DATA;
      break;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(tStr);
}

/*!
* \return	Pointer to read only string or NULL on error.
* \ingroup      WlzStrings
* \brief	Finds a string for the given transform type.
* \param	mType			Given model type.
* \param	dstErr			Destination error pointer, may
*                                       be null.
*/
const char	*WlzStringFromGMModelType(WlzGMModelType mType,
				          WlzErrorNum *dstErr)
{
  const char	*tStr = NULL;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  switch(mType)
  {
    case WLZ_GMMOD_2I:
      tStr = "WLZ_GMMOD_2I";
      break;
    case WLZ_GMMOD_2D:
      tStr = "WLZ_GMMOD_2D";
      break;
    case WLZ_GMMOD_2N:
      tStr = "WLZ_GMMOD_2N";
      break;
    case WLZ_GMMOD_3I:
      tStr = "WLZ_GMMOD_3I";
      break;
    case WLZ_GMMOD_3D:
      tStr = "WLZ_GMMOD_3D";
      break;
    case WLZ_GMMOD_3N:
      tStr = "WLZ_GMMOD_3N";
      break;
    default:
      errNum = WLZ_ERR_DOMAIN_TYPE;
      break;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(tStr);
}

/*!
* \return	Woolz GM model type.
* \ingroup      WlzStrings
* \brief	Finds an enumerated type for the given GM model type string.
* \param	tStr			Given GM Model type string.
* \param	dstErr			Destination error pointer, may
*                                       be null.
*/
WlzGMModelType	WlzStringToGMModelType(const char *tStr,
					WlzErrorNum *dstErr)
{
  int		tI0;
  WlzGMModelType mType = WLZ_GMMOD_2I;
  WlzErrorNum	errNum = WLZ_ERR_DOMAIN_TYPE;

  if(WlzStringMatchValue(&tI0, tStr,
			 "WLZ_GMMOD_2I", WLZ_GMMOD_2I,
			 "WLZ_GMMOD_2D", WLZ_GMMOD_2D,
			 "WLZ_GMMOD_2N", WLZ_GMMOD_2N,
			 "WLZ_GMMOD_3I", WLZ_GMMOD_3I,
			 "WLZ_GMMOD_3D", WLZ_GMMOD_3D,
			 "WLZ_GMMOD_3N", WLZ_GMMOD_3N,
			 NULL))
  {
    mType = (WlzGMModelType )tI0;
    errNum = WLZ_ERR_NONE;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(mType);
}

/*!
* \return	Pointer to read only string or NULL on error.
* \ingroup      WlzStrings
* \brief	Finds a string for the given grey type.
* \param	gType			Given grey type.
* \param	dstErr			Destination error pointer, may
*                                       be null.
*/
const char	*WlzStringFromGreyType(WlzGreyType gType,
				       WlzErrorNum *dstErr)
{
  const char	*gStr = NULL;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  switch(gType)
  {
    case WLZ_GREY_INT:
      gStr = "WLZ_GREY_INT";
      break;
    case WLZ_GREY_SHORT:
      gStr = "WLZ_GREY_SHORT";
      break;
    case WLZ_GREY_UBYTE:
      gStr = "WLZ_GREY_UBYTE";
      break;
    case WLZ_GREY_FLOAT:
      gStr = "WLZ_GREY_FLOAT";
      break;
    case WLZ_GREY_DOUBLE:
      gStr = "WLZ_GREY_DOUBLE";
      break;
    case WLZ_GREY_RGBA:
      gStr = "WLZ_GREY_RGBA";
      break;
    default:
      errNum = WLZ_ERR_GREY_TYPE;
      break;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(gStr);
}

/*!
* \return	Woolz grey type or WLZ_GREY_ERROR on error.
* \ingroup      WlzStrings
* \brief	Finds an enumerated type for the given grey type
*               string.
* \param	gStr			Given grey type string.
* \param	dstErr			Destination error pointer, may
*                                       be null.
*/
WlzGreyType	WlzStringToGreyType(const char *gStr,
				    WlzErrorNum *dstErr)
{
  int		tI0;
  WlzGreyType	gType = WLZ_GREY_ERROR;
  WlzErrorNum	errNum = WLZ_ERR_GREY_TYPE;

  if(WlzStringMatchValue(&tI0, gStr,
  		         "WLZ_GREY_INT", WLZ_GREY_INT,
			 "WLZ_GREY_SHORT", WLZ_GREY_SHORT,
			 "WLZ_GREY_UBYTE", WLZ_GREY_UBYTE,
			 "WLZ_GREY_FLOAT", WLZ_GREY_FLOAT,
			 "WLZ_GREY_DOUBLE", WLZ_GREY_DOUBLE,
			 "WLZ_GREY_RGBA", WLZ_GREY_RGBA,
			 NULL))
  {
    gType = (WlzGreyType )tI0;
    errNum = WLZ_ERR_NONE;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(gType);
}

/*!
* \return	Pointer to read only string or NULL on error.
* \ingroup      WlzStrings
* \brief	Finds a string for the given vertex type.
* \param	vType			Given vertex type.
* \param	dstErr			Destination error pointer, may
*                                       be null.
*/
const char	*WlzStringFromVertexType(WlzVertexType vType,
				       WlzErrorNum *dstErr)
{
  const char	*vStr = NULL;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  switch(vType)
  {
    case WLZ_VERTEX_I2:
      vStr = "WLZ_VERTEX_I2";
      break;
    case WLZ_VERTEX_F2:
      vStr = "WLZ_VERTEX_F2";
      break;
    case WLZ_VERTEX_D2:
      vStr = "WLZ_VERTEX_D2";
      break;
    case WLZ_VERTEX_I3:
      vStr = "WLZ_VERTEX_I3";
      break;
    case WLZ_VERTEX_F3:
      vStr = "WLZ_VERTEX_F3";
      break;
    case WLZ_VERTEX_D3:
      vStr = "WLZ_VERTEX_D3";
      break;
    case WLZ_VERTEX_L2:
      vStr = "WLZ_VERTEX_L2";
      break;
    case WLZ_VERTEX_L3:
      vStr = "WLZ_VERTEX_L3";
      break;
    default:
      errNum = WLZ_ERR_PARAM_TYPE;
      break;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(vStr);
}

/*!
* \return	Woolz vertex type or WLZ_VERTEX_ERROR on error.
* \ingroup      WlzStrings
* \brief	Finds an enumerated type for the given vertex type
*               string.
* \param	vStr			Given vertex type string.
* \param	dstErr			Destination error pointer, may
*                                       be null.
*/
WlzVertexType	WlzStringToVertexType(const char *vStr,
				      WlzErrorNum *dstErr)
{
  int		tI0;
  WlzGreyType	vType = WLZ_VERTEX_ERROR;
  WlzErrorNum	errNum = WLZ_ERR_PARAM_TYPE;

  if(WlzStringMatchValue(&tI0, vStr,
  		         "WLZ_VERTEX_I2", WLZ_VERTEX_I2,
			 "WLZ_VERTEX_F2", WLZ_VERTEX_F2,
			 "WLZ_VERTEX_D2", WLZ_VERTEX_D2,
			 "WLZ_VERTEX_I3", WLZ_VERTEX_I3,
			 "WLZ_VERTEX_F3", WLZ_VERTEX_F3,
			 "WLZ_VERTEX_D3", WLZ_VERTEX_D3,
			 "WLZ_VERTEX_L2", WLZ_VERTEX_L2,
			 "WLZ_VERTEX_L3", WLZ_VERTEX_L3,
			 NULL))
  {
    vType = (WlzVertexType )tI0;
    errNum = WLZ_ERR_NONE;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(vType);
}

/*!
* \return	Pointer to read only string or NULL on error.
* \ingroup      WlzStrings
* \brief	Finds a string for the given interpolation type.
* \param	iType			Given interpolation type.
* \param	dstErr			Destination error pointer, may
*                                       be null.
*/
const char	*WlzStringFromInterpolationType(WlzInterpolationType iType,
				       WlzErrorNum *dstErr)
{
  const char	*iStr = NULL;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  switch(iType)
  {
    case WLZ_INTERPOLATION_NEAREST:
      iStr = "WLZ_INTERPOLATION_NEAREST";
      break;
    case WLZ_INTERPOLATION_LINEAR:
      iStr = "WLZ_INTERPOLATION_LINEAR";
      break;
    case WLZ_INTERPOLATION_CLASSIFY_1:
      iStr = "WLZ_INTERPOLATION_CLASSIFY_1";
      break;
    case WLZ_INTERPOLATION_CALLBACK:
      iStr = "WLZ_INTERPOLATION_CALLBACK";
      break;
    default:
      errNum = WLZ_ERR_PARAM_DATA;
      break;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(iStr);
}

/*!
* \return	Woolz interpolation type.
* \ingroup      WlzStrings
* \brief	Finds an enumerated type for the given interpolation type
*               string.
* \param	iStr			Given interpolation type string.
* \param	dstErr			Destination error pointer, may
*                                       be null.
*/
WlzInterpolationType WlzStringToInterpolationType(const char *iStr,
				    WlzErrorNum *dstErr)
{
  int		tI0;
  WlzInterpolationType iType = WLZ_INTERPOLATION_NEAREST;
  WlzErrorNum	errNum = WLZ_ERR_PARAM_DATA;

  if(WlzStringMatchValue(&tI0, iStr,
		 "WLZ_INTERPOLATION_NEAREST", WLZ_INTERPOLATION_NEAREST,
		 "WLZ_INTERPOLATION_LINEAR", WLZ_INTERPOLATION_LINEAR,
		 "WLZ_INTERPOLATION_CLASSIFY_1", WLZ_INTERPOLATION_CLASSIFY_1,
		 "WLZ_INTERPOLATION_CALLBACK", WLZ_INTERPOLATION_CALLBACK,
		 NULL))
  {
    iType = (WlzInterpolationType )tI0;
    errNum = WLZ_ERR_NONE;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(iType);
}

/*!
* \return	Pointer to read only string or NULL on error.
* \ingroup      WlzStrings
* \brief	Finds a string for the given values attachment type.
* \param	at			Given values attachment type.
* \param	dstErr			Destination error pointer, may
*                                       be null.
*/
const char	*WlzStringFromValueAttachType(WlzValueAttach at,
				              WlzErrorNum *dstErr)
{
  const char	*aStr = NULL;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  switch(at)
  {
    case WLZ_VALUE_ATTACH_NONE:
      aStr = "WLZ_VALUE_ATTACH_NONE";
      break;
    case WLZ_VALUE_ATTACH_NOD:
      aStr = "WLZ_VALUE_ATTACH_NOD";
      break;
    case WLZ_VALUE_ATTACH_ELM:
      aStr = "WLZ_VALUE_ATTACH_ELM";
      break;
    default:
      errNum = WLZ_ERR_PARAM_DATA;
      break;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(aStr);
}

/*!
* \return	Woolz values attachment type.
* \ingroup      WlzStrings
* \brief	Finds an enumerated type for the given values attachment
* 		type string.
* \param	aStr			Given values attachment type string.
* \param	dstErr			Destination error pointer, may
*                                       be null.
*/
WlzValueAttach WlzStringToValuesAttachType(const char *aStr,
				    WlzErrorNum *dstErr)
{
  int		tI0;
  WlzValueAttach at = WLZ_VALUE_ATTACH_NONE;
  WlzErrorNum	errNum = WLZ_ERR_PARAM_DATA;

  if(WlzStringMatchValue(&tI0, aStr,
		 "WLZ_VALUE_ATTACH_NONE", WLZ_VALUE_ATTACH_NONE,
		 "WLZ_VALUE_ATTACH_NOD", WLZ_VALUE_ATTACH_NOD,
		 "WLZ_VALUE_ATTACH_ELM", WLZ_VALUE_ATTACH_ELM,
		 NULL))
  {
    at = (WlzValueAttach )tI0;
    errNum = WLZ_ERR_NONE;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(at);
}

/*!
* \return	Pointer to read only string or NULL on error.
* \ingroup      WlzStrings
* \brief	Finds a string for the given 3D view mode.
* \param	vMode			Given 3D view mode.
* \param	dstErr			Destination error pointer, may
*                                       be null.
*/
const char	*WlzStringFromThreeDViewMode(WlzThreeDViewMode vMode,
				             WlzErrorNum *dstErr)
{
  const char	*vStr = NULL;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  switch(vMode)
  {
    case WLZ_STATUE_MODE:
      vStr = "WLZ_STATUE_MODE";
      break;
    case WLZ_UP_IS_UP_MODE:
      vStr = "WLZ_UP_IS_UP_MODE";
      break;
    case WLZ_FIXED_LINE_MODE:
      vStr = "WLZ_FIXED_LINE_MODE";
      break;
    case WLZ_ZERO_ZETA_MODE:
      vStr = "WLZ_ZERO_ZETA_MODE";
      break;
    case WLZ_ZETA_MODE:
      vStr = "WLZ_ZETA_MODE";
      break;
    default:
      errNum = WLZ_ERR_PARAM_DATA;
      break;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(vStr);
}

/*!
* \return       Marker type.
* \brief        Gets a 3D view mode from a string.
* \param        vStr               	Given 3D view mode string.
* \param        dstErr                  Destination error pointer, may be NULL.
*/
WlzThreeDViewMode WlzStringToThreeDViewMode(const char *vStr,
				            WlzErrorNum *dstErr)
{
  int           tI0;
  WlzThreeDViewMode vMode = WLZ_STATUE_MODE;
  WlzErrorNum   errNum = WLZ_ERR_PARAM_TYPE;

  if(WlzStringMatchValue(&tI0, vStr,
			 "WLZ_STATUE_MODE", WLZ_STATUE_MODE,
			 "WLZ_UP_IS_UP_MODE", WLZ_UP_IS_UP_MODE,
			 "WLZ_FIXED_LINE_MODE", WLZ_FIXED_LINE_MODE,
			 "WLZ_ZERO_ZETA_MODE", WLZ_ZERO_ZETA_MODE,
			 "WLZ_ZETA_MODE", WLZ_ZETA_MODE,
                         NULL))
  {
    vMode = tI0;
    errNum = WLZ_ERR_NONE;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(vMode);
}

/*!
* \return	Pointer to read only string or NULL on error.
* \ingroup      WlzStrings
* \brief	Finds a string for the given region connected calculus
* 		class.
* \param	cls			Given RCC class.
* \param	dstErr			Destination error pointer, may
*                                       be null.
*/
const char	*WlzStringFromRCC(WlzRCCClass cls, WlzErrorNum *dstErr)
{
  const char	*str = NULL;
  WlzErrorNum   errNum = WLZ_ERR_NONE;

  switch(cls)
  {
    case WLZ_RCC_EMPTY:
      str = "WLZ_RCC_EMPTY";
      break;
    case WLZ_RCC_DC:
      str = "WLZ_RCC_DC";
      break;
    case WLZ_RCC_EC:
      str = "WLZ_RCC_EC";
      break;
    case WLZ_RCC_EQ:
      str = "WLZ_RCC_EQ";
      break;
    case WLZ_RCC_PO:
      str = "WLZ_RCC_PO";
      break;
    case WLZ_RCC_TPP:
      str = "WLZ_RCC_TPP";
      break;
    case WLZ_RCC_NTPP:
      str = "WLZ_RCC_NTPP";
      break;
    case WLZ_RCC_TPPI:
      str = "WLZ_RCC_TPPI";
      break;
    case WLZ_RCC_NTPPI:
      str = "WLZ_RCC_NTPPI";
      break;
    case WLZ_RCC_TSUR:
      str = "WLZ_RCC_TSUR";
      break;
    case WLZ_RCC_TSURI:
      str = "WLZ_RCC_TSURI";
      break;
    case WLZ_RCC_NTSUR:
      str = "WLZ_RCC_NTSUR";
      break;
    case WLZ_RCC_NTSURI:
      str = "WLZ_RCC_NTSURI";
      break;
    case WLZ_RCC_ENC:
      str = "WLZ_RCC_ENC";
      break;
    case WLZ_RCC_ENCI:
      str = "WLZ_RCC_ENCI";
      break;
    case WLZ_RCC_OST:
      str = "WLZ_RCC_OST";
      break;
    default:
      errNum = WLZ_ERR_PARAM_DATA;
      break;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(str);
}

/*!
* \return       Region connected calculus class.
* \brief        Gets a RCC class type from a string.
* \param        cStr               Given marker string.
* \param        dstErr                  Destination error pointer, may be NULL.
*/
WlzRCCClass	WlzStringToRCCClass(const char *cStr,
				    WlzErrorNum *dstErr)
{
  int		tI0;
  WlzRCCClass	cls = WLZ_RCC_EMPTY;
  WlzErrorNum	errNum = WLZ_ERR_PARAM_TYPE;

  if(WlzStringMatchValue(&tI0, cStr,         
			 "WLZ_RCC_EMPTY", WLZ_RCC_EMPTY,
			 "WLZ_RCC_DC", WLZ_RCC_DC,
			 "WLZ_RCC_EC", WLZ_RCC_EC,
			 "WLZ_RCC_EQ", WLZ_RCC_EQ,
			 "WLZ_RCC_PO", WLZ_RCC_PO,
			 "WLZ_RCC_TPP", WLZ_RCC_TPP,
			 "WLZ_RCC_NTPP", WLZ_RCC_NTPP,
			 "WLZ_RCC_TPPI", WLZ_RCC_TPPI,
			 "WLZ_RCC_NTPPI", WLZ_RCC_NTPPI,
			 "WLZ_RCC_TSUR", WLZ_RCC_TSUR,
			 "WLZ_RCC_TSURI", WLZ_RCC_TSURI,
			 "WLZ_RCC_NTSUR", WLZ_RCC_NTSUR,
			 "WLZ_RCC_NTSURI", WLZ_RCC_NTSURI,
			 "WLZ_RCC_ENC", WLZ_RCC_ENC,
			 "WLZ_RCC_ENCI", WLZ_RCC_ENCI,
			 "WLZ_RCC_OST", WLZ_RCC_OST,
			 NULL))
  {
    cls = tI0;
    errNum = WLZ_ERR_NONE;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(cls);
}

/*!
* \return	Pointer to read only string or NULL on error.
* \ingroup      WlzStrings
* \brief	Finds a string for the given values attachment type.
* \param	mType			Given values attachment type.
* \param	dstErr			Destination error pointer, may
*                                       be null.
*/
const char	*WlzStringFromMarkerType(WlzMarkerType mType,
				         WlzErrorNum *dstErr)
{
  const char	*mStr = NULL;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  switch(mType)
  {
    case WLZ_MARKER_NONE:
      mStr = "none";
      break;
    case WLZ_MARKER_POINT:
      mStr = "point";
      break;
    case WLZ_MARKER_SPHERE:
      mStr = "sphere";
      break;
    default:
      errNum = WLZ_ERR_PARAM_DATA;
      break;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(mStr);
}

/*!
* \return       Marker type.
* \brief        Gets a marker type from a string.
* \param        markerStr               Given marker string.
* \param        dstErr                  Destination error pointer, may be NULL.
*/
WlzMarkerType 	WlzStringToMarkerType(const char *markerStr,
				      WlzErrorNum *dstErr)
{
  int           tI0;
  WlzMarkerType markerType = WLZ_MARKER_NONE;
  WlzErrorNum   errNum = WLZ_ERR_PARAM_TYPE;

  if(WlzStringMatchValue(&tI0, markerStr,
                         "none", WLZ_MARKER_NONE,
                         "point", WLZ_MARKER_POINT,
                         "sphere", WLZ_MARKER_SPHERE,
                         NULL))
  {
    markerType = tI0;
    errNum = WLZ_ERR_NONE;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(markerType);
}

/*!
* \return	Pointer to read only string or NULL on error.
* \ingroup      WlzStrings
* \brief	Finds a string for the given error.
* \param	wlzErr			Given error code.
* \param	dstMsgStr		Destination pointer for a 'meaningful'
*					message string, may be NULL.
*/
const char	*WlzStringFromErrorNum(WlzErrorNum wlzErr,
				       const char **dstMsgStr)
{
  const char	*errStr = NULL,
    	        *msgStr = NULL;

  switch(wlzErr)
  {
  case WLZ_ERR_NONE:
    errStr = "WLZ_ERR_NONE";
    msgStr = "No error";
    break;
  case WLZ_ERR_EOO:
    errStr = "WLZ_ERR_EOO";
    msgStr = "End of object";
    break;
  case WLZ_ERR_OBJECT_NULL:
    errStr = "WLZ_ERR_OBJECT_NULL";
    msgStr = "NULL object";
    break;
  case WLZ_ERR_OBJECT_TYPE:
    errStr = "WLZ_ERR_OBJECT_TYPE";
    msgStr = "Invalid object type";
    break;
  case WLZ_ERR_OBJECT_DATA:
    errStr = "WLZ_ERR_OBJECT_DATA";
    msgStr = "Object data";
    break;
  case WLZ_ERR_DOMAIN_NULL :
    errStr = "WLZ_ERR_DOMAIN_NULL";
    msgStr = "NULL domain";
    break;
  case WLZ_ERR_DOMAIN_TYPE:
    errStr = "WLZ_ERR_DOMAIN_TYPE";
    msgStr = "Invalid domain type";
    break;
  case WLZ_ERR_DOMAIN_DATA:
    errStr = "WLZ_ERR_DOMAIN_DATA";
    msgStr = "Domain data";
    break;
  case WLZ_ERR_VALUES_NULL:
    errStr = "WLZ_ERR_VALUES_NULL";
    msgStr = "NULL values";
    break;
  case WLZ_ERR_VALUES_TYPE:
    errStr = "WLZ_ERR_VALUES_TYPE";
    msgStr = "Invalid values type";
    break;
  case WLZ_ERR_VALUES_DATA:
    errStr = "WLZ_ERR_VALUES_DATA";
    msgStr = "Values data";
    break;
  case WLZ_ERR_PROPERTY_NULL:
    errStr = "WLZ_ERR_PROPERTY_NULL";
    msgStr = "NULL property";
    break;
  case WLZ_ERR_GMELM_NULL:
    errStr = "WLZ_ERR_GMELM_NULL";
    msgStr = "Geometric model element NULL";
    break;
  case WLZ_ERR_GMELM_TYPE:
    errStr = "WLZ_ERR_GMELM_TYPE";
    msgStr = "Geometric model element type";
    break;
  case WLZ_ERR_GMELM_DATA:
    errStr = "WLZ_ERR_GMELM_DATA";
    msgStr = "Geometric model element data";
    break;
  case WLZ_ERR_PARAM_NULL:
    errStr = "WLZ_ERR_PARAM_NULL";
    msgStr = "NULL parameter";
    break;
  case WLZ_ERR_PARAM_TYPE:
    errStr = "WLZ_ERR_PARAM_TYPE";
    msgStr = "Invalid parameter type";
    break;
  case WLZ_ERR_PARAM_DATA:
    errStr = "WLZ_ERR_PARAM_DATA";
    msgStr = "Parameter data";
    break;
  case WLZ_ERR_INT_DATA:
    errStr = "WLZ_ERR_INT_DATA";
    msgStr = "Int data";
    break;
  case WLZ_ERR_SHORT_DATA:
    errStr = "WLZ_ERR_SHORT_DATA";
    msgStr = "Short int data";
    break;
  case WLZ_ERR_UBYTE_DATA:
    errStr = "WLZ_ERR_UBYTE_DATA";
    msgStr = "Unsigned byte data";
    break;
  case WLZ_ERR_FLOAT_DATA:
    errStr = "WLZ_ERR_FLOAT_DATA";
    msgStr = "Float data";
    break;
  case WLZ_ERR_DOUBLE_DATA:
    errStr = "WLZ_ERR_DOUBLE_DATA";
    msgStr = "Double data";
    break;
  case WLZ_ERR_GREY_TYPE:
    errStr = "WLZ_ERR_GREY_TYPE";
    msgStr = "Invalid grey type";
    break;
  case WLZ_ERR_GREY_DATA:
    errStr = "WLZ_ERR_GREY_DATA";
    msgStr = "Grey data";
    break;
  case WLZ_ERR_PLANEDOMAIN_TYPE:
    errStr = "WLZ_ERR_PLANEDOMAIN_TYPE";
    msgStr = "Invalid planedomain type";
    break;
  case WLZ_ERR_PLANEDOMAIN_DATA:
    errStr = "WLZ_ERR_PLANEDOMAIN_DATA";
    msgStr = "";
    break;
  case WLZ_ERR_INTERVALDOMAIN_NULL:
    errStr = "WLZ_ERR_INTERVALDOMAIN_NULL";
    msgStr = "NULL intervaldomain";
    break;
  case WLZ_ERR_INTERVALDOMAIN_TYPE:
    errStr = "WLZ_ERR_INTERVALDOMAIN_TYPE";
    msgStr = "Invalid intervaldomain type";
    break;
  case WLZ_ERR_INTERVALLINE_NULL:
    errStr = "WLZ_ERR_INTERVALLINE_NULL";
    msgStr = "NULL intervalline";
    break;
  case WLZ_ERR_INTERVAL_NULL:
    errStr = "WLZ_ERR_INTERVAL_NULL";
    msgStr = "NULL interval";
    break;
  case WLZ_ERR_INTERVAL_DATA:
    errStr = "WLZ_ERR_INTERVAL_DATA";
    msgStr = "Interval data";
    break;
  case WLZ_ERR_INTERVAL_ADJACENT:
    errStr = "WLZ_ERR_INTERVAL_ADJACENT";
    msgStr = "Adjacent interval";
    break;
  case WLZ_ERR_INTERVAL_BOUND:
    errStr = "WLZ_ERR_INTERVAL_BOUND";
    msgStr = "Interval bounds";
    break;
  case WLZ_ERR_INTERVAL_NUMBER:
    errStr = "WLZ_ERR_INTERVAL_NUMBER";
    msgStr = "Interval number";
    break;
  case WLZ_ERR_TRANSFORM_NULL:
    errStr = "WLZ_ERR_TRANSFORM_NULL";
    msgStr = "NULL transform";
    break;
  case WLZ_ERR_TRANSFORM_DATA:
    errStr = "WLZ_ERR_TRANSFORM_DATA";
    msgStr = "Transform data";
    break;
  case WLZ_ERR_TRANSFORM_TYPE:
    errStr = "WLZ_ERR_TRANSFORM_TYPE";
    msgStr = "Invalid transform type";
    break;
  case WLZ_ERR_VOXELVALUES_TYPE:
    errStr = "WLZ_ERR_VOXELVALUES_TYPE";
    msgStr = "Invalid voxelvalues";
    break;
  case WLZ_ERR_COLUMN_DATA:
    errStr = "WLZ_ERR_COLUMN_DATA";
    msgStr = "Column data";
    break;
  case WLZ_ERR_LINE_DATA:
    errStr = "WLZ_ERR_LINE_DATA";
    msgStr = "Line data";
    break;
  case WLZ_ERR_PLANE_DATA:
    errStr = "WLZ_ERR_PLANE_DATA";
    msgStr = "Plane data";
    break;
  case WLZ_ERR_BINARY_OPERATOR_TYPE:
    errStr = "WLZ_ERR_BINARY_OPERATOR_TYPE";
    msgStr = "Invalid binary operator type";
    break;
  case WLZ_ERR_COMPTHRESH_TYPE:
    errStr = "WLZ_ERR_COMPTHRESH_TYPE";
    msgStr = "Invalid computed threshold type";
    break;
  case WLZ_ERR_CONNECTIVITY_TYPE:
    errStr = "WLZ_ERR_CONNECTIVITY_TYPE";
    msgStr = "Invalid connectivity type";
    break;
  case WLZ_ERR_INTERPOLATION_TYPE:
    errStr = "WLZ_ERR_INTERPOLATION_TYPE";
    msgStr = "Invalid interpolation type";
    break;
  case WLZ_ERR_POLYGON_TYPE:
    errStr = "WLZ_ERR_POLYGON_TYPE";
    msgStr = "Invalid polygon type";
    break;
  case WLZ_ERR_RASTERDIR_TYPE:
    errStr = "WLZ_ERR_RASTERDIR_TYPE";
    msgStr = "Invalid raster direction type";
    break;
  case WLZ_ERR_LINKCOUNT_DATA:
    errStr = "WLZ_ERR_LINKCOUNT_DATA";
    msgStr = "Linkcount data";
    break;
  case WLZ_ERR_MEM_ALLOC:
    errStr = "WLZ_ERR_MEM_ALLOC";
    msgStr = "Memory allocation";
    break;
  case WLZ_ERR_MEM_FREE:
    errStr = "WLZ_ERR_MEM_FREE";
    msgStr = "Memory de-allocation";
    break;
  case WLZ_ERR_READ_EOF:
    errStr = "WLZ_ERR_READ_EOF";
    msgStr = "End of file when reading an object";
    break;
  case WLZ_ERR_READ_INCOMPLETE:
    errStr = "WLZ_ERR_READ_INCOMPLETE";
    msgStr = "Incomplete object read";
    break;
  case WLZ_ERR_WRITE_EOF:
    errStr = "WLZ_ERR_WRITE_EOF";
    msgStr = "End of file when writing an object";
    break;
  case WLZ_ERR_WRITE_INCOMPLETE:
    errStr = "WLZ_ERR_WRITE_INCOMPLETE";
    msgStr = "Incomplete object write";
    break;
  case WLZ_ERR_ALG:
    errStr = "WLZ_ERR_ALG";
    msgStr = "Numerical algorithm error";
    break;
  case WLZ_ERR_ALG_CONDITION:
    errStr = "WLZ_ERR_ALG_CONDITION";
    msgStr = "Numerical algorithm matrix condition number";
    break;
  case WLZ_ERR_ALG_SINGULAR:
    errStr = "WLZ_ERR_ALG_SINGULAR";
    msgStr = "Numerical algorithm singular matrix";
    break;
  case WLZ_ERR_ALG_HOMOGENEOUS:
    errStr = "WLZ_ERR_ALG_HOMOGENEOUS";
    msgStr = "Numerical algorithm homogeneous matrix";
    break;
  case WLZ_ERR_ALG_CONVERGENCE:
    errStr = "WLZ_ERR_ALG_CONVERGENCE";
    msgStr = "Numerical algorithm failed to converge";
    break;
  case WLZ_ERR_ALG_NONGLOBAL:
    errStr = "WLZ_ERR_ALG_NONGLOBAL";
    msgStr = "Numerical algorithm converged to local not global solution";
    break;
  case WLZ_ERR_UNIMPLEMENTED:
    errStr = "WLZ_ERR_UNIMPLEMENTED";
    msgStr = "Unimplemented feature";
    break;
  case WLZ_ERR_FILE_OPEN:
    errStr = "WLZ_ERR_FILE_OPEN";
    msgStr = "File open failure";
    break;
  case WLZ_ERR_FILE_FORMAT:
    errStr = "WLZ_ERR_FILE_FORMAT";
    msgStr = "Invalid file format";
    break;
  case WLZ_ERR_IMAGE_TYPE:
    errStr = "WLZ_ERR_IMAGE_TYPE";
    msgStr = "Attempt to read an image type not-supported in Woolz";
    break;
  case WLZ_ERR_DEGENERATE:
    errStr = "WLZ_ERR_DEGENERATE";
    msgStr = "Degenerate data, eg all vertices equal.";
    break;
  default:	/* FALLTHROUGH */
  case WLZ_ERR_UNSPECIFIED:
    errStr = "WLZ_ERR_UNSPECIFIED";
    msgStr = "Unspecified error";
    break;
  }
  if(dstMsgStr)
  {
    *dstMsgStr = msgStr;
  }
  return(errStr);
}

/*!
* \return	Matched error number.
* \ingroup      WlzStrings
* \brief	Finds an error number for the given error number string.
* \param	errStr			Given error number string.
*/
WlzErrorNum	WlzStringToErrorNum(const char *errStr)
{
  int		tI0;
  WlzErrorNum	errNum = WLZ_ERR_UNSPECIFIED;

  if(WlzStringMatchValue(&tI0, errStr,
	      "WLZ_ERR_NONE", WLZ_ERR_NONE,
	      "WLZ_ERR_EOO", WLZ_ERR_EOO,
	      "WLZ_ERR_OBJECT_NULL", WLZ_ERR_OBJECT_NULL,
	      "WLZ_ERR_OBJECT_TYPE", WLZ_ERR_OBJECT_TYPE,
	      "WLZ_ERR_OBJECT_DATA", WLZ_ERR_OBJECT_DATA,
	      "WLZ_ERR_DOMAIN_NULL", WLZ_ERR_DOMAIN_NULL ,
	      "WLZ_ERR_DOMAIN_TYPE", WLZ_ERR_DOMAIN_TYPE,
	      "WLZ_ERR_DOMAIN_DATA", WLZ_ERR_DOMAIN_DATA,
	      "WLZ_ERR_VALUES_NULL", WLZ_ERR_VALUES_NULL,
	      "WLZ_ERR_VALUES_TYPE", WLZ_ERR_VALUES_TYPE,
	      "WLZ_ERR_VALUES_DATA", WLZ_ERR_VALUES_DATA,
	      "WLZ_ERR_PROPERTY_NULL", WLZ_ERR_PROPERTY_NULL,
	      "WLZ_ERR_GMELM_NULL", WLZ_ERR_GMELM_NULL,
	      "WLZ_ERR_GMELM_TYPE", WLZ_ERR_GMELM_TYPE,
	      "WLZ_ERR_GMELM_DATA", WLZ_ERR_GMELM_DATA,
	      "WLZ_ERR_PARAM_NULL", WLZ_ERR_PARAM_NULL,
	      "WLZ_ERR_PARAM_TYPE", WLZ_ERR_PARAM_TYPE,
	      "WLZ_ERR_PARAM_DATA", WLZ_ERR_PARAM_DATA,
	      "WLZ_ERR_INT_DATA", WLZ_ERR_INT_DATA,
	      "WLZ_ERR_SHORT_DATA", WLZ_ERR_SHORT_DATA,
	      "WLZ_ERR_UBYTE_DATA", WLZ_ERR_UBYTE_DATA,
	      "WLZ_ERR_FLOAT_DATA", WLZ_ERR_FLOAT_DATA,
	      "WLZ_ERR_DOUBLE_DATA", WLZ_ERR_DOUBLE_DATA,
	      "WLZ_ERR_GREY_TYPE", WLZ_ERR_GREY_TYPE,
	      "WLZ_ERR_GREY_DATA", WLZ_ERR_GREY_DATA,
	      "WLZ_ERR_PLANEDOMAIN_TYPE", WLZ_ERR_PLANEDOMAIN_TYPE,
	      "WLZ_ERR_PLANEDOMAIN_DATA", WLZ_ERR_PLANEDOMAIN_DATA,
	      "WLZ_ERR_INTERVALDOMAIN_NULL", WLZ_ERR_INTERVALDOMAIN_NULL,
	      "WLZ_ERR_INTERVALDOMAIN_TYPE", WLZ_ERR_INTERVALDOMAIN_TYPE,
	      "WLZ_ERR_INTERVALLINE_NULL", WLZ_ERR_INTERVALLINE_NULL,
	      "WLZ_ERR_INTERVAL_NULL", WLZ_ERR_INTERVAL_NULL,
	      "WLZ_ERR_INTERVAL_DATA", WLZ_ERR_INTERVAL_DATA,
	      "WLZ_ERR_INTERVAL_ADJACENT", WLZ_ERR_INTERVAL_ADJACENT,
	      "WLZ_ERR_INTERVAL_BOUND", WLZ_ERR_INTERVAL_BOUND,
	      "WLZ_ERR_INTERVAL_NUMBER", WLZ_ERR_INTERVAL_NUMBER,
	      "WLZ_ERR_TRANSFORM_NULL", WLZ_ERR_TRANSFORM_NULL,
	      "WLZ_ERR_TRANSFORM_DATA", WLZ_ERR_TRANSFORM_DATA,
	      "WLZ_ERR_TRANSFORM_TYPE", WLZ_ERR_TRANSFORM_TYPE,
	      "WLZ_ERR_VOXELVALUES_TYPE", WLZ_ERR_VOXELVALUES_TYPE,
	      "WLZ_ERR_COLUMN_DATA", WLZ_ERR_COLUMN_DATA,
	      "WLZ_ERR_LINE_DATA", WLZ_ERR_LINE_DATA,
	      "WLZ_ERR_PLANE_DATA", WLZ_ERR_PLANE_DATA,
	      "WLZ_ERR_BINARY_OPERATOR_TYPE", WLZ_ERR_BINARY_OPERATOR_TYPE,
	      "WLZ_ERR_COMPTHRESH_TYPE", WLZ_ERR_COMPTHRESH_TYPE,
	      "WLZ_ERR_CONNECTIVITY_TYPE", WLZ_ERR_CONNECTIVITY_TYPE,
	      "WLZ_ERR_INTERPOLATION_TYPE", WLZ_ERR_INTERPOLATION_TYPE,
	      "WLZ_ERR_POLYGON_TYPE", WLZ_ERR_POLYGON_TYPE,
	      "WLZ_ERR_RASTERDIR_TYPE", WLZ_ERR_RASTERDIR_TYPE,
	      "WLZ_ERR_LINKCOUNT_DATA", WLZ_ERR_LINKCOUNT_DATA,
	      "WLZ_ERR_MEM_ALLOC", WLZ_ERR_MEM_ALLOC,
	      "WLZ_ERR_MEM_FREE", WLZ_ERR_MEM_FREE,
	      "WLZ_ERR_READ_EOF", WLZ_ERR_READ_EOF,
	      "WLZ_ERR_READ_INCOMPLETE", WLZ_ERR_READ_INCOMPLETE,
	      "WLZ_ERR_WRITE_EOF", WLZ_ERR_WRITE_EOF,
	      "WLZ_ERR_WRITE_INCOMPLETE", WLZ_ERR_WRITE_INCOMPLETE,
	      "WLZ_ERR_ALG", WLZ_ERR_ALG,
	      "WLZ_ERR_ALG_SINGULAR", WLZ_ERR_ALG_SINGULAR,
	      "WLZ_ERR_ALG_HOMOGENEOUS", WLZ_ERR_ALG_HOMOGENEOUS,
	      "WLZ_ERR_ALG_CONVERGENCE", WLZ_ERR_ALG_CONVERGENCE,
	      "WLZ_ERR_ALG_NONGLOBAL", WLZ_ERR_ALG_NONGLOBAL,
	      "WLZ_ERR_UNIMPLEMENTED", WLZ_ERR_UNIMPLEMENTED,
	      "WLZ_ERR_FILE_OPEN", WLZ_ERR_FILE_OPEN,
	      "WLZ_ERR_FILE_FORMAT", WLZ_ERR_FILE_FORMAT,
	      "WLZ_ERR_IMAGE_TYPE", WLZ_ERR_IMAGE_TYPE,
	      "WLZ_ERR_DEGENERATE", WLZ_ERR_DEGENERATE,
	      "WLZ_ERR_UNSPECIFIED", WLZ_ERR_UNSPECIFIED,
	      NULL))
  {
    errNum = (WlzErrorNum )tI0;
  }
  return(errNum);
}
