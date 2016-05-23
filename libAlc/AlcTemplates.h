#ifndef ALCTEMPLATES_H
#define ALCTEMPLATES_H
#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _AlcTemplates_h[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         libAlc/AlcTemplates.h
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
* \brief        Templates used by the 'C' pre-processor to generate the
*		body of the Woolz array allocation functions and the
*		associated freeing functions.
* \todo		-
* \bug          None known.
*/

#ifndef WLZ_EXT_BIND
#ifdef __cplusplus
extern "C" {
#endif
#endif /* WLZ_EXT_BIND */

/*!
* \def		ALC_TEMPLATE_C1D(D,T,M,F)
* \ingroup	AlcArray
* \brief	A template for functions which allocate 1 dimensional
*		zero'd arrays of any type.
* \param        D                     Destination pointer, of type T *.
* \param        T                     Type, eg char, short, int, ....
* \param        M                     Number of elements in array.
* \param        F                     String with name of function.
*/
#define ALC_TEMPLATE_C1D(D,T,M,F) \
  AlcErrno	alcErrno = ALC_ER_NONE; \
  \
  if((D) == NULL) \
    alcErrno = ALC_ER_NULLPTR; \
  else if((M) < 1) \
    alcErrno = ALC_ER_NUMELEM; \
  else if((*(D) = (T*)AlcCalloc((M), sizeof(T))) == NULL) \
    alcErrno = ALC_ER_ALLOC; \
  if(alcErrno != ALC_ER_NONE) \
  { \
    if(D) \
      *(D) = NULL; \
  } \
  return(alcErrno);

/*!
* \def		ALC_TEMPLATE_M1D(D,T,M,F)
* \ingroup	AlcArray
* \brief	A template for functions which allocate	1 dimensional
*		non-zero'd arrays of any type.
* \param        D                     Destination pointer, of type T *.
* \param        T                     Type, eg char, short, int, ....
* \param        M                     Number of elements in array.
* \param        F                     String with name of function.
*/
#define ALC_TEMPLATE_M1D(D,T,M,F) \
  AlcErrno	alcErrno = ALC_ER_NONE; \
  \
  if((D) == NULL) \
    alcErrno = ALC_ER_NULLPTR; \
  else if((M) < 1) \
    alcErrno = ALC_ER_NUMELEM; \
  else if((*(D) = (T*)AlcMalloc((M) * sizeof(T))) == NULL) \
    alcErrno = ALC_ER_ALLOC; \
  if(alcErrno != ALC_ER_NONE) \
  { \
    if(D) \
      *(D) = NULL; \
  } \
  return(alcErrno);

/*!
* \def		ALC_TEMPLATE_C2D(D,T,M,N,F)
* \ingroup	AlcArray
* \brief	A template for functions which allocate 2 dimensional
*		zero'd arrays of any type.
* \param        D                     Destination pointer, of type T **.
* \param        T                     Type, eg char, short, int, ....
* \param        M                     Number of 1D arrays.
* \param        N                     Number of elements in each 1D array.
* \param        F                     String with name of function.
*/
#define ALC_TEMPLATE_C2D(D,T,M,N,F) \
  size_t	index; \
  T		*dump0 = NULL; \
  T		**dump1 = NULL; \
  AlcErrno	alcErrno = ALC_ER_NONE; \
  \
  if((D) == NULL) \
    alcErrno = ALC_ER_NULLPTR; \
  else if(((M) < 1) || ((N) < 1)) \
    alcErrno = ALC_ER_NUMELEM; \
  else if(((dump0 = (T*)AlcCalloc((M) * (N), sizeof(T))) == NULL) || \
          ((dump1 = (T**)AlcMalloc((M) * sizeof(T*))) == NULL)) \
    alcErrno = ALC_ER_ALLOC; \
  if(alcErrno == ALC_ER_NONE) \
  { \
    *(D) = dump1; \
    for(index = 0; index < (M); ++index) \
    { \
      (*(D))[index] = dump0; \
      dump0 += (N); \
    } \
  } \
  else \
  { \
    if(D) \
      *(D) = NULL; \
    if(dump0) \
      AlcFree(dump0); \
    if(dump1) \
      AlcFree(dump1); \
  } \
  return(alcErrno);

/*!
* \def		ALC_TEMPLATE_M2D(D,T,M,N,F)
* \ingroup	AlcArray
* \brief	A template for functions which allocate 2 dimensional
*		non-zero'd arrays of any type.
* \param        D                     Destination pointer, of type T **.
* \param        T                     Type, eg char, short, int, ....
* \param        M                     Number of 1D arrays.
* \param        N                     Number of elements in each 1D array.
* \param        F                     String with name of function.
*/
#define ALC_TEMPLATE_M2D(D,T,M,N,F) \
  size_t	index; \
  T		*dump0 = NULL; \
  T  		**dump1 = NULL; \
  AlcErrno	alcErrno = ALC_ER_NONE; \
  \
  if((D) == NULL) \
    alcErrno = ALC_ER_NULLPTR; \
  else if(((M) < 1) || ((N) < 1)) \
    alcErrno = ALC_ER_NUMELEM; \
  else if(((dump0 = (T*)AlcMalloc((M) * (N) * sizeof(T))) == NULL) || \
          ((dump1 = (T**)AlcMalloc((M) * sizeof(T*))) == NULL)) \
    alcErrno = ALC_ER_ALLOC; \
  if(alcErrno == ALC_ER_NONE) \
  { \
    *(D) = dump1; \
    for(index = 0; index < (M); ++index) \
    { \
      (*(D))[index] = dump0; \
      dump0 += (N); \
    } \
  } \
  else \
  { \
    if(D) \
      *(D) = NULL; \
    if(dump0) \
      AlcFree(dump0); \
    if(dump1) \
      AlcFree(dump1); \
  } \
  return(alcErrno);

/*!
* \def		ALC_TEMPLATE_SYM_C2D(D,T,N,F)
* \ingroup	AlcArray
* \brief	A template for functions which allocate 2 dimensional
*		zero'd symetric arrays of any type. Obviously symetric
*               arrays are square, but in this representation only the
*		lower trinagle is stored.
* \param        D                     Destination pointer, of type T **.
* \param        T                     Type, eg char, short, int, ....
* \param        N                     Number of rows or columns.
* \param        F                     String with name of function.
*/
#define ALC_TEMPLATE_SYM_C2D(D,T,N,F) \
  size_t	totElm, \
  		index, \
		offset; \
  T		*dump0 = NULL; \
  T		**dump1 = NULL; \
  AlcErrno	alcErrno = ALC_ER_NONE; \
  \
  if((D) == NULL) \
  { \
    alcErrno = ALC_ER_NULLPTR; \
  } \
  else if((N) < 1) \
  { \
    alcErrno = ALC_ER_NUMELEM; \
  } \
  else \
  { \
    totElm = ((N) * ((N) + 1)) / 2; \
    if(((dump0 = (T*)AlcCalloc(totElm, sizeof(T))) == NULL) || \
        ((dump1 = (T**)AlcMalloc((N) * sizeof(T*))) == NULL)) \
    { \
      alcErrno = ALC_ER_ALLOC; \
    } \
  } \
  if(alcErrno == ALC_ER_NONE) \
  { \
    offset = 0; \
    *(D) = dump1; \
    for(index = 0; index < (N); ++index) \
    { \
      dump1[index] = dump0 + offset; \
      offset += index + 1; \
    } \
  } \
  else \
  { \
    if(D) \
    { \
      *(D) = NULL; \
    } \
    AlcFree(dump0); \
    AlcFree(dump1); \
  } \
  return(alcErrno);

/*!
* \def		ALC_TEMPLATE_SYM_M2D(D,T,N,F)
* \ingroup	AlcArray
* \brief	A template for functions which allocate 2 dimensional
*		zero'd symetric arrays of any type. Obviously symetric
*               arrays are square, but in this representation only the
*		lower trinagle is stored.
* \param        D                     Destination pointer, of type T **.
* \param        T                     Type, eg char, short, int, ....
* \param        N                     Number of rows or columns.
* \param        F                     String with name of function.
*/
#define ALC_TEMPLATE_SYM_M2D(D,T,N,F) \
  size_t	totElm, \
  		index, \
		offset; \
  T		*dump0 = NULL; \
  T		**dump1 = NULL; \
  AlcErrno	alcErrno = ALC_ER_NONE; \
  \
  if((D) == NULL) \
  { \
    alcErrno = ALC_ER_NULLPTR; \
  } \
  else if((N) < 1) \
  { \
    alcErrno = ALC_ER_NUMELEM; \
  } \
  else \
  { \
    totElm = ((N) * ((N) + 1)) / 2; \
    if(((dump0 = (T*)AlcMalloc(totElm * sizeof(T))) == NULL) || \
        ((dump1 = (T**)AlcMalloc((N) * sizeof(T*))) == NULL)) \
    { \
      alcErrno = ALC_ER_ALLOC; \
    } \
  } \
  if(alcErrno == ALC_ER_NONE) \
  { \
    offset = 0; \
    *(D) = dump1; \
    for(index = 0; index < (N); ++index) \
    { \
      dump1[index] = dump0 + offset; \
      offset += index + 1; \
    } \
  } \
  else \
  { \
    if(D) \
    { \
      *(D) = NULL; \
    } \
    AlcFree(dump0); \
    AlcFree(dump1); \
  } \
  return(alcErrno);

/*!
* \def		ALC_TEMPLATE_F2D(D,F)
* \ingroup	AlcArray
* \brief	A template for functions which free 2 dimensional
*		arrays of any type, actualy no type information
*		is used in freeing the array.
* \param        D                     Pointer for array to be free'd.
* \param        F                     String with name of function.
*/
#define ALC_TEMPLATE_F2D(D,F) \
  AlcErrno	alcErrno = ALC_ER_NONE; \
  \
  if((D == NULL) || (*(D) == NULL)) \
  { \
    alcErrno = ALC_ER_NULLPTR; \
  } \
  else \
  { \
    AlcFree(*(D)); \
    AlcFree(D); \
  } \
  return(alcErrno);


/*!
* \def		ALC_TEMPLATE_C3D(D,T,M,N,O,F)
* \ingroup	AlcArray
* \brief	A template for functions which allocate 3 dimensional
*		zero'd arrays of any type.
* \param        D                     Destination pointer, of type T **.
* \param        T                     Type, eg char, short, int, ....
* \param        M                     Number of 2D arrays.
* \param        N                     Number of 1D arrays.
* \param        O                     Number of elements in each 1D array.
* \param        F                     String with name of function.
*/
#define ALC_TEMPLATE_C3D(D,T,M,N,O,F) \
  size_t	index0, \
  		index1; \
  T		*dump0 = NULL, \
  		**dump1 = NULL, \
		***dump2 = NULL; \
  AlcErrno	alcErrno = ALC_ER_NONE; \
 \
  if((D) == NULL) \
    alcErrno = ALC_ER_NULLPTR; \
  else if(((M) < 1) || ((N) < 1) || ((O) < 1)) \
    alcErrno = ALC_ER_NUMELEM; \
  else if(((dump0 = (T*)AlcCalloc((M) * (N) * (O), sizeof(T))) == NULL) || \
          ((dump1 = (T**)AlcMalloc((M) * (N) * sizeof(T*))) == NULL) || \
          ((dump2 = (T***)AlcMalloc((M) * sizeof(T**))) == NULL)) \
    alcErrno = ALC_ER_ALLOC; \
  if(alcErrno == ALC_ER_NONE) \
  { \
    *(D) = dump2; \
    for(index0 = 0; index0 < (M); ++index0) \
    { \
      for(index1=0; index1 < (N); ++index1) \
      { \
	dump1[index1] = dump0; \
	dump0 += (O); \
      } \
      (*(D))[index0] = dump1; \
      dump1 += (N); \
    } \
  } \
  else \
  { \
    if(D) \
      *(D) = NULL; \
    if(dump2) \
      AlcFree(dump2); \
    if(dump1) \
      AlcFree(dump1); \
    if(dump0) \
      AlcFree(dump0); \
  } \
  return(alcErrno);

/*!
* \def		ALC_TEMPLATE_M3D(D,T,M,N,O,F)
* \ingroup	AlcArray
* \brief	A template for functions which allocate 3 dimensional
*		non-zero'd arrays of any type.
* \param        D                     Destination pointer, of type T **.
* \param        T                     Type, eg char, short, int, ....
* \param        M                     Number of 2D arrays.
* \param        N                     Number of 1D arrays.
* \param        O                     Number of elements in each 1D array.
* \param        F                     String with name of function.
*/
#define ALC_TEMPLATE_M3D(D,T,M,N,O,F) \
  size_t	index0, \
  		index1; \
  T		*dump0 = NULL, \
  		**dump1 = NULL, \
		***dump2 = NULL; \
  AlcErrno	alcErrno = ALC_ER_NONE; \
 \
  if((D) == NULL) \
    alcErrno = ALC_ER_NULLPTR; \
  else if(((M) < 1) || ((N) < 1) || ((O) < 1)) \
    alcErrno = ALC_ER_NUMELEM; \
  else if(((dump0 = (T*)AlcMalloc((M) * (N) * (O) * sizeof(T))) == NULL) || \
          ((dump1 = (T**)AlcMalloc((M) * (N) * sizeof(T*))) == NULL) || \
          ((dump2 = (T***)AlcMalloc((M) * sizeof(T**))) == NULL)) \
    alcErrno = ALC_ER_ALLOC; \
  if(alcErrno == ALC_ER_NONE) \
  { \
    *(D) = dump2; \
    for(index0 = 0; index0 < (M); ++index0) \
    { \
      for(index1=0; index1 < (N); ++index1) \
      { \
	dump1[index1] = dump0; \
	dump0 += (O); \
      } \
      (*(D))[index0] = dump1; \
      dump1 += (N); \
    } \
  } \
  else \
  { \
    if(D) \
      *(D) = NULL; \
    if(dump2) \
      AlcFree(dump2); \
    if(dump1) \
      AlcFree(dump1); \
    if(dump0) \
      AlcFree(dump0); \
  } \
  return(alcErrno); 

/*!
* \def		ALC_TEMPLATE_F3D(D,F)
* \ingroup	AlcArray
* \brief	A template for functions which free 3 dimensional
*		arrays of any type, actualy no type information
*		is used in freeing the array.
* \param        D                     Pointer for array to be free'd.
* \param        F                     String with name of function.
*/
#define ALC_TEMPLATE_F3D(D,F) \
  AlcErrno	alcErrno = ALC_ER_NONE; \
  \
  if((D == NULL) || (*(D) == NULL) || (**(D) == NULL)) \
  { \
    alcErrno = ALC_ER_NULLPTR; \
  } \
  else \
  { \
    AlcFree(**(D)); \
    AlcFree(*(D)); \
    AlcFree(D); \
  } \
  return(alcErrno);

#ifndef WLZ_EXT_BIND
#ifdef __cplusplus
}  					       /* Close scope of 'extern "C" */
#endif
#endif /* WLZ_EXT_BIND */

#endif /* ALCTEMPLATES_H */
