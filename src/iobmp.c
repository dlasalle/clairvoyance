/**
 * @file iobmp.c
 * @brief Functions for reading and writing from bmp files
 * @author Dominique LaSalle <lasalle@cs.umn.edu>
 * Copyright 2013
 * @version 1
 * @date 2013-03-06
 */




#ifndef CLAIRVOYANCE_IOBMP_C
#define CLAIRVOYANCE_IOBMP_C




#include "iobmp.h"




/******************************************************************************
* MACROS **********************************************************************
******************************************************************************/


#define PADDING (31)
#define WBITSIZE (32)
#define WBYTESIZE (WBITSIZE/8)




/******************************************************************************
* DOMLIB MACROS ***************************************************************
******************************************************************************/


#define DLMEM_PREFIX uint8
#define DLMEM_TYPE_T uint8_t
#define DLMEM_DLTYPE DLTYPE_INTEGRAL
#define DLMEM_STATIC
#include "dlmem_headers.h"
#undef DLMEM_STATIC
#undef DLMEM_DLTYPE
#undef DLMEM_TYPE_T
#undef DLMEM_PREFIX




/******************************************************************************
* PRIVATE FUNCTIONS ***********************************************************
******************************************************************************/


static int __read_int(
    FILE * const fin)
{
  int x;
  uint8_t bytes[4];
  if (fread(bytes,4,1,fin) != 1) {
    return -1;
  } else {
    x = bytes[0];
    x += bytes[1] << 8;
    x += bytes[2] << 8;
    x += bytes[3] << 8;
    return x;
  }
}


static short __read_short(
    FILE * const fin)
{
  int x;
  uint8_t bytes[2];
  if (fread(bytes,2,1,fin) != 1) {
    return -1;
  } else {
    x = bytes[0];
    x += bytes[1] << 8;
    return (short)x;
  }
}


static int __write_int(
    FILE * const fout, 
    const int i)
{
  uint8_t bytes[4];
  bytes[0] = i & 0xFF;
  bytes[1] = (i >> 8) & 0xFF;
  bytes[2] = (i >> 16) & 0xFF;
  bytes[3] = (i >> 24) & 0xFF;
  if (fwrite(bytes,4,1,fout) != 1) {
    return 0;
  } else {
    return 1;
  }
}


static int __write_short(
    FILE * const fout, 
    const short i)
{
  uint8_t bytes[2];
  bytes[0] = i & 0xFF;
  bytes[1] = (i >> 8) & 0xFF;
  if (fwrite(bytes,2,1,fout) != 1) {
    return 0;
  } else {
    return 1;
  }
}


static int __read_bmp_header(
    FILE * const fin, 
    bmp_header_t * const header)
{
  header->sig = __read_short(fin);
  header->file_size = __read_int(fin);
  header->reserved1 = __read_short(fin);
  header->reserved2 = __read_short(fin);
  header->pixel_array_offset = __read_int(fin);

  return BMP_SUCCESS;
}


static int __write_bmp_header(
    FILE * const fout, 
    bmp_header_t * const header)
{
  __write_short(fout,header->sig);
  __write_int(fout,header->file_size);
  __write_short(fout,header->reserved1);
  __write_short(fout,header->reserved2);
  __write_int(fout,header->pixel_array_offset);

  return BMP_SUCCESS;
}


static int __read_dib_header(
    FILE * const fin, 
    dib_header_t * const header)
{
  header->header_size = __read_int(fin);
  header->width = __read_int(fin);
  header->height = __read_int(fin);
  header->planes = __read_short(fin);
  header->bits_per_pixel = __read_short(fin);
  header->compression = __read_int(fin);
  header->image_size = __read_int(fin);

  return BMP_SUCCESS;
}


static int __write_dib_header(
    FILE * const fout, 
    dib_header_t * const header) 
{
  __write_int(fout,header->header_size);
  __write_int(fout,header->width);
  __write_int(fout,header->height);
  __write_short(fout,header->planes);
  __write_short(fout,header->bits_per_pixel);
  __write_int(fout,header->compression);
  __write_int(fout,header->image_size);
  __write_int(fout,header->x_pixels_per_meter);
  __write_int(fout,header->y_pixels_per_meter);
  __write_int(fout,header->colors_in_table);
  __write_int(fout,header->important_color_count);

  return BMP_SUCCESS;
}


static int __extract_rgb8(
    uint8_t const * const ptr, 
    size_t const offset, 
    real_t * const red, 
    real_t * const green, 
    real_t * const blue)
{
  *red = (real_t)ptr[offset];
  *green = (real_t)ptr[offset];
  *blue = (real_t)ptr[offset];

  return 1;
}


static int __extract_rgb24(
    uint8_t * const ptr, 
    size_t const offset, 
    real_t * const red, 
    real_t * const green, 
    real_t * const blue)
{
  *red = (real_t)ptr[offset*3+2];
  *green = (real_t)ptr[offset*3+1];
  *blue = (real_t)ptr[offset*3+0];

  /* printf("Read 24 bit pixel (%hhd, %hhd, %hhd)\n",ptr[offset*3+2],
      ptr[offset*3+1],ptr[offset*3+0]); */

  return 1;
}


static int __extract_rgb32(
    uint8_t const * const ptr, 
    size_t const offset, 
    real_t * const red, 
    real_t * const green, 
    real_t * const blue)
{
  *red = (real_t)ptr[offset*4+2];
  *green = (real_t)ptr[offset*4+1];
  *blue = (real_t)ptr[offset*4+0];

  /* printf("Read 32 bit pixel %zu (%hhd, %hhd, %hhd)\n",offset,ptr[offset*4+2],
      ptr[offset*4+1],ptr[offset*4+0]); */

  return 1;
}


