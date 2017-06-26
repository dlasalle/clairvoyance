/**
 * @file iojpeg.h
 * @brief Function prototypes for jpeg
 * @author Dominique LaSalle <lasalle@cs.umn.edu>
 * Copyright 2013
 * @version 1
 * @date 2013-06-03
 */




#ifndef CLAIRVOYANCE_IOJPEG_H
#define CLAIRVOYANCE_IOJPEG_H




#include "base.h"
#include "image.h"




/******************************************************************************
* FUNCTION PROTOTYPES *********************************************************
******************************************************************************/


int jpeg_write(
    char const * filename, 
    image_t const * image);




#endif
