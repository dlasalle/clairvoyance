/**
 * @file iobmp.h
 * @brief Functions prototypes and structs for reading and writing bmp files
 * @author Dominique LaSalle <lasalle@cs.umn.edu>
 * Copyright 2013
 * @version 1
 * @date 2013-03-06
 */




#ifndef CLAIRVOYANCE_IOBMP_H
#define CLAIRVOYANCE_IOBMP_H




#include "base.h"
#include "image.h"




/******************************************************************************
* TYPES ***********************************************************************
******************************************************************************/


typedef enum bmp_error_t {
  BMP_SUCCESS = 1,
  BMP_ERROR_READ = -1,
  BMP_ERROR_WRITE = -2,
  BMP_ERROR_OPEN = -3
} bmp_error_t;


typedef struct bmp_header_t {
  short sig;
  int file_size;
  short reserved1;
  short reserved2;
  int pixel_array_offset;
} bmp_header_t;


typedef struct dib_header_t {
  int header_size;
  int width;
  int height;
  short planes;
  short bits_per_pixel;
  int compression;
  int image_size;
  int x_pixels_per_meter;
  int y_pixels_per_meter;
  int colors_in_table;
  int important_color_count;
} dib_header_t;




/******************************************************************************
* FUNCTION PROTOTYPES *********************************************************
******************************************************************************/


image_t * bmp_read(
    char const * filename); 


int bmp_write(
    char const * filename, 
    image_t const * image);




#endif

