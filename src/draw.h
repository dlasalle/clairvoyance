/**
 * @file draw.h
 * @brief Matrix drawing prototypes
 * @author Dominique LaSalle <lasalle@cs.umn.edu>
 * Copyright 2013
 * @version 1
 * @date 2013-03-06
 */




#ifndef CLAIRVOYANCE_DRAW_H
#define CLAIRVOYANCE_DRAW_H




#include "base.h"
#include "image.h"




/******************************************************************************
* FUNCTION PROTOTYPES *********************************************************
******************************************************************************/


image_t * draw_matrix_file(
    char const * filein, 
    filetype_t ftype, 
    colortype_t ctype, 
    functiontype_t func, 
    size_t nx, 
    size_t ny);




#endif
