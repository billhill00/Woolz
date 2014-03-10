#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _WlzGreyScan_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         libWlz/WlzGreyScan.c
* \author       Richard Baldock, Bill Hill
* \date         September 2003
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
* \brief	Object grey value scanning functions.
* \ingroup	WlzValuesUtils
*/

#include <stdlib.h>
#include <limits.h>
#include <Wlz.h>

/*! 
* \return       Woolz error code.
* \ingroup      WlzValuesUtils
* \brief        Initialise interval and grey scanning of standard object
*		in standard direction.
*
* \param    	obj			Object to be scanned.
* \param    	iwsp			Interval scanning workspace.
* \param    	gwsp			Grey value table scanning workspace.
*/
WlzErrorNum 
WlzInitGreyScan(WlzObject	*obj,	/* raster and tranpl of 0 provided */
		WlzIntervalWSpace	*iwsp,
		WlzGreyWSpace	*gwsp)
{
  return(WlzInitGreyRasterScan(obj, iwsp, gwsp, WLZ_RASTERDIR_ILIC, 0));
}

/*!
* \return	Woolz error code.
* \ingroup	WlzValuesUtils
* \brief	This must be called when a grey workspace is no longer
* 		required as it frees resources of the workspace.
* \param	gwsp			Grey value table scanning workspace.
*/
WlzErrorNum
WlzEndGreyScan(WlzGreyWSpace *gwsp)
{
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(gwsp == NULL)
  {
    errNum = WLZ_ERR_PARAM_NULL;
  }
  else if(gwsp->gtable.core == NULL)
  {
    rrNum = WLZ_ERR_VALUES_NULL;
  }
  else
  {
    if(WlzGreyTableIsTiled(gwsp->gtable.core->type))
    {
      AlcFree(gwsp->gline->values.v);
    }
  }
  return(errNum);
}

/*! 
* \return       Woolz error code.
* \ingroup      WlzValuesUtils
* \brief        As WlzInitGreyScan(), but with choice of raster direction
* 		and transplanting,
* \param    	obj			Input object to be scanned.
* \param    	iwsp			Interval scanning workspace.
* \param    	gwsp			Grey value table scanning workspace.
* \param    	raster			Direction for the raster scan.
* \param    	tranpl			Flag to allow overwriting of
* 					grey-values.
*/
WlzErrorNum 
WlzInitGreyRasterScan(WlzObject 	*obj,
		      WlzIntervalWSpace *iwsp,
		      WlzGreyWSpace 	*gwsp,
		      WlzRasterDir 	raster,
		      int 		tranpl)
{
  WlzErrorNum	errNum;

  if((errNum = WlzInitRasterScan(obj,iwsp,raster)) != WLZ_ERR_NONE)
  {
    return(errNum);
  }
  return(WlzInitGreyWSpace(obj,iwsp,gwsp,tranpl));
}

