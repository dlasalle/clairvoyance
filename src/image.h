/**
 * @file image.h
 * @brief Image type and function prototypes
 * @author Dominique LaSalle <lasalle@cs.umn.edu>
 * Copyright 2013
 * @date 2013-03-07
 */




#ifndef CLAIRVOYANCE_IMAGE_H
#define CLAIRVOYANCE_IMAGE_H




#include "base.h"
#include "io.h"




/******************************************************************************
* TYPES ***********************************************************************
******************************************************************************/


typedef enum color_t {
  RED,
  GREEN,
  BLUE
} color_t;


typedef struct image_t {
  size_t width;
  size_t height;
  real_t * red;
  real_t * blue;
  real_t * green;
} image_t;




/******************************************************************************
* PROTOTYPES ******************************************************************
******************************************************************************/


void image_init(
    image_t * image);


void image_free(
    image_t * image);


image_t * image_create(
    size_t width, 
    size_t height, 
    real_t * red, 
    real_t * blue, 
    real_t * green);


image_t * image_create_grayscale(
    real_t const * mat, 
    size_t x, 
    size_t y,
    size_t nx, 
    size_t ny);


image_t * image_create_heatmap(
    real_t const * mat, 
    size_t x, 
    size_t y,
    size_t nx, 
    size_t ny);




#endif
