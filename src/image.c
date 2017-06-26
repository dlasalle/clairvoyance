/**
 * @file image.c
 * @brief Image related functions
 * @author Dominique LaSalle <lasalle@cs.umn.edu>
 * Copyright 2013
 * @date 2013-03-07
 */




#ifndef CLAIRVOYANCE_IMAGE_C
#define CLAIRVOYANCE_IMAGE_C




#include "base.h"
#include "image.h"




/******************************************************************************
* DOMLIB IMPORTS **************************************************************
******************************************************************************/


#define DLMEM_PREFIX image
#define DLMEM_TYPE_T image_t
#define DLMEM_DLTYPE DLTYPE_STRUCT
#define DLMEM_INITFUNCTION image_init
#define DLMEM_STATIC
#include "dlmem_headers.h"
#undef DLMEM_STATIC
#undef DLMEM_INITFUNCTION
#undef DLMEM_DLTYPE
#undef DLMEM_TYPE_T
#undef DLMEM_PREFIX




/******************************************************************************
* PUBLIC FUNCTIONS ************************************************************
******************************************************************************/


void image_init(
    image_t * const img)
{
  img->width = 0;
  img->height = 0;
  img->red = NULL;
  img->blue = NULL;
  img->green = NULL;
}


void image_free(
    image_t * img)
{
  if (img->red) {
    dl_free(img->red);
  }
  if (img->green) {
    dl_free(img->green);
  }
  if (img->blue) {
    dl_free(img->blue);
  }
  dl_free(img);
}



image_t * image_create(
    size_t const width, 
    size_t const height, 
    real_t * const red, 
    real_t * const green, 
    real_t * const blue)
{
  image_t * const img = image_calloc(1);

  /* handle attributes */
  img->width = width;
  img->height = height;

  /* handle color arrays */
  if (red) {
    img->red = red;
  } else {
    img->red = real_calloc(img->width*img->height);
  }
  if (green) {
    img->green = green;
  } else {
    img->green = real_calloc(img->width*img->height);
  }
  if (blue) {
    img->blue = blue;
  } else {
    img->blue = real_calloc(img->width*img->height);
  }

  return img;
}


image_t * image_create_grayscale(
    real_t const * mat, 
    size_t const x, 
    size_t const y, 
    size_t const nx, 
    size_t const ny)
{
  size_t i,j, idx;

  real_t * red, * green, * blue;

  red = real_alloc(nx*ny);
  green = real_alloc(nx*ny);
  blue = real_alloc(nx*ny);

  for (i=0;i<ny;++i) {
    for (j=0;j<nx;++j) {
      idx = ((i*y)/ny)*x + ((j*x)/nx);
      red[i*nx+j] = mat[idx];
      green[i*nx+j] = mat[idx];
      blue[i*nx+j] = mat[idx];
    }
  }

  return image_create(nx,ny,red,green,blue);
}


image_t * image_create_heatmap(
    real_t const * mat, 
    size_t const x, 
    size_t const y, 
    size_t const nx, 
    size_t const ny)
{
  size_t i,j, idx;

  real_t * red, * green, * blue;

  red = real_alloc(nx*ny);
  green = real_alloc(nx*ny);
  blue = real_alloc(nx*ny);

  for (i=0;i<ny;++i) {
    for (j=0;j<nx;++j) {
      idx = ((i*y)/ny)*x + ((j*x)/nx);
      if (mat[idx] < 255.0) {
        red[i*nx+j] = 0;
        green[i*nx+j] = 0;
        blue[i*nx+j] = mat[idx];
      } else if (mat[idx] < 510.0) {
        red[i*nx+j] = 0;
        green[i*nx+j] = mat[idx] - 255.0;
        blue[i*nx+j] = 510.0 - mat[idx];
      } else if (mat[idx] < 765.0) {
        red[i*nx+j] = mat[idx] - 510.0;
        green[i*nx+j] = 765.0 - mat[idx];
        blue[i*nx+j] = 0;
      } else {
        red[i*nx+j] = 255.0;
        green[i*nx+j] = mat[idx] - 765.0; 
        blue[i*nx+j] = mat[idx] - 765.0; 
      }
    }
  }

  return image_create(nx,ny,red,green,blue);
}




#endif
