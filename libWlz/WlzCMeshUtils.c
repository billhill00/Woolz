#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _WlzCMeshUtils_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         libWlz/WlzCMeshUtils.c
* \author       Bill Hill
* \date         June 2003
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
* \brief	Utility functions for 2D and 3D graph based conforming
* 		simplical meshes.
* \ingroup	WlzMesh
*/

#include <stdio.h>
#include <float.h>
#include <math.h>
#include <string.h>
#include <Wlz.h>

static void			WlzCMeshFilterLPL2D(
				  WlzCMesh2D *mesh,
				  WlzDVertex2 *vGIn,
				  WlzDVertex2 *vGOut,
				  double lambda,
				  int doBnd);
static void			WlzCMeshFilterLPL3D(
				  WlzCMesh3D *mesh,
				  WlzDVertex3 *vGIn,
				  WlzDVertex3 *vGOut,
				  double lambda,
				  int doBnd);
static WlzDVertex2 		WlzCMeshFilterLPLDelta2D(
				  WlzCMesh2D *mesh,
				  WlzCMeshNod2D *nod,
				  WlzDVertex2 *vBuf,
				  int doBnd);
static WlzDVertex3 		WlzCMeshFilterLPLDelta3D(
				  WlzCMesh3D *mesh,
				  WlzCMeshNod3D *nod,
				  WlzDVertex3 *vBuf,
				  int doBnd);
static WlzErrorNum		WlzCMeshValuesNormalise2D(
				  WlzObject *cObj,
				  int mapZero,
				  double vZero,
				  double vMin,
				  double vMax,
				  double oFrac);
static WlzErrorNum		WlzCMeshValuesNormalise3D(
				  WlzObject *cObj,
				  int mapZero,
				  double vZero,
				  double vMin,
				  double vMax,
				  double oFrac);

/*!
* \return	Maximum square edge length for the element.
* \ingroup      WlzMesh
* \brief        Computes the mesh maximum edge length of the given element.
* \param        elm			The given element.
*/
double		WlzCMeshElmMaxSqEdgLen2D(WlzCMeshElm2D *elm)
{
  double	dSq0,
  		dSq1;                    

  dSq0 = WlzGeomDistSq2D(elm->edu[0].nod->pos, elm->edu[1].nod->pos);
  dSq1 = WlzGeomDistSq2D(elm->edu[1].nod->pos, elm->edu[2].nod->pos);
  if(dSq1 > dSq0)
  {
    dSq0 = dSq1;
  }
  dSq1 = WlzGeomDistSq2D(elm->edu[2].nod->pos, elm->edu[0].nod->pos);
  if(dSq1 > dSq0)
  {
    dSq0 = dSq1;
  }
  return(dSq0);
}

/*!
* \return	Maximum square edge length for the element.
* \ingroup      WlzMesh
* \brief        Computes the mesh maximum edge length of the given element.
* \param        elm			The given element.
*/
double		WlzCMeshElmMaxSqEdgLen2D5(WlzCMeshElm2D5 *elm)
{
  double	dSq0,
  		dSq1;                    

  dSq0 = WlzGeomDistSq3D(elm->edu[0].nod->pos, elm->edu[1].nod->pos);
  dSq1 = WlzGeomDistSq3D(elm->edu[1].nod->pos, elm->edu[2].nod->pos);
  if(dSq1 > dSq0)
  {
    dSq0 = dSq1;
  }
  dSq1 = WlzGeomDistSq3D(elm->edu[2].nod->pos, elm->edu[0].nod->pos);
  if(dSq1 > dSq0)
  {
    dSq0 = dSq1;
  }
  return(dSq0);
}

/*!
* \return	Maximum square edge length for the element.
* \ingroup      WlzMesh
* \brief        Computes the mesh maximum edge length of the given element.
* \param        elm			The given element.
*/
double		WlzCMeshElmMaxSqEdgLen3D(WlzCMeshElm3D *elm)
{
  int		idN;
  double	dSq0,
  		dSq1;                    
  WlzCMeshNod3D	*nodes[4];

  dSq0 = 0.0;
  nodes[0] = WLZ_CMESH_ELM3D_GET_NODE_0(elm);
  nodes[1] = WLZ_CMESH_ELM3D_GET_NODE_1(elm);
  nodes[2] = WLZ_CMESH_ELM3D_GET_NODE_2(elm);
  nodes[3] = WLZ_CMESH_ELM3D_GET_NODE_3(elm);
  for(idN = 0; idN < 3; ++idN)
  {
    dSq1 = WlzGeomDistSq3D(nodes[idN]->pos, nodes[(idN + 1) % 3]->pos);
    if(dSq1 > dSq0)
    {
      dSq0 = dSq1;
    }
  }
  for(idN = 1; idN < 3; ++idN)
  {
    dSq0 = WlzGeomDistSq3D(nodes[3]->pos, nodes[idN]->pos);
    if(dSq1 > dSq0)
    {
      dSq0 = dSq1;
    }
  }
  return(dSq0);
}

/*!
* \ingroup      WlzMesh
* \brief        Computes the mesh maximum edge length which is used to
*               terminate vertex location. This should not be allowed
*               to become less than the actual maximum edge length or
*               vertex location may fail, also if it is far larger than
*               the actual maximum edge length then vertex location
*               will be inefficient when vertices are outside the mesh.
* \param        mesh                    The mesh.
*/
void     	WlzCMeshUpdateMaxSqEdgLen2D(WlzCMesh2D *mesh)
{
  int           idE;
  double        dSq;
  WlzCMeshElm2D *elm;

  if((mesh != NULL) && (mesh->type == WLZ_CMESH_2D))
  {
    mesh->maxSqEdgLen = 0.0;
    for(idE = 0; idE < mesh->res.elm.maxEnt; ++idE)
    {
      elm = (WlzCMeshElm2D *)AlcVectorItemGet(mesh->res.elm.vec, idE);
      if(elm->idx >= 0)
      {
	dSq = WlzGeomDistSq2D(elm->edu[0].nod->pos,
	                      elm->edu[1].nod->pos);
	if(dSq > mesh->maxSqEdgLen)
	{
	  mesh->maxSqEdgLen = dSq;
	}
	dSq = WlzGeomDistSq2D(elm->edu[1].nod->pos,
			      elm->edu[2].nod->pos);
	if(dSq > mesh->maxSqEdgLen)
	{
	  mesh->maxSqEdgLen = dSq;
	}
	dSq = WlzGeomDistSq2D(elm->edu[2].nod->pos,
	                      elm->edu[0].nod->pos);
	if(dSq > mesh->maxSqEdgLen)
	{
	  mesh->maxSqEdgLen = dSq;
	}
      }
    }
  }
}

/*!
* \ingroup      WlzMesh
* \brief        Computes the mesh maximum edge length which is used to
*               terminate vertex location. This should not be allowed
*               to become less than the actual maximum edge length or
*               vertex location may fail, also if it is far larger than
*               the actual maximum edge length then vertex location
*               will be inefficient when vertices are outside the mesh.
* \param        mesh                    The mesh.
*/
void     	WlzCMeshUpdateMaxSqEdgLen2D5(WlzCMesh2D5 *mesh)
{
  int           idE;
  double        dSq;
  WlzCMeshElm2D5 *elm;

  if((mesh != NULL) && (mesh->type == WLZ_CMESH_2D5))
  {
    mesh->maxSqEdgLen = 0.0;
    for(idE = 0; idE < mesh->res.elm.maxEnt; ++idE)
    {
      elm = (WlzCMeshElm2D5 *)AlcVectorItemGet(mesh->res.elm.vec, idE);
      if(elm->idx >= 0)
      {
	dSq = WlzGeomDistSq3D(elm->edu[0].nod->pos,
	                      elm->edu[1].nod->pos);
	if(dSq > mesh->maxSqEdgLen)
	{
	  mesh->maxSqEdgLen = dSq;
	}
	dSq = WlzGeomDistSq3D(elm->edu[1].nod->pos,
			      elm->edu[2].nod->pos);
	if(dSq > mesh->maxSqEdgLen)
	{
	  mesh->maxSqEdgLen = dSq;
	}
	dSq = WlzGeomDistSq3D(elm->edu[2].nod->pos,
	                      elm->edu[0].nod->pos);
	if(dSq > mesh->maxSqEdgLen)
	{
	  mesh->maxSqEdgLen = dSq;
	}
      }
    }
  }
}

/*!
* \ingroup      WlzMesh
* \brief        Computes the mesh maximum edge length which is used to
*               terminate vertex location. This should not be allowed
*               to become less than the actual maximum edge length or
*               vertex location may fail, also if it is far larger than
*               the actual maximum edge length then vertex location
*               will be inefficient when vertices are outside the mesh.
* \param        mesh                    The mesh.
*/
void            WlzCMeshUpdateMaxSqEdgLen3D(WlzCMesh3D *mesh)
{
  int           idE,
                idM,
		idN;
  double        dSq;
  WlzCMeshElm3D *elm;
  WlzCMeshNod3D	*nodes[4];

  if((mesh != NULL) && (mesh->type == WLZ_CMESH_3D))
  {
    mesh->maxSqEdgLen = 0.0;
    for(idE = 0; idE < mesh->res.elm.maxEnt; ++idE)
    {
      elm = (WlzCMeshElm3D *)AlcVectorItemGet(mesh->res.elm.vec, idE);
      if(elm->idx >= 0)
      {
	nodes[0] = WLZ_CMESH_ELM3D_GET_NODE_0(elm);
	nodes[1] = WLZ_CMESH_ELM3D_GET_NODE_1(elm);
	nodes[2] = WLZ_CMESH_ELM3D_GET_NODE_2(elm);
	nodes[3] = WLZ_CMESH_ELM3D_GET_NODE_3(elm);
	for(idM = 0; idM < 3; ++idM)
	{
	  for(idN = idM + 1; idN < 4; ++idN)
	  {
	    dSq = WlzGeomDistSq3D(nodes[idM]->pos, nodes[idN]->pos);
	    if(dSq > mesh->maxSqEdgLen)
	    {
	      mesh->maxSqEdgLen = dSq;
	    }
	  }
	}
      }
    }
  }
}

/*!
* \ingroup      WlzMesh
* \brief        Updates the bounding box of the 2D conforming mesh.
* \param        mesh                    The mesh.
*/
void     	WlzCMeshUpdateBBox2D(WlzCMesh2D *mesh)
{
  int           idN,
		firstNod;
  WlzCMeshNod2D *nod;
  WlzDBox2      bBox;

  if(mesh && (mesh->type == WLZ_CMESH_2D))
  {
    /* Update the bounding box. */
    firstNod = 1;
    for(idN = 0; idN < mesh->res.nod.maxEnt; ++idN)
    {
      nod = (WlzCMeshNod2D *)AlcVectorItemGet(mesh->res.nod.vec, idN);
      if(nod->idx >= 0)
      {
	if(firstNod)
	{
	  firstNod = 0;
	  bBox.xMin = bBox.xMax = nod->pos.vtX;
	  bBox.yMin = bBox.yMax = nod->pos.vtY;
	}
	else
	{
	  if(nod->pos.vtX < bBox.xMin)
	  {
	    bBox.xMin = nod->pos.vtX;
	  }
	  else if(nod->pos.vtX > bBox.xMax)
	  {
	    bBox.xMax = nod->pos.vtX;
	  }
	  if(nod->pos.vtY < bBox.yMin)
	  {
	    bBox.yMin = nod->pos.vtY;
	  }
	  else if(nod->pos.vtY > bBox.yMax)
	  {
	    bBox.yMax = nod->pos.vtY;
	  }
	}
      }
    }
    if(firstNod == 0)
    {
      mesh->bBox = bBox;
    }
  }
}

/*!
* \ingroup      WlzMesh
* \brief        Updates the bounding box of the 2D5 conforming mesh.
* \param        mesh                    The mesh.
*/
void            WlzCMeshUpdateBBox2D5(WlzCMesh2D5 *mesh)
{
  int           idN,
                firstNod;
  WlzCMeshNod2D5 *nod;
  WlzDBox3      bBox;

  if(mesh && (mesh->type == WLZ_CMESH_2D5))
  {
    /* Update the bounding box. */
    firstNod = 1;
    for(idN = 0; idN < mesh->res.nod.maxEnt; ++idN)
    {
      nod = (WlzCMeshNod2D5 *)AlcVectorItemGet(mesh->res.nod.vec, idN);
      if(nod->idx >= 0)
      {
	if(firstNod)
	{
	  firstNod = 0;
	  bBox.xMin = bBox.xMax = nod->pos.vtX;
	  bBox.yMin = bBox.yMax = nod->pos.vtY;
	  bBox.zMin = bBox.zMax = nod->pos.vtZ;
	}
	else
	{
	  if(nod->pos.vtX < bBox.xMin)
	  {
	    bBox.xMin = nod->pos.vtX;
	  }
	  else if(nod->pos.vtX > bBox.xMax)
	  {
	    bBox.xMax = nod->pos.vtX;
	  }
	  if(nod->pos.vtY < bBox.yMin)
	  {
	    bBox.yMin = nod->pos.vtY;
	  }
	  else if(nod->pos.vtY > bBox.yMax)
	  {
	    bBox.yMax = nod->pos.vtY;
	  }
	  if(nod->pos.vtZ < bBox.zMin)
	  {
	    bBox.zMin = nod->pos.vtZ;
	  }
	  else if(nod->pos.vtZ > bBox.zMax)
	  {
	    bBox.zMax = nod->pos.vtZ;
	  }
	}
      }
    }
    if(firstNod == 0)
    {
      mesh->bBox = bBox;
    }
  }
}

/*!
* \ingroup      WlzMesh
* \brief        Updates the bounding box of the 3D conforming mesh.
* \param        mesh                    The mesh.
*/
void            WlzCMeshUpdateBBox3D(WlzCMesh3D *mesh)
{
  int           idN,
                firstNod;
  WlzCMeshNod3D *nod;
  WlzDBox3      bBox;

  if(mesh && (mesh->type == WLZ_CMESH_3D))
  {
    /* Update the bounding box. */
    firstNod = 1;
    for(idN = 0; idN < mesh->res.nod.maxEnt; ++idN)
    {
      nod = (WlzCMeshNod3D *)AlcVectorItemGet(mesh->res.nod.vec, idN);
      if(nod->idx >= 0)
      {
	if(firstNod)
	{
	  firstNod = 0;
	  bBox.xMin = bBox.xMax = nod->pos.vtX;
	  bBox.yMin = bBox.yMax = nod->pos.vtY;
	  bBox.zMin = bBox.zMax = nod->pos.vtZ;
	}
	else
	{
	  if(nod->pos.vtX < bBox.xMin)
	  {
	    bBox.xMin = nod->pos.vtX;
	  }
	  else if(nod->pos.vtX > bBox.xMax)
	  {
	    bBox.xMax = nod->pos.vtX;
	  }
	  if(nod->pos.vtY < bBox.yMin)
	  {
	    bBox.yMin = nod->pos.vtY;
	  }
	  else if(nod->pos.vtY > bBox.yMax)
	  {
	    bBox.yMax = nod->pos.vtY;
	  }
	  if(nod->pos.vtZ < bBox.zMin)
	  {
	    bBox.zMin = nod->pos.vtZ;
	  }
	  else if(nod->pos.vtZ > bBox.zMax)
	  {
	    bBox.zMax = nod->pos.vtZ;
	  }
	}
      }
    }
    if(firstNod == 0)
    {
      mesh->bBox = bBox;
    }
  }
}

/*!
* \ingroup	WlzMesh
* \brief	Sets the node flags for all valid nodes of the mesh.
* \param	mesh			Given mesh.
* \param	flags			Flags to set.
*/
void		WlzCMeshSetNodFlags(WlzCMeshP mesh, unsigned int flags)
{
  if(mesh.v)
  {
    switch(mesh.m2->type)
    {
      case WLZ_CMESH_2D:
        WlzCMeshSetNodFlags2D(mesh.m2, flags);
	break;
      case WLZ_CMESH_3D:
        WlzCMeshSetNodFlags3D(mesh.m3, flags);
	break;
      default:
        break;
    }
  }
}

/*!
* \ingroup	WlzMesh
* \brief	Sets node flags for all valid nodes of the 2D mesh.
* \param	mesh			Given mesh.
* \param	flags			Flags to set.
*/
void		WlzCMeshSetNodFlags2D(WlzCMesh2D *mesh, unsigned int flags)
{
  int		idN;
  WlzCMeshNod2D *nod;

  if(mesh && (mesh->type == WLZ_CMESH_2D))
  {
    for(idN = 0; idN < mesh->res.nod.maxEnt; ++idN)
    {
      nod = (WlzCMeshNod2D *)AlcVectorItemGet(mesh->res.nod.vec, idN);
      if(nod->idx >= 0)
      {
	nod->flags |= flags;
      }
    }
  }
}

/*!
* \ingroup	WlzMesh
* \brief	Sets node flags for all valid nodes of the 3D mesh.
* \param	mesh			Given mesh.
* \param	flags			Flags to set.
*/
void		WlzCMeshSetNodFlags3D(WlzCMesh3D *mesh, unsigned int flags)
{
  int		idN;
  WlzCMeshNod3D *nod;

  if(mesh && (mesh->type == WLZ_CMESH_3D))
  {
    for(idN = 0; idN < mesh->res.nod.maxEnt; ++idN)
    {
      nod = (WlzCMeshNod3D *)AlcVectorItemGet(mesh->res.nod.vec, idN);
      if(nod->idx >= 0)
      {
	nod->flags |= flags;
      }
    }
  }
}

/*!
* \ingroup	WlzMesh
* \brief	Clears the node flags for all valid nodes of the mesh.
* \param	mesh			Given mesh.
* \param	flags			Flags to clear.
*/
void		WlzCMeshClearNodFlags(WlzCMeshP mesh, unsigned int flags)
{
  if(mesh.v)
  {
    switch(mesh.m2->type)
    {
      case WLZ_CMESH_2D:
        WlzCMeshClearNodFlags2D(mesh.m2, flags);
	break;
      case WLZ_CMESH_3D:
        WlzCMeshClearNodFlags3D(mesh.m3, flags);
	break;
      default:
        break;
    }
  }
}

/*!
* \ingroup	WlzMesh
* \brief	Clears node flags for all valid nodes of the 2D mesh.
* \param	mesh			Given mesh.
* \param	flags			Flags to clear.
*/
void		WlzCMeshClearNodFlags2D(WlzCMesh2D *mesh, unsigned int flags)
{
  int		idN;
  WlzCMeshNod2D *nod;

  if(mesh && (mesh->type == WLZ_CMESH_2D))
  {
    flags = ~flags;
    for(idN = 0; idN < mesh->res.nod.maxEnt; ++idN)
    {
      nod = (WlzCMeshNod2D *)AlcVectorItemGet(mesh->res.nod.vec, idN);
      if(nod->idx >= 0)
      {
	nod->flags &= flags;
      }
    }
  }
}

/*!
* \ingroup	WlzMesh
* \brief	Clears node flags for all valid nodes of the 3D mesh.
* \param	mesh			Given mesh.
* \param	flags			Flags to clear.
*/
void		WlzCMeshClearNodFlags3D(WlzCMesh3D *mesh, unsigned int flags)
{
  int		idN;
  WlzCMeshNod3D *nod;

  if(mesh && (mesh->type == WLZ_CMESH_3D))
  {
    flags = ~flags;
    for(idN = 0; idN < mesh->res.nod.maxEnt; ++idN)
    {
      nod = (WlzCMeshNod3D *)AlcVectorItemGet(mesh->res.nod.vec, idN);
      if(nod->idx >= 0)
      {
	nod->flags &= flags;
      }
    }
  }
}

/*!
* \ingroup	WlzMesh
* \brief	Clears the element flags for all valid elements of the mesh.
* \param	mesh			Given mesh.
* \param	flags			Flags to clear.
*/
void		WlzCMeshClearElmFlags(WlzCMeshP mesh, unsigned int flags)
{
  if(mesh.v)
  {
    switch(mesh.m2->type)
    {
      case WLZ_CMESH_2D:
        WlzCMeshClearElmFlags2D(mesh.m2, flags);
	break;
      case WLZ_CMESH_3D:
        WlzCMeshClearElmFlags3D(mesh.m3, flags);
	break;
      default:
        break;
    }
  }
}

/*!
* \ingroup	WlzMesh
* \brief	Clears element flags for all valid elements of the 2D mesh.
* \param	mesh			Given mesh.
* \param	flags			Flags to clear.
*/
void		WlzCMeshClearElmFlags2D(WlzCMesh2D *mesh, unsigned int flags)
{
  int		idN;
  WlzCMeshElm2D *elm;

  if(mesh && (mesh->type == WLZ_CMESH_2D))
  {
    flags = ~flags;
    for(idN = 0; idN < mesh->res.elm.maxEnt; ++idN)
    {
      elm = (WlzCMeshElm2D *)AlcVectorItemGet(mesh->res.elm.vec, idN);
      if(elm->idx >= 0)
      {
	elm->flags &= flags;
      }
    }
  }
}

/*!
* \ingroup	WlzMesh
* \brief	Clears element flags for all valid elements of the 3D mesh.
* \param	mesh			Given mesh.
* \param	flags			Flags to clear.
*/
void		WlzCMeshClearElmFlags3D(WlzCMesh3D *mesh, unsigned int flags)
{
  int		idN;
  WlzCMeshElm3D *elm;

  if(mesh && (mesh->type == WLZ_CMESH_3D))
  {
    flags = ~flags;
    for(idN = 0; idN < mesh->res.elm.maxEnt; ++idN)
    {
      elm = (WlzCMeshElm3D *)AlcVectorItemGet(mesh->res.elm.vec, idN);
      if(elm->idx >= 0)
      {
	elm->flags &= flags;
      }
    }
  }
}

/*!
* \return	Number of boundary nodes.
* \ingroup	WlzMesh
* \brief	Sets or clears the boundary node flag bit for all nodes
*		of the mesh.
* \param	mesh			Given mesh.
*/
int		WlzCMeshSetBoundNodFlags(WlzCMeshP mesh)
{
  int		nBnd = 0;
  if(mesh.v)
  {
    switch(mesh.m2->type)
    {
      case WLZ_CMESH_2D:
        nBnd = WlzCMeshSetBoundNodFlags2D(mesh.m2);
	break;
      case WLZ_CMESH_2D5:
        nBnd = WlzCMeshSetBoundNodFlags2D5(mesh.m2d5);
	break;
      case WLZ_CMESH_3D:
        nBnd = WlzCMeshSetBoundNodFlags3D(mesh.m3);
	break;
      default:
        break;
    }
  }
  return(nBnd);
}

/*!
* \return	Number of boundary nodes.
* \ingroup	WlzMesh
* \brief	Sets or clears the boundary node flag bit for all nodes
*		of the 2D mesh.
* \param	mesh			Given mesh.
*/
int		WlzCMeshSetBoundNodFlags2D(WlzCMesh2D *mesh)
{
  int		idN,
  		nBnd = 0;
  WlzCMeshNod2D *nod;

  if(mesh && (mesh->type == WLZ_CMESH_2D))
  {
    for(idN = 0; idN < mesh->res.nod.maxEnt; ++idN)
    {
      nod = (WlzCMeshNod2D *)AlcVectorItemGet(mesh->res.nod.vec, idN);
      if(nod->idx >= 0)
      {
	nod->flags &= ~(WLZ_CMESH_NOD_FLAG_BOUNDARY);
	if(WlzCMeshNodIsBoundary2D(nod))
	{
	  ++nBnd;
	  nod->flags |= WLZ_CMESH_NOD_FLAG_BOUNDARY;
	}
      }
    }
  }
  return(nBnd);
}

/*!
* \return	Number of boundary nodes.
* \ingroup	WlzMesh
* \brief	Sets or clears the boundary node flag bit for all nodes
*		of the 2D5 mesh.
* \param	mesh			Given mesh.
*/
int		WlzCMeshSetBoundNodFlags2D5(WlzCMesh2D5 *mesh)
{
  int		idN,
  		nBnd = 0;
  WlzCMeshNod2D5 *nod;

  if(mesh && (mesh->type == WLZ_CMESH_2D5))
  {
    for(idN = 0; idN < mesh->res.nod.maxEnt; ++idN)
    {
      nod = (WlzCMeshNod2D5 *)AlcVectorItemGet(mesh->res.nod.vec, idN);
      if(nod->idx >= 0)
      {
	nod->flags &= ~(WLZ_CMESH_NOD_FLAG_BOUNDARY);
	if(WlzCMeshNodIsBoundary2D5(nod))
	{
	  ++nBnd;
	  nod->flags |= WLZ_CMESH_NOD_FLAG_BOUNDARY;
	}
      }
    }
  }
  return(nBnd);
}

