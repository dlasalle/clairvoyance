/**
 * @file iopng.c
 * @brief Functions for writing PNGs
 * @author Dominique LaSalle <lasalle@cs.umn.edu>
 * Copyright 2013
 * @version 1
 * @date 2013-06-30
 */




#ifndef CLAIRVOYANCE_IOPNG_C
#define CLAIRVOYANCE_IOPNG_C




#include "iopng.h"

#ifndef NO_PNG_SUPPORT
#include <png.h>
#endif




/******************************************************************************
* PUBLIC FUNCTIONS ************************************************************
******************************************************************************/


int png_write(
    char const * const filename, 
    image_t const * const image) 
{
  #ifndef NO_PNG_SUPPORT
  size_t i,j,idx;
  FILE * fout = NULL;
  png_structp png_ptr = NULL;
  png_infop info_ptr = NULL;
  png_byte ** rows;

  fout = fopen(filename,"w");
  if (fout == NULL) {
    dl_error("Faild to open '%s' for writing\n",filename);
    perror("Failed due to:");
    return 0;
  }

  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
  if (png_ptr == NULL) {
    dl_error("Failed to initialize PNG library writer.\n");
    fclose(fout);
    return 0;
  }

  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    dl_error("Failed to initialize PNG library info.\n");
    png_destroy_write_struct(&png_ptr,NULL);
    fclose(fout);
    return 0;
  }

  png_set_IHDR(png_ptr,info_ptr,image->width,image->height,8,
      PNG_COLOR_TYPE_RGB,PNG_INTERLACE_NONE,
      PNG_COMPRESSION_TYPE_DEFAULT,PNG_FILTER_TYPE_DEFAULT);

  rows = png_malloc(png_ptr,image->height*sizeof(png_byte*));

  for (i=0;i<(size_t)image->height;++i) {
    rows[i] = png_malloc(png_ptr,image->width*sizeof(png_byte)*3);
    for (j=0;j<(size_t)image->width;++j) {
      idx = i*image->width+j;
      rows[i][j*3] = image->red[idx];
      rows[i][j*3+1] = image->green[idx];
      rows[i][j*3+2] = image->blue[idx];
    }
  }

  png_init_io(png_ptr,fout);
  png_set_rows(png_ptr,info_ptr,rows);
  png_write_png(png_ptr,info_ptr,PNG_TRANSFORM_IDENTITY,NULL);

  for (i=0;i<(size_t)image->height;++i) {
    png_free(png_ptr,rows[i]);
  }
  png_free(png_ptr,rows);
  png_destroy_write_struct(&png_ptr,&info_ptr);
  fclose(fout);

  return 1;
  #else
  fprintf(stderr,"Built without PNG support.\n");
  return 0;
  #endif
}




#endif
