/**
 * @file io.c
 * @brief I/O Stuff
 * @author Dominique LaSalle <lasalle@cs.umn.edu>
 * Copyright 2013
 * @version 1
 * @date 2013-06-27
 */




#ifndef CLAIRVOYANCE_IO_C
#define CLAIRVOYANCE_IO_C




#include "io.h"




/******************************************************************************
* CONSTANTS *******************************************************************
******************************************************************************/


static const int FILETYPE_HEADER[] = {
  [FILETYPE_METIS] = 1,
  [FILETYPE_CLUTO] = 1,
  [FILETYPE_CSR] = 0,
  [FILETYPE_CSR_HEADER] = 1,
  [FILETYPE_RAW] = 0,
  [FILETYPE_DENSE] = 0,
  [FILETYPE_DIMACS] = 1,
  [FILETYPE_POINT] = 0,
  [FILETYPE_COO] = 0,
  [FILETYPE_UNKNOWN] = 0
};




/******************************************************************************
* MACRO INVOCATIONS ***********************************************************
******************************************************************************/


#define DLMEM_PREFIX spmat_handle
#define DLMEM_TYPE_T spmat_handle_t
#define DLMEM_DLTYPE DLTYPE_STRUCT
#define DLMEM_STATIC
#include "dlmem_headers.h"
#undef DLMEM_STATIC
#undef DLMEM_DLTYPE
#undef DLMEM_TYPE_T
#undef DLMEM_PREFIX




/******************************************************************************
* PRIVATE FUNCTIONS ***********************************************************
******************************************************************************/


static inline int __is_comment(
    char c) 
{
  switch (c) {
    case '#':
    case '%':
    case '/':
    case '"':
    case '\'':
    case ';':
      return 1;
    default :
      return 0;
  }
}


static int __read_chunk(
    char * buffer, 
    size_t bufsize, 
    FILE * fin)
{
  char * tmpstr;

  tmpstr = fgets(buffer,bufsize,fin);

  /* see if something went wrong */
  if (tmpstr == NULL) {
    if (feof(fin)) {
      return 0; 
    } else if (ferror(fin)) {
      dl_error("Error while reading from file stream\n");
    }
  }

  return 1;
}




/******************************************************************************
* PUBLIC FUNCTIONS ************************************************************
******************************************************************************/


