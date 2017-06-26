/**
 * @file base.h
 * @brief Basic things 
 * @author Dominique LaSalle <lasalle@cs.umn.edu>
 * Copyright 2013
 * @version 1
 * @date 2013-06-20
 */




#ifndef CLAIRVOYANCE_BASE_H
#define CLAIRVOYANCE_BASE_H




#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <domlib.h>


#include "clairvoyance.h"




/******************************************************************************
* DEFINES *********************************************************************
******************************************************************************/


#define COLOR_BLACKWHITE_STRING "blackwhite"
#define COLOR_WHITEBLACK_STRING "whiteblack"
#define COLOR_GRAYSCALE_STRING "grayscale"
#define COLOR_INVGRAYSCALE_STRING "invgrayscale"
#define COLOR_HEATMAP_STRING "heatmap"
#define COLOR_INVHEATMAP_STRING "invheatmap"
#define FILETYPE_METIS_STRING "metis"
#define FILETYPE_CLUTO_STRING "cluto"
#define FILETYPE_CSR_STRING "csr"
#define FILETYPE_CSR_HEADER_STRING "csr_header"
#define FILETYPE_RAW_STRING "raw"
#define FILETYPE_DENSE_STRING "dense"
#define FILETYPE_DIMACS_STRING "dimacs"
#define FILETYPE_COO_STRING "coo"
#define FILETYPE_POINT_STRING "point"
#define FILETYPE_BMP_STRING "bmp"
#define FILETYPE_JPEG_STRING "jpeg"
#define FILETYPE_PNG_STRING "png"
#define FILETYPE_AUTO_STRING "auto"
#define FILETYPE_UNKNOWN_STRING "unknown"
#define FUNCTION_DENSITY_STRING "density"
#define FUNCTION_MAX_STRING "max"
#define FUNCTION_AVERAGE_STRING "average"




/******************************************************************************
* TYPES ***********************************************************************
******************************************************************************/


typedef enum clairvoyance_error_t {
  CLAIRVOYANCE_SUCCESS,
  CLAIRVOYANCE_ERROR_INVALIDINPUT
} clairvoyance_error_t;


typedef enum filetype_t {
  FILETYPE_METIS,
  FILETYPE_CLUTO,
  FILETYPE_CSR,
  FILETYPE_CSR_HEADER,
  FILETYPE_RAW,
  FILETYPE_DENSE,
  FILETYPE_DIMACS,
  FILETYPE_COO,
  FILETYPE_POINT,
  FILETYPE_BMP,
  FILETYPE_JPEG,
  FILETYPE_PNG,
  FILETYPE_AUTO,
  FILETYPE_UNKNOWN
} filetype_t;


typedef enum colortype_t {
  COLOR_BLACKWHITE,
  COLOR_WHITEBLACK,
  COLOR_GRAYSCALE,
  COLOR_INVGRAYSCALE,
  COLOR_HEATMAP,
  COLOR_INVHEATMAP,
  COLOR_UNKNOWN
} colortype_t;


typedef enum functiontype_t {
  FUNCTION_DENSITY,
  FUNCTION_MAX,
  FUNCTION_AVERAGE
} functiontype_t;


typedef enum storagetype_t {
  STORAGE_ROW,
  STORAGE_POINT
} storagetype_t;


typedef double real_t;




/******************************************************************************
* DOMLIB IMPORTS **************************************************************
******************************************************************************/



#define DLMEM_PREFIX real
#define DLMEM_TYPE_T real_t
#include "dlmem_headers.h"
#undef DLMEM_TYPE_T
#undef DLMEM_PREFIX




/******************************************************************************
* CONSTANTS *******************************************************************
******************************************************************************/


static const size_t DEFAULT_BUFFER_SIZE = 4096;


static const char * const FILETYPE_NAMES[] = {
  [FILETYPE_METIS] = FILETYPE_METIS_STRING,
  [FILETYPE_CLUTO] = FILETYPE_CLUTO_STRING,
  [FILETYPE_CSR] = FILETYPE_CSR_STRING,
  [FILETYPE_CSR_HEADER] = FILETYPE_CSR_HEADER_STRING,
  [FILETYPE_RAW] = FILETYPE_RAW_STRING,
  [FILETYPE_DENSE] = FILETYPE_DENSE_STRING,
  [FILETYPE_DIMACS] = FILETYPE_DIMACS_STRING,
  [FILETYPE_COO] = FILETYPE_COO_STRING,
  [FILETYPE_POINT] = FILETYPE_POINT_STRING,
  [FILETYPE_BMP] = FILETYPE_BMP_STRING,
  [FILETYPE_JPEG] = FILETYPE_JPEG_STRING,
  [FILETYPE_PNG] = FILETYPE_PNG_STRING,
  [FILETYPE_AUTO] = FILETYPE_AUTO_STRING,
  [FILETYPE_UNKNOWN] = FILETYPE_UNKNOWN_STRING
};


static const char * const COLOR_NAMES[] = {
  [COLOR_BLACKWHITE] = COLOR_BLACKWHITE_STRING,
  [COLOR_WHITEBLACK] = COLOR_WHITEBLACK_STRING,
  [COLOR_GRAYSCALE] = COLOR_GRAYSCALE_STRING,
  [COLOR_INVGRAYSCALE] = COLOR_INVGRAYSCALE_STRING,
  [COLOR_HEATMAP] = COLOR_HEATMAP_STRING,
  [COLOR_INVHEATMAP] = COLOR_INVHEATMAP_STRING,
};




/******************************************************************************
* INLINE FUNCTIONS ************************************************************
******************************************************************************/


static inline filetype_t translate_filetype(const char * const name)
{
  size_t i;
  for (i=0;i<sizeof(FILETYPE_NAMES)/sizeof(char*);++i) {
    if (strcmp(FILETYPE_NAMES[i],name) == 0) {
      return (filetype_t)i;
    }
  }
  dl_error("Unknown file type '%s'\n",name);
  return FILETYPE_UNKNOWN;
}


static inline int translate_colortype(const char * const name)
{
  int i;
  for (i=0;i<(int)(sizeof(COLOR_NAMES)/sizeof(char*));++i) {
    if (strcmp(name,COLOR_NAMES[i]) == 0) {
      return i;
    }
  }
  dl_error("Failed to find color type '%s'\n",name);
  return COLOR_UNKNOWN;
}




#endif
