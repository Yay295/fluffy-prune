/**************************************************************************//**
@file

@author John Colton

@brief C++ code to read in a 24-bit BMP image file.

Ref: Based on John M. Weiss, Ph.D's BMPload.cpp, which no longer works on a
     modern compiler.
RefRef: Based on code from RgbImage.cc (Buss, 3-D Computer Graphics, 2003).
******************************************************************************/

#ifndef LOAD_BMP_H
#define LOAD_BMP_H
#include <cstddef>
#include <iostream>
#include <fstream>

/**************************************************************************//**
@author Dr. Weiss and John Colton

@par Description: This function attempts to load a BMP image into a char array.

@param[in] filename  - The name of the BMP file to attempt to read.
@param[out] rows     - The number of rows in the image.
@param[out] columns  - The number of columns in the image.
@param[in,out] image - A char pointer to be filled with image data.

@returns false - The image could not be loaded.
@returns true  - The image was loaded. 'image' has been dynamically allocated,
                 and so must be deleted with 'delete[]'.
******************************************************************************/
bool loadBMP( const char * const filename, size_t & rows, size_t & columns, char * & image );

/**************************************************************************//**
@author Dr. Weiss and John Colton

@par Description: This function reads one pixel from a BMP file.

@param[in,out] in - The image file to read from.

@returns short - The pixel that was read.
******************************************************************************/
short readShort( std::istream & in );

/**************************************************************************//**
@author Dr. Weiss and John Colton

@par Description: This function reads one pixel from a BMP file.

@param[in,out] in - The image file to read from.

@returns int - The pixel that was read.
******************************************************************************/
int readLong( std::istream & in );

/**************************************************************************//**
@author Dr. Weiss and John Colton

@par Description: This function returns the number of bytes in one line of data
                  in a BMP file.

@param[in] columns - The number of columns in the image.

@returns size_t - The number of bytes in one line.
******************************************************************************/
size_t bytesPerRow( size_t columns );

#endif /* LOAD_BMP_H */
