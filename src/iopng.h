/**
 * @file iopng.h
 * @brief FUnction prototypes for writing PNGs
 * @author Dominique LaSalle <lasalle@cs.umn.edu>
 * Copyright 2013
 * @version 1
 * @date 2013-06-30
 */




#ifndef CLAIRVOYANCE_IOPNG_H
#define CLAIRVOYANCE_IOPNG_H




#include "base.h"
#include "image.h"




/******************************************************************************
* FUNCTION PROTOTYPES *********************************************************
******************************************************************************/


int png_write(
    char const * filename, 
    image_t const * image);




#endif