spmat_handle_t * open_matrix(
    char const * const name, 
    filetype_t const type)
{
  int flags, aux;
  size_t idx, nnz, linesize;
  ssize_t linelen;
  char * line, * sptr, * eptr;
  double v;
  spmat_handle_t * handle;

  line = NULL;
  handle = NULL;

  linesize = DEFAULT_BUFFER_SIZE;
  line = char_alloc(linesize);
  handle = spmat_handle_alloc(1);

  if (dl_open_file(name,"r",&(handle->fp)) != DL_FILE_SUCCESS) {
    dl_error("Failed to open '%s' for reading\n",name);
    perror("Cause:");
    goto FAIL;
  }

  if (FILETYPE_HEADER[type]) {
    while ((linelen = dl_get_next_line(handle->fp,&line,&linesize)) == 0 ||
        __is_comment(line[0]));
  }
  sptr = line;

  switch (type) {
    case FILETYPE_METIS:
      handle->use_rows = 1;
      handle->ncols = handle->nrows = strtoull(sptr,&eptr,10);
      if (sptr == eptr) {
        dl_error("Failed to read number of vertices from metis file '%s'\n",
            name);
        goto FAIL;
      }
      sptr = eptr;
      nnz = (size_t)strtoull(sptr,&eptr,10); /* don't care about nnz */
      if (nnz == 0) {
        wprintf("Sparse matrix is all zeros.\n");
      }
      if (sptr == eptr) {
        eprintf("Failed to read number of edges from metis file '%s'\n",name);
        goto FAIL;
      }
      sptr = eptr;
      flags = strtol(sptr,&eptr,10);
      if (sptr == eptr || flags == 0) {
        /* no weights */
        handle->val = 0;
        handle->nfields = 1;
        handle->idxoffset = 0;
        handle->lineoffset = 0;
      } else if (flags == 1) {
        /* only edge weights */
        handle->val = 1;
        handle->nfields = 2;
        handle->idxoffset = 0;
        handle->valoffset = 1;
        handle->lineoffset = 0;
      } else if (flags == 10) {
        /* only vertex weights */
        handle->val = 0;
        handle->nfields = 1;
        handle->idxoffset = 0;
        aux = strtol(sptr,&eptr,10);
        if (sptr == eptr) {
          handle->lineoffset = 1;
        } else {
          handle->lineoffset = aux;
        }
      } else if (flags == 11) {
        /* both weights */
        handle->val = 1;
        handle->nfields = 2;
        handle->idxoffset = 0;
        handle->valoffset = 1;
        aux = strtol(sptr,&eptr,10);
        if (sptr == eptr) {
          handle->lineoffset = 1;
        } else {
          handle->lineoffset = aux;
        }
      }
      break;
    case FILETYPE_CLUTO:
      handle->use_rows = 1;
      handle->val = 1;
      handle->nrows = strtoull(sptr,&eptr,10);
      if (sptr == eptr) {
        eprintf("Failed to read number of rows from cluto file '%s'\n",name);
        goto FAIL;
      }
      sptr = eptr;
      handle->ncols = strtoull(sptr,&eptr,10);
      if (sptr == eptr) {
        eprintf("Failed to read number of rows from cluto file '%s'\n",name);
        goto FAIL;
      }
      sptr = eptr;
      nnz = strtoull(sptr,&eptr,10); /* throw away nnz */
      sptr = eptr;
      handle->lineoffset = 0;
      handle->nfields = 2;
      handle->idxoffset = 0;
      handle->valoffset = 1;
      break;
    case FILETYPE_CSR_HEADER: /* the header is useless */
    case FILETYPE_CSR:
      handle->use_rows = 1;
      handle->val = 1;
      handle->nrows = 0;
      handle->ncols = 0;
      handle->lineoffset = 0;
      handle->idxoffset = 0;
      handle->valoffset = 1;
      handle->nfields = 2;
      while ((linelen = dl_get_next_line(handle->fp,&line,&linesize)) > -1) {
        if (__is_comment(line[0])) {
          continue;
        }
        /* find the maximum column */
        sptr = line;
        idx = strtoull(sptr,&eptr,10)+1;
        sptr = eptr;
        v = strtod(sptr,&eptr); /* ignore value for now */
        while (sptr != eptr) {
          if (v == 0) {
            wprintf("Found sparse 0 at idx: %zu\n",idx);
          }
          handle->ncols = dl_storemax(idx,handle->ncols);
          sptr = eptr;
          idx = strtoull(sptr,&eptr,10);
          sptr = eptr;
          v = strtod(sptr,&eptr); /* ignore value for now */
        }
        ++handle->nrows;
      }
      /* reset the fd */
      dl_reset_file(handle->fp);
      if (FILETYPE_HEADER[type]) {
        while ((linelen = dl_get_next_line(handle->fp,&line,&linesize)) == 0 ||
            __is_comment(line[0]));
      }
      break;
    case FILETYPE_COO:
    case FILETYPE_POINT:
      handle->use_rows = 0;
      handle->val = 1;
      handle->nrows = 0;
      handle->ncols = 0;
      handle->lineoffset = 0;
      handle->idxoffset = 1;
      handle->valoffset = 2;
      handle->nfields = 3;
      while ((linelen = dl_get_next_line(handle->fp,&line,&linesize)) > -1) {
        if (__is_comment(line[0])) {
          continue;
        }
        /* find the maximum row */
        sptr = line;
        idx = strtoull(sptr,&eptr,10)+1;
        sptr = eptr;
        handle->nrows = dl_storemax(idx,handle->nrows);
        /* find the maximum column */
        idx = strtoull(sptr,&eptr,10)+1;
        sptr = eptr;
        handle->ncols = dl_storemax(idx,handle->ncols);
        /* ignore value for now */
        v = strtod(sptr,&eptr);
      }
      dl_reset_file(handle->fp);
      break;
    default :
      dl_error("Unknown filetype '%d'\n",type);
  }

  handle->line = line;
  handle->linesize = linesize;

  dprintf("Created handle for %zux%zu matrix\n",handle->nrows,handle->ncols);

  return handle;

  FAIL:
  if (handle) {
    if (handle->fp) {
      dl_close_file(handle->fp);
    }
    dl_free(handle);
  }
  if (line) {
    dl_free(line);
  }
  return NULL;
}