static size_t __set_bmp(
    image_t const * const img, 
    bmp_header_t * const bmp_header, 
    dib_header_t * const dib_header, 
    uint8_t ** r_pixel_array)
{
  size_t i,j;
  size_t rowbytes,idx,px;
  int bpp = 32;

  rowbytes = ((bpp*img->width+PADDING)/WBITSIZE)*WBYTESIZE;

  *r_pixel_array = uint8_alloc(rowbytes*img->height);

  /* set bmp values */
  bmp_header->sig = 0x4d42;
  bmp_header->pixel_array_offset = 14+40;
  bmp_header->file_size = bmp_header->pixel_array_offset + 
      (rowbytes*img->height);

  /* set dib values */
  dib_header->header_size = 40;
  dib_header->width = img->width;
  dib_header->height = img->height;
  dib_header->bits_per_pixel = bpp;
  dib_header->compression = 0;
  dib_header->image_size = (rowbytes*img->height); 
  dib_header->planes = 1;
  dib_header->x_pixels_per_meter = 8096;
  dib_header->y_pixels_per_meter = 8096;
  dib_header->colors_in_table = 0; 
  dib_header->important_color_count = 0;

  for (i=0;i<img->height;++i) {
    for (j=0;j<img->width;++j) {
      idx = (j*4) + ((img->height-i-1)*rowbytes);
      px = j+(i*img->width);
      (*r_pixel_array)[idx+3] = 0xFF;
      (*r_pixel_array)[idx+2] = (uint8_t)img->red[px];
      (*r_pixel_array)[idx+1] = (uint8_t)img->green[px];
      (*r_pixel_array)[idx+0] = (uint8_t)img->blue[px];
    }
  }

  return rowbytes*img->height; 
}




/******************************************************************************
* PUBLIC FUNCTIONS ************************************************************
******************************************************************************/


image_t * bmp_read(
    char const * const filename)
{
  size_t i,j;
  size_t rowbytes, idx;

  FILE * fin = NULL;
  uint8_t * rowdata = NULL;
  image_t * img = NULL;
  bmp_header_t bmp_header;
  dib_header_t dib_header;

  if ((fin = fopen(filename,"rb")) == NULL) {
    eprintf("Failed to open '%s' for reading\n",filename);
    goto DIE;
  } 

  /* read the headers */
  if (__read_bmp_header(fin,&bmp_header) != BMP_SUCCESS) {
    eprintf("Failed to read BMP_HEADER\n");
    goto DIE;
  }

  if (__read_dib_header(fin,&dib_header) != BMP_SUCCESS) {
    eprintf("Failed to read DIB_HEADER\n");
    goto DIE;
  }

  /* allocate the image */
  img = image_create(dib_header.width,dib_header.height,NULL,NULL,NULL);
  
  /* move reader to the pixel array */
  fseek(fin,bmp_header.pixel_array_offset,SEEK_SET);

  /* read the pixel array */
  rowbytes = (
      (dib_header.bits_per_pixel*dib_header.width+PADDING)/WBITSIZE
    )*WBYTESIZE;
  rowdata = uint8_alloc(rowbytes);
  for (i=0;i<(size_t)dib_header.height;++i) {
    /* read each row */
    if (fread(rowdata,1,rowbytes,fin) < rowbytes) {
      eprintf("Failed to read row %zu\n",i);
      goto DIE;
    }
    for (j=0;j<(size_t)dib_header.width;++j) {
      idx = j + (i*dib_header.width);
      switch (dib_header.bits_per_pixel) {
        case 8:
          __extract_rgb8(rowdata,j,img->red+idx,img->green+idx,img->blue+idx);
          break;
        case 24:
          __extract_rgb24(rowdata,j,img->red+idx,img->green+idx,img->blue+idx);
          break;
        case 32:
          __extract_rgb32(rowdata,j,img->red+idx,img->green+idx,img->blue+idx);
          break;
        default:
          eprintf("Unsupported bits per pixel of %d\n",
              dib_header.bits_per_pixel);
          goto DIE;
      }
    }
  }
  fclose(fin);
  fin = NULL;

  dl_free(rowdata);

  return img;

  /* clean up */
  DIE:
  if (fin) {
    fclose(fin);
  }
  if (img) {
    image_free(img);
  }
  if (rowdata) {
    dl_free(rowdata);
  }
  return NULL;
}


int bmp_write(
    char const * const filename, 
    image_t const * const image)
{
  bmp_header_t bmp_header;
  dib_header_t dib_header;
  uint8_t * pixel_array = NULL;
  FILE * fout = NULL;

  size_t pa_size = __set_bmp(image,&bmp_header,&dib_header,&pixel_array);

  if ((fout = fopen(filename,"wb")) == NULL) {
    eprintf("Failed to open '%s' for writing\n",filename);
    goto DIE;
  }

  if (__write_bmp_header(fout,&bmp_header) != BMP_SUCCESS) {
    eprintf("Failed to write BMP_HEADER\n");
    goto DIE;
  }

  if (__write_dib_header(fout,&dib_header) != BMP_SUCCESS) {
    eprintf("Failed to write DIB_HEADER\n");
    goto DIE;
  }

  if (fwrite(pixel_array,1,pa_size,fout) != pa_size) {
    eprintf("Failed to write PIXEL_ARRAY\n");
    goto DIE;
  }

  dl_free(pixel_array);
  fclose(fout);

  return BMP_SUCCESS;

  DIE:
  if (fout) {
    fclose(fout);
  }
  if (pixel_array) {
    dl_free(pixel_array);
  }
  return BMP_ERROR_WRITE;
}




#undef PADDING
#undef WSIZE




#endif
