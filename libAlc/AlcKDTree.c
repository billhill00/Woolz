#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _AlcKDTree_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         libAlc/AlcKDTree.c
* \author       Bill Hill
* \date         November 2000
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
* \brief        A general purpose, arbitrary dimension, integer/floating
*		point binary space partition tree (kD-tree).
* \ingroup	AlcKDTree
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <Alc.h>

static void			AlcKDTBoundSet(
				  AlcKDTTree *tree,
				  AlcKDTNode *node,
				  int cmp);
static void			AlcKDTValuesSet(
				  AlcKDTTree *tree,
				  AlcPointP key0,
				  AlcPointP key1);
static void			AlcKDTPointFacts(AlcKDTTree *tree,
				  AlcPointP pnt,
				  FILE *fP);
static int			AlcKDTNodeFacts(
				  AlcKDTTree *tree,
				  AlcKDTNode *node,
				  FILE *fP);
static int			AlcKDTNodeValueCompare(
				  AlcKDTTree *tree,
				  AlcKDTNode *node,
				  AlcPointP key);
static int			AlcKDTNodeIntersectsSphereBB(
				  AlcKDTTree *tree,
				  AlcKDTNode *node,
				  AlcPointP centre,
				  double radius);
static double			AlcKDTKeyDistSq(
				  AlcKDTTree *tree,
				  AlcPointP key0,
				  AlcPointP key1);
static AlcErrno        		AlcKDTTreeExpand(
				  AlcKDTTree *tree);
static AlcKDTNode		*AlcKDTNodeAlcNew(
				  AlcKDTTree *tree,
				  AlcErrno *dstErr);
static AlcKDTNode 		*AlcKDTNodeGetNN(
				  AlcKDTTree *tree,
				  AlcKDTNode *node,
				  AlcPointP key,
				  double minDist,
				  double *dstDist);