int read_points(
    spmat_handle_t * const handle, 
    size_t const npx, 
    size_t const npy, 
    real_t * const pden, 
    real_t * const pavg, 
    real_t * const pmax)
{
  size_t i,j,nfill,idx;
  ssize_t linelen;
  double val;

  double const xscale = npx/(double)handle->ncols;
  double const yscale = npy/(double)handle->nrows;

  while ((linelen = dl_get_next_line(handle->fp,&handle->line,
              &handle->linesize)) > 0) {
    /* skip comment lines */
    if (__is_comment(handle->line[0])) {
      continue;
    }
    
    nfill = sscanf(handle->line,"%zu %zu %lf",&i,&j,&val);
    if (nfill < 2) {
      dl_error("Point had %zu elements\n",nfill);
      return 0;
    } else if (nfill < 3) {
      val = 1.0;
    }
    idx = (i*yscale)*npx + j*xscale;

    DL_ASSERT(idx < npx*npy,"Bad index %zu (%zu,%zu) from (%zu/%zu, " \
        "%zu/%zu) for %zux%zu",idx,idx/npx,idx%npx,i,handle->nrows,j, \
        handle->ncols,npx,npy);

    if (pden) {
      ++pden[idx];
    }
    if (pavg) {
      pavg[idx] += val;
    }
    if (pmax && pmax[idx] < val) {
      pmax[idx] = val;
    }
  }
  
  return 1;
}


int read_row(
    spmat_handle_t * const handle, 
    size_t const ypix, 
    real_t * const pden, 
    real_t * const pmax, 
    real_t * const pavg, 
    size_t const npix)
{
  size_t ne,idx;
  ssize_t linelen;
  char * sptr, * eptr;
  double val;
  const double scale = npix/(double)handle->ncols;
  const size_t offset = ypix*npix;

  /* skip comment lines */
  while ((linelen = dl_get_next_line(handle->fp,&handle->line,
          &handle->linesize)) > 0 && __is_comment(handle->line[0]));

  if (linelen == 0) {
    /* empty line */
    return 1;
  }

  ne = 0;
  sptr = handle->line;
  val = strtod(sptr,&eptr);
  /* skip offset */
  while (ne < handle->lineoffset && sptr != eptr) {
    ++ne;
    val = strtod(sptr,&eptr);
  }
  if (ne < handle->lineoffset) {
    dl_error("Failed to read in header of row\n");
    return 0;
  }

  /* read the actual data for the line */
  ne = 0; /* reset element coutner */
  idx = 0;
  while (sptr != eptr) {
    if (ne % handle->nfields == handle->idxoffset) {
      idx = offset + val*scale;
      if (pden) {
        pden[idx] += 1; 
      }
      if (!handle->val) {
        /* if we dont' have values */
        if (pavg) {
          pavg[idx] += 1.0; 
        }
        if (pmax && pmax[idx] < 1.0) {
          pmax[idx] = 1.0; 
        }
      }
    } else if (handle->val && ne %handle->nfields == handle->valoffset) {
      if (pavg) {
        pavg[idx] += val; 
      }
      if (pmax && pmax[idx] < val) {
        pmax[idx] = val; 
      }
    }
    ++ne;
    sptr = eptr;
    val = strtod(sptr,&eptr);
  }

  return 1;
}


int close_matrix(
    spmat_handle_t * handle)
{
  if (handle->fp) {
    dl_close_file(handle->fp);
  }
  if (handle->line) {
    dl_free(handle->line);
  }
  dl_free(handle);
  return 0;
}




#endif
