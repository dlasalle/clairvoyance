/**
 * @file clairvoyance_bin.c
 * @brief The main function and command line partsing
 * @author Dominique LaSalle <lasalle@cs.umn.edu>
 * Copyright 2013
 * @version 1
 * @date 2013-06-30
 */




#ifndef CLAIRVOYANCE_BIN_C
#define CLAIRVOYANCE_C




#include "base.h"
#include "draw.h"
#include "iojpeg.h"
#include "iobmp.h"
#include "iopng.h"




/******************************************************************************
* CONSTANTS *******************************************************************
******************************************************************************/


typedef enum clairvoyance_option_t {
  OPTION_COLOR,
  OPTION_FUNCTION,
  OPTION_SIZE,
  OPTION_INPUTTYPE,
  OPTION_HELP
} clairvoyance_option_t;


static const cmd_opt_pair_t COLOR_CHOICES[] = {
  {COLOR_BLACKWHITE_STRING,"Pixels containing no non-zeros are colored black, "
    "and pixels containing at least one non-zero are colored white.",
    COLOR_BLACKWHITE},
  {COLOR_WHITEBLACK_STRING,"Pixels containing no non-zeros are colored white, "
    "and pixels containing at least one non-zero are colored black.",
    COLOR_WHITEBLACK},
  {COLOR_GRAYSCALE_STRING,"Pixels containing no non-zeros are colored black, "
    "and pixels containing non-zeros are shaded gray depending on the "
    "density of non-zeros.",COLOR_GRAYSCALE},
  {COLOR_INVGRAYSCALE_STRING,"Pixels containing no non-zeros are colored "
    "white, and pixels containing non-zeros are shaded gray depending on the "
    "density of non-zeros.",COLOR_INVGRAYSCALE},
  {COLOR_HEATMAP_STRING,"Pixels containing no non-zeros are colored black, "
    "and pixels containing non-zeros are colored a shade of blue, green, red, "
    "and white.",COLOR_HEATMAP},
  {COLOR_INVHEATMAP_STRING,"Pixels containing no non-zeros are colored white, "
    "and pixels containing non-zeros are colored a shade of red, green, blue, "
    "and black.",COLOR_INVHEATMAP}
};


static const cmd_opt_pair_t INPUT_CHOICES[] = {
  {FILETYPE_METIS_STRING,"Metis graph format",FILETYPE_METIS},
  {FILETYPE_CLUTO_STRING,"Cluto matrix format",FILETYPE_CLUTO},
  {FILETYPE_CSR_STRING,"CSR without a header matrix format",FILETYPE_CSR},
  {FILETYPE_CSR_HEADER_STRING,"CSR with a header matrix format",
    FILETYPE_CSR_HEADER},
  {FILETYPE_POINT_STRING,"Point (ijv) matrix format",FILETYPE_POINT},
  {FILETYPE_AUTO_STRING,"Determine the file format from the filename.",
    FILETYPE_AUTO}
};


static const cmd_opt_pair_t FUNCTION_CHOICES[] = {
  {FUNCTION_DENSITY_STRING,"Intensity based on the density of non-zero "
    "values in a pixel.",FUNCTION_DENSITY},
  {FUNCTION_MAX_STRING,"Intensity based on the maximum value "
    "in a pixel.",FUNCTION_MAX},
  {FUNCTION_AVERAGE_STRING,"Intensity based on the average value "
    "in a pixel.",FUNCTION_AVERAGE}
};


static const cmd_opt_t OPTS[] = {
  {OPTION_HELP,'h',"help","Display this help page.",CMD_OPT_FLAG,NULL,0},
  {OPTION_COLOR,'c',"color","The coloring of zeros and non-zeros to use.",
    CMD_OPT_CHOICE,COLOR_CHOICES,sizeof(COLOR_CHOICES)/sizeof(cmd_opt_pair_t)},
  {OPTION_INPUTTYPE,'i',"inputtype","The format of the input (default will "
    "guess it from the filename).",CMD_OPT_CHOICE,INPUT_CHOICES,
    sizeof(INPUT_CHOICES)/sizeof(cmd_opt_pair_t)},
  {OPTION_FUNCTION,'f',"function","The function to use for pixel values.",
    CMD_OPT_CHOICE,FUNCTION_CHOICES,
    sizeof(FUNCTION_CHOICES)/sizeof(cmd_opt_pair_t)},
  {OPTION_SIZE,'s',"size","The size of the image to be rendered.",
    CMD_OPT_STRING,NULL,0}
};


static const size_t NOPTS = sizeof(OPTS)/sizeof(cmd_opt_t);




/******************************************************************************
* PRIVATE FUNCTIONS ***********************************************************
******************************************************************************/


static inline int __endswith(
    char const * const str, 
    char const * const ext)
{
  size_t lstr, lext;
  if (!str || !ext) {
    return 0;
  } else {
    lstr = strlen(str);
    lext = strlen(ext);
    if (lext > lstr) {
      return 0;
    } else {
      return strncmp(str+lstr-lext,ext,lext) == 0;
    }
  }
}


static void __usage(
    FILE * const out, 
    char const * const name)
{
  fprintf(out,"Clairvoyance Version %d.%d.%d\n",CLAIRVOYANCE_VER_MAJOR, \
      CLAIRVOYANCE_VER_MINOR,CLAIRVOYANCE_VER_SUBMINOR);
  fprintf(out,"USAGE:\n");
  fprintf(out,"%s [options] <inputfile> <outputfile>\n",name);
  fprintf(out,"\n");
  fprintf(out,"Options:\n");
  fprint_cmd_opts(out,OPTS,NOPTS);
}




