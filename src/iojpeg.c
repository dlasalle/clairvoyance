/**
 * @file iojpeg.c
 * @brief JPEG Writing functions
 * @author Dominique LaSalle <lasalle@cs.umn.edu>
 * Copyright 2013
 * @version 1
 * @date 2013-06-30
 */




#ifndef CLAIRVOYANCE_IOJPEG_C
#define CLAIRVOYANCE_IOJPEG_C




#ifndef NO_JPEG_SUPPORT
/* #include <libjpeg.h> */
#include <stdlib.h>
#include <stdio.h>
#include <jpeglib.h>
#endif

#include "iojpeg.h"




/******************************************************************************
* PUBLIC FUNCTIONS ************************************************************
******************************************************************************/


int jpeg_write(
    char const * const filename, 
    image_t const * const image)
{
  #ifndef NO_JPEG_SUPPORT
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  unsigned char * stride;

  size_t i,j,idx;
  FILE * fp;

  stride = (unsigned char*)malloc(image->width*3);

  fp = fopen(filename,"w");

  jpeg_create_compress(&cinfo);
  jpeg_stdio_dest(&cinfo,fp);

  cinfo.image_width = image->width;
  cinfo.image_height = image->height;
  cinfo.input_components = 3;
  cinfo.in_color_space = JCS_RGB;
  cinfo.err = jpeg_std_error(&jerr);

  jpeg_set_defaults(&cinfo);
  jpeg_start_compress(&cinfo,FALSE);

  for (i=0;i<image->height;++i) {
    for (j=0;j<image->width;++j) {
      idx = i*image->width + j;
      stride[j*3] = image->red[idx];
      stride[j*3+1] = image->green[idx];
      stride[j*3+2] = image->blue[idx];
    }
    jpeg_write_scanlines(&cinfo,&stride,1);
  }

  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);

  free(stride);

  fclose(fp);

  return 1;
  #else
  fprintf(stderr,"Built without JPEG support.\n");
  return 0;
  #endif
}




#endif