/*! 
* \return       Woolz error code.
* \ingroup      WlzValuesUtils
* \brief        Attach grey workspace to interval scanning workspace.
* \param    	obj			Object to be scanned.
* \param    	iwsp			Interval scanning workspace.
* \param    	gwsp			Value table scanning workspace.
* \param    	tranpl			Flag to enable value transplanting.
*/
WlzErrorNum 
WlzInitGreyWSpace(WlzObject 		*obj,
		  WlzIntervalWSpace 	*iwsp,
		  WlzGreyWSpace 	*gwsp,
		  int 			tranpl)
{
  WlzValues 	vdmn;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(obj == NULL)
  {
    errNum = WLZ_ERR_OBJECT_NULL;
  }
  else if((iwsp == NULL) || (gwsp == NULL))
  {
    errNum = WLZ_ERR_PARAM_NULL;
  }
  else
  {
    iwsp->gryptr = gwsp;
    /* Set up gwsp */
    (void )memset(gwsp, 0, sizeof(WlzGreyWSpace));
    vdmn = gwsp->gtable;
    gwsp->gtable = obj->values;
    gwsp->pixeltype = WlzGreyTableTypeToGreyType(vdmn.v->type, &errNum);
  }
  if(errNum == WLZ_ERR_NONE)
  {
    gwsp->gdomaintype = WlzGreyTableTypeToTableType(vdmn.v->type, &errNum);
  }
  if(errNum == WLZ_ERR_NONE)
  {
    switch (gwsp->gdomaintype)
    {

      case WLZ_GREY_TAB_RAGR:
	/* Synchronise grey table line pointer to interval domain
	   - pointer when incremented points to first line. */
	gwsp->gline = vdmn.v->vtblines + iwsp->linpos - vdmn.v->line1;
	break;

      case WLZ_GREY_TAB_RECT:
	/* Rectangular grey table does not have valueline pointers. */
	break;

      case WLZ_GREY_TAB_INTL:
	/* Synchronise grey table line pointer to interval domain
	   - pointer when incremented points to first line. */
	gwsp->gline = (WlzValueLine *) (vdmn.i->vil + iwsp->linpos -
	                                vdmn.i->line1);
	break;
      case WLZ_GREY_TAB_TILED:
	/* Create a buffer large enough to hold any single line of values. */
	errNum = WLZ_ERR_VALUES_TYPE; /* HACK TODO */
        break;
      default:
	errNum = WLZ_ERR_VALUES_TYPE;
	break;

    }
    gwsp->intptr = iwsp;
    gwsp->tranpl = tranpl;
  }
  return(errNum);
}

/*! 
* \ingroup      WlzValuesUtils
* \brief        Obtain next interval and its grey table.
*
* 		Note - cryptic documentation:
*	 	If tranpl and gvio=0 and scan already started (check line
*	 	number) then the previous grey interval must be collected,
*	 	if tranpl and gvio=1 then must output this interval (unless
*	 	scan finished).
* \return       Woolz error.
* \param    	iwsp			Interval scanning workspace - this
* 					must be initialised.
*/
WlzErrorNum 
WlzNextGreyInterval(WlzIntervalWSpace *iwsp)
{
  WlzGreyWSpace *gwsp = iwsp->gryptr;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(gwsp->tranpl && (gwsp->gvio == 0) &&
      (iwsp->linpos != (iwsp->linbot - iwsp->lineraster)))
  {
    errNum = WlzGreyInterval(iwsp);
  }
  if(errNum == WLZ_ERR_NONE)
  {
    errNum = WlzNextInterval(iwsp);
  }
  if((errNum == WLZ_ERR_NONE) && (!gwsp->tranpl || (gwsp->gvio == 1)))
  {
    errNum = WlzGreyInterval(iwsp);
  }
  return(errNum);
}