/*!
* \return	Number of boundary nodes.
* \ingroup	WlzMesh
* \brief	Sets or clears the boundary node flag bit for all nodes
*		of the 3D mesh.
* \param	mesh			Given mesh.
*/
int		WlzCMeshSetBoundNodFlags3D(WlzCMesh3D *mesh)
{
  int		idN,
  		nBnd = 0;
  WlzCMeshNod3D *nod;

  if(mesh && (mesh->type == WLZ_CMESH_3D))
  {
    for(idN = 0; idN < mesh->res.nod.maxEnt; ++idN)
    {
      nod = (WlzCMeshNod3D *)AlcVectorItemGet(mesh->res.nod.vec, idN);
      if(nod->idx >= 0)
      {
	nod->flags &= ~(WLZ_CMESH_NOD_FLAG_BOUNDARY);
	if(WlzCMeshNodIsBoundary3D(nod))
	{
	  ++nBnd;
	  nod->flags |= WLZ_CMESH_NOD_FLAG_BOUNDARY;
	}
      }
    }
  }
  return(nBnd);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzMesh
* \brief	Gets the indeces of the boundary nodes of the mesh.
* 		The boundary nodes will be unordered.
* \param	mesh			Given mesh.
* \param	dstNNod			Return pointer for number of boundary
* 					nodes, must be non NULL.
* \param	dstNod			Return pointer for array of boundary
* 					node indices, must be non NULL.
*/
WlzErrorNum	WlzCMeshGetBoundNodes(WlzCMeshP mesh, int *dstNNod,
				int **dstNod)
{
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(mesh.v == NULL)
  {
    errNum = WLZ_ERR_DOMAIN_NULL;
  }
  else
  {
    switch(mesh.m2->type)
    {
      case WLZ_CMESH_2D:
        errNum = WlzCMeshGetBoundNodes2D(mesh.m2, dstNNod, dstNod);
	break;
      case WLZ_CMESH_2D5:
        errNum = WlzCMeshGetBoundNodes2D5(mesh.m2d5, dstNNod, dstNod, 0);
	break;
      case WLZ_CMESH_3D:
        errNum = WlzCMeshGetBoundNodes3D(mesh.m3, dstNNod, dstNod);
	break;
      default:
	errNum = WLZ_ERR_DOMAIN_TYPE;
        break;
    }
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzMesh
* \brief	Gets the indeces of the boundary nodes of the 2D mesh.
* \param	mesh			Given mesh.
* \param	dstNNod			Return pointer for number of boundary
* 					nodes, must be non NULL.
* \param	dstNod			Return pointer for array of boundary
* 					node indices, must be non NULL.
*/
WlzErrorNum	WlzCMeshGetBoundNodes2D(WlzCMesh2D *mesh, int *dstNNod,
				        int **dstNod)
{
  int		nNod = 0;
  int		*nodes = NULL;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(mesh == NULL)
  {
    errNum = WLZ_ERR_DOMAIN_NULL;
  }
  else if((dstNNod == NULL) || (dstNod == NULL))
  {
    errNum = WLZ_ERR_PARAM_NULL;
  }
  else if(mesh->type != WLZ_CMESH_2D)
  {
    errNum = WLZ_ERR_DOMAIN_TYPE;
  }
  else
  {
    nNod = WlzCMeshCountBoundNodes2D(mesh);
    if(nNod > 0)
    {
      if((nodes = AlcMalloc(sizeof(int) * nNod)) == NULL)
      {
        errNum = WLZ_ERR_MEM_ALLOC;
      }
    }
    if(nodes != NULL)
    {
      int	idB,
      		idN;

      idB = 0;
      for(idN = 0; idN < mesh->res.nod.maxEnt; ++idN)
      {
	WlzCMeshNod2D *nod;

	nod = (WlzCMeshNod2D *)AlcVectorItemGet(mesh->res.nod.vec, idN);
	if(nod->idx >= 0)
	{
	  if(WlzCMeshNodIsBoundary2D(nod))
	  {
	    nodes[idB] = nod->idx;
	    ++idB;
	  }
	}
      }
    }
  }
  if(errNum != WLZ_ERR_NONE)
  {
    nNod = 0;
    AlcFree(nodes);
    nodes = NULL;
  }
  *dstNNod = nNod;
  *dstNod = nodes;
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzMesh
* \brief	Gets the indices of the boundary nodes of the 2D5 mesh.
* \param	mesh			Given mesh.
* \param	dstNNod			Return pointer for number of boundary
* 					nodes, must be non NULL.
* \param	dstNod			Return pointer for array of boundary
* 					node indices, must be non NULL.
* \param	ordered			If zero gets the indices of all
* 					boundary nodes, unordered. If
* 					non-zero gets the boundary nodes
* 					as a single cycle of node indices,
* 					from the first boundary node found.
*/
WlzErrorNum	WlzCMeshGetBoundNodes2D5(WlzCMesh2D5 *mesh, int *dstNNod,
				        int **dstNod, int ordered)
{
  int		nNod = 0;
  int		*nodes = NULL;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(mesh == NULL)
  {
    errNum = WLZ_ERR_DOMAIN_NULL;
  }
  else if((dstNNod == NULL) || (dstNod == NULL))
  {
    errNum = WLZ_ERR_PARAM_NULL;
  }
  else if(mesh->type != WLZ_CMESH_2D5)
  {
    errNum = WLZ_ERR_DOMAIN_TYPE;
  }
  else
  {
    nNod = WlzCMeshCountBoundNodes2D5(mesh);
    if(nNod <= 0)
    {
      errNum = WLZ_ERR_DOMAIN_DATA;
    }
    else
    {
      if((nodes = AlcMalloc(sizeof(int) * nNod)) == NULL)
      {
        errNum = WLZ_ERR_MEM_ALLOC;
      }
    }
  }
  if(errNum == WLZ_ERR_NONE)
  {
    int	idB,
	      idN;

    idB = 0;
    /* Find a boundary node. */
    if(!ordered)
    {
      for(idN = 0; idN < mesh->res.nod.maxEnt; ++idN)
      {
	WlzCMeshNod2D5 *nod;

	nod = (WlzCMeshNod2D5 *)AlcVectorItemGet(mesh->res.nod.vec, idN);
	if(nod->idx >= 0)
	{
	  if(WlzCMeshNodIsBoundary2D5(nod))
	  {
	    nodes[idB] = nod->idx;
	    ++idB;
	  }
	}
      }
    }
    else
    {
      int	bnd = 0;
      WlzCMeshEdgU2D5 *edu0,
		      *edu1 = NULL;

      /* Find a node with a boundary edge use (knowing there is one). */
      for(idN = 0; (bnd == 0) && (idN < mesh->res.nod.maxEnt); ++idN)
      {
	WlzCMeshNod2D5 *nod;

	nod = (WlzCMeshNod2D5 *)AlcVectorItemGet(mesh->res.nod.vec, idN);
	if((nod->idx >= 0) && (nod->edu != NULL))
	{
	  edu0 = edu1 = nod->edu;
	  do
	  {
	    if((edu1->opp == NULL) || (edu1->opp == edu1))
	    {
	      bnd = 1;
	      break;
	    }
            edu1 = edu1->nnxt;
	  } while(edu1 && (edu1 != edu0));
	}
      }
      /* Walk around the boundary from the found edge use recording the
       * node indices. */
      if(edu1 == NULL)
      {
        errNum = WLZ_ERR_DOMAIN_DATA;
      }
      else
      {
	edu0 = edu1;
	do
	{
	  nodes[idB++] = edu1->nod->idx;
#ifdef WLZ_DEBUG_CMESHUTILS
	  (void )fprintf(stderr, "%4d %3.3g %3.3g %3.3g\n",
			 edu1->nod->idx, edu1->nod->pos.vtX,
			 edu1->nod->pos.vtY, edu1->nod->pos.vtZ);
#endif /* WLZ_DEBUG_CMESHUTILS */
	  edu1 = edu1->next;
	  while((edu1->opp != NULL) || (edu1->opp == edu1))
	  {
	    edu1 = edu1->nnxt;
	  }
	} while((edu1 != edu0) && (idB < nNod));
	nNod = idB;
      }
    }
  }
  if(errNum != WLZ_ERR_NONE)
  {
    nNod = 0;
    AlcFree(nodes);
    nodes = NULL;
  }
  *dstNNod = nNod;
  *dstNod = nodes;
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzMesh
* \brief	Gets the indeces of the boundary nodes of the 3D mesh.
* \param	mesh			Given mesh.
* \param	dstNNod			Return pointer for number of boundary
* 					nodes, must be non NULL.
* \param	dstNod			Return pointer for array of boundary
* 					node indices, must be non NULL.
*/
WlzErrorNum	WlzCMeshGetBoundNodes3D(WlzCMesh3D *mesh, int *dstNNod,
				        int **dstNod)
{
  int		nNod = 0;
  int		*nodes = NULL;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(mesh == NULL)
  {
    errNum = WLZ_ERR_DOMAIN_NULL;
  }
  else if((dstNNod == NULL) || (dstNod == NULL))
  {
    errNum = WLZ_ERR_PARAM_NULL;
  }
  else if(mesh->type != WLZ_CMESH_3D)
  {
    errNum = WLZ_ERR_DOMAIN_TYPE;
  }
  else
  {
    nNod = WlzCMeshCountBoundNodes3D(mesh);
    if(nNod > 0)
    {
      if((nodes = AlcMalloc(sizeof(int) * nNod)) == NULL)
      {
        errNum = WLZ_ERR_MEM_ALLOC;
      }
    }
    if(nodes != NULL)
    {
      int	idB,
      		idN;

      idB = 0;
      for(idN = 0; idN < mesh->res.nod.maxEnt; ++idN)
      {
	WlzCMeshNod3D *nod;

	nod = (WlzCMeshNod3D *)AlcVectorItemGet(mesh->res.nod.vec, idN);
	if(nod->idx >= 0)
	{
	  if(WlzCMeshNodIsBoundary3D(nod))
	  {
	    nodes[idB] = nod->idx;
	    ++idB;
	  }
	}
      }
    }
  }
  if(errNum != WLZ_ERR_NONE)
  {
    nNod = 0;
    AlcFree(nodes);
    nodes = NULL;
  }
  *dstNNod = nNod;
  *dstNod = nodes;
  return(errNum);
}

/*!
* \return	Number of boundary nodes.
* \ingroup	WlzMesh
* \brief	Counts the number of boundary nodes of the mesh.
* \param	mesh			Given mesh.
*/
int		WlzCMeshCountBoundNodes(WlzCMeshP mesh)
{
  int		nBnd = 0;
  if(mesh.v)
  {
    switch(mesh.m2->type)
    {
      case WLZ_CMESH_2D:
        nBnd = WlzCMeshCountBoundNodes2D(mesh.m2);
	break;
      case WLZ_CMESH_2D5:
        nBnd = WlzCMeshCountBoundNodes2D5(mesh.m2d5);
	break;
      case WLZ_CMESH_3D:
        nBnd = WlzCMeshCountBoundNodes3D(mesh.m3);
	break;
      default:
        break;
    }
  }
  return(nBnd);
}

/*!
* \return	Number of boundary nodes.
* \ingroup	WlzMesh
* \brief	Counts the number of boundary nodes of the 2D mesh.
* \param	mesh			Given mesh.
*/
int		WlzCMeshCountBoundNodes2D(WlzCMesh2D *mesh)
{
  int		idN,
  		nBnd = 0;
  WlzCMeshNod2D *nod;

  if(mesh && (mesh->type == WLZ_CMESH_2D))
  {
    for(idN = 0; idN < mesh->res.nod.maxEnt; ++idN)
    {
      nod = (WlzCMeshNod2D *)AlcVectorItemGet(mesh->res.nod.vec, idN);
      if(nod->idx >= 0)
      {
	if(WlzCMeshNodIsBoundary2D(nod))
	{
	  ++nBnd;
	}
      }
    }
  }
  return(nBnd);
}

/*!
* \return	Number of boundary nodes.
* \ingroup	WlzMesh
* \brief	Counts the number of boundary nodes of the 2D5 mesh.
* \param	mesh			Given mesh.
*/
int		WlzCMeshCountBoundNodes2D5(WlzCMesh2D5 *mesh)
{
  int		idN,
  		nBnd = 0;
  WlzCMeshNod2D5 *nod;

  if(mesh && (mesh->type == WLZ_CMESH_2D5))
  {
    for(idN = 0; idN < mesh->res.nod.maxEnt; ++idN)
    {
      nod = (WlzCMeshNod2D5 *)AlcVectorItemGet(mesh->res.nod.vec, idN);
      if(nod->idx >= 0)
      {
	if(WlzCMeshNodIsBoundary2D5(nod))
	{
	  ++nBnd;
	}
      }
    }
  }
  return(nBnd);
}

/*!
* \return	Number of boundary nodes.
* \ingroup	WlzMesh
* \brief	Counts the number of boundary nodes of the 3D mesh.
* \param	mesh			Given mesh.
*/
int		WlzCMeshCountBoundNodes3D(WlzCMesh3D *mesh)
{
  int		idN,
  		nBnd = 0;
  WlzCMeshNod3D *nod;

  if(mesh && (mesh->type == WLZ_CMESH_3D))
  {
    for(idN = 0; idN < mesh->res.nod.maxEnt; ++idN)
    {
      nod = (WlzCMeshNod3D *)AlcVectorItemGet(mesh->res.nod.vec, idN);
      if(nod->idx >= 0)
      {
	if(WlzCMeshNodIsBoundary3D(nod))
	{
	  ++nBnd;
	}
      }
    }
  }
  return(nBnd);
}

/*!
* \return	Number of boundary elements.
* \ingroup	WlzMesh
* \brief	Sets or clears the boundary element flag bit for all elements
*		of the mesh.
* \param	mesh			Given mesh.
*/
int		WlzCMeshSetBoundElmFlags(WlzCMeshP mesh)
{
  int		nBnd = 0;
  if(mesh.v)
  {
    switch(mesh.m2->type)
    {
      case WLZ_CMESH_2D:
        nBnd = WlzCMeshSetBoundElmFlags2D(mesh.m2);
	break;
      case WLZ_CMESH_3D:
        nBnd = WlzCMeshSetBoundElmFlags3D(mesh.m3);
	break;
      default:
        break;
    }
  }
  return(nBnd);
}

/*!
* \return	Number of boundary elements.
* \ingroup	WlzMesh
* \brief	Sets or clears the boundary element flag bit for all elements
*		of the 2D mesh.
* \param	mesh			Given mesh.
*/
int		WlzCMeshSetBoundElmFlags2D(WlzCMesh2D *mesh)
{
  int		idN,
  		nBnd = 0;
  WlzCMeshElm2D *elm;

  if(mesh && (mesh->type == WLZ_CMESH_2D))
  {
    for(idN = 0; idN < mesh->res.elm.maxEnt; ++idN)
    {
      elm = (WlzCMeshElm2D *)AlcVectorItemGet(mesh->res.elm.vec, idN);
      if(elm->idx >= 0)
      {
	elm->flags &= ~(WLZ_CMESH_ELM_FLAG_BOUNDARY);
	if(WlzCMeshElmIsBoundary2D(elm))
	{
	  ++nBnd;
	  elm->flags |= WLZ_CMESH_ELM_FLAG_BOUNDARY;
	}
      }
    }
  }
  return(nBnd);
}

/*!
* \return	Number of boundary elements.
* \ingroup	WlzMesh
* \brief	Sets or clears the boundary element flag bit for all elements
*		of the 3D mesh.
* \param	mesh			Given mesh.
*/
int		WlzCMeshSetBoundElmFlags3D(WlzCMesh3D *mesh)
{
  int		idN,
  		nBnd = 0;
  WlzCMeshElm3D *elm;

  if(mesh && (mesh->type == WLZ_CMESH_3D))
  {
    for(idN = 0; idN < mesh->res.elm.maxEnt; ++idN)
    {
      elm = (WlzCMeshElm3D *)AlcVectorItemGet(mesh->res.elm.vec, idN);
      if(elm->idx >= 0)
      {
	elm->flags &= ~(WLZ_CMESH_ELM_FLAG_BOUNDARY);
	if(WlzCMeshElmIsBoundary3D(elm))
	{
	  ++nBnd;
	  elm->flags |= WLZ_CMESH_ELM_FLAG_BOUNDARY;
	}
      }
    }
  }
  return(nBnd);
}

/*!
* \return	Non-zero if the node is a boundary node.
* \ingroup	WlzMesh
* \brief	Checks whether the node is a boundary node by examining
* 		the edges which use the node. If one of these edges does
*		not have an opposite edge (other than itself) the node is
*		a boundary node.
* \param	nod			Given node of mesh.
*/
int		WlzCMeshNodIsBoundary2D(WlzCMeshNod2D *nod)
{
  int		isBnd = 0;
  WlzCMeshEdgU2D *edu0,
  		*edu1;

  if(nod && (nod->idx >= 0) && nod->edu)
  {
    edu0 = edu1 = nod->edu;
    do
    {
      if((edu1->opp == NULL) || (edu1->opp == edu1))
      {
	isBnd = 1;
	break;
      }
      edu1 = edu1->nnxt;
    } while(edu1 != edu0);
  }
  return(isBnd);
}

/*!
* \return	Non-zero if the node is a boundary node.
* \ingroup	WlzMesh
* \brief	Checks whether the node is a boundary node by examining
* 		the edges which use the node. If one of these edges does
*		not have an opposite edge (other than itself) the node is
*		a boundary node.
* \param	nod			Given node of mesh.
*/
int		WlzCMeshNodIsBoundary2D5(WlzCMeshNod2D5 *nod)
{
  int		isBnd = 0;
  WlzCMeshEdgU2D5 *edu0,
  		  *edu1;

  if(nod && (nod->idx >= 0) && nod->edu)
  {
    edu0 = edu1 = nod->edu;
    do
    {
      if((edu1->opp == NULL) || (edu1->opp == edu1))
      {
	isBnd = 1;
	break;
      }
      edu1 = edu1->nnxt;
    } while(edu1 != edu0);
  }
  return(isBnd);
}

/*!
* \return	Non-zero if the node is a boundary node.
* \ingroup	WlzMesh
* \brief	Checks whether the node is a boundary node by examining
* 		the faces which use the node. If one of these faces does
*		not have an opposite face (other than itself) the node is
*		a boundary node.
* \param	nod			Given node of mesh.
*/
int		WlzCMeshNodIsBoundary3D(WlzCMeshNod3D *nod)
{
  int		isBnd = 0;
  WlzCMeshFace	*fce;
  WlzCMeshEdgU3D *edu0,
  		*edu1;

  if(nod && (nod->idx >= 0) && nod->edu)
  {
    edu0 = edu1 = nod->edu;
    do
    {
      fce = edu1->face;
      if((fce->opp == NULL) || (fce->opp == fce))
      {
	isBnd = 1;
	break;
      }
      edu1 = edu1->nnxt;
    } while(edu1 != edu0);
  }
  return(isBnd);
}

/*!
* \return	Non-zero if the element is a boundary element.
* \ingroup	WlzMesh
* \brief	Checks whether the element is a boundary node by examining
* 		the edges for opposite edges.
* \param	elm			Given element of mesh.
*/
int		WlzCMeshElmIsBoundary2D(WlzCMeshElm2D *elm)
{
  int		isBnd = 0;

  isBnd = (elm->edu[0].opp == NULL) || (elm->edu[0].opp == &(elm->edu[0])) ||
          (elm->edu[1].opp == NULL) || (elm->edu[1].opp == &(elm->edu[1])) ||
          (elm->edu[2].opp == NULL) || (elm->edu[2].opp == &(elm->edu[2]));
  return(isBnd);
}

/*!
* \return	Non-zero if the element is a boundary element.
* \ingroup	WlzMesh
* \brief	Checks whether the element is a boundary node by examining
* 		the faces for opposite faces.
* \param	elm			Given element of mesh.
*/
int		WlzCMeshElmIsBoundary3D(WlzCMeshElm3D *elm)
{
  int		isBnd = 0;

  isBnd = (elm->face[0].opp == NULL) || (elm->face[0].opp == &(elm->face[0])) ||
          (elm->face[1].opp == NULL) || (elm->face[1].opp == &(elm->face[1])) ||
          (elm->face[2].opp == NULL) || (elm->face[2].opp == &(elm->face[2])) ||
          (elm->face[3].opp == NULL) || (elm->face[3].opp == &(elm->face[3]));
  return(isBnd);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzMesh
* \brief	Applies a Laplacian smoothing to the 2D mesh in which
*		nodes are iteratively moved to the centroid of their
*		imediate neighbours. If a node is on a boundary of the
*		mesh then it is moved on the boundary, ie to the centroid
*		of it's neighboring boundary nodes.
*		Before calling this function all nodes must have had the
*		boundary node flag bit set or cleared appropriately.
*		This function will shrink meshes when the boundary
*		parameter is set.
*
*		Each node at position \f$p_i\f$ is moved to \f$p'_i\f$:
*		\f[
                    p'_i = (1 - \alpha)p_i +
		           \frac{\alpha}{n}\sum_{j}^{n}{p_{ij}}
		\f]
*		where \f$\alpha\f$ is the weight factor.
* \param	mesh			Given mesh.
* \param	itr			Number of iterations.
* \param	alpha			Weight factor.
* \param	doBnd			Apply smoothing to boundary nodes
*					if non-zero.
* \param	update			Update the mesh bucket grid and
*					maximum edge length.
*/
WlzErrorNum	WlzCMeshLaplacianSmooth(WlzCMeshP mesh,
					  int itr, double alpha,
					  int doBnd, int update)
{
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(mesh.v == NULL)
  {
    errNum = WLZ_ERR_DOMAIN_NULL;
  }
  else
  {
    switch(mesh.m2->type)
    {
      case WLZ_CMESH_2D:
        errNum = WlzCMeshLaplacianSmooth2D(mesh.m2, itr, alpha, doBnd, update);
	break;
      case WLZ_CMESH_3D:
        errNum = WlzCMeshLaplacianSmooth3D(mesh.m3, itr, alpha, doBnd, update);
	break;
      default:
        errNum = WLZ_ERR_DOMAIN_TYPE;
	break;
    }
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzMesh
* \brief	Applies a Laplacian smoothing to the 2D mesh in which
*		nodes are iteratively moved to the centroid of their
*		imediate neighbours. See WlzCMeshLaplacianSmooth().
* \param	mesh			Given mesh.
* \param	itr			Number of iterations.
* \param	alpha			Weight factor.
* \param	doBnd			Apply smoothing to boundary nodes
*					if non-zero.
* \param	update			Update the mesh bucket grid and
*					maximum edge length.
*/
WlzErrorNum	WlzCMeshLaplacianSmooth2D(WlzCMesh2D *mesh,
					  int itr, double alpha,
					  int doBnd, int update)
{
  int		idI,
  		idN,
  		nCnt;
  WlzDVertex2	nPos;
  WlzCMeshNod2D *nod,
  		*oNod;
  WlzCMeshEdgU2D *edu0,
  		*edu1;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(mesh && (mesh->type == WLZ_CMESH_2D))
  {
    for(idI = 0; idI < itr; ++idI)
    {
      for(idN = 0; idN < mesh->res.nod.maxEnt; ++idN)
      {
	nod = (WlzCMeshNod2D *)AlcVectorItemGet(mesh->res.nod.vec, idN);
	if(nod->idx >= 0)
	{
          if(doBnd || ((nod->flags & WLZ_CMESH_NOD_FLAG_BOUNDARY) == 0))
	  {
	    nCnt = 0;
	    nPos.vtX = nPos.vtY = 0.0;
	    edu0 = edu1 = nod->edu;
	    do
	    {
	      oNod = edu1->next->nod;
	      nPos.vtX += oNod->pos.vtX;
	      nPos.vtY += oNod->pos.vtY;
	      ++nCnt;
	      edu1 = edu1->nnxt;
	    } while(edu0 != edu1);
	    if(nCnt > 0)
	    {
	      nod->pos.vtX = (1.0 - alpha) * nod->pos.vtX + 
			     alpha * nPos.vtX / nCnt;
	      nod->pos.vtY = (1.0 - alpha) * nod->pos.vtY + 
			     alpha * nPos.vtY / nCnt;
	    }
	  }
	}
      }
    }
    if(update)
    {
      WlzCMeshUpdateBBox2D(mesh);
      WlzCMeshUpdateMaxSqEdgLen2D(mesh);
      errNum = WlzCMeshReassignGridCells2D(mesh, 0);
    }
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzMesh
* \brief	Applies a Laplacian smoothing to the 3D mesh in which
*		nodes are iteratively moved to the centroid of their
*		imediate neighbours. See WlzCMeshLaplacianSmooth().
* \param	mesh			Given mesh.
* \param	itr			Number of iterations.
* \param	alpha			Weight factor.
* \param	doBnd			Apply smoothing to boundary nodes
*					if non-zero.
* \param	update			Update the mesh bucket grid and
*					maximum edge length.
*/
WlzErrorNum	WlzCMeshLaplacianSmooth3D(WlzCMesh3D *mesh,
					  int itr, double alpha,
					  int doBnd, int update)
{
  int		idI,
  		idN,
  		nCnt;
  WlzDVertex3	nPos;
  WlzCMeshNod3D *nod,
  		*oNod;
  WlzCMeshEdgU3D *edu0,
  		*edu1;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(mesh && (mesh->type == WLZ_CMESH_3D))
  {
    for(idI = 0; idI < itr; ++idI)
    {
      for(idN = 0; idN < mesh->res.nod.maxEnt; ++idN)
      {
	nod = (WlzCMeshNod3D *)AlcVectorItemGet(mesh->res.nod.vec, idN);
	if(nod->idx >= 0)
	{
          if(doBnd || ((nod->flags & WLZ_CMESH_NOD_FLAG_BOUNDARY) == 0))
	  {
	    nCnt = 0;
	    nPos.vtX = nPos.vtY = nPos.vtZ = 0.0;
	    edu0 = edu1 = nod->edu;
	    do
	    {
	      oNod = edu1->next->nod;
	      nPos.vtX += oNod->pos.vtX;
	      nPos.vtY += oNod->pos.vtY;
	      nPos.vtZ += oNod->pos.vtZ;
	      ++nCnt;
	      edu1 = edu1->nnxt;
	    } while(edu0 != edu1);
	    if(nCnt > 0)
	    {
	      nod->pos.vtX = (1.0 - alpha) * nod->pos.vtX + 
			     alpha * nPos.vtX / nCnt;
	      nod->pos.vtY = (1.0 - alpha) * nod->pos.vtY + 
			     alpha * nPos.vtY / nCnt;
	      nod->pos.vtZ = (1.0 - alpha) * nod->pos.vtZ + 
			     alpha * nPos.vtZ / nCnt;
	    }
	  }
	}
      }
    }
    if(update)
    {
      WlzCMeshUpdateBBox3D(mesh);
      WlzCMeshUpdateMaxSqEdgLen3D(mesh);
      errNum = WlzCMeshReassignGridCells3D(mesh, 0);
    }
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzMesh
* \brief	Applies a low pass filter to the mesh in which
*		nodes are moved. If a node is on a boundary of the
*		mesh then it is moved on the boundary.
*		Before calling this function all nodes must have had the
*		boundary node flag bit set or cleared appropriately.
*		This function should not significantly shrink meshes
*		because it applies a low pass filter.
* \param	mesh			Given mesh.
* \param	kPB			The band pass frequency parameter.
* \param	kSB			The band stop frequency parameter.
* \param	dPB			The pass band maximum deviation.
* \param	dSB			The stop band maximum deviation.
* \param	maxItr			Maximum number of iterations.
* \param	doBnd			Apply smoothing to boundary nodes
*					if non-zero.
* \param	update			Update the mesh bucket grid and
*					maximum edge length.
*/
WlzErrorNum	WlzCMeshLPFilter(WlzCMeshP mesh,
                                 double kPB, double kSB,
				 double dPB, double dSB,
				 int maxItr, int doBnd,
				 int update)
{
  int		nItr;
  double	lambda,
  		mu;
  WlzErrorNum   errNum = WLZ_ERR_NONE;

  if(mesh.v == NULL)
  {
    errNum = WLZ_ERR_NONE;
  }
  else
  {
    errNum = WlzGMFilterGeomLPParam(&lambda, &mu, &nItr, kPB, kSB, dPB, dSB);
  }
  if(errNum == WLZ_ERR_NONE)
  {
    if(nItr > maxItr)
    {
      nItr = maxItr;
    }
    errNum = WlzCMeshLPFilterLM(mesh, lambda, mu, nItr, doBnd, update);
  }
  if(errNum == WLZ_ERR_NONE)
  {
    if(update)
    {
      switch(mesh.m2->type)
      {
        case WLZ_CMESH_2D:
	  WlzCMeshUpdateBBox2D(mesh.m2);
	  WlzCMeshUpdateMaxSqEdgLen2D(mesh.m2);
	  errNum = WlzCMeshReassignGridCells2D(mesh.m2, 0);
	  break;
        case WLZ_CMESH_3D:
	  WlzCMeshUpdateBBox3D(mesh.m3);
	  WlzCMeshUpdateMaxSqEdgLen3D(mesh.m3);
	  errNum = WlzCMeshReassignGridCells3D(mesh.m3, 0);
	  break;
      }
    }
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzMesh
* \brief	Applies a low pass filter to the geometry of the given
*		mesh. See WlzGMFilterGeomLPLM(). This will change the
*		boundary node/element flags.
* \param	mesh			Given mesh.
* \param	lambda			Positive filter parameter.
* \param	mu			Negative filter parameter.
* \param	nItr			Number of itterations.
* \param	doBnd			Filter boundary nodes in non-zero.
* \param	update			Update the mesh bucket grid,
*					bounding box and maximum edge length.
*/
WlzErrorNum	WlzCMeshLPFilterLM(WlzCMeshP mesh,
				   double lambda, double mu,
				   int nItr, int doBnd, int update)
{
  int		idI,
  		nVtx = 0;
  WlzVertexP	vtxBuf[2];
  WlzVertexType	vtxType;
  WlzErrorNum   errNum = WLZ_ERR_NONE;

  if(mesh.v == NULL)
  {
    errNum = WLZ_ERR_NONE;
  }
  else if((lambda < DBL_EPSILON) || (lambda > (1.0 - DBL_EPSILON)) ||
          (-mu < DBL_EPSILON) || (-mu > (1.0 - DBL_EPSILON)) ||
	  (nItr < 1))
  {
    errNum = WLZ_ERR_PARAM_DATA;
  }
  if(errNum == WLZ_ERR_NONE)
  {
    vtxBuf[0] = WlzDVerticesFromCMesh(mesh, &nVtx, &vtxType, 0, &errNum);
  }
  if(errNum == WLZ_ERR_NONE)
  {
    (void )WlzCMeshSetBoundNodFlags(mesh);
    switch(mesh.m2->type)
    {
      case WLZ_CMESH_2D:
        if((vtxBuf[1].d2 = (WlzDVertex2 *)
	                   AlcMalloc(sizeof(WlzDVertex2) * nVtx)) == NULL)
        {
	  errNum = WLZ_ERR_MEM_ALLOC;
	}
	else
	{
	  for(idI = 0; idI < nItr; ++idI)
	  {
	    WlzCMeshFilterLPL2D(mesh.m2, vtxBuf[0].d2, vtxBuf[1].d2,
	                        lambda, doBnd);
	    WlzCMeshFilterLPL2D(mesh.m2, vtxBuf[1].d2, vtxBuf[0].d2,
	                        lambda, doBnd);
	  }
	}
	break;
      case WLZ_CMESH_3D:
        if((vtxBuf[1].d3 = (WlzDVertex3 *)
	                   AlcMalloc(sizeof(WlzDVertex3) * nVtx)) == NULL)
        {
	  errNum = WLZ_ERR_MEM_ALLOC;
	}
	else
	{
	  for(idI = 0; idI < nItr; ++idI)
	  {
	    WlzCMeshFilterLPL3D(mesh.m3, vtxBuf[0].d3, vtxBuf[1].d3,
	                        lambda, doBnd);
	    WlzCMeshFilterLPL3D(mesh.m3, vtxBuf[1].d3, vtxBuf[0].d3,
	                        lambda, doBnd);
	  }
	}
	break;
    }
  }
  if(errNum == WLZ_ERR_NONE)
  {
    errNum = WlzCMeshSetVertices(mesh, vtxBuf[0], update);
  }
  return(errNum); 
}

/*!
* \return	Woolz error code.
* \ingroup	WlzMesh
* \brief        Sets the position of all valid nodes in the mesh.
*		The mesh will still need to have it's bounding box,
*		maximum edge length and grid box set after this function
*		has repositioned the nodes.
*		Vertex type must be correct for mesh type, ie
*		WlzDVertex2 for WlzCMesh2D and WlzDVertex3 for
*		WlzCMesh3D.
* \param        mesh			The given mesh.
* \param        vtxBuf                  The buffer with vnode positions
*                                       that are to be set in the model.
* \param	update			Update the mesh bucket grid,
*					bounding box and maximum edge length.
*/
WlzErrorNum	WlzCMeshSetVertices(WlzCMeshP mesh, WlzVertexP vtxBuf,
				    int update)
{
  WlzErrorNum	errNum = WLZ_ERR_NONE;
  
  if(mesh.v == NULL)
  {
    errNum = WLZ_ERR_DOMAIN_NULL;
  }
  else if(vtxBuf.v == NULL)
  {
    errNum = WLZ_ERR_PARAM_NULL;
  }
  else
  {
    switch(mesh.m2->type)
    {
      case WLZ_CMESH_2D:
        WlzCMeshSetVertices2D(mesh.m2, vtxBuf.d2, update);
	break;
      case WLZ_CMESH_3D:
        WlzCMeshSetVertices3D(mesh.m3, vtxBuf.d3, update);
	break;
      default:
	errNum = WLZ_ERR_DOMAIN_TYPE;
        break;
    }
  }
  return(errNum);
}

/*!
* \ingroup	WlzMesh
* \brief        Sets the position of all valid nodes in the mesh.
*		The mesh will still need to have it's bounding box,
*		maximum edge length and grid box set after this function
*		has repositioned the nodes.
*		The parameters are assumed vaild.
* \param        mesh			The given mesh.
* \param        vtxBuf                  The buffer with vnode positions
*                                       that are to be set in the model.
* \param	update			Update the mesh bucket grid,
*					bounding box and maximum edge length.
*/
void		WlzCMeshSetVertices2D(WlzCMesh2D *mesh, WlzDVertex2 *vtxBuf,
				      int update)
{
  int		idx,
  		cnt;
  AlcVector	*vec;
  WlzCMeshNod2D *nod;

  cnt = mesh->res.elm.maxEnt;
  vec = mesh->res.nod.vec;
  for(idx = 0; idx < cnt; ++idx)
  {
    nod = (WlzCMeshNod2D *)AlcVectorItemGet(vec, idx);
    if(nod && (nod->idx >= 0))
    {
      nod->pos = *(vtxBuf + idx);
    }
  }
  if(update)
  {
    WlzCMeshUpdateBBox2D(mesh);
    WlzCMeshUpdateMaxSqEdgLen2D(mesh);
    (void )WlzCMeshReassignGridCells2D(mesh, 0);
  }
}

/*!
* \ingroup	WlzMesh
* \brief        Sets the position of all valid nodes in the mesh.
*		The mesh will still need to have it's bounding box,
*		maximum edge length and grid box set after this function
*		has repositioned the nodes.
*		The parameters are assumed vaild.
* \param        mesh			The given mesh.
* \param        vtxBuf                  The buffer with vnode positions
*                                       that are to be set in the model.
* \param	update			Update the mesh bucket grid,
*					bounding box and maximum edge length.
*/
void		WlzCMeshSetVertices3D(WlzCMesh3D *mesh, WlzDVertex3 *vtxBuf,
				      int update)
{
  int		idx,
  		cnt;
  AlcVector	*vec;
  WlzCMeshNod3D *nod;

  cnt = mesh->res.nod.maxEnt;
  vec = mesh->res.nod.vec;
  for(idx = 0; idx < cnt; ++idx)
  {
    nod = (WlzCMeshNod3D *)AlcVectorItemGet(vec, idx);
    if(nod && (nod->idx >= 0))
    {
      nod->pos = *(vtxBuf + idx);
    }
  }
  if(update)
  {
    WlzCMeshUpdateBBox3D(mesh);
    WlzCMeshUpdateMaxSqEdgLen3D(mesh);
    (void )WlzCMeshReassignGridCells3D(mesh, 0);
  }
}

/*!
* \return	Woolz error code.
* \ingroup	WlzMesh
* \brief	Checks that the 2D or 3D mesh has valid connectivities.
*		This function is slow and should only be used when
*		debugging mesh connectivities - it is not intended for
*		routine use. With an invalid mesh this checking function
*		may provoke NULL pointer access or segmentation faults.
* \param	mesh			Given mesh.
* \param	dstElm			Destination mesh element pointer
*					for last mesh element, may be NULL.
* \param	allErr			If non zero the checking conmtinues
*					after an error has been found, else if
*					zero the checking stops after the first
*					error has been found.
* \param	fP			Stream for diagnostic output
*					statements - may be NULL in which case
*					there will be no diagnostic output.
*/
WlzErrorNum 	WlzCMeshVerify(WlzCMeshP mesh, void **dstElm,
				 int allErr, FILE *fP)
{
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(mesh.v == NULL)
  {
    errNum = WLZ_ERR_DOMAIN_NULL;
  }
  else
  {
    switch(mesh.m2->type)
    {
      case WLZ_CMESH_2D:
        errNum = WlzCMeshVerify2D(mesh.m2, (WlzCMeshElm2D **)dstElm,
				  allErr, fP);
        break;
      case WLZ_CMESH_3D:
        errNum = WlzCMeshVerify3D(mesh.m3, (WlzCMeshElm3D **)dstElm,
				  allErr, fP);
        break;
      default:
        errNum = WLZ_ERR_DOMAIN_TYPE;
	break;
    }
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzMesh
* \brief	Checks that the 2D mesh has valid connectivities.
*		This function is slow and should only be used when
*		debugging mesh connectivities - it is not intended for
*		routine use. With an invalid mesh this checking function
*		may provoke NULL pointer access or segmentation faults.
* \param	mesh			Given mesh.
* \param	dstElm			Destination mesh element pointer
*					for last mesh element, may be NULL.
* \param	allErr			If non zero the checking conmtinues
*					after an error has been found, else if
*					zero the checking stops after the first
*					error has been found.
* \param	fP			Stream for diagnostic output
*					statements - may be NULL in which case
*					there will be no diagnostic output.
*/
WlzErrorNum 	WlzCMeshVerify2D(WlzCMesh2D *mesh, WlzCMeshElm2D **dstElm,
				 int allErr, FILE *fP)
{
  int		cnt0,
		cnt1,
  		idE0,
		idE1,
  		idN;
  WlzCMeshEdgU2D *edu0,
  		*edu1;
  WlzCMeshElm2D	*elm = NULL;
  WlzCMeshNod2D	*nod;
  WlzCMeshCell2D *cell;
  WlzIVertex2	idx;
  WlzErrorNum	errNum0,
  		errNum1 = WLZ_ERR_NONE;
  const int	nnxtLimit = 1000;
  char		msgBuf[1000];

  if(mesh == NULL)
  {
    errNum1 = WLZ_ERR_DOMAIN_NULL;
  }
  else if(mesh->type != WLZ_CMESH_2D)
  {
    errNum1 = WLZ_ERR_DOMAIN_TYPE;
  }
  if(errNum1 == WLZ_ERR_NONE)
  {
    /* Verify the grid of cells has reasonable linked lists. */
    idx.vtY = 0;
    while(((allErr == 0)  || (errNum1 == WLZ_ERR_NONE)) &&
          (idx.vtY < mesh->cGrid.nCells.vtY))
    {
      idx.vtX = 0;
      while(((allErr == 0)  || (errNum1 == WLZ_ERR_NONE)) &&
            (idx.vtX < mesh->cGrid.nCells.vtX))
      {
        /* Verify node linked list. */
	cell = *(mesh->cGrid.cells + idx.vtY) + idx.vtX;
	cnt0 = 0;
	nod = cell->nod;
	while((nod != NULL) && (cnt0 < nnxtLimit))
	{
	  nod = nod->next;
	  ++cnt0;
	}
	if(cnt0 >= nnxtLimit)
	{
	  errNum0 = WLZ_ERR_DOMAIN_DATA;
	  (void )sprintf(msgBuf,
			 "cell[%d][%d].nod->next cycle > %d",
			 idx.vtY, idx.vtX, nnxtLimit);
	}
	/* Verify element linked lists - not written yet. */
        ++idx.vtX; 
      }
      ++idx.vtY;
    }
  }
  if(errNum1 == WLZ_ERR_NONE)
  {
    idE0 = 0;
    while((idE0 < mesh->res.elm.maxEnt) &&
          ((allErr == 0)  || (errNum1 == WLZ_ERR_NONE)))
    {
      /* Verify elements of mesh. */
      elm = (WlzCMeshElm2D *)AlcVectorItemGet(mesh->res.elm.vec, idE0);
      if(elm->idx >= 0)
      {
	idN = 0;
	while((idN < 3) &&
	      ((allErr == 0)  || (errNum1 == WLZ_ERR_NONE)))
	{
	  errNum0 = WLZ_ERR_NONE;
	  /* Verify each edge of element. */
	  if(((allErr == 0)  || (errNum0 == WLZ_ERR_NONE)) &&
	     (elm->edu[idN].next != &(elm->edu[(idN + 1) % 3])))
	  {
	    errNum0 = WLZ_ERR_DOMAIN_DATA;
	    (void )sprintf(msgBuf,
	                   "elm[%d]->edu[%d].next != &(elm[%d]->edu[%d])",
			   idE0, idN, idE0, (idN + 1) % 3);
	  }
	  /* Verify that each edge is directed from a node. */
	  if(((allErr == 0)  || (errNum0 == WLZ_ERR_NONE)) &&
	     (elm->edu[idN].nod == NULL))
	  {
	    (void )sprintf(msgBuf,
	    		   "elm[%d]->edu[%d].nod == NULL",
			   idE0, idN);
	    errNum0 = WLZ_ERR_DOMAIN_DATA;
	  }
	  /* Verify that each edge's node has not been deleted. */
	  if(((allErr == 0)  || (errNum0 == WLZ_ERR_NONE)) &&
	     (elm->edu[idN].nod->idx < 0))
	  {
	    errNum0 = WLZ_ERR_DOMAIN_DATA;
	    (void )sprintf(msgBuf,
	    		   "elm[%d]->edu[%d].nod->idx < 0",
			   idE0, idN);
	  }
	  /* Verify that the each edge's node is the node. */
	  if(((allErr == 0)  || (errNum0 == WLZ_ERR_NONE)) &&
	     (elm->edu[idN].nod->edu->nod != elm->edu[idN].nod))
	  {
	    errNum0 = WLZ_ERR_DOMAIN_DATA;
	    (void )sprintf(msgBuf,
		"elm[%d]->edu[%d].nod->edu->nod != elm[%d]->edu[%d].nod",
		idE0, idN, idE0, idN);
	  }
	  /* Verify that an opposite opposite edge is the edge. */
	  if(((allErr == 0)  || (errNum0 == WLZ_ERR_NONE)) &&
	     ((elm->edu[idN].opp != NULL) &&
	     (elm->edu[idN].opp->opp != &(elm->edu[idN]))))
	  {
	    errNum0 = WLZ_ERR_DOMAIN_DATA;
	    (void )sprintf(msgBuf,
	    		   "elm[%d]->edu[%d].opp->opp != &(elm[%d]->edu[%d])",
			   idE0, idN, idE0, idN);
	  }
	  if(errNum1 == WLZ_ERR_NONE)
	  {
	    errNum1 = errNum0;
	  }
	  ++idN;
	}
	/* Check element areas are positive). */
	if((allErr == 0)  || (errNum1 == WLZ_ERR_NONE))
	{
	  if(WlzCMeshElmSnArea22D(elm) < WLZ_MESH_TOLERANCE_SQ)
	  {
	    errNum0 = WLZ_ERR_DOMAIN_DATA;
	    (void )sprintf(msgBuf,
	    		   "WlzCMeshElmSnArea22D(elm[%d]) < %g",
			   idE0, WLZ_MESH_TOLERANCE_SQ);
	  }
	}
      }
      ++idE0;
    }
  }
  if(errNum1 == WLZ_ERR_NONE)
  {
    idN = 0;
    while((idN < mesh->res.nod.maxEnt) &&
          ((allErr == 0)  || (errNum1 == WLZ_ERR_NONE)))
    {
      nod = (WlzCMeshNod2D *)AlcVectorItemGet(mesh->res.nod.vec, idN);
      if(nod->idx >= 0)
      {
	/* Check that the number of edge uses of each node is reasonable. */
	cnt0 = 0;
        edu0 = edu1 = nod->edu;
	do
	{
	  edu1 = edu1->nnxt;
	}
	while((cnt0++ < nnxtLimit) && (edu1 != edu0));
	if(cnt0 >= nnxtLimit)
	{
	  errNum0 = WLZ_ERR_DOMAIN_DATA;
	  (void )sprintf(msgBuf,
			 "elm[%d]->edu[%d].nod->edu->nnxt cycle > %d",
			 idE0, idN, nnxtLimit);
	}
	/* Check that the number of edge uses of each node is correct. */
	if((allErr == 0)  || (errNum0 == WLZ_ERR_NONE))
	{
	  cnt1 = 0;
	  idE0 = 0;
	  while((idE0 < mesh->res.elm.maxEnt) &&
		((allErr == 0)  || (errNum0 == WLZ_ERR_NONE)))
	  {
	    elm = (WlzCMeshElm2D *)AlcVectorItemGet(mesh->res.elm.vec, idE0);
	    if(elm->idx >= 0)
	    {
	      for(idE1 = 0; idE1 < 3; ++idE1)
	      {
	        if(elm->edu[idE1].nod == nod)
		{
		  ++cnt1;
		}
	      }
	    }
	    ++idE0;
	  }
	  if(cnt0 != cnt1)
	  {
	    errNum0 = WLZ_ERR_DOMAIN_DATA;
	    (void )sprintf(msgBuf,
	                   "node %d edu->nnxt loop is inconsistant",
			   idN);
	  }
	}
	if(errNum1 == WLZ_ERR_NONE)
	{
	  errNum1 = errNum0;
	}
      }
      ++idN;
    }
  }
  if(dstElm)
  {
    *dstElm = elm;
  }
  return(errNum1);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzMesh
* \brief	Checks that the 2D5 mesh has valid connectivities.
*		This function is slow and should only be used when
*		debugging mesh connectivities - it is not intended for
*		routine use. With an invalid mesh this checking function
*		may provoke NULL pointer access or segmentation faults.
* \todo		This function has yet to be implemented.
* \param	mesh			Given mesh.
* \param	dstElm			Destination mesh element pointer
*					for last mesh element, may be NULL.
* \param	allErr			If non zero the checking conmtinues
*					after an error has been found, else if
*					zero the checking stops after the first
*					error has been found.
* \param	fP			Stream for diagnostic output
*					statements - may be NULL in which case
*					there will be no diagnostic output.
*/
WlzErrorNum 	WlzCMeshVerify2D5(WlzCMesh2D5 *mesh, WlzCMeshElm2D5 **dstElm,
				  int allErr, FILE *fP)
{
  WlzCMeshElm2D5 *elm = NULL;
  WlzErrorNum	errNum1 = WLZ_ERR_UNIMPLEMENTED;

  if(mesh == NULL)
  {
    errNum1 = WLZ_ERR_DOMAIN_NULL;
  }
  else if(mesh->type != WLZ_CMESH_2D5)
  {
    errNum1 = WLZ_ERR_DOMAIN_TYPE;
  }
  if(dstElm)
  {
    *dstElm = elm;
  }
  return(errNum1);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzMesh
* \brief	Checks that the 3D mesh has valid connectivities.
*		This function is slow and should only be used when
*		debugging mesh connectivities - it is not intended for
*		routine use. With an invalid mesh this checking function
*		may provoke NULL pointer access or segmentation faults.
* \param	mesh			Given mesh.
* \param	dstElm			Destination mesh element pointer
*					for last mesh element, may be NULL.
* \param	allErr			If non zero the checking conmtinues
*					after an error has been found, else if
*					zero the checking stops after the first
*					error has been found.
* \param	fP			Stream for diagnostic output
*					statements - may be NULL in which case
*					there will be no diagnostic output.
*/
WlzErrorNum 	WlzCMeshVerify3D(WlzCMesh3D *mesh, WlzCMeshElm3D **dstElm,
				 int allErr, FILE *fP)
{
  int		idE,
  		idF,
		idN,
		cnt;
  WlzCMeshNod3D	*nod;
  WlzCMeshEdgU3D *edu,
  		*edu0,
		*edu1;
  WlzCMeshElm3D	*elm = NULL;
  WlzCMeshFace	*fce;
  WlzErrorNum	errNum0,
  		errNum1 = WLZ_ERR_NONE;
  const int	nnxtLimit = 1000;
  char		msgBuf[1000];


  if(mesh == NULL)
  {
    errNum1 = WLZ_ERR_DOMAIN_NULL;
  }
  else if(mesh->type != WLZ_CMESH_3D)
  {
    errNum1 = WLZ_ERR_DOMAIN_TYPE;
  }
  else
  {
    idE = 0;
    while((idE < mesh->res.elm.maxEnt) &&
          ((allErr == 0)  || (errNum1 == WLZ_ERR_NONE)))
    {
      /* Verify elements of mesh. */
      elm = (WlzCMeshElm3D *)AlcVectorItemGet(mesh->res.elm.vec, idE);
      if(elm->idx >= 0)
      {
	/* Verify faces of element. */
	idF = 0;
	while((idF < 4) &&
	      ((allErr == 0)  || (errNum1 == WLZ_ERR_NONE)))
	{
	  errNum0 = WLZ_ERR_NONE;
	  fce = elm->face + idF;
	  if(fce->elm != elm)
	  {
	    errNum0 = WLZ_ERR_DOMAIN_DATA;
	    (void )sprintf(msgBuf,
	                   "elm[%d]->face[%d].elm != &(elm[%d])",
			   idE, idF, idE);
	  }
	  if((allErr == 0)  || (errNum0 == WLZ_ERR_NONE))
	  {
	    if(fce->opp != NULL)
	    {
	      if(fce->opp->opp->elm->idx != elm->idx)
	      {
	        errNum0 = WLZ_ERR_DOMAIN_DATA;
		(void )sprintf(msgBuf,
		"elm[%d]->face[%d]->opp->opp->elm->idx != elm[%d]->idx\n",
		idE, idF, idE);
	      }
	    }
	  }
	  if((allErr == 0)  || (errNum0 == WLZ_ERR_NONE))
	  {
	    for(idN = 0; idN < 3; ++idN)
	    {
	      edu = fce->edu + idN;
	      if(edu->face != fce)
	      {
		errNum0 = WLZ_ERR_DOMAIN_DATA;
	        (void )sprintf(msgBuf,
		  "elm[%d]->face[%d].edu[%d]->face != elm[%d]->face[%d]\n",
		  idE, idF, idN, idE, idF);
	      }
	      if(edu->next->next->next != edu)
	      {
	        errNum0 = WLZ_ERR_DOMAIN_DATA;
		(void )sprintf(msgBuf,
                               "elm[%d]->face[%d].edu[%d].next->next->next |= "
			       "elm[%d]->face[%d].edu[%d]\n",
			       idE, idF, idN, idE, idF, idN);
	      }
	      nod = edu->nod;
	      if(nod == NULL)
	      {
	        errNum0 = WLZ_ERR_DOMAIN_DATA;
		(void )sprintf(msgBuf,
                               "elm[%d]->face[%d].edu[%d].nod == NULL\n",
			       idE, idF, idN);
	      }
	      else
	      {
	        if(nod->idx < 0)
		{
		  errNum0 = WLZ_ERR_DOMAIN_DATA;
		  (void )sprintf(msgBuf,
		                 "elm[%d]->face[%d].edu[%d].nod->idx < 0\n",
				 idE, idF, idN);

		}
		else if(nod->edu == NULL)
		{
		  errNum0 = WLZ_ERR_DOMAIN_DATA;
		  (void )sprintf(msgBuf,
			       "elm[%d]->face[%d].edu[%d].nod->edu == NULL\n",
			       idE, idF, idN);
		}
		if((allErr == 0)  || (errNum0 == WLZ_ERR_NONE))
		{
		  cnt = 0;
		  edu1 = edu0 = nod->edu;
		  do
		  {
		    edu1 = edu1->nnxt;
		  }
		  while((cnt++ < nnxtLimit) && (edu1 != edu0));
		  if(cnt >= nnxtLimit)
		  {
		    errNum0 = WLZ_ERR_DOMAIN_DATA;
		    (void )sprintf(msgBuf,
			 "elm[%d]->face[%d].edu[%d].nod->edu->nnxt cycle > %d",
				   idE, idF, idN, nnxtLimit);
		  }
		}
	      }
	    }
	  }
	  ++idF;
	}
	if(errNum1 == WLZ_ERR_NONE)
	{
	  errNum1 = errNum0;
	}
	/* Check element volumes are positive). */
	if((allErr == 0)  || (errNum1 == WLZ_ERR_NONE))
	{
	  if(WlzCMeshElmSnVolume63D(elm) < WLZ_MESH_TOLERANCE_SQ)
	  {
	    errNum0 = WLZ_ERR_DOMAIN_DATA;
	    (void )sprintf(msgBuf,
			   "WlzCMeshElmSnVolume6(elm[%d]) < %g",
			   idE, WLZ_MESH_TOLERANCE_SQ);
	  }
	}
	if(errNum1 == WLZ_ERR_NONE)
	{
	  errNum1 = errNum0;
	}
	/* Check the node ordering is consistent. */
	if((allErr == 0)  || (errNum1 == WLZ_ERR_NONE))
	{
	  if((elm->face[1].edu[0].nod != elm->face[0].edu[0].nod) ||
	     (elm->face[1].edu[2].nod != elm->face[0].edu[1].nod) ||
	     (elm->face[2].edu[0].nod != elm->face[0].edu[0].nod) ||
	     (elm->face[2].edu[1].nod != elm->face[0].edu[2].nod) ||
	     (elm->face[2].edu[2].nod != elm->face[1].edu[1].nod) ||
	     (elm->face[3].edu[0].nod != elm->face[0].edu[2].nod) ||
	     (elm->face[3].edu[1].nod != elm->face[0].edu[1].nod) ||
	     (elm->face[3].edu[2].nod != elm->face[1].edu[1].nod))
          {
	    errNum0 = WLZ_ERR_DOMAIN_DATA;
	    (void )sprintf(msgBuf,
	                   "Node ordering for elm[%d] is inconsistent",
			   idE);
	  }
	}
	if(errNum1 == WLZ_ERR_NONE)
	{
	  errNum1 = errNum0;
	}
      }
      ++idE;
    }
  }
  if(dstElm)
  {
    *dstElm = elm;
  }
  return(errNum1);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzMesh
* \brief	Computes some simple geometric features of all valid
*		elements in a mesh.
* \param	mesh			Given mesh.
* \param	dstNElm			Destination pointer for the number
*					of mesh elements. May be NULL.
* \param	dstIdx			Destination pointer for element
* 					indices. May be NULL.
* \param	dstVol			Destination pointer for the area
*					or volume of the elements. May be
*					NULL.
* \param	dstMinLen		Destination pointer for the minimum
*					edge length of the elements.
* \param	dstMaxLen		Destination pointer for the minimum
*					edge length of the elements. May be
*					NULL.
*/
WlzErrorNum 	WlzCMeshCmpElmFeat(WlzCMeshP mesh, int *dstNElm,
				   int **dstIdx, double **dstVol,
				   double **dstMinLen, double **dstMaxLen)
{
  int		nElm = 0;
  int		*idx = NULL;
  double	*vol = NULL,
  		*minLen = NULL,
		*maxLen = NULL;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(mesh.v == NULL)
  {
    errNum = WLZ_ERR_DOMAIN_NULL;
  }
  else
  {
    switch(mesh.m2->type)
    {
      case WLZ_CMESH_2D:
        errNum = WlzCMeshCmpElmFeat2D(mesh.m2, &nElm, &idx, &vol,
	                              &minLen, &maxLen);
        break;
      case WLZ_CMESH_3D:
        errNum = WlzCMeshCmpElmFeat3D(mesh.m3, &nElm, &idx, &vol,
	                              &minLen, &maxLen);
        break;
      default:
        errNum = WLZ_ERR_DOMAIN_TYPE;
	break;
    }
  }
  if(errNum == WLZ_ERR_NONE)
  {
    if(dstNElm)
    {
      *dstNElm = nElm;
    }
    if(dstIdx)
    {
      *dstIdx = idx;
    }
    if(dstVol)
    {
      *dstVol = vol;
    }
    if(dstMinLen)
    {
      *dstMinLen = minLen;
    }
    if(dstMaxLen)
    {
      *dstMaxLen = maxLen;
    }
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzMesh
* \brief	Computes some simple geometric features of all valid
*		elements in a 2D mesh.
* \param	mesh			Given 2D mesh.
* \param	dstNElm			Destination pointer for the number
*					of mesh elements. May be NULL.
* \param	dstIdx			Destination pointer for element
* 					indices. May be NULL.
* \param	dstVol			Destination pointer for the area
*					or volume of the elements. May be
*					NULL.
* \param	dstMinLen		Destination pointer for the minimum
*					edge length of the elements.
* \param	dstMaxLen		Destination pointer for the minimum
*					edge length of the elements. May be
*					NULL.
*/
WlzErrorNum 	WlzCMeshCmpElmFeat2D(WlzCMesh2D *mesh, int *dstNElm,
				   int **dstIdx, double **dstVol,
				   double **dstMinLen, double **dstMaxLen)
{
  int		idE,
		idV;
  int		*idx = NULL;
  double	*vol = NULL,
  		*minLen = NULL,
		*maxLen = NULL;
  double	len[3];
  WlzDVertex2	tV0;
  WlzCMeshElm2D	*elm;
  WlzCMeshNod2D	*nod[3];
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(mesh == NULL)
  {
    errNum = WLZ_ERR_DOMAIN_NULL;
  }
  else if(mesh->type != WLZ_CMESH_2D)
  {
    errNum = WLZ_ERR_DOMAIN_TYPE;
  }
  else if(mesh->res.elm.maxEnt > 0)
  {
    if(dstIdx &&
       ((idx = AlcCalloc(mesh->res.elm.maxEnt, sizeof(int))) == NULL))
    {
      errNum = WLZ_ERR_MEM_ALLOC;
    }
    if(dstVol &&
       ((vol = AlcCalloc(mesh->res.elm.maxEnt, sizeof(double))) == NULL))
    {
      errNum = WLZ_ERR_MEM_ALLOC;
    }
    if(dstMinLen &&
       ((minLen = AlcCalloc(mesh->res.elm.maxEnt, sizeof(double))) == NULL))
    {
      errNum = WLZ_ERR_MEM_ALLOC;
    }
    if(dstMaxLen &&
       ((maxLen = AlcCalloc(mesh->res.elm.maxEnt, sizeof(double))) == NULL))
    {
      errNum = WLZ_ERR_MEM_ALLOC;
    }
  }
  if(errNum == WLZ_ERR_NONE)
  {
    idV = 0;
    for(idE = 0; idE < mesh->res.elm.maxEnt; ++idE)
    {
      elm = (WlzCMeshElm2D *)AlcVectorItemGet(mesh->res.elm.vec, idE);
      if(elm->idx >= 0)
      {
	nod[0] = elm->edu[0].nod;
	nod[1] = elm->edu[1].nod;
	nod[2] = elm->edu[2].nod;
	if(idx)
	{
	  *(idx + idV) = elm->idx;
	}
	if(vol)
	{
	  *(vol + idV) = WlzGeomTriangleSnArea2(nod[0]->pos,
                                nod[1]->pos, nod[2]->pos) / 2.0;
	}
	if(minLen || maxLen)
	{
	  WLZ_VTX_2_SUB(tV0, nod[0]->pos, nod[1]->pos);
	  len[0] = WLZ_VTX_2_SQRLEN(tV0);
	  WLZ_VTX_2_SUB(tV0, nod[0]->pos, nod[2]->pos);
	  len[1] = WLZ_VTX_2_SQRLEN(tV0);
	  WLZ_VTX_2_SUB(tV0, nod[1]->pos, nod[2]->pos);
	  len[2] = WLZ_VTX_2_SQRLEN(tV0);
	}
	if(minLen)
	{
	  AlgRankSelectD(len, 3, 0);
	  *(minLen + idV) = (len[0] < WLZ_MESH_TOLERANCE)? 0.0: sqrt(len[0]);
	}
	if(maxLen)
	{
	  AlgRankSelectD(len, 3, 2);
	  *(maxLen + idV) = (len[2] < WLZ_MESH_TOLERANCE)? 0.0: sqrt(len[2]);
	}
        ++idV;
      }
    }
  }
  if(errNum == WLZ_ERR_NONE)
  {
    if(dstNElm)
    {
      *dstNElm = idV;
    }
    if(dstIdx)
    {
      *dstIdx = idx;
    }
    if(dstVol)
    {
      *dstVol = vol;
    }
    if(dstMinLen)
    {
      *dstMinLen = minLen;
    }
    if(dstMaxLen)
    {
      *dstMaxLen = maxLen;
    }
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzMesh
* \brief	Computes some simple geometric features of all valid
*		elements in a 3D mesh.
* \param	mesh			Given 3D mesh.
* \param	dstNElm			Destination pointer for the number
*					of mesh elements. May be NULL.
* \param	dstIdx			Destination pointer for element
* 					indices. May be NULL.
* \param	dstVol			Destination pointer for the area
*					or volume of the elements. May be
*					NULL.
* \param	dstMinLen		Destination pointer for the minimum
*					edge length of the elements.
* \param	dstMaxLen		Destination pointer for the minimum
*					edge length of the elements. May be
*					NULL.
*/
WlzErrorNum 	WlzCMeshCmpElmFeat3D(WlzCMesh3D *mesh, int *dstNElm,
				   int **dstIdx, double **dstVol,
				   double **dstMinLen, double **dstMaxLen)
{
  int		idE,
		idV;
  int		*idx = NULL;
  double	*vol = NULL,
  		*minLen = NULL,
		*maxLen = NULL;
  double	len[6];
  WlzDVertex3	tV0;
  WlzCMeshElm3D	*elm;
  WlzCMeshNod3D	*nod[4];
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(mesh == NULL)
  {
    errNum = WLZ_ERR_DOMAIN_NULL;
  }
  else if(mesh->type != WLZ_CMESH_3D)
  {
    errNum = WLZ_ERR_DOMAIN_TYPE;
  }
  else if(mesh->res.elm.maxEnt > 0)
  {
    if(dstIdx &&
       ((idx = AlcCalloc(mesh->res.elm.maxEnt, sizeof(int))) == NULL))
    {
      errNum = WLZ_ERR_MEM_ALLOC;
    }
    if(dstVol &&
       ((vol = AlcCalloc(mesh->res.elm.maxEnt, sizeof(double))) == NULL))
    {
      errNum = WLZ_ERR_MEM_ALLOC;
    }
    if(dstMinLen &&
       ((minLen = AlcCalloc(mesh->res.elm.maxEnt, sizeof(double))) == NULL))
    {
      errNum = WLZ_ERR_MEM_ALLOC;
    }
    if(dstMaxLen &&
       ((maxLen = AlcCalloc(mesh->res.elm.maxEnt, sizeof(double))) == NULL))
    {
      errNum = WLZ_ERR_MEM_ALLOC;
    }
  }
  if(errNum == WLZ_ERR_NONE)
  {
    idV = 0;
    for(idE = 0; idE < mesh->res.elm.maxEnt; ++idE)
    {
      elm = (WlzCMeshElm3D *)AlcVectorItemGet(mesh->res.elm.vec, idE);
      if(elm->idx >= 0)
      {
	nod[0] = WLZ_CMESH_ELM3D_GET_NODE_0(elm);
	nod[1] = WLZ_CMESH_ELM3D_GET_NODE_1(elm);
	nod[2] = WLZ_CMESH_ELM3D_GET_NODE_2(elm);
	nod[3] = WLZ_CMESH_ELM3D_GET_NODE_3(elm);
	if(idx)
	{
	  *(idx + idV) = elm->idx;
	}
	if(vol)
	{
	  *(vol + idV) = WlzGeomTetraSnVolume6(nod[0]->pos,
                                nod[1]->pos, nod[2]->pos, nod[3]->pos) / 6.0;
	}
	if(minLen || maxLen)
	{
	  WLZ_VTX_3_SUB(tV0, nod[0]->pos, nod[1]->pos);
	  len[0] = WLZ_VTX_3_SQRLEN(tV0);
	  WLZ_VTX_3_SUB(tV0, nod[0]->pos, nod[2]->pos);
	  len[1] = WLZ_VTX_3_SQRLEN(tV0);
	  WLZ_VTX_3_SUB(tV0, nod[0]->pos, nod[3]->pos);
	  len[2] = WLZ_VTX_3_SQRLEN(tV0);
	  WLZ_VTX_3_SUB(tV0, nod[1]->pos, nod[2]->pos);
	  len[3] = WLZ_VTX_3_SQRLEN(tV0);
	  WLZ_VTX_3_SUB(tV0, nod[1]->pos, nod[3]->pos);
	  len[4] = WLZ_VTX_3_SQRLEN(tV0);
	  WLZ_VTX_3_SUB(tV0, nod[2]->pos, nod[3]->pos);
	  len[5] = WLZ_VTX_3_SQRLEN(tV0);
	}
	if(minLen)
	{
	  AlgRankSelectD(len, 6, 0);
	  *(minLen + idV) = (len[0] < WLZ_MESH_TOLERANCE)? 0.0: sqrt(len[0]);
	}
	if(maxLen)
	{
	  AlgRankSelectD(len, 6, 5);
	  *(maxLen + idV) = (len[5] < WLZ_MESH_TOLERANCE)? 0.0: sqrt(len[5]);
	}
        ++idV;
      }
    }
  }
  if(errNum == WLZ_ERR_NONE)
  {
    if(dstNElm)
    {
      *dstNElm = idV;
    }
    if(dstIdx)
    {
      *dstIdx = idx;
    }
    if(dstVol)
    {
      *dstVol = vol;
    }
    if(dstMinLen)
    {
      *dstMinLen = minLen;
    }
    if(dstMaxLen)
    {
      *dstMaxLen = maxLen;
    }
  }
  return(errNum);
}

/*!
* \return       Twice the signed area of the 2D mesh element.
* \ingroup      WlzMesh
* \brief        Computes twice the signed area of the 2D mesh element.
* \param        elm                     Given mesh element.
*/
double          WlzCMeshElmSnArea22D(WlzCMeshElm2D *elm)
{
  double        area;

  area = WlzGeomTriangleSnArea2(elm->edu[0].nod->pos,
                                elm->edu[1].nod->pos,
                                elm->edu[2].nod->pos);
  return(area);
}

/*!
* \return       Twice the square of the area of the 2D5 mesh element.
* \ingroup      WlzMesh
* \brief        Computes twice the square of te area of the 2D5 mesh
* 		element.
* \param        elm                     Given mesh element.
*/
double          WlzCMeshElmSqArea22D5(WlzCMeshElm2D5 *elm)
{
  double        area;

  area = WlzGeomTriangleArea2Sq3(elm->edu[0].nod->pos,
                                 elm->edu[1].nod->pos,
                                 elm->edu[2].nod->pos);
  return(area);
}

/*!
* \return       Siz times the signed volume of the 3D mesh element.
* \ingroup      WlzMesh
* \brief        Computes six times the signed volume of the 3D mesh
*		element.
* \param        elm                     Given mesh element.
*/
double          WlzCMeshElmSnVolume63D(WlzCMeshElm3D *elm)
{
  double        vol;
  WlzCMeshNod3D	*nod[4];

  nod[0] = WLZ_CMESH_ELM3D_GET_NODE_0(elm);
  nod[1] = WLZ_CMESH_ELM3D_GET_NODE_1(elm);
  nod[2] = WLZ_CMESH_ELM3D_GET_NODE_2(elm);
  nod[3] = WLZ_CMESH_ELM3D_GET_NODE_3(elm);
  vol = WlzGeomTetraSnVolume6(nod[0]->pos,
                              nod[1]->pos,
                              nod[2]->pos,
                              nod[3]->pos);
  return(vol);
}

/*!
* \ingroup	WlzMesh
* \brief	Gets the three nodes of a 2D element.
* \param	elm			Given mesh element.
* \param	dstNod0			First destination pointer for node.
* \param	dstNod1			Second destination pointer for node.
* \param	dstNod2			Third destination pointer for node.
*/
void		WlzCMeshElmGetNodes2D(WlzCMeshElm2D *elm,
				      WlzCMeshNod2D **dstNod0,
				      WlzCMeshNod2D **dstNod1,
				      WlzCMeshNod2D **dstNod2)
{
  *dstNod0 = WLZ_CMESH_ELM2D_GET_NODE_0(elm);
  *dstNod1 = WLZ_CMESH_ELM2D_GET_NODE_1(elm);
  *dstNod2 = WLZ_CMESH_ELM2D_GET_NODE_2(elm);
}

/*!
* \ingroup	WlzMesh
* \brief	Gets the three nodes of a 2D5 element.
* \param	elm			Given mesh element.
* \param	dstNod0			First destination pointer for node.
* \param	dstNod1			Second destination pointer for node.
* \param	dstNod2			Third destination pointer for node.
*/
void		WlzCMeshElmGetNodes2D5(WlzCMeshElm2D5 *elm,
				       WlzCMeshNod2D5 **dstNod0,
				       WlzCMeshNod2D5 **dstNod1,
				       WlzCMeshNod2D5 **dstNod2)
{
  *dstNod0 = WLZ_CMESH_ELM2D5_GET_NODE_0(elm);
  *dstNod1 = WLZ_CMESH_ELM2D5_GET_NODE_1(elm);
  *dstNod2 = WLZ_CMESH_ELM2D5_GET_NODE_2(elm);
}

/*!
* \ingroup	WlzMesh
* \brief	Gets the four nodes of a 3D element.
* \param	elm			Given mesh element.
* \param	dstNod0			First destination pointer for node.
* \param	dstNod1			Second destination pointer for node.
* \param	dstNod2			Third destination pointer for node.
* \param	dstNod3			Forth destination pointer for node.
*/
void		WlzCMeshElmGetNodes3D(WlzCMeshElm3D *elm,
				      WlzCMeshNod3D **dstNod0,
				      WlzCMeshNod3D **dstNod1,
				      WlzCMeshNod3D **dstNod2,
				      WlzCMeshNod3D **dstNod3)
{
  *dstNod0 = WLZ_CMESH_ELM3D_GET_NODE_0(elm);
  *dstNod1 = WLZ_CMESH_ELM3D_GET_NODE_1(elm);
  *dstNod2 = WLZ_CMESH_ELM3D_GET_NODE_2(elm);
  *dstNod3 = WLZ_CMESH_ELM3D_GET_NODE_3(elm);
}

/*!
* \ingroup	WlzMesh
* \brief	Gets the axis aligned bounding box of a 2D element.
* \param	elm			Given mesh element.
*/
WlzDBox2	WlzCMeshElmBBox2D(WlzCMeshElm2D *elm)
{
  WlzDVertex2	p0,
  		p1,
		p2;
  WlzDBox2	bBox;

  p0 = elm->edu[0].nod->pos;
  p1 = elm->edu[1].nod->pos;
  p2 = elm->edu[2].nod->pos;
  bBox.xMin = ALG_MIN3(p0.vtX, p1.vtX, p2.vtX);
  bBox.yMin = ALG_MIN3(p0.vtY, p1.vtY, p2.vtY);
  bBox.xMax = ALG_MAX3(p0.vtX, p1.vtX, p2.vtX);
  bBox.yMax = ALG_MAX3(p0.vtY, p1.vtY, p2.vtY);
  return(bBox);
}

/*!
* \ingroup	WlzMesh
* \brief	Gets the axis aligned bounding box of a 3D element.
* \param	elm			Given mesh element.
*/
WlzDBox3	WlzCMeshElmBBox2D5(WlzCMeshElm2D5 *elm)
{
  int		idx;
  WlzCMeshNod2D5 *nod;
  WlzDBox3	bBox;

  nod = elm->edu[0].nod;
  bBox.xMin = bBox.xMax = nod->pos.vtX;
  bBox.yMin = bBox.yMax = nod->pos.vtY;
  bBox.zMin = bBox.zMax = nod->pos.vtZ;
  for(idx = 1; idx <= 2; ++idx)
  {
    nod = elm->edu[idx].nod;
    if(nod->pos.vtX < bBox.xMin)
    {
      bBox.xMin = nod->pos.vtX;
    }
    else if(nod->pos.vtX > bBox.xMax)
    {
      bBox.xMax = nod->pos.vtX;
    }
    if(nod->pos.vtY < bBox.yMin)
    {
      bBox.yMin = nod->pos.vtY;
    }
    else if(nod->pos.vtY > bBox.yMax)
    {
      bBox.yMax = nod->pos.vtY;
    }
    if(nod->pos.vtZ < bBox.zMin)
    {
      bBox.zMin = nod->pos.vtZ;
    }
    else if(nod->pos.vtZ > bBox.zMax)
    {
      bBox.zMax = nod->pos.vtZ;
    }
  }
  return(bBox);
}

/*!
* \ingroup	WlzMesh
* \brief	Gets the axis aligned bounding box of a 3D element.
* \param	elm			Given mesh element.
*/
WlzDBox3	WlzCMeshElmBBox3D(WlzCMeshElm3D *elm)
{
  int		idx;
  WlzCMeshNod3D	*nod;
  WlzDBox3	bBox;

  nod = elm->face[1].edu[1].nod;
  bBox.xMin = bBox.xMax = nod->pos.vtX;
  bBox.yMin = bBox.yMax = nod->pos.vtY;
  bBox.zMin = bBox.zMax = nod->pos.vtZ;
  for(idx = 0; idx <= 2; ++idx)
  {
    nod = elm->face[0].edu[idx].nod;
    if(nod->pos.vtX < bBox.xMin)
    {
      bBox.xMin = nod->pos.vtX;
    }
    else if(nod->pos.vtX > bBox.xMax)
    {
      bBox.xMax = nod->pos.vtX;
    }
    if(nod->pos.vtY < bBox.yMin)
    {
      bBox.yMin = nod->pos.vtY;
    }
    else if(nod->pos.vtY > bBox.yMax)
    {
      bBox.yMax = nod->pos.vtY;
    }
    if(nod->pos.vtZ < bBox.zMin)
    {
      bBox.zMin = nod->pos.vtZ;
    }
    else if(nod->pos.vtZ > bBox.zMax)
    {
      bBox.zMax = nod->pos.vtZ;
    }
  }
  return(bBox);
}

/*!
* \return	Number of elements in array.
* \ingroup	WlzMesh
* \brief	Finds the elements which use the given node and returns
* 		their indices in an array. The indices are unique and in
* 		acending order. The array will be resized as needed.
* \param	gNod			Given node.
* \param	gMaxElm			Maximum space currently allocated
* 					in the array.
* \param	gElmIdxAry		Pointer to array of ints, which may
* 					be NULL or a valid array for maxElm
* 					ints which can be reallocated as
* 					needed by AlcRealloc().
* \param	dstErr			Destination error pointer, may be NULL.
*/
int				WlzCMeshElmUseNod3D(
				  WlzCMeshNod3D *gNod,
				  int *gMaxElm,
				  int **gElmIdxAry,
				  WlzErrorNum *dstErr)
{
  int		nEdu = 0,
  		mElm = 0,
  		nElm = 0;
  int		*elmIdxAry = NULL;
  WlzCMeshEdgU3D *edu0,
                 *edu1;
  WlzErrorNum	errNum = WLZ_ERR_NONE;
  const int	alcStep = 256;

  if(gMaxElm && *gMaxElm && *gElmIdxAry)
  {
    mElm = *gMaxElm;
    elmIdxAry = *gElmIdxAry;
  }
  /* Collect element indices. */
  edu0 = edu1 = gNod->edu;
  do
  {
    if(nEdu >= mElm)
    {
      mElm += alcStep;
      if((elmIdxAry = (int *)
                      AlcRealloc(elmIdxAry, mElm * sizeof(int))) == NULL)
      {
        errNum = WLZ_ERR_MEM_ALLOC;
	break;
      }
    }
    elmIdxAry[nEdu++] = edu1->face->elm->idx;
    edu1 = edu1->nnxt;
  } while(edu0 != edu1);
  if(errNum == WLZ_ERR_NONE)
  {
    int		idN;

    /* Sort element indices. */
    (void )AlgHeapSort(elmIdxAry, nEdu, sizeof(int), AlgHeapSortCmpIFn);
    /* Squeeze out duplicates. */
    for(idN = 1; idN < nEdu; ++idN)
    {
      if(elmIdxAry[idN] != elmIdxAry[nElm])
      {
        elmIdxAry[++nElm] = elmIdxAry[idN];
      }
    }
    if(gMaxElm)
    {
      *gMaxElm = mElm;
    }
    *gElmIdxAry = elmIdxAry;
  }
  else
  {
    (void )AlcFree(elmIdxAry);
    *gElmIdxAry = NULL;
    nElm = 0;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(nElm);
}
		

/*!
* \ingroup      WlzMesh
* \brief        Filters the geometry of the verticies in a 2D mesh using
*               the given input and output buffers for the mesh node
*		positions.
* \note         See WlzGMFilterGeomLPLM().
* \param        model                   The given model.
* \param        vGIn                    Input vertex geometries.
* \param        vGOut                   Output vertex geometries.
* \param        lambda                  The filter parameter.
* \param        nonMan                  If non-zero allows non manifold
*/
static void	WlzCMeshFilterLPL2D(WlzCMesh2D *mesh,
				    WlzDVertex2 *vGIn, WlzDVertex2 *vGOut,
				    double lambda, int doBnd)
{
  int           idx,
                cnt;
  WlzDVertex2   tV0,
                tV1;
  WlzCMeshNod2D  *cN;
  AlcVector     *vec;

  cnt = mesh->res.nod.maxEnt;
  vec = mesh->res.nod.vec;
  for(idx = 0; idx < cnt; ++idx)
  {
    cN = (WlzCMeshNod2D *)AlcVectorItemGet(vec, idx);
    if(cN->idx >= 0)
    {
      tV0 = *(vGIn + idx);
      if(doBnd || ((cN->flags & WLZ_CMESH_NOD_FLAG_BOUNDARY) == 0))
      {
	tV1 = WlzCMeshFilterLPLDelta2D(mesh, cN, vGIn, doBnd);
	tV0.vtX += lambda * tV1.vtX;
	tV0.vtY += lambda * tV1.vtY;
      }
      *(vGOut + idx) = tV0;
    }
  }
}

/*!
* \ingroup      WlzMesh
* \brief        Filters the geometry of the verticies in a 3D mesh using
*               the given input and output buffers for the mesh node
*		positions.
* \note         See WlzGMFilterGeomLPLM().
* \param        model                   The given model.
* \param        vGIn                    Input vertex geometries.
* \param        vGOut                   Output vertex geometries.
* \param        lambda                  The filter parameter.
* \param        nonMan                  If non-zero allows non manifold
*/
static void	WlzCMeshFilterLPL3D(WlzCMesh3D *mesh,
				    WlzDVertex3 *vGIn, WlzDVertex3 *vGOut,
				    double lambda, int doBnd)
{
  int           idx,
                cnt;
  WlzDVertex3   tV0,
                tV1;
  WlzCMeshNod3D  *cN;
  AlcVector     *vec;

  cnt = mesh->res.nod.maxEnt;
  vec = mesh->res.nod.vec;
  for(idx = 0; idx < cnt; ++idx)
  {
    cN = (WlzCMeshNod3D *)AlcVectorItemGet(vec, idx);
    if(cN->idx >= 0)
    {
      tV0 = *(vGIn + idx);
      if(doBnd || ((cN->flags & WLZ_CMESH_NOD_FLAG_BOUNDARY) == 0))
      {
	tV1 = WlzCMeshFilterLPLDelta3D(mesh, cN, vGIn, doBnd);
	tV0.vtX += lambda * tV1.vtX;
	tV0.vtY += lambda * tV1.vtY;
	tV0.vtZ += lambda * tV1.vtZ;
      }
      *(vGOut + idx) = tV0;
    }
  }
}

/*!
* \return	Vertex displacement.
* \ingroup	WlzMesh
* \brief	Computes the displacement of the given node, for use
*		by WlzCMeshFilterLPL2D(). This is just the displacement
*		from the nodes position to the mean of the directly
*		connected neighbour node positions. All positions are
*		taken from the vertex buffer using the nodes index value.
* \param	mesh			The mesh.
* \param	nod			The current node.
* \param	vBuf			Buffer of node positions.
* \param	doBnd			Filter boundary nodes if non-zero.
*/
static WlzDVertex2 WlzCMeshFilterLPLDelta2D(WlzCMesh2D *mesh,
					WlzCMeshNod2D *nod, WlzDVertex2 *vBuf,
					int doBnd)
{
  int           nN = 0;
  double        tD0;
  WlzCMeshNod2D	*oNod;
  WlzCMeshEdgU2D *edu0,
  		*edu1;
  WlzDVertex2   nP,
                sP;

  sP.vtX = sP.vtY = 0.0;
  if((nod->idx >= 0) &&
     (doBnd || ((nod->flags & WLZ_CMESH_NOD_FLAG_BOUNDARY) != 0)))
  {
    edu0 = edu1 = nod->edu;
    do
    {
      oNod = edu1->next->nod;
      nP = *(vBuf + oNod->idx);
      WLZ_VTX_2_ADD(sP, sP, nP);
      ++nN;
      edu1 = edu1->nnxt;
    } while(edu0 != edu1);
    if(nN > 0)
    {
      tD0 = 1.0 / nN;
      nP = *(vBuf + nod->idx);
      sP.vtX = (tD0 * sP.vtX) - nP.vtX;
      sP.vtY = (tD0 * sP.vtY) - nP.vtY;
    }
  }
  return(sP);
}

/*!
* \return	Vertex displacement.
* \ingroup	WlzMesh
* \brief	Computes the displacement of the given node, for use
*		by WlzCMeshFilterLPL3D(). This is just the displacement
*		from the nodes position to the mean of the directly
*		connected neighbour node positions. All positions are
*		taken from the vertex buffer using the nodes index value.
* \param	mesh			The mesh.
* \param	nod			The current node.
* \param	vBuf			Buffer of node positions.
* \param	doBnd			Filter boundary nodes if non-zero.
*/
static WlzDVertex3 WlzCMeshFilterLPLDelta3D(WlzCMesh3D *mesh,
					WlzCMeshNod3D *nod, WlzDVertex3 *vBuf,
					int doBnd)
{
  int           nN = 0;
  double        tD0;
  WlzCMeshNod3D	*oNod;
  WlzCMeshEdgU3D *edu0,
  		*edu1;
  WlzDVertex3   nP,
                sP;

  sP.vtX = sP.vtY = sP.vtZ = 0.0;
  if((nod->idx >= 0) &&
     (doBnd || ((nod->flags & WLZ_CMESH_NOD_FLAG_BOUNDARY) != 0)))
  {
    edu0 = edu1 = nod->edu;
    do
    {
      oNod = edu1->next->nod;
      nP = *(vBuf + oNod->idx);
      WLZ_VTX_3_ADD(sP, sP, nP);
      ++nN;
      edu1 = edu1->nnxt;
    } while(edu0 != edu1);
    if(nN > 0)
    {
      tD0 = 1.0 / nN;
      nP = *(vBuf + nod->idx);
      sP.vtX = (tD0 * sP.vtX) - nP.vtX;
      sP.vtY = (tD0 * sP.vtY) - nP.vtY;
      sP.vtZ = (tD0 * sP.vtZ) - nP.vtZ;
    }
  }
  return(sP);
}

/*!
* \return	Copy of the given mesh.
* \ingroup	WlzMesh
* \brief	Creates a copy of the given constrained mesh.
* 		If the squeeze flag is set then the copy will have all
* 		invalid (deleted) entities squeezed out so that the
* 		mew mesh will only have valid mesh entities that are
* 		contiguous and start with index 0.
* 		As well as copying the given mesh this function will
* 		also copy data associated with the nodes of the mesh.
* 		The associated data are held in AlcVector data structures
* 		and are indexed using the node index.
* 		If (datSz == 0) || (newDat == NULL) || (gvnDat == NULL)
* 		then the associated data will be ignored.
* \param	gvnMesh			Given mesh.
* \param	squeeze			Squeeze out invalid (deleted entities
* 					if non zero).
* \param	datSz			Size of associated datum.
* \param	newDat			Destination pointer for the copied
* 					associated data, may be NULL.
* \param	gvnDat			Given associated data, may be NULL.
* \param	dstErr			Destination error pointer, may be NULL.
*/
WlzCMeshP	WlzCMeshCopy(WlzCMeshP gvnMesh, int squeeze, size_t datSz,
				AlcVector **newDat, AlcVector *gvnDat,
				WlzErrorNum *dstErr)
{
  WlzCMeshP	newMesh;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  newMesh.v = NULL;
  if(gvnMesh.v == NULL)
  {
    errNum = WLZ_ERR_DOMAIN_NULL;
  }
  else
  {
    switch(gvnMesh.m2->type)
    {
      case WLZ_CMESH_2D:
        newMesh.m2 = WlzCMeshCopy2D(gvnMesh.m2, squeeze, datSz, newDat,
	                            gvnDat, &errNum);
        break;
      case WLZ_CMESH_2D5:
        newMesh.m2d5 = WlzCMeshCopy2D5(gvnMesh.m2d5, squeeze, datSz, newDat,
				       gvnDat, &errNum);
        break;
      case WLZ_CMESH_3D:
        newMesh.m3 = WlzCMeshCopy3D(gvnMesh.m3, squeeze, datSz, newDat,
				    gvnDat, &errNum);
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
  return(newMesh);
}

/*!
* \return	Copy of the given mesh.
* \ingroup	WlzMesh
* \brief	Creates a copy of the given constrained mesh.
* 		If the squeeze flag is set then the copy will have all
* 		invalid (deleted) entities squeezed out so that the
* 		mew mesh will only have valid mesh entities that are
* 		contiguous and start with index 0.
* 		As well as copying the given mesh this function will
* 		also copy data associated with the nodes of the mesh.
* 		The associated data are held in AlcVector data structures
* 		and are indexed using the node index.
* 		If (datSz == 0) || (newDat == NULL) || (gvnDat == NULL)
* 		then the associated data will be ignored.
* \param	gvnMesh			Given mesh.
* \param	squeeze			Squeeze out invalid (deleted entities
* 					if non zero).
* \param	datSz			Size of associated datum.
* \param	newDat			Destination pointer for the copied
* 					associated data, may be NULL.
* \param	gvnDat			Given associated data, may be NULL.
* \param	dstErr			Destination error pointer, may be NULL.
*/
WlzCMesh2D	*WlzCMeshCopy2D(WlzCMesh2D *gvnMesh, int squeeze,
				size_t datSz,
				AlcVector **newDat, AlcVector *gvnDat,
				WlzErrorNum *dstErr)
{
  int		idE,
  		idN;
  AlcVector	*prvDat = NULL;
  WlzCMesh2D	*newMesh = NULL;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(gvnMesh == NULL)
  {
    errNum = WLZ_ERR_DOMAIN_NULL;
  }
  else if(gvnMesh->type != WLZ_CMESH_2D)
  {
    errNum = WLZ_ERR_DOMAIN_TYPE;
  }
  else if((datSz > 0) && (newDat != NULL) && (gvnDat != NULL))
  {
    if((prvDat = AlcVectorNew(1, datSz, gvnDat->blkSz, NULL)) == NULL)
    {
      errNum = WLZ_ERR_MEM_ALLOC;
    }
  }
  if((errNum == WLZ_ERR_NONE) &&
     (gvnMesh->res.nod.numEnt > 0) && (gvnMesh->res.elm.numEnt > 0))
  {
    newMesh = WlzCMeshNew2D(&errNum);
    if(errNum == WLZ_ERR_NONE)
    {
      newMesh->bBox = gvnMesh->bBox;
      newMesh->maxSqEdgLen = gvnMesh->maxSqEdgLen;
      errNum = WlzCMeshReassignGridCells2D(newMesh, gvnMesh->res.nod.numEnt);
    }
    if(errNum == WLZ_ERR_NONE)
    {
      int newMaxNod,
      	  newMaxElm;

      newMaxNod = (squeeze)? gvnMesh->res.nod.numEnt: gvnMesh->res.nod.maxEnt;
      newMaxElm = (squeeze)? gvnMesh->res.elm.numEnt: gvnMesh->res.elm.maxEnt;
      if((AlcVectorExtend(newMesh->res.nod.vec, newMaxNod) != ALC_ER_NONE) ||
         (AlcVectorExtend(newMesh->res.elm.vec, newMaxElm) != ALC_ER_NONE) ||
         ((prvDat != NULL) &&
          (AlcVectorExtend(prvDat, newMaxNod) != ALC_ER_NONE)))
      {
        errNum = WLZ_ERR_MEM_ALLOC;
      }
    }
    if(errNum == WLZ_ERR_NONE)          
    {
      if(squeeze != 0)    /* Make entities with indices contigous from zero. */
      {
	for(idE = 0; idE < gvnMesh->res.elm.maxEnt; ++idE)
	{
	  WlzCMeshElm2D *gvnElm;

	  gvnElm = (WlzCMeshElm2D *)AlcVectorItemGet(gvnMesh->res.elm.vec, idE);
	  if(gvnElm->idx >= 0)
	  {
	    double sA2 = 0.0;

	    sA2 = WlzCMeshElmSnArea22D(gvnElm);
	    if(fabs(sA2) > WLZ_MESH_TOLERANCE_SQ)   /* Squeeze out zero area
	                                               elements. */
	    {
	      WlzCMeshNod2D *gvnNodes[3],
			    *newNodes[3];

	      /* Copy Nodes. */
	      gvnNodes[0] = WLZ_CMESH_ELM2D_GET_NODE_0(gvnElm);
	      gvnNodes[1] = WLZ_CMESH_ELM2D_GET_NODE_1(gvnElm);
	      gvnNodes[2] = WLZ_CMESH_ELM2D_GET_NODE_2(gvnElm);
	      for(idN = 0; idN < 3; ++idN)
	      {
		if(WlzCMeshLocateNod2D(newMesh, gvnNodes[idN]->pos,
				       WLZ_MESH_TOLERANCE,
				       newNodes + idN) == 0)
		{
		  newNodes[idN] = WlzCMeshNewNod2D(newMesh, gvnNodes[idN]->pos,
						   NULL);
		  /* Copy node associated data. */
		  if(prvDat != NULL)
		  {
		    void *ascP;

		    ascP = AlcVectorItemGet(prvDat, newNodes[idN]->idx);
		    memcpy(ascP, AlcVectorItemGet(gvnDat, gvnNodes[idN]->idx),
			   datSz);
		  }
		}
	      }
	      /* Copy element. */
	      (void )WlzCMeshNewElm2D(newMesh,
				      newNodes[0], newNodes[1], newNodes[2],
				      1, &errNum);
	      if(errNum != WLZ_ERR_NONE)
	      {
		break;
	      }
	    }
	  }
	}
      }
      else /* squeeze == 0, preserve entity indices in copy. */
      {
	/* Copy Nodes. */
        for(idN = 0; idN < gvnMesh->res.nod.maxEnt; ++idN)
	{
	  WlzCMeshNod2D *gvnNod,
	  		*newNod;

	  gvnNod = (WlzCMeshNod2D *)AlcVectorItemGet(gvnMesh->res.nod.vec,
	  					     idN);
	  if(gvnNod->idx < 0)
	  {
            newNod = WlzCMeshAllocNod2D(newMesh);
	    newNod->idx = -1;
	    ++(newMesh->res.nod.numEnt);
	  }
	  else
	  {
	    newNod = WlzCMeshNewNod2D(newMesh, gvnNod->pos, NULL);
	    /* Copy node associated data. */
	    if(prvDat != NULL)
	    {
	      void *ascP;

	      ascP = AlcVectorItemGet(prvDat, newNod->idx);
	      memcpy(ascP, AlcVectorItemGet(gvnDat, gvnNod->idx), datSz);
	    }
	  }
	}
	/* Copy elements. */
	for(idE = 0; idE < gvnMesh->res.elm.maxEnt; ++idE)
	{
	  WlzCMeshElm2D *gvnElm;

	  gvnElm = (WlzCMeshElm2D *)AlcVectorItemGet(gvnMesh->res.elm.vec, idE);
	  if(gvnElm->idx < 0)
	  {
	    (void )WlzCMeshAllocElm2D(newMesh);
	    --(newMesh->res.elm.numEnt);
	  }
	  else
	  {
	    WlzCMeshNod2D *gvnNod;
	    WlzCMeshNod2D *newNodes[3];

	    gvnNod = WLZ_CMESH_ELM2D_GET_NODE_0(gvnElm);
	    newNodes[0] = (WlzCMeshNod2D *)
	    		  AlcVectorItemGet(gvnMesh->res.nod.vec, gvnNod->idx);
	    gvnNod = WLZ_CMESH_ELM2D_GET_NODE_1(gvnElm);
	    newNodes[1] = (WlzCMeshNod2D *)
	                  AlcVectorItemGet(gvnMesh->res.nod.vec, gvnNod->idx);
	    gvnNod = WLZ_CMESH_ELM2D_GET_NODE_2(gvnElm);
	    newNodes[2] = (WlzCMeshNod2D *)
	                  AlcVectorItemGet(gvnMesh->res.nod.vec, gvnNod->idx);
	    (void )WlzCMeshNewElm2D(newMesh,
	                            newNodes[0], newNodes[1], newNodes[2],
				    1, &errNum);
	    if(errNum != WLZ_ERR_NONE)
	    {
	      break;
	    }
	  }
	}
      }
    }
  }
  if(errNum == WLZ_ERR_NONE)
  {
    if(prvDat != NULL)
    {
      *newDat = prvDat;
    }
  }
  else
  {
    if(newMesh != NULL)
    {
      WlzCMeshFree2D(newMesh);
      newMesh = NULL;
    }
    if(prvDat != NULL)
    {
      (void )AlcVectorFree(prvDat);
    }
  }
  if(dstErr != NULL)
  {
    *dstErr = errNum;
  }
  return(newMesh);
}

/*!
* \return	Copy of the given mesh.
* \ingroup	WlzMesh
* \brief	Creates a copy of the given constrained mesh.
* 		If the squeeze flag is set then the copy will have all
* 		invalid (deleted) entities squeezed out so that the
* 		mew mesh will only have valid mesh entities that are
* 		contiguous and start with index 0.
* 		As well as copying the given mesh this function will
* 		also copy data associated with the nodes of the mesh.
* 		The associated data are held in AlcVector data structures
* 		and are indexed using the node index.
* 		If (datSz == 0) || (newDat == NULL) || (gvnDat == NULL)
* 		then the associated data will be ignored.
* \param	gvnMesh			Given mesh.
* \param	squeeze			Squeeze out invalid (deleted entities
* 					if non zero).
* \param        datSz                   Size of associated datum.
* \param        newDat                  Destination pointer for the copied
*                                       associated data, may be NULL.
* \param        gvnDat                  Given associated data, may be NULL.
* \param	dstErr			Destination error pointer, may be NULL.
*/
WlzCMesh2D5	*WlzCMeshCopy2D5(WlzCMesh2D5 *gvnMesh, int squeeze,
				 size_t datSz,
                                 AlcVector **newDat, AlcVector *gvnDat,
                                 WlzErrorNum *dstErr)
{
  int		idE,
  		idN;
  AlcVector	*prvDat = NULL;
  WlzCMesh2D5	*newMesh = NULL;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(gvnMesh == NULL)
  {
    errNum = WLZ_ERR_DOMAIN_NULL;
  }
  else if(gvnMesh->type != WLZ_CMESH_2D5)
  {
    errNum = WLZ_ERR_DOMAIN_TYPE;
  }
  else if((datSz > 0) && (newDat != NULL) && (gvnDat != NULL))
  {
    if((prvDat = AlcVectorNew(1, datSz, gvnDat->blkSz, NULL)) == NULL)
    {
      errNum = WLZ_ERR_MEM_ALLOC;
    }
  }
  if((errNum == WLZ_ERR_NONE) &&
     (gvnMesh->res.nod.numEnt > 0) && (gvnMesh->res.elm.numEnt > 0))
  {
    newMesh = WlzCMeshNew2D5(&errNum);
    if(errNum == WLZ_ERR_NONE)
    {
      newMesh->bBox = gvnMesh->bBox;
      newMesh->maxSqEdgLen = gvnMesh->maxSqEdgLen;
      errNum = WlzCMeshReassignGridCells2D5(newMesh, gvnMesh->res.nod.numEnt);
    }
    if(errNum == WLZ_ERR_NONE)
    {
      int newMaxNod,
      	  newMaxElm;

      newMaxNod = (squeeze)? gvnMesh->res.nod.numEnt: gvnMesh->res.nod.maxEnt;
      newMaxElm = (squeeze)? gvnMesh->res.elm.numEnt: gvnMesh->res.elm.maxEnt;
      if((AlcVectorExtend(newMesh->res.nod.vec, newMaxNod) != ALC_ER_NONE) ||
         (AlcVectorExtend(newMesh->res.elm.vec, newMaxElm) != ALC_ER_NONE) ||
         ((prvDat != NULL) &&
          (AlcVectorExtend(prvDat, newMaxNod) != ALC_ER_NONE)))
      {
        errNum = WLZ_ERR_MEM_ALLOC;
      }
    }
    if(errNum == WLZ_ERR_NONE)          
    {
      if(squeeze != 0)    /* Make entities with indices contigous from zero. */
      {
	for(idE = 0; idE < gvnMesh->res.elm.maxEnt; ++idE)
	{
	  WlzCMeshElm2D5 *gvnElm;

	  gvnElm = (WlzCMeshElm2D5 *)AlcVectorItemGet(gvnMesh->res.elm.vec,
	  					      idE);
	  if(gvnElm->idx >= 0)
	  {
	    double sQA2 = 0.0;

	    sQA2 = WlzCMeshElmSqArea22D5(gvnElm);
	    if(fabs(sQA2) > WLZ_MESH_TOLERANCE_SQ)
	    {
	      WlzCMeshNod2D5 *gvnNodes[3],
			     *newNodes[3];

	      /* Copy Nodes. */
	      gvnNodes[0] = WLZ_CMESH_ELM2D5_GET_NODE_0(gvnElm);
	      gvnNodes[1] = WLZ_CMESH_ELM2D5_GET_NODE_1(gvnElm);
	      gvnNodes[2] = WLZ_CMESH_ELM2D5_GET_NODE_2(gvnElm);
	      for(idN = 0; idN < 3; ++idN)
	      {
		if(WlzCMeshLocateNod2D5(newMesh, gvnNodes[idN]->pos,
					WLZ_MESH_TOLERANCE,
					newNodes + idN) == 0)
		{
		  newNodes[idN] = WlzCMeshNewNod2D5(newMesh,
		  				    gvnNodes[idN]->pos, NULL);
		  /* Copy node associated data. */
		  if(prvDat != NULL)
		  {
		    void *ascP;

		    ascP = AlcVectorItemGet(prvDat, newNodes[idN]->idx);
		    memcpy(ascP, AlcVectorItemGet(gvnDat, gvnNodes[idN]->idx),
			   datSz);
		  }
		}
	      }
	      /* Copy element. */
	      (void )WlzCMeshNewElm2D5(newMesh,
				       newNodes[0], newNodes[1], newNodes[2],
				       1, &errNum);
	      if(errNum != WLZ_ERR_NONE)
	      {
		break;
	      }
	    }
	  }
	}
      }
      else /* squeeze == 0, preserve entity indices in copy. */
      {
	/* Copy Nodes. */
        for(idN = 0; idN < gvnMesh->res.nod.maxEnt; ++idN)
	{
	  WlzCMeshNod2D5 *gvnNod,
	  		 *newNod;

	  gvnNod = (WlzCMeshNod2D5 *)AlcVectorItemGet(gvnMesh->res.nod.vec,
	  					      idN);
	  if(gvnNod->idx < 0)
	  {
            newNod = WlzCMeshAllocNod2D5(newMesh);
	    newNod->idx = -1;
	    ++(newMesh->res.nod.numEnt);
	  }
	  else
	  {
	    newNod = WlzCMeshNewNod2D5(newMesh, gvnNod->pos, NULL);
	    /* Copy node associated data. */
	    if(prvDat != NULL)
	    {
	      void *ascP;

	      ascP = AlcVectorItemGet(prvDat, newNod->idx);
	      memcpy(ascP, AlcVectorItemGet(gvnDat, gvnNod->idx), datSz);
	    }
	  }
	}
	/* Copy elements. */
	for(idE = 0; idE < gvnMesh->res.elm.maxEnt; ++idE)
	{
	  WlzCMeshElm2D5 *gvnElm;

	  gvnElm = (WlzCMeshElm2D5 *)AlcVectorItemGet(gvnMesh->res.elm.vec,
	                                              idE);
	  if(gvnElm->idx < 0)
	  {
	    (void )WlzCMeshAllocElm2D5(newMesh);
	    --(newMesh->res.elm.numEnt);
	  }
	  else
	  {
	    WlzCMeshNod2D5 *gvnNod;
	    WlzCMeshNod2D5 *newNodes[3];

	    gvnNod = WLZ_CMESH_ELM2D_GET_NODE_0(gvnElm);
	    newNodes[0] = (WlzCMeshNod2D5 *)
	    		  AlcVectorItemGet(gvnMesh->res.nod.vec, gvnNod->idx);
	    gvnNod = WLZ_CMESH_ELM2D_GET_NODE_1(gvnElm);
	    newNodes[1] = (WlzCMeshNod2D5 *)
	                  AlcVectorItemGet(gvnMesh->res.nod.vec, gvnNod->idx);
	    gvnNod = WLZ_CMESH_ELM2D_GET_NODE_2(gvnElm);
	    newNodes[2] = (WlzCMeshNod2D5 *)
	                  AlcVectorItemGet(gvnMesh->res.nod.vec, gvnNod->idx);
	    (void )WlzCMeshNewElm2D5(newMesh,
	                            newNodes[0], newNodes[1], newNodes[2],
				    1, &errNum);
	    if(errNum != WLZ_ERR_NONE)
	    {
	      break;
	    }
	  }
	}
      }
    }
  }
  if(errNum == WLZ_ERR_NONE)
  {
    if(prvDat != NULL)
    {
      *newDat = prvDat;
    }
  }
  else
  {
    if(newMesh != NULL)
    {
      WlzCMeshFree2D5(newMesh);
      newMesh = NULL;
    }
    if(prvDat != NULL)
    {
      (void )AlcVectorFree(prvDat);
    }
  }
  if(dstErr != NULL)
  {
    *dstErr = errNum;
  }
  return(newMesh);
}

/*!
* \return	Copy of the given mesh.
* \ingroup	WlzMesh
* \brief	Creates a copy of the given constrained mesh.
* 		If the squeeze flag is set then the copy will have all
* 		invalid (deleted) entities squeezed out so that the
* 		mew mesh will only have valid mesh entities that are
* 		contiguous and start with index 0.
* 		As well as copying the given mesh this function will
* 		also copy data associated with the nodes of the mesh.
* 		The associated data are held in AlcVector data structures
* 		and are indexed using the node index.
* 		If (datSz == 0) || (newDat == NULL) || (gvnDat == NULL)
* 		then the associated data will be ignored.
* \param	gvnMesh			Given mesh.
* \param	squeeze			Squeeze out invalid (deleted entities
* 					if non zero).
* \param        datSz                   Size of associated datum.
* \param        newDat                  Destination pointer for the copied
*                                       associated data, may be NULL.
* \param        gvnDat                  Given associated data, may be NULL.
* \param	dstErr			Destination error pointer, may be NULL.
*/
WlzCMesh3D	*WlzCMeshCopy3D(WlzCMesh3D *gvnMesh, int squeeze,
				size_t datSz,
                                AlcVector **newDat, AlcVector *gvnDat,
                                WlzErrorNum *dstErr)
{
  int		idE,
  		idN;
  AlcVector	*prvDat = NULL;
  WlzCMesh3D	*newMesh = NULL;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(gvnMesh == NULL)
  {
    errNum = WLZ_ERR_DOMAIN_NULL;
  }
  else if(gvnMesh->type != WLZ_CMESH_3D)
  {
    errNum = WLZ_ERR_DOMAIN_TYPE;
  }
  else if((datSz > 0) && (newDat != NULL) && (gvnDat != NULL))
  {
    if((prvDat = AlcVectorNew(1, datSz, gvnDat->blkSz, NULL)) == NULL)
    {
      errNum = WLZ_ERR_MEM_ALLOC;
    }
  }
  if((errNum == WLZ_ERR_NONE) &&
     (gvnMesh->res.nod.numEnt > 0) && (gvnMesh->res.elm.numEnt > 0))
  {
    newMesh = WlzCMeshNew3D(&errNum);
    if(errNum == WLZ_ERR_NONE)
    {
      newMesh->bBox = gvnMesh->bBox;
      newMesh->maxSqEdgLen = gvnMesh->maxSqEdgLen;
      errNum = WlzCMeshReassignGridCells3D(newMesh, gvnMesh->res.nod.numEnt);
    }
    if(errNum == WLZ_ERR_NONE)
    {
      int newMaxNod,
      	  newMaxElm;

      newMaxNod = (squeeze)? gvnMesh->res.nod.numEnt: gvnMesh->res.nod.maxEnt;
      newMaxElm = (squeeze)? gvnMesh->res.elm.numEnt: gvnMesh->res.elm.maxEnt;
      if((AlcVectorExtend(newMesh->res.nod.vec, newMaxNod) != ALC_ER_NONE) ||
         (AlcVectorExtend(newMesh->res.elm.vec, newMaxElm) != ALC_ER_NONE) ||
         ((prvDat != NULL) &&
          (AlcVectorExtend(prvDat, newMaxNod) != ALC_ER_NONE)))
      {
        errNum = WLZ_ERR_MEM_ALLOC;
      }
    }
    if(errNum == WLZ_ERR_NONE)          
    {
      if(squeeze != 0)    /* Make entities with indices contigous from zero. */
      {
	for(idE = 0; idE < gvnMesh->res.elm.maxEnt; ++idE)
	{
	  WlzCMeshElm3D *gvnElm;

	  gvnElm = (WlzCMeshElm3D *)AlcVectorItemGet(gvnMesh->res.elm.vec, idE);
	  if(gvnElm->idx >= 0)
	  {
	    double sV6 = 0.0;

            sV6 = WlzCMeshElmSnVolume63D(gvnElm);
	    if(fabs(sV6) > WLZ_MESH_TOLERANCE_SQ)
	    {
	      WlzCMeshNod3D *gvnNodes[4],
			    *newNodes[4];

	      /* Copy Nodes. */
	      gvnNodes[0] = WLZ_CMESH_ELM3D_GET_NODE_0(gvnElm);
	      gvnNodes[1] = WLZ_CMESH_ELM3D_GET_NODE_1(gvnElm);
	      gvnNodes[2] = WLZ_CMESH_ELM3D_GET_NODE_2(gvnElm);
	      gvnNodes[3] = WLZ_CMESH_ELM3D_GET_NODE_3(gvnElm);
	      for(idN = 0; idN < 4; ++idN)
	      {
		WlzIVertex3 dumGrdPos;
		WlzCMeshNod3D *dumNod;

		if(WlzCMeshLocateNod3D(newMesh, gvnNodes[idN]->pos,
				       &dumGrdPos, &dumNod,
				       WLZ_MESH_TOLERANCE,
				       newNodes + idN) == 0)
		{
		  newNodes[idN] = WlzCMeshNewNod3D(newMesh, gvnNodes[idN]->pos,
						   NULL);
		  /* Copy node associated data. */
		  if(prvDat != NULL)
		  {
		    void *ascP;

		    ascP = AlcVectorItemGet(prvDat, newNodes[idN]->idx);
		    memcpy(ascP, AlcVectorItemGet(gvnDat, gvnNodes[idN]->idx),
			   datSz);
		  }
		}
	      }
	      /* Copy element. */
	      (void )WlzCMeshNewElm3D(newMesh, newNodes[0], newNodes[1],
				      newNodes[2], newNodes[3], 1, &errNum);
	      if(errNum != WLZ_ERR_NONE)
	      {
		break;
	      }
	    }
	  }
	}
      }
      else /* squeeze == 0, preserve entity indices in copy. */
      {
	/* Copy Nodes. */
        for(idN = 0; idN < gvnMesh->res.nod.maxEnt; ++idN)
	{
	  WlzCMeshNod3D *gvnNod,
	  		*newNod;

	  gvnNod = (WlzCMeshNod3D *)AlcVectorItemGet(gvnMesh->res.nod.vec,
	  					     idN);
	  if(gvnNod->idx < 0)
	  {
            newNod = WlzCMeshAllocNod3D(newMesh);
	    newNod->idx = -1;
	    ++(newMesh->res.nod.numEnt);
	  }
	  else
	  {
	    newNod = WlzCMeshNewNod3D(newMesh, gvnNod->pos, NULL);
	    /* Copy node associated data. */
	    if(prvDat != NULL)
	    {
	      void *ascP;

	      ascP = AlcVectorItemGet(prvDat, newNod->idx);
	      memcpy(ascP, AlcVectorItemGet(gvnDat, gvnNod->idx), datSz);
	    }
	  }
	}
	/* Copy elements. */
	for(idE = 0; idE < gvnMesh->res.elm.maxEnt; ++idE)
	{
	  WlzCMeshElm3D *gvnElm;

	  gvnElm = (WlzCMeshElm3D *)AlcVectorItemGet(gvnMesh->res.elm.vec, idE);
	  if(gvnElm->idx < 0)
	  {
	    (void )WlzCMeshAllocElm3D(newMesh);
	    --(newMesh->res.elm.numEnt);
	  }
	  else
	  {
	    WlzCMeshNod3D *gvnNod;
	    WlzCMeshNod3D *newNodes[4];

	    gvnNod = WLZ_CMESH_ELM3D_GET_NODE_0(gvnElm);
	    newNodes[0] = (WlzCMeshNod3D *)
	    		  AlcVectorItemGet(gvnMesh->res.nod.vec, gvnNod->idx);
	    gvnNod = WLZ_CMESH_ELM3D_GET_NODE_1(gvnElm);
	    newNodes[1] = (WlzCMeshNod3D *)
	                  AlcVectorItemGet(gvnMesh->res.nod.vec, gvnNod->idx);
	    gvnNod = WLZ_CMESH_ELM3D_GET_NODE_2(gvnElm);
	    newNodes[2] = (WlzCMeshNod3D *)
	                  AlcVectorItemGet(gvnMesh->res.nod.vec, gvnNod->idx);
	    gvnNod = WLZ_CMESH_ELM3D_GET_NODE_3(gvnElm);
	    newNodes[3] = (WlzCMeshNod3D *)
	                  AlcVectorItemGet(gvnMesh->res.nod.vec, gvnNod->idx);
	    (void )WlzCMeshNewElm3D(newMesh, newNodes[0], newNodes[1],
	    			    newNodes[2], newNodes[3], 1, &errNum);
	    if(errNum != WLZ_ERR_NONE)
	    {
	      break;
	    }
	  }
	}
      }
    }
  }
  if(errNum == WLZ_ERR_NONE)
  {
    if(prvDat != NULL)
    {
      *newDat = prvDat;
    }
  }
  else
  {
    if(newMesh != NULL)
    {
      WlzCMeshFree3D(newMesh);
      newMesh = NULL;
    }
    if(prvDat != NULL)
    {
      (void )AlcVectorFree(prvDat);
    }
  }
  if(dstErr != NULL)
  {
    *dstErr = errNum;
  }
  return(newMesh);
}

/*!
* \return       Woolz error code.
* \ingroup      WlzMesh
* \brief        Reorders nodes in any elements which have negative
		area.
* \param        mesh                     The given constrained mesh.
*/
WlzErrorNum     WlzCMeshFixNegativeElms(WlzCMeshP mesh)
{
  WlzErrorNum   errNum = WLZ_ERR_NONE;

  if(mesh.v == NULL)
  {
    errNum = WLZ_ERR_DOMAIN_NULL;
  }
  else
  {
    switch(mesh.m2->type)
    {
      case WLZ_CMESH_2D:
        errNum = WlzCMeshFixNegativeElms2D(mesh.m2);
        break;
      case WLZ_CMESH_3D:
        errNum = WlzCMeshFixNegativeElms3D(mesh.m3);
        break;
      default:
        errNum = WLZ_ERR_DOMAIN_TYPE;
        break;
    }
  }
  return(errNum);
}

/*!
* \return       Woolz error code.
* \ingroup      WlzMesh
* \brief        Reorders nodes in any elements which have negative
		area.
* \param        mesh                     Given 2D constrained mesh.
*/
WlzErrorNum     WlzCMeshFixNegativeElms2D(WlzCMesh2D *mesh)
{
  int		idE,
  		idG;
  double	sA2;
  WlzCMeshElm2D *elm;
  WlzCMeshEdgU2D *edu0,
  		*edu1,
		*edu2;
  WlzCMeshNod2D	*nod[3];
  WlzErrorNum   errNum = WLZ_ERR_NONE;

  if(mesh == NULL)
  {
    errNum = WLZ_ERR_DOMAIN_NULL;
  }
  else if(mesh->type != WLZ_CMESH_2D)
  {
    errNum = WLZ_ERR_DOMAIN_TYPE;
  }
  else
  {
    for(idE = 0; idE < mesh->res.elm.maxEnt; ++idE)
    {
      elm = (WlzCMeshElm2D *)AlcVectorItemGet(mesh->res.elm.vec, idE);
      if(elm->idx >= 0)
      {
	nod[0] = WLZ_CMESH_ELM2D_GET_NODE_0(elm);
	nod[1] = WLZ_CMESH_ELM2D_GET_NODE_1(elm);
	nod[2] = WLZ_CMESH_ELM2D_GET_NODE_2(elm);
	sA2 = WlzGeomTriangleSnArea2(nod[0]->pos, nod[1]->pos, nod[2]->pos);
	if(fabs(sA2) < WLZ_MESH_TOLERANCE_SQ)
	{
	  /* Might need to remove the element and repair the mesh here? */
	  errNum = WLZ_ERR_DOMAIN_DATA;
	}
	else if(sA2 < WLZ_MESH_TOLERANCE_SQ)
	{
	  /* Unlink the edge uses from the rest of the mesh. */
	  for(idG = 0; idG < 3; ++idG)
	  {
	    edu0 = elm->edu + idG;
	    if((edu0->opp != NULL) && (edu0->opp->opp != NULL) &&
	       (edu0->opp->opp->elm == elm))
            {
	      edu0->opp->opp = NULL;
	    }
	    if(edu0 == edu0->nnxt)
	    {
	      edu0->nnxt = NULL;
	    }
	    else
	    {
	      edu1 = edu0;
	      while((edu2 = edu1->nnxt) != edu0)
	      {
	        edu1 = edu2;
	      }
	      edu1->nnxt = edu0->nnxt;
	      if(edu0->nod->edu == edu0)
	      {
	        edu0->nod->edu = edu1;
	      }
	    }
	  }
	  /* Create connnectivities with the element and to the rest of the
	   * mesh. */
	  errNum = WlzCMeshSetElm2D(mesh, elm, nod[1], nod[0], nod[2], 0);
	}
      }
    }
  }
  return(errNum);
}

/*!
* \return       Woolz error code.
* \ingroup      WlzMesh
* \brief        Reorders nodes in any elements which have negative
		area.
* \param        mesh                     Given 3D constrained mesh.
*/
WlzErrorNum     WlzCMeshFixNegativeElms3D(WlzCMesh3D *mesh)
{
  int		idE,
		idF,
  		idG;
  double	sV6;
  WlzCMeshElm3D *elm;
  WlzCMeshFace	*fce0;
  WlzCMeshEdgU3D *edu0,
  		*edu1,
		*edu2;
  WlzCMeshNod3D	*nod[4];
  WlzErrorNum   errNum = WLZ_ERR_NONE;

  if(mesh == NULL)
  {
    errNum = WLZ_ERR_DOMAIN_NULL;
  }
  else if(mesh->type != WLZ_CMESH_3D)
  {
    errNum = WLZ_ERR_DOMAIN_TYPE;
  }
  else
  {
    for(idE = 0; idE < mesh->res.elm.maxEnt; ++idE)
    {
      elm = (WlzCMeshElm3D *)AlcVectorItemGet(mesh->res.elm.vec, idE);
      if(elm->idx >= 0)
      {
	nod[0] = WLZ_CMESH_ELM3D_GET_NODE_0(elm);
	nod[1] = WLZ_CMESH_ELM3D_GET_NODE_1(elm);
	nod[2] = WLZ_CMESH_ELM3D_GET_NODE_2(elm);
	nod[3] = WLZ_CMESH_ELM3D_GET_NODE_3(elm);
	sV6 = WlzGeomTetraSnVolume6(nod[0]->pos, nod[1]->pos, nod[2]->pos,
				    nod[3]->pos);
	if(fabs(sV6) < WLZ_MESH_TOLERANCE_SQ)
	{
	  /* Might need to remove the element and repair the mesh here? */
	  errNum = WLZ_ERR_DOMAIN_DATA;
	}
	else if(sV6 < WLZ_MESH_TOLERANCE_SQ)
	{
	  for(idF = 0; idF < 4; ++idF)
	  {
	    fce0 = elm->face + idF;
	    /* Unlink edge uses from the nodes. */
	    for(idG = 0; idG < 3; ++idG)
	    {
	      edu0 = fce0->edu + idG;
	      if(edu0 == edu0->nnxt)
	      {
		edu0->nnxt = NULL;
	      }
	      else
	      {
		edu1 = edu0;
		edu2 = edu1->nnxt;
		while(edu2 != edu0)
		{
		  edu1 = edu2;
		  edu2 = edu2->nnxt;
		}
		edu1->nnxt = edu2->nnxt;
		edu1->nod->edu = edu1;
	      }
	    }
	    /* Unlink face. Need to make sure that the opp - opp link is back
	     * to this element and not some other that will replace it. */
	    if((fce0->opp != NULL) && (fce0->opp->opp != NULL) &&
	       (fce0->opp->opp->elm == elm))
	    {
	      fce0->opp->opp = NULL;
	    }
	  }
	  /* Create connnectivities with the element and to the rest of the
	   * mesh. */
	  errNum = WlzCMeshSetElm3D(mesh, elm, nod[1], nod[0], nod[2],
	  			    nod[3], 0);
	}
      }
    }
  }
  return(errNum);
}

/*!
* \ingroup	WlzMesh
* \brief	Computes simple mesh node and element grid cell location 
*		statistics.
*		This function is probably only useful for optimising the
*		number of location cells allocated.
* \param	mesh			Given mesh.
* \param	dstMinNodPerCell	Destination pointer for the
* 					minimum number of nodes per cell.
* \param	dstMaxNodPerCell	Destination pointer for the
* 					maximum number of nodes per cell.
* \param	dstMeanNodPerCell	Destination pointer for the
* 					mean number of nodes per cell.
* \param	dstMinElmPerCell	Destination pointer for the
* 					minimum number of nodes per cell.
* \param	dstMaxElmPerCell	Destination pointer for the
* 					maximum number of nodes per cell.
* \param	dstMeanElmPerCell	Destination pointer for the
* 					mean number of nodes per cell.
*/
void	 	WlzCMeshGetCellStats(WlzCMeshP mesh,
				     int *dstMinNodPerCell,
				     int *dstMaxNodPerCell,
				     double *dstMeanNodPerCell,
				     int *dstMinElmPerCell,
				     int *dstMaxElmPerCell,
				     double *dstMeanElmPerCell)
{
  if(mesh.v != NULL)
  {
    switch(mesh.m2->type)
    {
      case WLZ_CMESH_2D:
        WlzCMeshGetCellStats2D(mesh.m2,
			dstMinNodPerCell, dstMaxNodPerCell, dstMeanNodPerCell,
			dstMinElmPerCell, dstMaxElmPerCell, dstMeanElmPerCell);
        break;
      case WLZ_CMESH_3D:
        WlzCMeshGetCellStats3D(mesh.m3,
			dstMinNodPerCell, dstMaxNodPerCell, dstMeanNodPerCell,
			dstMinElmPerCell, dstMaxElmPerCell, dstMeanElmPerCell);
        break;
      default:
	break;
    }
  }
}

/*!
* \ingroup	WlzMesh
* \brief	Computes simple 2D mesh node and element grid cell location 
*		statistics.
*		This function is probably only useful for optimising the
*		number of location cells allocated.
* \param	mesh			Given mesh.
* \param	dstMinNodPerCell	Destination pointer for the
* 					minimum number of nodes per cell.
* \param	dstMaxNodPerCell	Destination pointer for the
* 					maximum number of nodes per cell.
* \param	dstMeanNodPerCell	Destination pointer for the
* 					mean number of nodes per cell.
* \param	dstMinElmPerCell	Destination pointer for the
* 					minimum number of nodes per cell.
* \param	dstMaxElmPerCell	Destination pointer for the
* 					maximum number of nodes per cell.
* \param	dstMeanElmPerCell	Destination pointer for the
* 					mean number of nodes per cell.
*/
void	 	WlzCMeshGetCellStats2D(WlzCMesh2D *mesh,
				       int *dstMinNodPerCell,
				       int *dstMaxNodPerCell,
				       double *dstMeanNodPerCell,
				       int *dstMinElmPerCell,
				       int *dstMaxElmPerCell,
				       double *dstMeanElmPerCell)
{
  int		cntNPC = 0,
  		nNPC = 0,
  		minNPC = 0,
  		maxNPC = 0,
		sumNPC = 0,
     		cntEPC = 0,
		nEPC = 0,
  		minEPC = 0,
  		maxEPC = 0,
		sumEPC = 0;
  WlzIVertex2	idx,
		nCells;
  WlzCMeshNod2D	*nod;
  WlzCMeshCellElm2D *cElm;
  WlzCMeshCell2D *cell;

  nCells = mesh->cGrid.nCells;
  for(idx.vtY = 0; idx.vtY < nCells.vtY; ++idx.vtY)
  {
    for(idx.vtX = 0; idx.vtX < nCells.vtX; ++idx.vtX)
    {
      nNPC = 0;
      cell = *(mesh->cGrid.cells + idx.vtY) + idx.vtX;
      nod = cell->nod;
      if(nod != NULL)
      {
	++cntNPC;
	do
	{
	  ++nNPC;
	  nod = nod->next;
	} while((nod != NULL) && (nod != cell->nod));
      }
      nEPC = 0;
      cElm = cell->cElm;
      if(cElm != NULL)
      {
	++cntEPC;
	do
	{
	  ++nEPC;
	  cElm = cElm->nextCell;
	} while((cElm != NULL) && (cElm != cell->cElm));
      }
      if(cntNPC > 0)
      {
	if(cntNPC == 1)
	{
	  minNPC = maxNPC = sumNPC = nNPC;
	}
	else
	{
	  if(nNPC < minNPC)
	  {
	    minNPC = nNPC;
	  }
	  else if(nNPC > maxNPC)
	  {
	    maxNPC = nNPC;
	  }
	  sumNPC += nNPC;
	}
      }
      if(cntEPC > 0)
      {
	if(cntEPC == 1)
	{
	  minEPC = maxEPC = sumEPC = nEPC;
	}
	else
	{
	  if(nEPC < minEPC)
	  {
	    minEPC = nEPC;
	  }
	  else if(nEPC > maxEPC)
	  {
	    maxEPC = nEPC;
	  }
	  sumEPC += nEPC;
	}
      }
    }
  }
  if(cntNPC > 0)
  {
    *dstMinNodPerCell = minNPC;
    *dstMaxNodPerCell = maxNPC;
    *dstMeanNodPerCell = (double )sumNPC / (double )cntNPC;
  }
  else
  {
    *dstMinNodPerCell = 0;
    *dstMaxNodPerCell = 0;
    *dstMeanNodPerCell = 0.0;
  }
  if(cntEPC > 0)
  {
    *dstMinElmPerCell = minEPC;
    *dstMaxElmPerCell = maxEPC;
    *dstMeanElmPerCell = (double )sumEPC / (double )cntEPC;
  }
  else
  {
    *dstMinElmPerCell = 0;
    *dstMaxElmPerCell = 0;
    *dstMeanElmPerCell = 0.0;
  }
}

/*!
* \ingroup	WlzMesh
* \brief	Computes simple 3D mesh node and element grid cell location 
*		statistics.
*		This function is probably only useful for optimising the
*		number of location cells allocated.
* \param	mesh			Given mesh.
* \param	dstMinNodPerCell	Destination pointer for the
* 					minimum number of nodes per cell.
* \param	dstMaxNodPerCell	Destination pointer for the
* 					maximum number of nodes per cell.
* \param	dstMeanNodPerCell	Destination pointer for the
* 					mean number of nodes per cell.
* \param	dstMinElmPerCell	Destination pointer for the
* 					minimum number of nodes per cell.
* \param	dstMaxElmPerCell	Destination pointer for the
* 					maximum number of nodes per cell.
* \param	dstMeanElmPerCell	Destination pointer for the
* 					mean number of nodes per cell.
*/
void	 	WlzCMeshGetCellStats3D(WlzCMesh3D *mesh,
				       int *dstMinNodPerCell,
				       int *dstMaxNodPerCell,
				       double *dstMeanNodPerCell,
				       int *dstMinElmPerCell,
				       int *dstMaxElmPerCell,
				       double *dstMeanElmPerCell)
{
  int		cntNPC = 0,
  		nNPC = 0,
  		minNPC = 0,
  		maxNPC = 0,
		sumNPC = 0,
     		cntEPC = 0,
		nEPC = 0,
  		minEPC = 0,
  		maxEPC = 0,
		sumEPC = 0;
  WlzIVertex3	idx,
		nCells;
  WlzCMeshNod3D	*nod;
  WlzCMeshCellElm3D *cElm;
  WlzCMeshCell3D *cell;

  nCells = mesh->cGrid.nCells;
  for(idx.vtZ = 0; idx.vtZ < nCells.vtZ; ++idx.vtZ)
  {
    for(idx.vtY = 0; idx.vtY < nCells.vtY; ++idx.vtY)
    {
      for(idx.vtX = 0; idx.vtX < nCells.vtX; ++idx.vtX)
      {
	nNPC = 0;
	cell = *(*(mesh->cGrid.cells + idx.vtZ) + idx.vtY) + idx.vtX;
	nod = cell->nod;
	if(nod != NULL)
	{
	  ++cntNPC;
	  do
	  {
	    ++nNPC;
	    nod = nod->next;
	  } while((nod != NULL) && (nod != cell->nod));
	}
	nEPC = 0;
	cElm = cell->cElm;
	if(cElm != NULL)
	{
	  ++cntEPC;
	  do
	  {
	    ++nEPC;
	    cElm = cElm->nextCell;
	  } while((cElm != NULL) && (cElm != cell->cElm));
	}
	if(cntNPC > 0)
	{
	  if(cntNPC == 1)
	  {
	    minNPC = maxNPC = sumNPC = nNPC;
	  }
	  else
	  {
	    if(nNPC < minNPC)
	    {
	      minNPC = nNPC;
	    }
	    else if(nNPC > maxNPC)
	    {
	      maxNPC = nNPC;
	    }
	    sumNPC += nNPC;
	  }
	}
	if(cntEPC > 0)
	{
	  if(cntEPC == 1)
	  {
	    minEPC = maxEPC = sumEPC = nEPC;
	  }
	  else
	  {
	    if(nEPC < minEPC)
	    {
	      minEPC = nEPC;
	    }
	    else if(nEPC > maxEPC)
	    {
	      maxEPC = nEPC;
	    }
	    sumEPC += nEPC;
	  }
	}
      }
    }
  }
  if(cntNPC > 0)
  {
    *dstMinNodPerCell = minNPC;
    *dstMaxNodPerCell = maxNPC;
    *dstMeanNodPerCell = (double )sumNPC / (double )cntNPC;
  }
  else
  {
    *dstMinNodPerCell = 0;
    *dstMaxNodPerCell = 0;
    *dstMeanNodPerCell = 0.0;
  }
  if(cntEPC > 0)
  {
    *dstMinElmPerCell = minEPC;
    *dstMaxElmPerCell = maxEPC;
    *dstMeanElmPerCell = (double )sumEPC / (double )cntEPC;
  }
  else
  {
    *dstMinElmPerCell = 0;
    *dstMaxElmPerCell = 0;
    *dstMeanElmPerCell = 0.0;
  }
}

/*!
* \return	New node index look up table or NULL on error.
* \ingroup	WlzMesh
* \brief	Allocates and populates a node look up table which maps
* 		node indices to the range 0 - (n nodes - 1). The size of
* 		the look up table is the maximum number of nodes allocated
* 		in the mesh.
* \param	mesh			The mesh.
* \param	dstNNod			Destination pointer for the number of
* 					nodes in the look up table.
*/
int		*WlzCMeshMakeNodIdxTbl2D(WlzCMesh2D *mesh, int *dstNNod)
{
  int		nNod = 0;
  int		*idxTb = NULL;

  if((idxTb = AlcMalloc(sizeof(int) * mesh->res.nod.maxEnt)) != NULL) 
  {
    nNod = WlzCMeshSetNodIdxTbl2D(mesh, idxTb);
  }
  if(dstNNod)
  {
    *dstNNod = nNod;
  }
  return(idxTb);
}

/*!
* \return	New node index look up table or NULL on error.
* \ingroup	WlzMesh
* \brief	Allocates and populates a node look up table which maps
* 		node indices to the range 0 - (n nodes - 1). The size of
* 		the look up table is the maximum number of nodes allocated
* 		in the mesh.
* \param	mesh			The mesh.
* \param	dstNNod			Destination pointer for the number of
* 					nodes in the look up table.
*/
int		*WlzCMeshMakeNodIdxTbl2D5(WlzCMesh2D5 *mesh, int *dstNNod)
{
  int		nNod = 0;
  int		*idxTb = NULL;

  if((idxTb = AlcMalloc(sizeof(int) * mesh->res.nod.maxEnt)) != NULL) 
  {
    nNod = WlzCMeshSetNodIdxTbl2D5(mesh, idxTb);
  }
  if(dstNNod)
  {
    *dstNNod = nNod;
  }
  return(idxTb);
}

/*!
* \return	New node index look up table or NULL on error.
* \ingroup	WlzMesh
* \brief	Allocates and populates a node look up table which maps
* 		node indices to the range 0 - (n nodes - 1). The size of
* 		the look up table is the maximum number of nodes allocated
* 		in the mesh.
* \param	mesh			The mesh.
* \param	dstNNod			Destination pointer for the number of
* 					nodes in the look up table.
*/
int		*WlzCMeshMakeNodIdxTbl3D(WlzCMesh3D *mesh, int *dstNNod)
{
  int		nNod = 0;
  int		*idxTb = NULL;

  if((idxTb = AlcMalloc(sizeof(int) * mesh->res.nod.maxEnt)) != NULL) 
  {
    nNod = WlzCMeshSetNodIdxTbl3D(mesh, idxTb);
  }
  if(dstNNod)
  {
    *dstNNod = nNod;
  }
  return(idxTb);
}

/*!
* \return	Number of nodes in the look up table.
* \ingroup	WlzMesh
* \brief	Populates a node look up table which maps node indices to
* 		the range 0 - (n nodes - 1). The size of the look up table
* 		must be at least the maximum number of nodes allocated in
* 		the mesh.
* \param	mesh			The mesh.
* \param	idxTb			The allocated index table to be
* 					populated.
*/
int		WlzCMeshSetNodIdxTbl2D(WlzCMesh2D *mesh, int *idxTb)
{
  int		idN,
  		nNod = 0;

  for(idN = 0; idN < mesh->res.nod.maxEnt; ++idN)
  {
    WlzCMeshNod2D *nod;

    nod = (WlzCMeshNod2D *)AlcVectorItemGet(mesh->res.nod.vec, idN);
    if(nod->idx >= 0)
    {
      idxTb[idN] = nNod++;
    }
  }
  return(nNod);
}

/*!
* \return	Number of nodes in the look up table.
* \ingroup	WlzMesh
* \brief	Populates a node look up table which maps node indices to
* 		the range 0 - (n nodes - 1). The size of the look up table
* 		must be at least the maximum number of nodes allocated in
* 		the mesh.
* \param	mesh			The mesh.
* \param	idxTb			The allocated index table to be
* 					populated.
*/
int		WlzCMeshSetNodIdxTbl2D5(WlzCMesh2D5 *mesh, int *idxTb)
{
  int		idN,
  		nNod = 0;

  for(idN = 0; idN < mesh->res.nod.maxEnt; ++idN)
  {
    WlzCMeshNod2D5 *nod;

    nod = (WlzCMeshNod2D5 *)AlcVectorItemGet(mesh->res.nod.vec, idN);
    if(nod->idx >= 0)
    {
      idxTb[idN] = nNod++;
    }
  }
  return(nNod);
}

/*!
* \return	Number of nodes in the look up table.
* \ingroup	WlzMesh
* \brief	Populates a node look up table which maps node indices to
* 		the range 0 - (n nodes - 1). The size of the look up table
* 		must be at least the maximum number of nodes allocated in
* 		the mesh.
* \param	mesh			The mesh.
* \param	idxTb			The allocated index table to be
* 					populated.
*/
int		WlzCMeshSetNodIdxTbl3D(WlzCMesh3D *mesh, int *idxTb)
{
  int		idN,
  		nNod = 0;

  for(idN = 0; idN < mesh->res.nod.maxEnt; ++idN)
  {
    WlzCMeshNod3D *nod;

    nod = (WlzCMeshNod3D *)AlcVectorItemGet(mesh->res.nod.vec, idN);
    if(nod->idx >= 0)
    {
      idxTb[idN] = nNod++;
    }
  }
  return(nNod);
}

/*!
* \return	New element index look up table or NULL on error.
* \ingroup	WlzMesh
* \brief	Allocates and populates an element look up table which maps
* 		element indices to the range 0 - (n elements - 1). The size
* 		of the look up table is the maximum number of elements
* 		allocated in the mesh.
* \param	mesh			The mesh.
* \param	dstNElm			Destination pointer for the number of
* 					elements in the look up table.
*/
int		*WlzCMeshMakeElmIdxTbl2D(WlzCMesh2D *mesh, int *dstNElm)
{
  int		nElm = 0;
  int		*idxTb = NULL;

  if((idxTb = AlcMalloc(sizeof(int) * mesh->res.elm.maxEnt)) != NULL) 
  {
    nElm = WlzCMeshSetElmIdxTbl2D(mesh, idxTb);
  }
  if(dstNElm)
  {
    *dstNElm = nElm;
  }
  return(idxTb);
}

/*!
* \return	New element index look up table or NULL on error.
* \ingroup	WlzMesh
* \brief	Allocates and populates an element look up table which maps
* 		element indices to the range 0 - (n elements - 1). The size
* 		of the look up table is the maximum number of elements
* 		allocated in the mesh.
* \param	mesh			The mesh.
* \param	dstNElm			Destination pointer for the number of
* 					elements in the look up table.
*/
int		*WlzCMeshMakeElmIdxTbl2D5(WlzCMesh2D5 *mesh, int *dstNElm)
{
  int		nElm = 0;
  int		*idxTb = NULL;

  if((idxTb = AlcMalloc(sizeof(int) * mesh->res.elm.maxEnt)) != NULL) 
  {
    nElm = WlzCMeshSetElmIdxTbl2D5(mesh, idxTb);
  }
  if(dstNElm)
  {
    *dstNElm = nElm;
  }
  return(idxTb);
}

/*!
* \return	New element index look up table or NULL on error.
* \ingroup	WlzMesh
* \brief	Allocates and populates an element look up table which maps
* 		element indices to the range 0 - (n elements - 1). The size
* 		of the look up table is the maximum number of elements
* 		allocated in the mesh.
* \param	mesh			The mesh.
* \param	dstNElm			Destination pointer for the number of
* 					elements in the look up table.
*/
int		*WlzCMeshMakeElmIdxTbl3D(WlzCMesh3D *mesh, int *dstNElm)
{
  int		nElm = 0;
  int		*idxTb = NULL;

  if((idxTb = AlcMalloc(sizeof(int) * mesh->res.elm.maxEnt)) != NULL) 
  {
    nElm = WlzCMeshSetElmIdxTbl3D(mesh, idxTb);
  }
  if(dstNElm)
  {
    *dstNElm = nElm;
  }
  return(idxTb);
}

/*!
* \return	Number of elements in the look up table.
* \ingroup	WlzMesh
* \brief	Populates a element look up table which maps ellement indices
* 		to the range 0 - (n elements - 1). The size of the look up
* 		table must be at least the maximum number of elements
* 		allocated in the mesh.
* \param	mesh			The mesh.
* \param	idxTb			The allocated index table to be
* 					populated.
*/
int		WlzCMeshSetElmIdxTbl2D(WlzCMesh2D *mesh, int *idxTb)
{
  int		idE,
  		nElm = 0;

  for(idE = 0; idE < mesh->res.elm.maxEnt; ++idE)
  {
    WlzCMeshElm2D *elm;

    elm = (WlzCMeshElm2D *)AlcVectorItemGet(mesh->res.elm.vec, idE);
    if(elm->idx >= 0)
    {
      idxTb[idE] = nElm++;
    }
  }
  return(nElm);
}

/*!
* \return	Number of elements in the look up table.
* \ingroup	WlzMesh
* \brief	Populates a element look up table which maps ellement indices
* 		to the range 0 - (n elements - 1). The size of the look up
* 		table must be at least the maximum number of elements
* 		allocated in the mesh.
* \param	mesh			The mesh.
* \param	idxTb			The allocated index table to be
* 					populated.
*/
int		WlzCMeshSetElmIdxTbl2D5(WlzCMesh2D5 *mesh, int *idxTb)
{
  int		idE,
  		nElm = 0;

  for(idE = 0; idE < mesh->res.elm.maxEnt; ++idE)
  {
    WlzCMeshElm2D5 *elm;

    elm = (WlzCMeshElm2D5 *)AlcVectorItemGet(mesh->res.elm.vec, idE);
    if(elm->idx >= 0)
    {
      idxTb[idE] = nElm++;
    }
  }
  return(nElm);
}

/*!
* \return	Number of elements in the look up table.
* \ingroup	WlzMesh
* \brief	Populates a element look up table which maps ellement indices
* 		to the range 0 - (n elements - 1). The size of the look up
* 		table must be at least the maximum number of elements
* 		allocated in the mesh.
* \param	mesh			The mesh.
* \param	idxTb			The allocated index table to be
* 					populated.
*/
int		WlzCMeshSetElmIdxTbl3D(WlzCMesh3D *mesh, int *idxTb)
{
  int		idE,
  		nElm = 0;

  for(idE = 0; idE < mesh->res.elm.maxEnt; ++idE)
  {
    WlzCMeshElm3D *elm;

    elm = (WlzCMeshElm3D *)AlcVectorItemGet(mesh->res.elm.vec, idE);
    if(elm->idx >= 0)
    {
      idxTb[idE] = nElm++;
    }
  }
  return(nElm);
}

/*!
* \return	Number of nodes directly connected to the given
* 		node (including itself) or zero if the node is
* 		invalid.
* \ingroup	WlzMesh
* \brief	Computes the number of nodes directly connected to the given
* 		node.
* \param	nod			Given node.
*/
int		WlzCMeshNodNbrCnt2D5(WlzCMeshNod2D5 *nod)
{
  int		cnt = 0;
  WlzCMeshEdgU2D5 *edu;

  if(nod && (nod->idx >= 0) && ((edu = nod->edu) != NULL))
  {
    cnt = 1;
    do
    {
      ++cnt;
      edu = edu->nnxt;
    }
    while(edu != nod->edu);
  }
  return(cnt);
}

/*!
* \return	Number of nodes in the ring plus one for the given node or
* 		zero on error.
* \ingroup	WlzMesh
* \brief	Gathers the indices of the nodes that form a ring or
* 		partial rings around the given node. Where members of the
*		ring are the immediate neighbours of the given node. The
*		given node's index will always be the first in the
*		buffer but the remainder of the node indices may be
*		unsorted.
* \param	nod			Given node which must be valid.
* \param	maxIdxBuf		Pointer to the current maximum number
* 					of indices possible in the current
* 					buffer. This must be valid and may
* 					be modified on return.
* \param	idxBuf			Pointer to the current node index
* 					buffer. This must either be valid
* 					or a pointer to a NULL array. The
* 					array pointer may be modified on
* 					return.
* \param	dstErr			Destination error pointer, may be NULL.
*/
int		WlzCMeshNodRingNodIndices2D(WlzCMeshNod2D *nod,
					     int *maxIdxBuf,
					     int **idxBuf,
				             WlzErrorNum *dstErr)
{
  int		nN;
  WlzCMeshEdgU2D *edu0,
  		*edu1;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  /* Count the number of edge uses directed away from this node plus one. */
  nN = 1;
  edu1 = edu0 = nod->edu;
  do
  {
    if((edu1->opp == NULL) || (edu1->opp == edu1))
    {
      nN += 2;
    }
    else
    {
      nN += 1;
    }
    edu1 = edu1->nnxt;
  } while((edu1 != NULL) && (edu1 != edu0));
  /* Re-allocate the index buffer if required. */
  if((*idxBuf == NULL) || (*maxIdxBuf < nN))
  {
    *maxIdxBuf = 2 * nN;
    if((*idxBuf = (int *)
                  AlcRealloc(*idxBuf, sizeof(int) * *maxIdxBuf)) == NULL)
    {
      nN = 0;
      *maxIdxBuf = 0;
      errNum = WLZ_ERR_MEM_ALLOC;
    }
  }
  /* Fill the buffer with the indices of the nodes on the opposite ends
   * of all edge uses directed away from the node. */
  if(errNum == WLZ_ERR_NONE)
  {
    int idN;

    idN = 1;
    edu1 = edu0;
    (*idxBuf)[0] = nod->idx;
    do
    {
      if((edu1->opp == NULL) || (edu1->opp == edu1))
      {
        (*idxBuf)[idN++] = edu1->next->nod->idx;
      }
      (*idxBuf)[idN++] = edu1->next->next->nod->idx;
      edu1 = edu1->nnxt;
    } while((edu1 != NULL) && (edu1 != edu0));
  }
  if(errNum != WLZ_ERR_NONE)
  {
    nN = 0;
  }
  if(dstErr != NULL)
  {
    *dstErr = errNum;
  }
  return(nN);
}

/*!
* \return	Number of nodes in the ring plus one for the given node or
* 		zero on error.
* \ingroup	WlzMesh
* \brief	Gathers the indices of the nodes that form a ring or
* 		partial rings around the given node. Where members of the
*		ring are the immediate neighbours of the given node. The
*		given node's index will always be the first in the
*		buffer but the remainder of the node indices may be
*		unsorted.
* \param	nod			Given node which must be valid.
* \param	maxIdxBuf		Pointer to the current maximum number
* 					of indices possible in the current
* 					buffer. This must be valid and may
* 					be modified on return.
* \param	idxBuf			Pointer to the current node index
* 					buffer. This must either be valid
* 					or a pointer to a NULL array. The
* 					array pointer may be modified on
* 					return.
* \param	dstErr			Destination error pointer, may be NULL.
*/
int		WlzCMeshNodRingNodIndices2D5(WlzCMeshNod2D5 *nod,
					     int *maxIdxBuf,
					     int **idxBuf,
				             WlzErrorNum *dstErr)
{
  int		nN;
  WlzCMeshEdgU2D5 *edu0,
  		*edu1;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  /* Count the number of edge uses directed away from this node plus one. */
  nN = 1;
  edu1 = edu0 = nod->edu;
  do
  {
    if((edu1->opp == NULL) || (edu1->opp == edu1))
    {
      nN += 2;
    }
    else
    {
      nN += 1;
    }
    edu1 = edu1->nnxt;
  } while((edu1 != NULL) && (edu1 != edu0));
  /* Re-allocate the index buffer if required. */
  if((*idxBuf == NULL) || (*maxIdxBuf < nN))
  {
    *maxIdxBuf = 2 * nN;
    if((*idxBuf = (int *)
                  AlcRealloc(*idxBuf, sizeof(int) * *maxIdxBuf)) == NULL)
    {
      nN = 0;
      *maxIdxBuf = 0;
      errNum = WLZ_ERR_MEM_ALLOC;
    }
  }
  /* Fill the buffer with the indices of the nodes on the opposite ends
   * of all edge uses directed away from the node. */
  if(errNum == WLZ_ERR_NONE)
  {
    int idN;

    idN = 1;
    edu1 = edu0;
    (*idxBuf)[0] = nod->idx;
    do
    {
      if((edu1->opp == NULL) || (edu1->opp == edu1))
      {
        (*idxBuf)[idN++] = edu1->next->nod->idx;
      }
      (*idxBuf)[idN++] = edu1->next->next->nod->idx;
      edu1 = edu1->nnxt;
    } while((edu1 != NULL) && (edu1 != edu0));
  }
  if(errNum != WLZ_ERR_NONE)
  {
    nN = 0;
  }
  if(dstErr != NULL)
  {
    *dstErr = errNum;
  }
  return(nN);
}

/*!
* \return	Number of nodes in the ring plus one for the given node or
* 		zero on error.
* \ingroup	WlzMesh
* \brief	Gathers the indices of the nodes that form a ring or
* 		partial rings around the given node. Where members of the
*		ring are the immediate neighbours of the given node. The
*		given node's index will always be the first in the
*		buffer but the remainder of the node indices may be
*		unsorted.
* \param	nod			Given node which must be valid.
* \param	maxIdxBuf		Pointer to the current maximum number
* 					of indices possible in the current
* 					buffer. This must be valid and may
* 					be modified on return.
* \param	idxBuf			Pointer to the current node index
* 					buffer. This must either be valid
* 					or a pointer to a NULL array. The
* 					array pointer may be modified on
* 					return.
* \param	dstErr			Destination error pointer, may be NULL.
*/
int		WlzCMeshNodRingNodIndices3D(WlzCMeshNod3D *nod,
					     int *maxIdxBuf,
					     int **idxBuf,
				             WlzErrorNum *dstErr)
{
  int		nN;
  WlzCMeshEdgU3D *edu0,
  		*edu1;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  /* Count the number of edge uses directed away from this node plus one. */
  nN = 1;
  edu1 = edu0 = nod->edu;
  do
  {
    ++nN;
    edu1 = edu1->nnxt;
  } while((edu1 != NULL) && (edu1 != edu0));
  /* Re-allocate the index buffer if required. */
  if((*idxBuf == NULL) || (*maxIdxBuf < nN))
  {
    *maxIdxBuf = 2 * nN;
    if((*idxBuf = (int *)
                  AlcRealloc(*idxBuf, sizeof(int) * *maxIdxBuf)) == NULL)
    {
      nN = 0;
      *maxIdxBuf = 0;
      errNum = WLZ_ERR_MEM_ALLOC;
    }
  }
  /* Fill the buffer with the indices of the nodes on the opposite ends
   * of all edge uses directed away from the node. */
  if(errNum == WLZ_ERR_NONE)
  {
    int idN0;

    idN0 = 1;
    edu1 = edu0;
    (*idxBuf)[0] = nod->idx;
    do
    {
      int idN1,
      	  idN2;

      idN1 = edu1->next->next->nod->idx;
      for(idN2 = 0; idN2 < idN0; ++idN2)
      {
        if(idN1 == (*idxBuf)[idN2])
	{
	  break;
	}
      }
      if(idN0 == idN2)
      {
        (*idxBuf)[idN0++] = idN1;
      }
      edu1 = edu1->nnxt;
    } while((edu1 != NULL) && (edu1 != edu0));
    nN = idN0;
  }
  if(errNum != WLZ_ERR_NONE)
  {
    nN = 0;
  }
  if(dstErr != NULL)
  {
    *dstErr = errNum;
  }
  return(nN);
}

/*!
* \return	Number of elements in the ring or zero on error.
* \ingroup	WlzMesh
* \brief	Gathers the indices of the elements that form a ring or
* 		partial rings around the given node. Where members of the
*		ring are immediate neighbours of the given node. The
*		element indices may be unsorted.
* \param	nod			Given node which must be valid.
* \param	maxIdxBuf		Pointer to the current maximum number
* 					of indices possible in the current
* 					buffer. This must be valid and may
* 					be modified on return.
* \param	idxBuf			Pointer to the current node index
* 					buffer. This must either be valid
* 					or a pointer to a NULL array. The
* 					array pointer may be modified on
* 					return.
* \param	dstErr			Destination error pointer, may be NULL.
*/
int		WlzCMeshNodRingElmIndices2D5(WlzCMeshNod2D5 *nod,
					     int *maxIdxBuf,
					     int **idxBuf,
				             WlzErrorNum *dstErr)
{
  int		nE;
  WlzCMeshEdgU2D5 *edu0,
  		*edu1;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  /* Count the number of edge uses directed away from this node, there
   * will be one per element. */
  nE = 0;
  edu1 = edu0 = nod->edu;
  do
  {
    ++nE;
    edu1 = edu1->nnxt;
  } while((edu1 != NULL) && (edu1 != edu0));
  /* Re-allocate the index buffer if required. */
  if((*idxBuf == NULL) || (*maxIdxBuf < nE))
  {
    *maxIdxBuf = 2 * nE;
    if((*idxBuf = (int *)
                  AlcRealloc(*idxBuf, sizeof(int) * *maxIdxBuf)) == NULL)
    {
      nE = 0;
      *maxIdxBuf = 0;
      errNum = WLZ_ERR_MEM_ALLOC;
    }
  }
  /* Fill the buffer with the indices of the elements that use the node. */
  if(errNum == WLZ_ERR_NONE)
  {
    int idN;

    idN = 0;
    edu1 = edu0;
    (*idxBuf)[0] = nod->idx;
    for(idN = 1; idN < nE; ++idN)
    {
      (*idxBuf)[idN] = edu1->elm->idx;
      edu1 = edu1->nnxt;
    }
  }
  if(errNum != WLZ_ERR_NONE)
  {
    nE = 0;
  }
  if(dstErr != NULL)
  {
    *dstErr = errNum;
  }
  return(nE);
}

/*!
* \return	Number of nodes in the ring or zero on error.
* \ingroup	WlzMesh
* \brief	Gathers the indices of the nodes that form a ring or
* 		partial rings around the given element. Where members
* 		of the ring are the nodes of the element followed by
* 		the immediate edge neighbours of these nodes. The
*		node indices may be unsorted (apart from the element
*		nodes being first and in edge use order) but they will
*		not have duplicates.
* \param	elm			Given element which must be valid.
* \param	maxIdxBuf		Pointer to the current maximum number
* 					of indices possible in the current
* 					buffer. This must be valid and may
* 					be modified on return.
* \param	idxBuf			Pointer to the current node index
* 					buffer. This must either be valid
* 					or a pointer to a NULL array. The
* 					array pointer may be modified on
* 					return.
* \param	dstErr			Destination error pointer, may be NULL.
*/
int		WlzCMeshElmRingNodIndices2D(WlzCMeshElm2D *elm, int *maxIdxBuf,
					    int **idxBuf, WlzErrorNum *dstErr)
{
  int		idE,
  		idN,
  		nN;
  WlzCMeshEdgU2D *edu0,
  		*edu1;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  /* Count the number of edge uses directed away from this node, there
   * will be one per element. */
  nN = 3;
  for(idE = 0; idE < 3; ++idE)
  {
    edu1 = edu0 = elm->edu[idE].nod->edu;
    do
    {
      ++nN;
      edu1 = edu1->nnxt;
    } while((edu1 != NULL) && (edu1 != edu0));
  }
  /* Re-allocate the index buffer if required. */
  if((*idxBuf == NULL) || (*maxIdxBuf < nN))
  {
    *maxIdxBuf = 2 * nN;
    if((*idxBuf = (int *)
                  AlcRealloc(*idxBuf, sizeof(int) * *maxIdxBuf)) == NULL)
    {
      nN = 0;
      *maxIdxBuf = 0;
      errNum = WLZ_ERR_MEM_ALLOC;
    }
  }
  /* Fill the buffer with the indices of the elements that use the node. */
  if(errNum == WLZ_ERR_NONE)
  {
    int		*buf;

    buf = *idxBuf;
    for(idE = 0; idE < 3; ++idE)
    {
      buf[idE] = elm->edu[idE].nod->idx;
    }
    nN = 3;
    for(idE = 0; idE < 3; ++idE)
    {
      edu1 = edu0 = elm->edu[idE].nod->edu;
      do
      {
        int	nIdx;

	nIdx = edu1->next->nod->idx;
	for(idN = 0; idN < nN; ++idN)
	{
	  if(nIdx == buf[idN])
	  {
	    nIdx = -1;
	    break;
	  }
	}
	if(nIdx >= 0)
	{
	  buf[nN++] = nIdx;
	}
	nIdx = edu1->next->next->nod->idx;
	for(idN = 0; idN < nN; ++idN)
	{
	  if(nIdx == buf[idN])
	  {
	    nIdx = -1;
	    break;
	  }
	}
	if(nIdx >= 0)
	{
	  buf[nN++] = nIdx;
	}
        edu1 = edu1->nnxt;
      } while((edu1 != NULL) && (edu1 != edu0));
    }
  }
  if(errNum != WLZ_ERR_NONE)
  {
    nN = 0;
  }
  if(dstErr != NULL)
  {
    *dstErr = errNum;
  }
  return(nN);
}

/*!
* \return	Number of elements in the ring or zero on error.
* \ingroup	WlzMesh
* \brief	Gathers the indices of the elements that form a ring
* 		around (ie are directly connected to) the given element.
* 		The element indices will be partially ordered with the
* 		given element's index first, followed by edge connected
* 		neighbouring elements and then the remaining node only
* 		connected elements. There will not be duplicates.
* \param	elm			Given element which must be valid.
* \param	maxIdxBuf		Pointer to the current maximum number
* 					of indices possible in the current
* 					buffer. This must be valid and may
* 					be modified on return.
* \param	idxBuf			Pointer to the current element index
* 					buffer. This must either be valid
* 					or a pointer to a NULL array. The
* 					array pointer may be modified on
* 					return.
* \param	dstErr			Destination error pointer, may be NULL.
*/
int		WlzCMeshElmRingElmIndices2D(WlzCMeshElm2D *elm, int *maxIdxBuf,
					    int **idxBuf, WlzErrorNum *dstErr)
{
  int		idE,
  		nE;
  WlzCMeshEdgU2D *edu0,
  		*edu1;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  /* Count the number of edge uses directed away from each node of the
   * given element, there will be one per element, but elements may be
   * counted several times. */
  nE = 0;
  for(idE = 0; idE < 3; ++idE)
  {
    edu1 = edu0 = elm->edu[idE].nod->edu;
    do
    {
      ++nE;
      edu1 = edu1->nnxt;
    } while((edu1 != NULL) && (edu1 != edu0));
  }
  /* Re-allocate the index buffer if required. */
  if((*idxBuf == NULL) || (*maxIdxBuf < nE))
  {
    *maxIdxBuf = 2 * nE;
    if((*idxBuf = (int *)
                  AlcRealloc(*idxBuf, sizeof(int) * *maxIdxBuf)) == NULL)
    {
      nE = 0;
      *maxIdxBuf = 0;
      errNum = WLZ_ERR_MEM_ALLOC;
    }
  }
  /* Fill the buffer with the indices of the elements that use the nodes. */
  if(errNum == WLZ_ERR_NONE)
  {
    int		*buf;

    /* First the given element. */
    nE = 1;
    buf = *idxBuf;
    buf[0] = elm->idx;
    /* Then edge connected neighbouring elements. */
    for(idE = 0; idE < 3; ++idE)
    {
      edu0 = elm->edu[idE].opp;
      if((edu0 != NULL) && (edu0 != edu0->opp))
      {
        buf[nE++] = edu0->elm->idx;
      }
    }
    /* Finally the remaining node connected elements. */
    for(idE = 0; idE < 3; ++idE)
    {
      edu1 = edu0 = elm->edu[idE].nod->edu;
      do
      {
        int	idB,
		eIdx;

	eIdx = edu1->elm->idx;
	for(idB = 0; idB < nE; ++idB)
	{
	  if(eIdx == buf[idB])
	  {
	    eIdx = -1;
	    break;
	  }
	}
	if(eIdx >= 0)
	{
	  buf[nE++] = eIdx;
	}
        edu1 = edu1->nnxt;
      } while((edu1 != NULL) && (edu1 != edu0));
    }
  }
  if(errNum != WLZ_ERR_NONE)
  {
    nE = 0;
  }
  if(dstErr != NULL)
  {
    *dstErr = errNum;
  }
  return(nE);
}

/*!
* \return	Number of elements in the ring or zero on error.
* \ingroup	WlzMesh
* \brief	Gathers the indices of the elements that form a ring
* 		around (ie are directly connected to) the given element.
* 		The element indices will be partially ordered with the
* 		given element's index first, followed by face connected
* 		neighbouring elements and then the remaining node or
* 		edge only connected elements. There will not be duplicates.
* \param	elm			Given element which must be valid.
* \param	maxIdxBuf		Pointer to the current maximum number
* 					of indices possible in the current
* 					buffer. This must be valid and may
* 					be modified on return.
* \param	idxBuf			Pointer to the current element index
* 					buffer. This must either be valid
* 					or a pointer to a NULL array. The
* 					array pointer may be modified on
* 					return.
* \param	dstErr			Destination error pointer, may be NULL.
*/
int		WlzCMeshElmRingElmIndices3D(WlzCMeshElm3D *elm, int *maxIdxBuf,
					    int **idxBuf, WlzErrorNum *dstErr)
{
  int		idE,
  		nE;
  WlzCMeshEdgU3D *edu0,
  		*edu1;
  WlzCMeshNod3D *nodes[4];
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  /* Count the number of edge uses directed away from each node of the
   * given element, there will be one per face and so three per element,
   * but elements may be counted several times. */
  nE = 0;
  nodes[0] = WLZ_CMESH_ELM3D_GET_NODE_0(elm);
  nodes[1] = WLZ_CMESH_ELM3D_GET_NODE_1(elm);
  nodes[2] = WLZ_CMESH_ELM3D_GET_NODE_2(elm);
  nodes[3] = WLZ_CMESH_ELM3D_GET_NODE_3(elm);
  for(idE = 0; idE < 4; ++idE)
  {
    edu1 = edu0 = nodes[idE]->edu;
    do
    {
      ++nE;
      edu1 = edu1->nnxt;
    } while((edu1 != NULL) && (edu1 != edu0));
  }
  nE /= 3;
  /* Re-allocate the index buffer if required. */
  if((*idxBuf == NULL) || (*maxIdxBuf < nE))
  {
    *maxIdxBuf = 2 * nE;
    if((*idxBuf = (int *)
                  AlcRealloc(*idxBuf, sizeof(int) * *maxIdxBuf)) == NULL)
    {
      nE = 0;
      *maxIdxBuf = 0;
      errNum = WLZ_ERR_MEM_ALLOC;
    }
  }
  /* Fill the buffer with the indices of the elements that use the nodes. */
  if(errNum == WLZ_ERR_NONE)
  {
    int		*buf;

    /* First the given element. */
    nE = 1;
    buf = *idxBuf;
    buf[0] = elm->idx;
    /* Then face connected neighbouring elements. */
    for(idE = 0; idE < 4; ++idE)
    {
      WlzCMeshFace *fce;

      fce = elm->face[idE].opp;
      if((fce != NULL) && (fce != fce->opp))
      {
        buf[nE++] = fce->elm->idx;
      }
    }
    /* Finally the remaining node connected elements. */
    for(idE = 0; idE < 4; ++idE)
    {
      edu1 = edu0 = nodes[idE]->edu;
      do
      {
        int	idB,
		eIdx;

	eIdx = edu1->face->elm->idx;
	for(idB = 0; idB < nE; ++idB)
	{
	  if(eIdx == buf[idB])
	  {
	    eIdx = -1;
	    break;
	  }
	}
	if(eIdx >= 0)
	{
	  buf[nE++] = eIdx;
	}
        edu1 = edu1->nnxt;
      } while((edu1 != NULL) && (edu1 != edu0));
    }
  }
  if(errNum != WLZ_ERR_NONE)
  {
    nE = 0;
  }
  if(dstErr != NULL)
  {
    *dstErr = errNum;
  }
  return(nE);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzMesh
* \brief	Given a conforming mesh with attached values; this function
* 		normalises the values to the given range in place.
* 		If the zero value is not mapped to a given value explicitly
* 		then the the output values will be given using:
* 		\f[
 		V = \frac{U - L}{u - l} (v - l) + L 
		\f]
*		otherwise with the zero value explicitly mapped the values
*		will be
*		\f[
		\begin{array}{l}
		f = \min{\frac{U - V_0}{u}, \frac{L - V_0}{l}} \\
 		V = f v + V_0
		\end{array}
		\f]
		Here \f$U\f$ and \f$L\f$ are the required maximum and minimum
		values on return, \f$u\f$ and \f$l\f$ are the given mesh
		maximum and minimum values, \f$v\f$ is a given value and
		\f$V\f$ is a returned value. \f$V_0\f$ is the returned value
		corresponding to the given value of zero.
* \param	cObj			Conforming mesh object with values
* 					to be normalised attached..
* \param	mapZero			Map the value of zero to the zero
* 					value if non zero.
* \param	vZero			Value for zero if mapZero is non-zero.
* \param	vMin			Minimum value on return.
* \param	vMax			Maximum value on return.
* \param	oFrac			Outlier fraction. Assumes that this
* 					fraction of the values are extremal
* 					outliers and normalises to the range
* 					of mesh values minus this fraction at
* 					both ends of the value range.
*/
WlzErrorNum	WlzCMeshValuesNormalise(WlzObject *cObj,
					int mapZero,
				        double vZero,
					double vMin, double vMax,
					double oFrac)
{
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(cObj == NULL)
  {
    errNum = WLZ_ERR_OBJECT_NULL;
  }
  else if(cObj->domain.core == NULL)
  {
    errNum = WLZ_ERR_DOMAIN_NULL;
  }
  else if(cObj->values.core == NULL)
  {
    errNum = WLZ_ERR_VALUES_NULL;
  }
  else
  {
    switch(cObj->type)
    {
      case WLZ_CMESH_2D:
	errNum = WlzCMeshValuesNormalise2D(cObj, mapZero, vZero, vMin, vMax,
					   oFrac);
        break;
      case WLZ_CMESH_3D:
	errNum = WlzCMeshValuesNormalise3D(cObj, mapZero, vZero, vMin, vMax,
					   oFrac);
        break;
      default:
	errNum = WLZ_ERR_OBJECT_TYPE;
        break;
    }
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzMesh
* \brief	Given a 2D conforming mesh with attached values; this function
* 		normalises the values to the given range in place.
* 		See WlzCMeshValuesNormalise().
* \param	cObj			Conforming mesh object with values
* 					to be normalised attached..
* \param	mapZero			Map the value of zero to the zero
* 					value if non zero.
* \param	vZero			Value for zero if mapZero is non-zero.
* \param	vMin			Minimum value on return.
* \param	vMax			Maximum value on return.
* \param	oFrac			Outlier fraction. Assumes that this
* 					fraction of the values are extremal
* 					outliers and normalises to the range
* 					of mesh values minus this fraction at
* 					both ends of the value range.
* \param	dstErr
*/
WlzErrorNum	WlzCMeshValuesNormalise2D(WlzObject *cObj,
					  int mapZero,
					  double vZero,
					  double vMin, double vMax,
					  double oFrac)
{
  WlzErrorNum	errNum = WLZ_ERR_UNIMPLEMENTED;

  return(errNum);
}


/*!
* \return	Woolz error code.
* \ingroup	WlzMesh
* \brief	Given a 3D conforming mesh with attached values; this function
* 		normalises the values to the given range in place.
* 		See WlzCMeshValuesNormalise().
* \param	cObj			Conforming mesh object with values
* 					to be normalised attached..
* \param	mapZero			Map the value of zero to the zero
* 					value if non zero.
* \param	vZero			Value for zero if mapZero is non-zero.
* \param	vMin			Minimum value on return.
* \param	vMax			Maximum value on return.
* \param	oFrac			Outlier fraction. Assumes that this
* 					fraction of the values are extremal
* 					outliers and normalises to the range
* 					of mesh values minus this fraction at
* 					both ends of the value range.
* \param	dstErr
*/
WlzErrorNum	WlzCMeshValuesNormalise3D(WlzObject *cObj,
					  int mapZero,
					  double vZero,
					  double vMin, double vMax,
					  double oFrac)
{
  int		nVal = 0;
  double	g0,
  		g1,
		grd,
  		off0,
		off1,
		uMin = 0.0,
  		uMax = 0.0;
  WlzCMesh3D	*mesh;
  WlzIndexedValues *ixv;
  WlzErrorNum	errNum = WLZ_ERR_NONE;
  const double	eps = 1.0e-06;

  if((mesh = cObj->domain.cm3)->type != WLZ_CMESH_3D)
  {
    errNum = WLZ_ERR_DOMAIN_TYPE;
  }
  else if((ixv = cObj->values.x)->type != WLZ_INDEXED_VALUES)
  {
    errNum = WLZ_ERR_VALUES_TYPE;
  }
  else
  {
    if((ixv->rank < 0) || ((ixv->rank > 0) && (ixv->dim[0] < 1)))
    {
      errNum = WLZ_ERR_VALUES_DATA;
    }
    else
    {
      int       idX;

      if(ixv->rank == 0)
      {
        nVal = 1;
      }
      else
      {
	nVal = ixv->dim[0];
	for(idX = 1; idX < ixv->rank; ++idX)
	{
	  if(ixv->dim[idX] < 1)
	  {
	    errNum = WLZ_ERR_VALUES_DATA;
	    break;
	  }
	  nVal *= ixv->dim[idX];
	}
      }
    }
  }
  if((errNum == WLZ_ERR_NONE) && (nVal <= 0))
  {
    errNum = WLZ_ERR_VALUES_DATA;
  }
  if(errNum == WLZ_ERR_NONE)
  {
    switch(ixv->vType)
    {
      case WLZ_GREY_LONG:  /* FALLTHROUGH */
      case WLZ_GREY_INT:   /* FALLTHROUGH */
      case WLZ_GREY_SHORT: /* FALLTHROUGH */
      case WLZ_GREY_UBYTE: /* FALLTHROUGH */
      case WLZ_GREY_FLOAT: /* FALLTHROUGH */
      case WLZ_GREY_DOUBLE:
	break;
      default:
	errNum = WLZ_ERR_VALUES_DATA;
	break;
      
    }
  }
  if(errNum == WLZ_ERR_NONE)
  {
    switch(ixv->attach)
    {
      case WLZ_VALUE_ATTACH_NOD: /* FALLTHROUGH */
      case WLZ_VALUE_ATTACH_ELM:
	break;
      default:
	errNum = WLZ_ERR_VALUES_DATA;
	break;
    }
  }
  if(errNum == WLZ_ERR_NONE)
  {
    int		idN,
	      	firstVal = 1;

    switch(ixv->attach)
    {

      case WLZ_VALUE_ATTACH_NOD:
        for(idN = 0; idN < mesh->res.nod.maxEnt; ++idN)
	{
	  WlzCMeshNod3D *nod;

	  nod = (WlzCMeshNod3D *)AlcVectorItemGet(mesh->res.nod.vec, idN);
	  if(nod->idx >= 0)
	  {
	    int		idX;
	    WlzGreyP	v;

	    v.v = WlzIndexedValueGet(ixv, nod->idx);
	    switch(ixv->vType)
	    {
  	      case WLZ_GREY_LONG:
		idX = 0;
		if(firstVal)
		{
		  idX = 1;
		  firstVal = 0;
		  uMin = uMax = v.lnp[0];
		}
	        while(idX < nVal)
		{
		  if(v.lnp[idX] < uMin)
		  {
		    uMin = v.lnp[idX];
		  }
		  else if(v.lnp[idX] > uMax)
		  {
		    uMax = v.lnp[idX];
		  }
		  ++idX;
		}
		break;
  	      case WLZ_GREY_INT:
		idX = 0;
		if(firstVal)
		{
		  idX = 1;
		  firstVal = 0;
		  uMin = uMax = v.inp[0];
		}
	        while(idX < nVal)
		{
		  if(v.inp[idX] < uMin)
		  {
		    uMin = v.inp[idX];
		  }
		  else if(v.inp[idX] > uMax)
		  {
		    uMax = v.inp[idX];
		  }
		  ++idX;
		}
		break;
  	      case WLZ_GREY_SHORT:
		idX = 0;
		if(firstVal)
		{
		  idX = 1;
		  firstVal = 0;
		  uMin = uMax = v.shp[0];
		}
	        while(idX < nVal)
		{
		  if(v.shp[idX] < uMin)
		  {
		    uMin = v.shp[idX];
		  }
		  else if(v.shp[idX] > uMax)
		  {
		    uMax = v.shp[idX];
		  }
		  ++idX;
		}
		break;
  	      case WLZ_GREY_UBYTE:
		idX = 0;
		if(firstVal)
		{
		  idX = 1;
		  firstVal = 0;
		  uMin = uMax = v.ubp[0];
		}
	        while(idX < nVal)
		{
		  if(v.ubp[idX] < uMin)
		  {
		    uMin = v.ubp[idX];
		  }
		  else if(v.ubp[idX] > uMax)
		  {
		    uMax = v.ubp[idX];
		  }
		  ++idX;
		}
		break;
  	      case WLZ_GREY_FLOAT:
		idX = 0;
		if(firstVal)
		{
		  idX = 1;
		  firstVal = 0;
		  uMin = uMax = v.flp[0];
		}
	        while(idX < nVal)
		{
		  if(v.flp[idX] < uMin)
		  {
		    uMin = v.flp[idX];
		  }
		  else if(v.flp[idX] > uMax)
		  {
		    uMax = v.flp[idX];
		  }
		  ++idX;
		}
		break;
  	      case WLZ_GREY_DOUBLE:
		idX = 0;
		if(firstVal)
		{
		  idX = 1;
		  firstVal = 0;
		  uMin = uMax = v.dbp[0];
		}
	        while(idX < nVal)
		{
		  if(v.dbp[idX] < uMin)
		  {
		    uMin = v.dbp[idX];
		  }
		  else if(v.dbp[idX] > uMax)
		  {
		    uMax = v.dbp[idX];
		  }
		  ++idX;
		}
		break;
	      default:
		break;
	    }
	  }
	}
	break;
      case WLZ_VALUE_ATTACH_ELM:
        for(idN = 0; idN < mesh->res.elm.maxEnt; ++idN)
	{
	  WlzCMeshElm3D *elm;

	  elm = (WlzCMeshElm3D *)AlcVectorItemGet(mesh->res.elm.vec, idN);
	  if(elm->idx >= 0)
	  {
	    int		idX;
	    WlzGreyP	v;

	    v.v = WlzIndexedValueGet(ixv, elm->idx);
	    switch(ixv->vType)
	    {
  	      case WLZ_GREY_LONG:
		idX = 0;
		if(firstVal)
		{
		  idX = 1;
		  firstVal = 0;
		  uMin = uMax = v.lnp[0];
		}
	        while(idX < nVal)
		{
		  if(v.lnp[idX] < uMin)
		  {
		    uMin = v.lnp[idX];
		  }
		  else if(v.lnp[idX] > uMax)
		  {
		    uMax = v.lnp[idX];
		  }
		  ++idX;
		}
		break;
  	      case WLZ_GREY_INT:
		idX = 0;
		if(firstVal)
		{
		  idX = 1;
		  firstVal = 0;
		  uMin = uMax = v.inp[0];
		}
	        while(idX < nVal)
		{
		  if(v.inp[idX] < uMin)
		  {
		    uMin = v.inp[idX];
		  }
		  else if(v.inp[idX] > uMax)
		  {
		    uMax = v.inp[idX];
		  }
		  ++idX;
		}
		break;
  	      case WLZ_GREY_SHORT:
		idX = 0;
		if(firstVal)
		{
		  idX = 1;
		  firstVal = 0;
		  uMin = uMax = v.shp[0];
		}
	        while(idX < nVal)
		{
		  if(v.shp[idX] < uMin)
		  {
		    uMin = v.shp[idX];
		  }
		  else if(v.shp[idX] > uMax)
		  {
		    uMax = v.shp[idX];
		  }
		  ++idX;
		}
		break;
  	      case WLZ_GREY_UBYTE:
		idX = 0;
		if(firstVal)
		{
		  idX = 1;
		  firstVal = 0;
		  uMin = uMax = v.ubp[0];
		}
	        while(idX < nVal)
		{
		  if(v.ubp[idX] < uMin)
		  {
		    uMin = v.ubp[idX];
		  }
		  else if(v.ubp[idX] > uMax)
		  {
		    uMax = v.ubp[idX];
		  }
		  ++idX;
		}
		break;
  	      case WLZ_GREY_FLOAT:
		idX = 0;
		if(firstVal)
		{
		  idX = 1;
		  firstVal = 0;
		  uMin = uMax = v.flp[0];
		}
	        while(idX < nVal)
		{
		  if(v.flp[idX] < uMin)
		  {
		    uMin = v.flp[idX];
		  }
		  else if(v.flp[idX] > uMax)
		  {
		    uMax = v.flp[idX];
		  }
		  ++idX;
		}
		break;
  	      case WLZ_GREY_DOUBLE:
		idX = 0;
		if(firstVal)
		{
		  idX = 1;
		  firstVal = 0;
		  uMin = uMax = v.dbp[0];
		}
	        while(idX < nVal)
		{
		  if(v.dbp[idX] < uMin)
		  {
		    uMin = v.dbp[idX];
		  }
		  else if(v.dbp[idX] > uMax)
		  {
		    uMax = v.dbp[idX];
		  }
		  ++idX;
		}
		break;
	      default:
		break;
	    }
	  }
	}
	break;
      default:
	break;
    }
    if(fabs(oFrac) > eps)
    {
      g0 = (uMax - uMin) * oFrac;
      uMin += g0;
      uMax -= g0;
      if(uMax - uMin < eps)
      {
        g0 = (uMax + uMin) * 0.5;
	uMin = uMax = g0;
      }
    }
    if(mapZero)
    {
      g0 = (fabs(uMax) > eps)? (vMax - vZero) / uMax: DBL_MAX;
      g1 = (fabs(uMin) > eps)? (vZero - vMin) / uMin: DBL_MAX;
      grd = ALG_MIN(g0, g1);
      off0 = 0.0;
      off1 = vZero;
    }
    else
    {
      if((g0 = uMax - uMin) < eps)
      {
        g0 = 1.0;
      }
      grd = (vMax - vMin) / g0;
      off0 = uMin;
      off1 = vMin;
    }
    switch(ixv->attach)
    {
      int	idN;

      case WLZ_VALUE_ATTACH_NOD:
        for(idN = 0; idN < mesh->res.nod.maxEnt; ++idN)
	{
	  WlzCMeshNod3D *nod;

	  nod = (WlzCMeshNod3D *)AlcVectorItemGet(mesh->res.nod.vec, idN);
	  if(nod->idx >= 0)
	  {
	    int		idX;
	    WlzGreyP	v;

	    v.v = WlzIndexedValueGet(ixv, nod->idx);
	    switch(ixv->vType)
	    {
  	      case WLZ_GREY_LONG:
		for(idX = 0; idX < nVal; ++idX)
		{
		  v.lnp[idX] = grd * (v.lnp[idX] - off0) + off1;
		  if(v.lnp[idX] < vMin)
		  {
		    v.lnp[idX] = vMin;
		  }
		  else if(v.lnp[idX] > vMax)
		  {
		    v.lnp[idX] = vMax;
		  }
		}
		break;
  	      case WLZ_GREY_INT:
		for(idX = 0; idX < nVal; ++idX)
		{
		  v.inp[idX] = grd * (v.inp[idX] - off0) + off1;
		  if(v.inp[idX] < vMin)
		  {
		    v.inp[idX] = vMin;
		  }
		  else if(v.inp[idX] > vMax)
		  {
		    v.inp[idX] = vMax;
		  }
		}
		break;
  	      case WLZ_GREY_SHORT:
		for(idX = 0; idX < nVal; ++idX)
		{
		  v.shp[idX] = grd * (v.shp[idX] - off0) + off1;
		  if(v.shp[idX] < vMin)
		  {
		    v.shp[idX] = vMin;
		  }
		  else if(v.shp[idX] > vMax)
		  {
		    v.shp[idX] = vMax;
		  }
		}
		break;
  	      case WLZ_GREY_UBYTE:
		for(idX = 0; idX < nVal; ++idX)
		{
		  v.ubp[idX] = grd * (v.ubp[idX] - off0) + off1;
		  if(v.ubp[idX] < vMin)
		  {
		    v.ubp[idX] = vMin;
		  }
		  else if(v.ubp[idX] > vMax)
		  {
		    v.ubp[idX] = vMax;
		  }
		}
		break;
  	      case WLZ_GREY_FLOAT:
		for(idX = 0; idX < nVal; ++idX)
		{
		  v.flp[idX] = grd * (v.flp[idX] - off0) + off1;
		  if(v.flp[idX] < vMin)
		  {
		    v.flp[idX] = vMin;
		  }
		  else if(v.flp[idX] > vMax)
		  {
		    v.flp[idX] = vMax;
		  }
		}
		break;
  	      case WLZ_GREY_DOUBLE:
		for(idX = 0; idX < nVal; ++idX)
		{
		  v.dbp[idX] = grd * (v.dbp[idX] - off0) + off1;
		  if(v.dbp[idX] < vMin)
		  {
		    v.dbp[idX] = vMin;
		  }
		  else if(v.dbp[idX] > vMax)
		  {
		    v.dbp[idX] = vMax;
		  }
		}
		break;
	      default:
		break;
	    }
	  }
	}
	break;
      case WLZ_VALUE_ATTACH_ELM:
        for(idN = 0; idN < mesh->res.elm.maxEnt; ++idN)
	{
	  WlzCMeshElm3D *elm;

	  elm = (WlzCMeshElm3D *)AlcVectorItemGet(mesh->res.elm.vec, idN);
	  if(elm->idx >= 0)
	  {
	    int		idX;
	    WlzGreyP	v;

	    v.v = WlzIndexedValueGet(ixv, elm->idx);
	    switch(ixv->vType)
	    {
  	      case WLZ_GREY_LONG:
		for(idX = 0; idX < nVal; ++idX)
		{
		  v.lnp[idX] = grd * (v.lnp[idX] - off0) + off1;
		  if(v.lnp[idX] < vMin)
		  {
		    v.lnp[idX] = vMin;
		  }
		  else if(v.lnp[idX] > vMax)
		  {
		    v.lnp[idX] = vMax;
		  }
		}
		break;
  	      case WLZ_GREY_INT:
		for(idX = 0; idX < nVal; ++idX)
		{
		  v.inp[idX] = grd * (v.inp[idX] - off0) + off1;
		  if(v.inp[idX] < vMin)
		  {
		    v.inp[idX] = vMin;
		  }
		  else if(v.inp[idX] > vMax)
		  {
		    v.inp[idX] = vMax;
		  }
		}
		break;
  	      case WLZ_GREY_SHORT:
		for(idX = 0; idX < nVal; ++idX)
		{
		  v.shp[idX] = grd * (v.shp[idX] - off0) + off1;
		  if(v.shp[idX] < vMin)
		  {
		    v.shp[idX] = vMin;
		  }
		  else if(v.shp[idX] > vMax)
		  {
		    v.shp[idX] = vMax;
		  }
		}
		break;
  	      case WLZ_GREY_UBYTE:
		for(idX = 0; idX < nVal; ++idX)
		{
		  v.ubp[idX] = grd * (v.ubp[idX] - off0) + off1;
		  if(v.ubp[idX] < vMin)
		  {
		    v.ubp[idX] = vMin;
		  }
		  else if(v.ubp[idX] > vMax)
		  {
		    v.ubp[idX] = vMax;
		  }
		}
		break;
  	      case WLZ_GREY_FLOAT:
		for(idX = 0; idX < nVal; ++idX)
		{
		  v.flp[idX] = grd * (v.flp[idX] - off0) + off1;
		  if(v.flp[idX] < vMin)
		  {
		    v.flp[idX] = vMin;
		  }
		  else if(v.flp[idX] > vMax)
		  {
		    v.flp[idX] = vMax;
		  }
		}
		break;
  	      case WLZ_GREY_DOUBLE:
		for(idX = 0; idX < nVal; ++idX)
		{
		  v.dbp[idX] = grd * (v.dbp[idX] - off0) + off1;
		  if(v.dbp[idX] < vMin)
		  {
		    v.dbp[idX] = vMin;
		  }
		  else if(v.dbp[idX] > vMax)
		  {
		    v.dbp[idX] = vMax;
		  }
		}
		break;
	      default:
		break;
	    }
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
* \return	Array of bytes set to non zero value when range test
* 		is satisfied where the array indices correspond to
* 		the indices of the given objects values or NULL on error.
* 		The returned array should be freed using AlcFree().
* \ingroup	WlzMesh
* \brief	Allocates and array of byte values and then sets these
* 		to non-zero values for the conforming mesh value indices
* 		where the values satisfy the given range test.
* 		If either the lower or upper limit is NaN then the
* 		test is simply for the conforming meshes values being
* 		NaN.
* \param	obj			Given conforming mesh object
* 					with values.
* \param	lo			Lower limit value of range,
* 					may be NaN.
* \param	hi			Upper limit value of range,
* 					may be NaN.
* \param	in			If zero set bytes to non-zero when
* 					test is not satisfied, otherwise if
* 					zero set bytes to non-zero when
* 					test is satisfied.
* \param	dstErr			Destination error pointer, may be NULL.
*/
WlzUByte			*WlzCMeshIndexMaskFromValueRange(
				  WlzObject *obj,
				  double lo,
				  double hi,
				  int in,
				  WlzErrorNum *dstErr)
{
  size_t	maxIdx = 0;
  WlzUByte	*found = NULL;
  WlzIndexedValues *ixv;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(obj == NULL)
  {
    errNum = WLZ_ERR_OBJECT_NULL;
  }
  else if((obj->type != WLZ_CMESH_2D) && (obj->type != WLZ_CMESH_3D))
  {
    errNum = WLZ_ERR_OBJECT_TYPE;
  }
  else if(obj->domain.core == NULL)
  {
    errNum = WLZ_ERR_DOMAIN_NULL;
  }
  else if(obj->values.core == NULL)
  {
    errNum = WLZ_ERR_VALUES_NULL;
  }
  else if((obj->domain.core->type != WLZ_CMESH_2D) &&
          (obj->domain.core->type != WLZ_CMESH_3D))
  {
    errNum = WLZ_ERR_DOMAIN_TYPE;
  }
  else if((ixv = obj->values.x)->type != WLZ_INDEXED_VALUES)
  {
    errNum = WLZ_ERR_VALUES_TYPE;
  }
  else if(ixv->rank != 0)
  {
    /* Only allow scalar indexed values. */
    errNum = WLZ_ERR_VALUES_DATA;
  }
  else
  {
    switch(ixv->attach)
    {
      case WLZ_VALUE_ATTACH_NOD:
	maxIdx = (obj->domain.core->type == WLZ_CMESH_2D)?
		 obj->domain.cm2->res.nod.maxEnt:
		 obj->domain.cm3->res.nod.maxEnt;
        break;
      case WLZ_VALUE_ATTACH_ELM:
	maxIdx = (obj->domain.core->type == WLZ_CMESH_2D)?
		 obj->domain.cm2->res.elm.maxEnt:
		 obj->domain.cm3->res.elm.maxEnt;
        break;
      default:
	errNum = WLZ_ERR_VALUES_DATA;
	break;
    }
  }
  if(errNum == WLZ_ERR_NONE)
  {
    if((found = (WlzUByte *)AlcCalloc(maxIdx, sizeof(WlzUByte))) == NULL)
    {
      errNum = WLZ_ERR_MEM_ALLOC;
    }
  }
  if(errNum == WLZ_ERR_NONE)
  {
    switch(ixv->vType)
    {
      case WLZ_GREY_LONG:
      case WLZ_GREY_INT: 
      case WLZ_GREY_SHORT:
      case WLZ_GREY_UBYTE:
      case WLZ_GREY_FLOAT:
      case WLZ_GREY_DOUBLE:
	break;
      default:
	errNum = WLZ_ERR_VALUES_DATA;
	break;
    }
  }
  if(errNum == WLZ_ERR_NONE)
  {
    int	i;

    if(isnan(lo) || isnan(hi))
    {
      switch(ixv->vType)
      {
        case WLZ_GREY_FLOAT:
	  for(i = 0; i < maxIdx; ++i)
	  {
	    WlzGreyP	v;

	    v.v = WlzIndexedValueGet(ixv, i);
	    found[i] = isnan(v.flp[0]);
	  }
	  break;
        case WLZ_GREY_DOUBLE:
	  for(i = 0; i < maxIdx; ++i)
	  {
	    WlzGreyP	v;

	    v.v = WlzIndexedValueGet(ixv, i);
	    found[i] = isnan(v.dbp[0]);
	  }
	  break;
        default:
	  break;
      }
    }
    else
    {
      switch(ixv->vType)
      {
	case WLZ_GREY_LONG:
	  for(i = 0; i < maxIdx; ++i)
	  {
	    WlzGreyP	v;

	    v.v = WlzIndexedValueGet(ixv, i);
	    found[i] = !((v.lnp[0] < lo) || (v.lnp[0] > hi));
	  }
	  break;
	case WLZ_GREY_INT: 
	  for(i = 0; i < maxIdx; ++i)
	  {
	    WlzGreyP	v;

	    v.v = WlzIndexedValueGet(ixv, i);
	    found[i] = !((v.inp[0] < lo) || (v.inp[0] > hi));
	  }
	  break;
	case WLZ_GREY_SHORT:
	  for(i = 0; i < maxIdx; ++i)
	  {
	    WlzGreyP	v;

	    v.v = WlzIndexedValueGet(ixv, i);
	    found[i] = !((v.shp[0] < lo) || (v.shp[0] > hi));
	  }
	  break;
	case WLZ_GREY_UBYTE:
	  for(i = 0; i < maxIdx; ++i)
	  {
	    WlzGreyP	v;

	    v.v = WlzIndexedValueGet(ixv, i);
	    found[i] = !((v.ubp[0] < lo) || (v.ubp[0] > hi));
	  }
	  break;
	case WLZ_GREY_FLOAT:
	  for(i = 0; i < maxIdx; ++i)
	  {
	    WlzGreyP	v;

	    v.v = WlzIndexedValueGet(ixv, i);
	    found[i] = !((v.flp[0] < lo) || (v.flp[0] > hi));
	  }
	  break;
	case WLZ_GREY_DOUBLE:
	  for(i = 0; i < maxIdx; ++i)
	  {
	    WlzGreyP	v;

	    v.v = WlzIndexedValueGet(ixv, i);
	    found[i] = !((v.dbp[0] < lo) || (v.dbp[0] > hi));
	  }
	  break;
	default:
	  break;
      }
    }
    if(!in)
    {
      for(i = 0; i < maxIdx; ++i)
      {
        found[i] = !found[i];
      }
    }
  }
  if(errNum != WLZ_ERR_NONE)
  {
    AlcFree(found);
    found = NULL;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(found);
}