/*!
* \return     	KD-tree data structure, or NULL on error.
* \ingroup	AlcKDTree
* \brief        Creates a KD-tree data structure.
* \param        type			Type of tree node key.
* \param	dim			Dimension of tree (must be >= 1).
* \param	tol			Tollerance for key comparision,
*					only used if the tree has floating
*					point keys. If the tolerance is
*					negative it is set to a default
*					value.
* \param	nNodes			Expected number of nodes in the
*					tree,  0 if unknown. This
*					can be used to optimise node
*					allocation.
* \param        dstErr		    	Destination pointer for error
*                                       code, may be NULL.
*/
AlcKDTTree	*AlcKDTTreeNew(AlcPointType type, int dim, double tol,
			       size_t nNodes, AlcErrno *dstErr)
{
  size_t	keySz;
  AlcKDTTree	*tree = NULL;
  AlcErrno	errNum = ALC_ER_NONE;
  const size_t	minNodeBlockSz = 1024;
  const double	defTol = 1.0E-06;

  if(dim < 1)
  {
    errNum = ALC_ER_PARAM;
  }
  else
  {
    switch(type)
    {
      case ALC_POINTTYPE_INT:
	keySz = sizeof(int) * dim;
	break;
      case ALC_POINTTYPE_DBL:
	keySz = sizeof(double) * dim;
	break;
      default:
	errNum = ALC_ER_PARAM;
	break;
    }
  }
  if(errNum == ALC_ER_NONE)
  {
    if((tree = (AlcKDTTree *)AlcCalloc(1, sizeof(AlcKDTTree))) == NULL)
    {
      errNum = ALC_ER_ALLOC;
    }
    else
    {
      tree->type = type;
      tree->dim = dim;
      tree->keySz = keySz;
      tree->tol = (tol < 0.0)? defTol: tol;
      if((tree->nodeBlockSz = nNodes / 10) < minNodeBlockSz)
      {
	tree->nodeBlockSz = minNodeBlockSz;
      }
    }
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(tree);
}

/*!
* \return      	Number of nodes.
* \ingroup	AlcKDTree
* \brief        Prints facts about the kD-tree structure to a file.
* \param        tree		 	The KD-tree tree
* \param	fP			Output file stream.
*/
int		AlcKDTTreeFacts(AlcKDTTree *tree, FILE *fP)
{
  int		nNodes = 0;

  if(tree)
  {
    (void )fprintf(fP, "type         %-d\n", (int )(tree->type));
    (void )fprintf(fP, "dim          %-d\n", tree->dim);
    (void )fprintf(fP, "keySz        %-d\n", (int )(tree->keySz));
    (void )fprintf(fP, "tol          %-g\n", tree->tol);
    (void )fprintf(fP, "nNodes       %-d\n", (int )(tree->nNodes));
    (void )fprintf(fP, "root         0x-%lx\n",
		    (unsigned long )(tree->root));
    (void )fprintf(fP, "nodeStack    0x%-lx\n",
		    (unsigned long)(tree->nodeStack));
    (void )fprintf(fP, "nodeBlockSz  0x%-lx\n",
		    (unsigned long)(tree->nodeBlockSz));
    (void )fprintf(fP, "freeStack    0x%-lx\n",
		   (unsigned long)(tree->freeStack));
    (void )fprintf(fP, "\n");
    nNodes = AlcKDTNodeFacts(tree, tree->root, fP);
  }
  return(nNodes);
}

/*!
* \return      	Number of child nodes.
* \ingroup	AlcKDTree
* \brief        Prints facts about the kD-tree node structure to a file.
* \param        tree		 	The KD-tree tree.
* \param	node			The KD-tree node.
* \param	fP			Output file stream.
*/
static int	AlcKDTNodeFacts(AlcKDTTree *tree, AlcKDTNode *node, FILE *fP)
{
  int		nNodes = 0;

  if(node)
  {
    nNodes = 1;
    (void )fprintf(fP, "idx          %-d\n", (int )(node->idx));
    (void )fprintf(fP, "split        %-d\n", node->split);
    (void )fprintf(fP, "parent       0x%-lx (%d)\n",
		    (unsigned long )(node->parent),
		    (node->parent)? (int )(node->parent->idx): -1);
    (void )fprintf(fP, "childN       0x%-lx (%d)\n",
		    (unsigned long )(node->childN),
		    node->childN? (int )(node->childN->idx): -1);
    (void )fprintf(fP, "childP       0x%-lx (%d)\n",
		    (unsigned long )(node->childP),
		    node->childP? (int )(node->childP->idx): -1);
    (void )fprintf(fP, "key          ");
    AlcKDTPointFacts(tree, node->key, fP);
    (void )fprintf(fP, "\n");
    (void )fprintf(fP, "boundN       ");
    AlcKDTPointFacts(tree, node->boundN, fP);
    (void )fprintf(fP, "\n");
    (void )fprintf(fP, "boundP       ");
    AlcKDTPointFacts(tree, node->boundP, fP);
    (void )fprintf(fP, "\n\n");
    if(node->childN)
    {
      nNodes += AlcKDTNodeFacts(tree, node->childN, fP);
    }
    if(node->childP)
    {
      nNodes += AlcKDTNodeFacts(tree, node->childP, fP);
    }
  }
  return(nNodes);
}

/*!
* \return       <void>
* \ingroup	AlcKDTree
* \brief        Prints facts the given point.
* \param        tree		 	The KD-tree tree.
* \param	pnt			Given point.
* \param	fP 			Output file stream.
*/
static void	AlcKDTPointFacts(AlcKDTTree *tree, AlcPointP pnt, FILE *fP)
{
  int		idx;

  for(idx = 0; idx < tree->dim; ++idx)
  {
    if(tree->type == ALC_POINTTYPE_INT)
    {
      (void )fprintf(fP, "%-d ", *(pnt.kI + idx));
    }
    else /* tree->type == ALC_POINTTYPE_DBL */
    {
      (void )fprintf(fP, "%-g ", *(pnt.kD + idx));
    }
  }
}

/*!
* \return:     	Error code.
* \ingroup	AlcKDTree
* \brief        Expands the given tree by allocating more nodes and
*		then adding them to the available node stack.
* \param        tree		 	The KD-tree data structure.
*/
static AlcErrno AlcKDTTreeExpand(AlcKDTTree *tree)
{
  size_t	cnt;
  AlcKDTNode	*node;
  AlcPointP	key;
  AlcBlockStack	*nStk,
		*kStk;
  AlcErrno	errNum = ALC_ER_NONE;

  if(tree == NULL)
  {
    errNum = ALC_ER_NULLPTR;
  }
  else
  {
    nStk = AlcBlockStackNew(tree->nodeBlockSz, sizeof(AlcKDTNode),
			    tree->freeStack, &errNum);
  }
  if(errNum == ALC_ER_NONE)
  {
    tree->freeStack = nStk;
    kStk = AlcBlockStackNew(tree->nodeBlockSz, 3 * tree->keySz,
			    tree->freeStack, &errNum);
  }
  if(errNum == ALC_ER_NONE)
  {
    tree->freeStack = kStk;
    nStk->elmCnt = nStk->maxElm;
    node = (AlcKDTNode *)(nStk->elements);
    kStk->elmCnt = kStk->maxElm;
    key.kV = kStk->elements;
    cnt = tree->nodeBlockSz;
    if(tree->type == ALC_POINTTYPE_INT)
    {
      while(cnt-- > 0)
      {
	node->childN = tree->nodeStack;
	tree->nodeStack = node;
	node->key.kI = key.kI; 
	node->boundN.kI = key.kI + tree->dim; 
	node->boundP.kI = key.kI + (2 * tree->dim); 
	++node;
	key.kI += (3 * tree->dim);
      }
    }
    else /* tree->type == ALC_POINTTYPE_DBL */
    {
      while(cnt-- > 0)
      {
	node->childN = tree->nodeStack;
	tree->nodeStack = node;
	node->key.kD = key.kD; 
	node->boundN.kD = key.kD + tree->dim; 
	node->boundP.kD = key.kD + (2 * tree->dim); 
	++node;
	key.kD += (3 * tree->dim);
      }
    }
  }
  return(errNum);
}

/*!
* \return      	Error code.
* \ingroup	AlcKDTree
* \brief        Free's the given KD-tree data structure and any nodes
*               in the tree.
* \param        tree			The KD-tree data structure.
*/
AlcErrno        AlcKDTTreeFree(AlcKDTTree *tree)
{
  AlcErrno	errNum = ALC_ER_NONE;

  if(tree)
  {
    if(tree->freeStack)
    {
      errNum = AlcBlockStackFree(tree->freeStack);
    }
    AlcFree(tree);
  }
  return(errNum);
}

/*!
* \return      	New node, NULL on error.
* \ingroup	AlcKDTree
* \brief        Allocates a new node and sets it's fields.
* \param        tree		 	The KD-tree data structure.
* \param	parent			Parent node, NULL if this is
*					the first node of the tree.
* \param	key			Key values to test agains those
*					of the given node.
*					(int or double) and dimension.
* \param	cmp			The sign of cmp indicates which
*					of the parent's children the new
*					node will become:
*					  - = 0, parent == null
*					  - > 0, childP
*					  - < 0, childN
* \param	dstErr			Destination pointer for error
*					code, may be NULL.
*/
AlcKDTNode	*AlcKDTNodeNew(AlcKDTTree *tree, AlcKDTNode *parent,
			       AlcPointP key, int cmp,
			       AlcErrno *dstErr)
{
  AlcKDTNode	*node = NULL;
  AlcErrno	errNum = ALC_ER_NONE;

  if((node = AlcKDTNodeAlcNew(tree, &errNum)) != NULL)
  {
    /* node->idx and node->key.kV set by AlcKDTNodeGetNew() */
    node->split = (parent == NULL)? 0: (parent->split + 1) % tree->dim;
    node->parent = parent;
    node->childP = node->childN = NULL;
    AlcKDTValuesSet(tree, node->key, key);
    AlcKDTBoundSet(tree, node, cmp);
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(node);
}

/*!
* \return       <void>
* \ingroup	AlcKDTree
* \brief        Sets the bounding values of the given node.
* \param        tree		 	The KD-tree data structure.
* \param	node			Node for which to set bounds.
* \param	cmp			The sign of cmp indicates which
*					of the parent's children the new
*					node will be:
*					  - = 0, parent == null
*					  - > 0, childP
*					  - < 0, childN
*/
static void	AlcKDTBoundSet(AlcKDTTree *tree, AlcKDTNode *node, int cmp)
{
  int		idx;
  AlcKDTNode 	*parent;

  if((cmp == 0) || ((parent = node->parent) == NULL))
  {
    if(tree->type == ALC_POINTTYPE_INT)
    {
      for(idx = 0; idx < tree->dim; ++idx)
      {
	*(node->boundN.kI + idx) = INT_MIN; 		   /* INT_MIN is -ve */
	*(node->boundP.kI + idx) = INT_MAX;
      }
    }
    else /* tree->type == ALC_POINTTYPE_DBL */
    {
      for(idx = 0; idx < tree->dim; ++idx)
      {
	*(node->boundN.kD + idx) = -DBL_MAX; 		   /* DBL_MAX is +ve */
	*(node->boundP.kD + idx) = DBL_MAX;
      }
    }
  }
  else
  {
    AlcKDTValuesSet(tree, node->boundN, parent->boundN);
    AlcKDTValuesSet(tree, node->boundP, parent->boundP);
    if(cmp > 0) /* node will be childP in parent */
    {
      if(tree->type == ALC_POINTTYPE_INT)
      {
	*(node->boundP.kI + parent->split) = *(parent->key.kI + parent->split);
      }
      else /* tree->type == ALC_POINTTYPE_DBL */
      {
	*(node->boundP.kD + parent->split) = *(parent->key.kD + parent->split);
      }
    }
    else /* cmp < 0, node will be childN in parent */
    {
      if(tree->type == ALC_POINTTYPE_INT)
      {
	*(node->boundN.kI + parent->split) = *(parent->key.kI + parent->split);
      }
      else /* tree->type == ALC_POINTTYPE_DBL */
      {
	*(node->boundN.kD + parent->split) = *(parent->key.kD + parent->split);
      }
    }
  }
}

/*!
* \return      	New node, NULL on error.
* \ingroup	AlcKDTree
* \brief        Allocates a new node by popping it from the stack of
*		available free nodes.
* \param        tree<AlcKDTTree *>: 	The KD-tree data structure.
* \param	parent			Parent node, NULL if this is
*					the first node of the tree.
* \param	keyVal			Values to test agains those of
*					of the given node. These MUST
*					be of the appropriate type
*					(int or double) and dimension.
* \param	dstErr			Destination pointer for error
*					code, may be NULL.
*/
static AlcKDTNode *AlcKDTNodeAlcNew(AlcKDTTree *tree, AlcErrno *dstErr)
{
  AlcKDTNode	*node = NULL;
  AlcErrno	errNum = ALC_ER_NONE;

  if(tree->nodeStack == NULL)
  {
    errNum = AlcKDTTreeExpand(tree);
  }
  if(errNum == ALC_ER_NONE)
  {
    node = tree->nodeStack;
    node->idx = tree->nNodes++;
    tree->nodeStack = tree->nodeStack->childN;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(node);
}

/*!
* \return       void
* \ingroup	AlcKDTree
* \brief        Free's the given KD-tree node and all it's child nodes
*		by pushing them onto the stack of available nodes.
* \param        tree		 	The KD-tree data structure.
* \param	node		 	The KD-tree node data structure.
*/
void		AlcKDTNodeFree(AlcKDTTree *tree, AlcKDTNode *node)
{
  if(tree && node)
  {
    AlcKDTNodeFree(tree, node->childN);
    AlcKDTNodeFree(tree, node->childP);
    node->childN = tree->nodeStack;
    tree->nodeStack = node;
  }
}

/*!
* \return	New node added to tree, NULL on error or if key
*		matched in tree.
* \ingroup	AlcKDTree
* \brief  	Checks for a node in the tree with the given key, if
*		such a node doesn't exist then insert a new one.
* \param     	tree			Given tree.
* \param	keyVal			Key values which must be
*					consistent with the tree's node
*					key type and dimension.
* \param	dstFndNod		Destination pointer for matched
*					node, may be NULL.
* \param	dstErr			Destination pointer for error
*					code, may be NULL.
*/
AlcKDTNode	*AlcKDTInsert(AlcKDTTree *tree,  void *keyVal,
			      AlcKDTNode **dstFndNod, AlcErrno *dstErr)
{

  int		cmp;
  AlcKDTNode	*nodS,
		*newNod = NULL,
		*tstNod = NULL;
  AlcPointP	key;
  AlcErrno	errNum = ALC_ER_NONE;

  if((tree == NULL) || (keyVal == NULL))
  {
    errNum = ALC_ER_NULLPTR;
  }
  else
  {
    key.kV = keyVal;
    if(tree->root == NULL)
    {
      newNod = tree->root = AlcKDTNodeNew(tree, NULL, key, 0, &errNum);
    }
    else
    {
      /* Search for matching key */
      nodS = tstNod = tree->root;
      while(nodS && ((cmp = AlcKDTNodeValueCompare(tree, tstNod, key)) != 0))
      {
	if(cmp > 0)
	{
	  if((nodS = tstNod->childP) == NULL)
	  {
	    newNod = tstNod->childP = AlcKDTNodeNew(tree, tstNod, key, cmp,
						    &errNum);
	  }
	}
	else /* cmp < 0 */
	{
	  if((nodS = tstNod->childN) == NULL)
	  {
	    newNod = tstNod->childN = AlcKDTNodeNew(tree, tstNod, key, cmp,
						    &errNum);
	  }
	}
	if(nodS)
	{
	  tstNod = nodS;
	}
      }
      if(dstFndNod)
      {
	*dstFndNod = (cmp == 0)? tstNod: NULL;
      }
    }
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(newNod);
}

/*!
* \return	Matched node in tree, NULL on error or if key not matched.
* \ingroup	AlcKDTree
* \brief  	Searches for a node with a matching key to the given key.
* \param     	tree			Given tree,
* \param	keyVal			Key values which must be
*					consistent with the tree's node
*					key type and dimension.
* \param	dstErr			Destination pointer for error
*					code, may be NULL.
*/
AlcKDTNode	*AlcKDTGetMatch(AlcKDTTree *tree,  void *keyVal,
				AlcErrno *dstErr)
{

  int		cmp;
  AlcKDTNode	*tstNod = NULL;
  AlcPointP	key;
  AlcErrno	errNum = ALC_ER_NONE;

  if((tree == NULL) || (keyVal == NULL))
  {
    errNum = ALC_ER_NULLPTR;
  }
  else
  {
    key.kV = keyVal;
    if(tree->root != NULL)
    {
      /* Search for matching key */
      tstNod = tree->root;
      while(tstNod && ((cmp = AlcKDTNodeValueCompare(tree, tstNod, key)) != 0))
      {
	if(cmp > 0)
	{
	  tstNod = tstNod->childP;
	}
	else /* cmp < 0 */
	{
	  tstNod = tstNod->childN;
	}
      }
    }
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(tstNod);
}

/*!
* \return	Leaf node containing the given
* \ingroup	AlcKDTree
*					key or NULL if tree has no nodes.
* \brief  	Searches for the leaf node containing the given key.
*		progressing downwards in the tree.
* \param     	tree			Given tree,
* \param	node			Node to search from.
* \param	key			Key values which must be
*					consistent with the tree's node
*					key type and dimension.
*/
AlcKDTNode 	*AlcKDTGetLeaf(AlcKDTTree *tree,  AlcKDTNode *node,
			       AlcPointP key)
{

  int		cmp;
  AlcKDTNode	*tstNod0,
		*tstNod1,
		*leafNode = NULL;

  tstNod1 = tstNod0 = node;
  while(tstNod1 && ((cmp = AlcKDTNodeValueCompare(tree, tstNod0, key)) != 0))
  {
    if(cmp > 0)
    {
      tstNod1 = tstNod0->childP;
    }
    else /* cmp < 0 */
    {
      tstNod1 = tstNod0->childN;
    }
    if(tstNod1)
    {
      tstNod0 = tstNod1;
    }
  }
  leafNode = tstNod0;
  return(leafNode);
}

/*!
* \return	Nearest neighbour node in tree, NULL on error or if
* \ingroup	AlcKDTree
*		tree has no nodes.
* \brief  	Searches for the nearest neighbour node to the given
*		key within the tree.
* \param     	tree			Given tree,
* \param	keyVal			Key values which must be
*					consistent with the tree's node
*					key type and dimension.
* \param	minDist			Maximum distance between given
*					key and the nearest neighbour,
*					any node at a greater distance
*					is not a nearest neighbour.
*					Confusingly used for the minimum
*					key-node distance found.
* \param	dstNNDist		Destination pointer for distance
*					to nearest neighbour, may be NULL.
* \param	dstErr			Destination pointer for error
*					code, may be NULL.
*/
AlcKDTNode	*AlcKDTGetNN(AlcKDTTree *tree,  void *keyVal,
			     double minDist, double *dstNNDist,
			     AlcErrno *dstErr)
{

  double	dist,
		distSq;
  AlcPointP	key;
  AlcKDTNode	*tstNode0,
		*tstNode1,
		*nNNode = NULL;
  AlcErrno	errNum = ALC_ER_NONE;

  if((tree == NULL) || (keyVal == NULL))
  {
    errNum = ALC_ER_NULLPTR;
  }
  else
  {
    key.kV = keyVal;
    if(tree->root != NULL)
    {
      /* Find leaf node which encloses the given key. */
      tstNode0 = AlcKDTGetLeaf(tree, tree->root, key);
      /* Update the minimum distance criterea for a nearest neighbour. */
      distSq = AlcKDTKeyDistSq(tree, tstNode0->key, key);
      if(distSq < (minDist * minDist))
      {
	nNNode = tstNode0;
	dist = minDist = sqrt(distSq);
      }
      /* Walk up the tree until either the parent is NULL (at root of tree)
       * or the parent node does not intersect the hyper-sphere with
       * centre at key with radius minDist). */
      while(((tstNode1 = tstNode0->parent) != NULL) &&
	    AlcKDTNodeIntersectsSphereBB(tree, tstNode1, key, minDist))
      {
	tstNode0 = tstNode1;
      }
      /* Walk back down the tree testing nodes that intersect the hyper-sphere
       * until either the child is NULL (at a leaf) or the child does not
       * intersect the hyper-sphere, updating the nearest neighbour node
       * and distance while walking. */
      tstNode1 = AlcKDTNodeGetNN(tree, tstNode0, key, minDist, &dist);
      if(tstNode1 && (dist < minDist))
      {
	minDist = dist;
	nNNode = tstNode1;
      }
    }
  }
  if(dstNNDist)
  {
    if(nNNode != NULL)
    {
      *dstNNDist = minDist;
    }
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(nNNode);
}

/*!
* \return:	Nearest neighbour node in tree, NULL on error or if tree
* \ingroup	AlcKDTree
*		has no nodes.
* \brief  	Walks down the tree testing nodes that intersect the
* 		hyper-sphere until either the child is NULL (at a leaf)
*		or the child does not intersect the hyper-sphere,
*		updating the nearest neighbour node and distance while
*		walking.
* \param     	tree			Given tree,
* \param	node			Given node.
* \param	key			Given key.
* \param	minDist			Minimum distance between given
*					key and the nearest neighbour,
*					any node at a greater distance
*					is not a nearest neighbour.
* \param	doubleminDist		Destination pointer for minimum
*					distance to nearest neighbour
*					so far.
*/
static AlcKDTNode *AlcKDTNodeGetNN(AlcKDTTree *tree,  AlcKDTNode *node,
				   AlcPointP key, double minDist,
				   double *dstDist)
{
  double	tstDist,
		tstDistSq;
  AlcKDTNode	*tstNode,
		*nNNode = NULL;

  /* If this node's bounding box intersects the hyper-sphere check
   * the node and its children for the nearest neighbour. */
  if(AlcKDTNodeIntersectsSphereBB(tree, node, key, minDist))
  {
    tstDistSq = AlcKDTKeyDistSq(tree, node->key, key);
    if(tstDistSq < (minDist * minDist))
    {
      nNNode = node;
      minDist = sqrt(tstDistSq);
    }
    if(node->childN)
    {
      tstNode = AlcKDTNodeGetNN(tree, node->childN, key, minDist,
				&tstDist); 			/* Recursive */
      if(tstNode && (tstDist < minDist))
      {
	nNNode = tstNode;
	minDist = tstDist;
      }
    }
    if(node->childP)
    {
      tstNode = AlcKDTNodeGetNN(tree, node->childP, key, minDist,
				&tstDist); 			/* Recursive */
      if(tstNode && (tstDist < minDist))
      {
	nNNode = tstNode;
	minDist = tstDist;
      }
    }
    *dstDist = minDist;
  }
  return(nNNode);
}


/*!
* \return	Non zero if the given node intersects the given sphere.
* \ingroup	AlcKDTree
* \brief  	Computes whether the given node intersects the given
*		hyper-sphere's bounding box.
*		There are 2 algorithms that can be used.
*		* Always true test, NOT RECOMENDED for anything except
*		  testing.
*		* Box intersection test, checks for an intersection
*		  between the bounding boxes of the node and the
*		  hyper-sphere.
* 		
* 		This function will in most cases dominate the performance
* 		of kD-tree queries so there's been some effort to optimise
* 		the code.
* \param     	tree			Given tree,
* \param	node			Given node.
* \param	centre			Given key at centre of the
*					hyper-sphere.
* \param	radius			Radius of the hyper-sphere.
*/
static int	AlcKDTNodeIntersectsSphereBB(AlcKDTTree *tree,
				             AlcKDTNode *node,
					     AlcPointP centre,
					     double radius)
#ifdef ALC_KDT_ALWAYSTRUE_TEST
{
  return(1);
}
#else
{
  int  		idx,
		inside = 1;

  idx = 0;
  if(tree->type == ALC_POINTTYPE_INT)
  {
    int		radF,
    		radC;

    radF = (int )floor(radius);
    radC = (int )ceil(radius);
    switch(tree->dim)
    {
      case 2:
	{
	  inside &= (node->boundP.kI[0] >= (centre.kI[0] - radF)) &
		    (node->boundN.kI[0] <= (centre.kI[0] + radC)) &
	            (node->boundP.kI[1] >= (centre.kI[1] - radF)) &
		    (node->boundN.kI[1] <= (centre.kI[1] + radC));
	}
	break;
      case 3:
	{
	  inside &= (node->boundP.kI[0] >= (centre.kI[0] - radF)) &
		    (node->boundN.kI[0] <= (centre.kI[0] + radC)) &
	            (node->boundP.kI[1] >= (centre.kI[1] - radF)) &
		    (node->boundN.kI[1] <= (centre.kI[1] + radC)) &
	            (node->boundP.kI[2] >= (centre.kI[2] - radF)) &
		    (node->boundN.kI[2] <= (centre.kI[2] + radC));
	}
	break;
      default:
	do
	{
	  inside &= (node->boundP.kI[idx] >= (centre.kI[idx] - radF)) &
		    (node->boundN.kI[idx] <= (centre.kI[idx] + radC));
	}
	while(inside && (++idx < tree->dim));
	break;
    }
  }
  else /* tree->type == ALC_POINTTYPE_DBL */
  {
    do
    {
      inside &= (node->boundP.kD[idx] >= (centre.kD[idx] - radius)) &
		(node->boundN.kD[idx] <= (centre.kD[idx] + radius));
    }
    while(inside && (++idx < tree->dim));
  }
  return(inside);
}
#endif

/*!
* \return	<void>
* \ingroup	AlcKDTree
* \brief  	Sets the first value to the value of the second.
* \param     	tree			Tree, used to determine value type.
* \param	val0			First value.
* \param	val1			Second value.
*/
static void	AlcKDTValuesSet(AlcKDTTree *tree,
				AlcPointP val0, AlcPointP val1)
{
  int		idx;

  if(tree->type == ALC_POINTTYPE_INT)
  {
    for(idx = 0; idx < tree->dim; ++idx)
    {
      *(val0.kI + idx) = *(val1.kI  + idx);
    }
  }
  else /* tree->type == ALC_POINTTYPE_DBL */
  {
    for(idx = 0; idx < tree->dim; ++idx)
    {
      *(val0.kD + idx) = *(val1.kD + idx);
    }
  }
}

/*!
* \return 	Square of distance between the two keys.
* \ingroup	AlcKDTree
* \brief  	Computes the squared distance between the two keys.
* \param     	tree			Tree, used to determine key type.
*		key0			First key.
*		key1			Second key.
*/
static double	AlcKDTKeyDistSq(AlcKDTTree *tree,
				AlcPointP key0, AlcPointP key1)
{
  int		idx;
  double	tD0,
		distSq = 0;

  if(tree->type == ALC_POINTTYPE_INT)
  {
    for(idx = 0; idx < tree->dim; ++idx)
    {
      tD0 = (double )(*(key0.kI + idx) - *(key1.kI + idx));
      distSq += tD0 * tD0;
    }
  }
  else /* tree->type = ALC_POINTTYPE_DBL */
  {
    for(idx = 0; idx < tree->dim; ++idx)
    {
      tD0 = *(key0.kD + idx) - *(key1.kD + idx);
      distSq += tD0 * tD0;
    }
  }
  return(distSq);
}

/*!
* \return	Result of comparision 0, -ve or +ve.
* \ingroup	AlcKDTree
* \brief  	Compares the values of the given key with those of the
*		given node.
* \param     	tree			Tree, used to determine key type
*					and tolerance.
* \param	node			Given node.
* \param	key			Values to test agains those of
*					of the given node.
*/
static int	AlcKDTNodeValueCompare(AlcKDTTree *tree, AlcKDTNode *node,
				       AlcPointP key)
{
  int		jIdx,
		kIdx,
		cmp = 0;

  if(tree->type == ALC_POINTTYPE_INT)
  {
    jIdx = kIdx = node->split;
    do
    {
      cmp = *(node->key.kI + jIdx) - *(key.kI + jIdx);
    } while((cmp == 0) && ((jIdx = (jIdx + 1) % tree->dim) != kIdx));
  }
  else /* tree->type == ALC_POINTTYPE_DBL */
  {
    jIdx = kIdx = node->split;
    do
    {
      double	diff;

      diff = *(node->key.kD + jIdx) - *(key.kD + jIdx);
      if(fabs(diff) > tree->tol)
      {
	cmp = (diff > 0)? +1: -1;
      }
    } while((cmp == 0) && ((jIdx = (jIdx + 1) % tree->dim) != kIdx));
  }
  return(cmp);
}

#ifdef ALC_KDT_TEST
int		main(int argc, char *argv[])
{
  int		idx0,
		idx1;
  double	tD0,
		dist,
		sumDist;
  AlcKDTTree	*tree;
  AlcKDTNode	*newNode,
		*fndNode;
  AlcErrno	errNum = ALC_ER_NONE;
#ifdef ALC_KDT_TEST_SMALL
  int		nNodes = 10;
#ifdef ALC_KDT_TEST_SMALL_INT
  int		dat[2];
  const int	data[10][2] =
#else /* ! ALC_KDT_TEST_SMALL_INT */
  double	dat[2];
  const double	data[10][2] =
#endif /* ALC_KDT_TEST_SMALL_INT */
  {
    {50, 50}, /* 0/A */
    {10, 70}, /* 1/B */
    {80, 15}, /* 2/C */
    {25, 20}, /* 3/D */
    {40, 85}, /* 4/E */
    {70, 85}, /* 5/F */
    {10, 60}, /* 6/G */
    {30, 30}, /* 7 */
    {60, 60}, /* 8 */
    {65, 25}  /* 9 */
  };
#else /* ! ALC_KDT_TEST_SMALL */
  int	nNodes = 200000;
  double	dat[2];
  static double	data[200000][2];
#endif /* ALC_KDT_TEST_SMALL */

  system("date");
#ifndef ALC_KDT_TEST_SMALL
  for(idx0 = 0; idx0 < nNodes; ++idx0)
  {
      dat[0] = drand48();
      dat[1] = drand48();
      data[idx0][0] = dat[0];
      data[idx0][1] = dat[1];
  }
#endif /* ! ALC_KDT_TEST_SMALL */
  system("date");
/* #define ALC_KDT_TEST_BRUTEFORCE */
#ifdef ALC_KDT_TEST_BRUTEFORCE
  dist = DBL_MAX;
  sumDist = 0.0;
  for(idx0 = 0; idx0 < nNodes / 2; ++idx0)
  {
    for(idx1 = nNodes / 2; idx1 < nNodes; ++idx1)
    {
      dat[0] = data[idx0][0] - data[idx1][0];
      dat[1] = data[idx0][1] - data[idx1][1];
      tD0 = (dat[0] * dat[0]) + (dat[1] * dat[1]);
      if(tD0 < dist)
      {
	dist = tD0;
	sumDist += sqrt(dist);
      }
    }
  }
  system("date");
  printf("%g\n", sumDist);
#else /* ! ALC_KDT_TEST_BRUTEFORCE */
#ifdef ALC_KDT_TEST_SMALL
#ifdef ALC_KDT_TEST_SMALL_INT
  if((tree = AlcKDTTreeNew(ALC_POINTTYPE_INT, 2, -1.0, 0,
			   &errNum)) != NULL)
#else /* ! ALC_KDT_TEST_SMALL_INT */
  if((tree = AlcKDTTreeNew(ALC_POINTTYPE_DBL, 2, -1.0, 0,
			   &errNum)) != NULL)
#endif /* ALC_KDT_TEST_SMALL_INT */
#else /* ! ALC_KDT_TEST_SMALL */
  if((tree = AlcKDTTreeNew(ALC_POINTTYPE_DBL, 2, -1.0, nNodes / 2,
			   &errNum)) != NULL)
#endif /* ALC_KDT_TEST_SMALL */
  {
    idx0 = 0;
    do
    {
      dat[0] = data[idx0][0]; dat[1] = data[idx0][1];
      newNode = AlcKDTInsert(tree, dat, &fndNode, &errNum);
      ++idx0;
#ifdef ALC_KDT_TEST_SMALL
    } while((errNum == ALC_ER_NONE) && (idx0 < nNodes));
#else /* ! ALC_KDT_TEST_SMALL */
    } while((errNum == ALC_ER_NONE) && (idx0 < (nNodes / 2)));
#endif /* ALC_KDT_TEST_SMALL */
  }
  system("date");
#ifdef ALC_KDT_TEST_SMALL
  dat[0] = 30; dat[1] = 65;
  fndNode = AlcKDTGetNN(tree, dat, DBL_MAX, &dist, &errNum);
  idx0 = AlcKDTTreeFacts(tree, stdout);
  (void )printf("AlcKDTTreeFacts() found %d nodes.\n", idx0);
#else /* ! ALC_KDT_TEST_SMALL */
  sumDist = 0.0;
  if(errNum == ALC_ER_NONE)
  {
    idx0 = nNodes / 2;
    do
    {
      dat[0] = data[idx0][0]; dat[1] = data[idx0][1];
      fndNode = AlcKDTGetNN(tree, dat, DBL_MAX, &dist, &errNum);
      if(fndNode)
      {
	sumDist += dist;
      }
      ++idx0;
    } while((errNum == ALC_ER_NONE) && (idx0 < nNodes));
  }
  system("date");
  printf("%g\n", sumDist);
#endif /* ALC_KDT_TEST_SMALL */
  (void )AlcKDTTreeFree(tree);
#endif /* ALC_KDT_TEST_BRUTEFORCE */
  return(errNum);
}
#endif /* ALC_KDT_TEST */