/*! 
* \ingroup      WlzValuesUtils
* \brief        Handle grey table for an interval. This must follow a
 call to "nextinterval".
*
* \return       Woolz error.
* \param    iwsp	Interval scaning workspace.
* \par      Source:
*                WlzGreyScan.c
*/
WlzErrorNum 
WlzGreyInterval(WlzIntervalWSpace *iwsp)
{
  WlzGreyP  g;
  WlzValues vdmn;
  WlzGreyWSpace *gwsp = iwsp->gryptr;
  int i;
  WlzValueIntervalLine *vil;
  WlzValueLine *val;

  vdmn = gwsp->gtable;
  switch( gwsp->gdomaintype ){

  case WLZ_GREY_TAB_RAGR:
    if (iwsp->nwlpos){
      /* new line signal, move to
	 next line of greytable */
      gwsp->gline += iwsp->nwlpos*iwsp->lineraster;
    }

    /* pointer to grey values */
    switch( gwsp->pixeltype ){

    case WLZ_GREY_INT:
      g.inp = (gwsp->gline->values.inp + iwsp->lftpos
	       - vdmn.v->kol1 - gwsp->gline->vkol1);
      break;

    case WLZ_GREY_SHORT:
      g.shp = (gwsp->gline->values.shp + iwsp->lftpos
	       - vdmn.v->kol1 - gwsp->gline->vkol1);
      break;

    case WLZ_GREY_UBYTE:
      g.ubp = (gwsp->gline->values.ubp + iwsp->lftpos
	       - vdmn.v->kol1 - gwsp->gline->vkol1);
      break;

    case WLZ_GREY_FLOAT:
      g.flp = (gwsp->gline->values.flp + iwsp->lftpos
	       - vdmn.v->kol1 - gwsp->gline->vkol1);
      break;

    case WLZ_GREY_DOUBLE:
      g.dbp = (gwsp->gline->values.dbp + iwsp->lftpos
	       - vdmn.v->kol1 - gwsp->gline->vkol1);
      break;

    case WLZ_GREY_RGBA:
      g.rgbp = (gwsp->gline->values.rgbp + iwsp->lftpos
	       - vdmn.v->kol1 - gwsp->gline->vkol1);
      break;

    default:
      return( WLZ_ERR_GREY_TYPE );
      /* break */

    }
    break;

  case WLZ_GREY_TAB_RECT:
    /* pointer to grey values */
    switch (gwsp->pixeltype) {

    case WLZ_GREY_INT:
      g.inp = (vdmn.r->values.inp + (iwsp->linpos - vdmn.r->line1)
	       * vdmn.r->width + iwsp->lftpos - vdmn.r->kol1);
      break;

    case WLZ_GREY_SHORT:
      g.shp = (vdmn.r->values.shp + (iwsp->linpos - vdmn.r->line1)
	       * vdmn.r->width + iwsp->lftpos - vdmn.r->kol1);
      break;

    case WLZ_GREY_UBYTE:
      g.ubp = (vdmn.r->values.ubp + (iwsp->linpos - vdmn.r->line1)
	       * vdmn.r->width + iwsp->lftpos - vdmn.r->kol1);
      break;

    case WLZ_GREY_FLOAT:
      g.flp = (vdmn.r->values.flp + (iwsp->linpos - vdmn.r->line1)
	       * vdmn.r->width + iwsp->lftpos - vdmn.r->kol1);
      break;

    case WLZ_GREY_DOUBLE:
      g.dbp = (vdmn.r->values.dbp + (iwsp->linpos - vdmn.r->line1)
	       * vdmn.r->width + iwsp->lftpos - vdmn.r->kol1);
      break;

    case WLZ_GREY_RGBA:
      g.rgbp = (vdmn.r->values.rgbp + (iwsp->linpos - vdmn.r->line1)
	       * vdmn.r->width + iwsp->lftpos - vdmn.r->kol1);
      break;

    default:
      return( WLZ_ERR_GREY_TYPE );
      /* break */

    }
    break;

  case WLZ_GREY_TAB_INTL:
    if (iwsp->nwlpos){				
      /* new line signal, move to
	 next line of greytable */
      gwsp->gline =
	(WlzValueLine *) (((WlzValueIntervalLine *) gwsp->gline)
			  + iwsp->nwlpos*iwsp->lineraster);
    }

    vil = (WlzValueIntervalLine *) gwsp->gline;
    val = vil->vtbint;
    for (i=0; i<vil->nintvs; i++) {
      if( (vdmn.i->kol1 + val->vkol1 <= iwsp->lftpos)
	  && (vdmn.i->kol1 + val->vlastkl >= iwsp->rgtpos) ) {
	switch (gwsp->pixeltype) {

	case WLZ_GREY_INT:
	  g.inp = val->values.inp + iwsp->lftpos - vdmn.i->kol1 - val->vkol1;
	  break;

	case WLZ_GREY_SHORT:
	  g.shp = val->values.shp + iwsp->lftpos - vdmn.i->kol1 - val->vkol1;
	  break;

	case WLZ_GREY_UBYTE:
	  g.ubp = val->values.ubp + iwsp->lftpos - vdmn.i->kol1 - val->vkol1;
	  break;

	case WLZ_GREY_FLOAT:
	  g.flp = val->values.flp + iwsp->lftpos - vdmn.i->kol1 - val->vkol1;
	  break;

	case WLZ_GREY_DOUBLE:
	  g.dbp = val->values.dbp + iwsp->lftpos - vdmn.i->kol1 - val->vkol1;
	  break;

	case WLZ_GREY_RGBA:
	  g.rgbp = val->values.rgbp + iwsp->lftpos - vdmn.i->kol1 - val->vkol1;
	  break;

	default:
	  return( WLZ_ERR_GREY_TYPE );
	  /* break */

	}
	goto G_OK;
      }
      val++;
    }

    return( WLZ_ERR_VALUES_DATA );

  G_OK:
    break;

  default:
    return( WLZ_ERR_VALUES_TYPE );

  }


  if (gwsp->tranpl != 0) {	/* transplant mode */
    /* 
     * MOD 12/1/92 - in transplant mode, we will always copy to or from
     * an integer array.  This change is made so that routines like
     * seqpar can more easily deal with differing pixel types.
     */
    switch (gwsp->gvio) {

    case 0 :		/* input to object */
      switch(gwsp->pixeltype ) {

      case WLZ_GREY_INT:
	for (i=0; i<iwsp->colrmn; i++)
	  *g.inp++ = gwsp->u_grintptr.inp[i];
	break;

      case WLZ_GREY_SHORT:
	for (i=0; i<iwsp->colrmn; i++)
	  *g.shp++ = (short )WLZ_CLAMP(gwsp->u_grintptr.inp[i],
	  			       SHRT_MIN, SHRT_MAX);
	break;

      case WLZ_GREY_UBYTE:
	for (i=0; i<iwsp->colrmn; i++)
	  *g.ubp++ = (WlzUByte )WLZ_CLAMP(gwsp->u_grintptr.inp[i], 0, 255);
	break;

      case WLZ_GREY_FLOAT:
	for (i=0; i<iwsp->colrmn; i++)
	  *g.flp++ = (float )(gwsp->u_grintptr.inp[i]);
	break;

      case WLZ_GREY_DOUBLE:
	for (i=0; i<iwsp->colrmn; i++)
	  *g.dbp++ = gwsp->u_grintptr.inp[i];
	break;

      case WLZ_GREY_RGBA:
	for (i=0; i<iwsp->colrmn; i++)
	  *g.rgbp++ = gwsp->u_grintptr.inp[i];
	break;

      default:
	/* can't get here because grey-type already checked */
	break;

      }
      break;

    case 1 :		/* output from object */
      switch(gwsp->pixeltype ) {

      case WLZ_GREY_INT:
	for (i=0; i<iwsp->colrmn; i++)
	  gwsp->u_grintptr.inp[i] = *g.inp++;
	break;

      case WLZ_GREY_SHORT:
	for (i=0; i<iwsp->colrmn; i++)
	  gwsp->u_grintptr.inp[i] = *g.shp++;
	break;

      case WLZ_GREY_UBYTE:
	for (i=0; i<iwsp->colrmn; i++)
	  gwsp->u_grintptr.inp[i] = *g.ubp++;
	break;

      case WLZ_GREY_FLOAT:
	for (i=0; i<iwsp->colrmn; i++)
	  gwsp->u_grintptr.inp[i] = (int )(*g.flp++);
	break;

      case WLZ_GREY_DOUBLE:
	for (i=0; i<iwsp->colrmn; i++)
	  gwsp->u_grintptr.inp[i] = (int )(*g.dbp++);
	break;

      case WLZ_GREY_RGBA:
	for (i=0; i<iwsp->colrmn; i++)
	  gwsp->u_grintptr.inp[i] = *g.rgbp++;
	break;

      default:
	/* can't get here because grey-type already checked */
	break;

      }
      break;

    default:
      return( WLZ_ERR_INT_DATA );

    }
  }
  else {
    gwsp->u_grintptr = g;	/* direct mode */
  }

  return( WLZ_ERR_NONE );
}
