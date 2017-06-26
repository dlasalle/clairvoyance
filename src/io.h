#ifndef CLAIRVOYANCE_IO_H
#define CLAIRVOYANCE_IO_H


#include "base.h"
#include "dlfile.h"




/******************************************************************************
* TYPES ***********************************************************************
******************************************************************************/


typedef struct spmat_handle_t {
  file_t * fp;
  storagetype_t type;
  char * line;
  int val, use_rows;
  size_t nrows;
  size_t ncols;
  size_t linesize;
  size_t nfields;
  size_t idxoffset;
  size_t valoffset;
  size_t lineoffset;
}  spmat_handle_t;




/******************************************************************************
* FUNCTION PROTOTYPES *********************************************************
******************************************************************************/


spmat_handle_t * open_matrix(
    char const * name, 
    filetype_t type);


int read_points(
    spmat_handle_t * handle, 
    size_t npx, 
    size_t npy, 
    real_t * pden, 
    real_t * pavg, 
    real_t * pmax);


int read_row(
    spmat_handle_t * handle, 
    size_t xpix, 
    real_t * pden, 
    real_t * pmax, 
    real_t * pavg, 
    size_t npix);


int close_matrix(
    spmat_handle_t * handle);




#endif
