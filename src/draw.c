/**
 * @file draw.c
 * @brief Drawing functions
 * @author Dominique LaSalle <lasalle@cs.umn.edu>
 * Copyright 2014
 * @version 1
 * @date 2014-06-27
 */




#ifndef CLAIRVOYANCE_DRAW_C
#define CLAIRVOYANCE_DRAW_C




#include "draw.h"




/******************************************************************************
* PRIVATE FUNCTIONS ***********************************************************
******************************************************************************/


static inline int __normalize(
    real_t * const val, 
    size_t const n, 
    real_t const nmin, 
    real_t const nmax) 
{
  real_t max, min, scale, range;
  size_t i;
  const real_t nrange = nmax -nmin;

  val[0] = min = max = sqrt(val[0]);

  for (i=1;i<n;++i) {
    val[i] = sqrt(val[i]);
    if (val[i] > max) {
      max = val[i];
    }
    if (val[i] < min) {
      min = val[i];
    }
  }
  range = max - min;
  if (range == 0) {
    if (min == 0) {
      scale = 0;
    } else {
      scale = nmin / min;
    }
  } else {
    scale = nrange / range;
  }
  for (i=0;i<n;++i) {
    val[i] = ((val[i]-min)*scale)+nmin;
  }
  return 1;
}


static inline int __invert(
    real_t * const val, 
    size_t const n, 
    real_t const max)
{
  size_t i;
  for (i=0;i<n;++i) {
    val[i] = max - val[i];
  }
  return 1;
}


static inline int __truncate(
    real_t * const val, 
    size_t const n, 
    real_t const thresh, 
    real_t const off, 
    real_t const on) 
{
  size_t i;
  for (i=0;i<n;++i) {
    if (val[i] > thresh) {
      val[i] = on;
    } else {
      val[i] = off;
    }
  }
  return 1;
}


static int __fix(
    real_t * const val, 
    size_t const n, 
    size_t const ne, 
    real_t const max) 
{
  size_t i;
  const real_t invrne = max / sqrt(ne);

  for (i=0;i<n;++i) {
    val[i] = sqrt(val[i])*invrne;
  }
  return 1;
}




/******************************************************************************
* PUBLIC FUNCTIONS ************************************************************
******************************************************************************/


image_t * draw_matrix_file(
    char const * const filein, 
    filetype_t const ftype, 
    colortype_t const ctype, 
    functiontype_t const func, 
    size_t const nx, 
    size_t const ny)
{
  size_t i;
  image_t * img = NULL;
  real_t * out, * den = NULL, * max = NULL, * avg = NULL;
  size_t x,y;

  spmat_handle_t * handle = open_matrix(filein,ftype);

  x = dl_min(nx,handle->ncols);
  y = dl_min(ny,handle->nrows);

  out = real_init_alloc(0.0,x*y);

  switch (func) {
    case FUNCTION_DENSITY:
      den = out;
      break;
    case FUNCTION_MAX:
      max = out;
      break;
    case FUNCTION_AVERAGE:
      avg = out;
      break;
    default:
      dl_error("Unsupported function type %d\n",func);
      break;
  }
  if (handle->use_rows) {
    for (i=0;i<handle->nrows;++i) {
      read_row(handle,(i*y)/handle->nrows,den,max,avg,x);
    }
  } else {
    read_points(handle,x,y,den,max,avg);
  }

  close_matrix(handle);

  switch (ctype) {
    case COLOR_BLACKWHITE:
      __truncate(out,x*y,0,0,255.0); 
      img = image_create_grayscale(out,x,y,nx,ny);
      break;
    case COLOR_WHITEBLACK:
      __truncate(out,x*y,0,0,255.0); 
      __invert(out,x*y,255.0);
      img = image_create_grayscale(out,x,y,nx,ny);
      break;
    case COLOR_GRAYSCALE:
      __normalize(out,x*y,0,255.0);
      img = image_create_grayscale(out,x,y,nx,ny);
      break;
    case COLOR_INVGRAYSCALE:
      __normalize(out,x*y,0,255.0);
      __invert(out,x*y,255.0);
      img = image_create_grayscale(out,x,y,nx,ny);
      break;
    case COLOR_HEATMAP:
      __normalize(out,x*y,0,1020.0);
      img = image_create_heatmap(out,x,y,nx,ny);
      break;
    case COLOR_INVHEATMAP:
      __normalize(out,x*y,0,1020.0);
      __invert(out,x*y,1020.0);
      img = image_create_heatmap(out,x,y,nx,ny);
      break;
    default:
      dl_error("Unsupported coloring type %d\n",ctype);
      break;
  }

  dl_free(out);

  return img;
}




#endif