/******************************************************************************
* PUBLIC FUNCTIONS ************************************************************
******************************************************************************/


int main(
    int argc, 
    char ** argv) 
{
  int err;
  size_t nargs;
  image_t * img;
  const char * infile, * outfile;
  filetype_t otype;
  filetype_t itype;
  colortype_t ctype;
  functiontype_t ftype;
  cmd_arg_t * args;
  size_t i, xarg, width, height;

  args = NULL;
  height = width = 512;
  ctype = COLOR_HEATMAP;
  itype = FILETYPE_AUTO;
  otype = FILETYPE_AUTO;
  ftype = FUNCTION_DENSITY;
  outfile = NULL;
  infile = NULL;
  img = NULL;
  err = CLAIRVOYANCE_SUCCESS;

  err = cmd_parse_args(argc-1,argv+1,OPTS,NOPTS,&args,&nargs);
  if (err != DL_CMDLINE_SUCCESS) {
    __usage(stderr,argv[0]);
    err = CLAIRVOYANCE_ERROR_INVALIDINPUT;
    goto END;
  }

  /* handle args */
  for (i=0;i<nargs;++i) {
    if (args[i].type == CMD_OPT_XARG) {
      /* parse later */
    } else {
      switch(args[i].id) {
        case OPTION_COLOR:
          ctype = (colortype_t)args[i].val.o;
          break;
        case OPTION_INPUTTYPE:
          itype = (filetype_t)args[i].val.o;
          break;
        case OPTION_FUNCTION:
          ftype = (functiontype_t)args[i].val.o;
          break;
        case OPTION_SIZE:
          if (sscanf(args[i].val.s,"%zux%zu",&width,&height) != 2) {
            eprintf("Invalid size format '%s', should be in the format "
                "<width>x<height> (ie. 256x256)\n",args[i].val.s);
            err = CLAIRVOYANCE_ERROR_INVALIDINPUT;
            goto END;
          }
          break;
        case OPTION_HELP:
          __usage(stdout,argv[0]);
          return 0;
          break;
        default:
          break; /* cmdline.c handles validity */
      }
    }
  }
  xarg = 0;
  for (i=0;i<(size_t)(argc-1);++i) {
    if (args[i].type == CMD_OPT_XARG) {
      switch (xarg) {
        case 0:
          infile = args[i].val.s;
          if (itype == FILETYPE_AUTO) {
            if (__endswith(infile,".metis") || __endswith(infile,".chaco") ||
                __endswith(infile,".graph")) {
              itype = FILETYPE_METIS;
            } else if (__endswith(infile,".cluto") || 
                __endswith(infile,".clu")) {
              itype = FILETYPE_CLUTO;
            } else if (__endswith(infile,".csr")) {
              itype = FILETYPE_CSR;
            } else if (__endswith(infile,".ij")) {
              itype = FILETYPE_POINT;
            } else {
              eprintf("Unknown input filetype: '%s'\n",outfile);
              err = CLAIRVOYANCE_ERROR_INVALIDINPUT;
              goto END;
            }
          }
          break;
        case 1:
          outfile = args[i].val.s;
          if (strlen(outfile) < 5) {
            eprintf("Invalid output file extension '%s'\n",argv[3]); 
            err = CLAIRVOYANCE_ERROR_INVALIDINPUT;
            goto END;
          }
          if (__endswith(outfile,".bmp")) {
            otype = FILETYPE_BMP;
          } else if (__endswith(outfile,".jpg") || 
              __endswith(outfile,".jpeg")) {
            otype = FILETYPE_JPEG;
          } else if (__endswith(outfile,".png")) {
            otype = FILETYPE_PNG;
          } else {
            eprintf("Unknown file extension '%s'\n",outfile);
            err = CLAIRVOYANCE_ERROR_INVALIDINPUT;
            goto END;
          }
          break;
        default:
          eprintf("Unknown extra argument '%s'\n",args[i].val.s);
          err = CLAIRVOYANCE_ERROR_INVALIDINPUT;
          goto END;
      }
      ++xarg;
    }
  }
  if (xarg != 2) {
    eprintf("Did not supply both input and output files!\n");
    __usage(stderr,argv[0]);
    err = CLAIRVOYANCE_ERROR_INVALIDINPUT;
    goto END;
  }

  img = draw_matrix_file(infile,itype,ctype,ftype,width,height);

  switch (otype) {
    case FILETYPE_BMP:
      bmp_write(outfile,img);
      break;
    case FILETYPE_PNG:
      png_write(outfile,img);
      break;
    case FILETYPE_JPEG:
      jpeg_write(outfile,img);
      break;
    default:
      eprintf("Unimplemented filetype '%s'\n",argv[3]);
      err = CLAIRVOYANCE_ERROR_INVALIDINPUT;
      goto END;
  }
        
  printf("Wrote %zux%zu image '%s' from '%s' in %s format.\n",img->width,
      img->height,outfile,infile,FILETYPE_NAMES[itype]);

  END:
  
  if (img) {
    image_free(img);
  }

  if (err != CLAIRVOYANCE_SUCCESS) {
    return 1;
  } else {
    return 0;
  }
}




#endif
