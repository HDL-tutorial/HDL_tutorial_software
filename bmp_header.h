/*
 * bmp_header.h 
 *
 * Created on: 2015/08/03
 *      Author: Masaaki
 */

#include <stdio.h>

#pragma once

#ifndef BMP_HEADER_H_
#define BMP_HEADER_H_

#define BITMAPFILEHEADERSIZE 14
#define BITMAPINFOHEADERSIZE 40

// BITMAPFILEHEADER 14bytes
typedef struct tagBITMAPFILEHEADER {
  unsigned short bfType;
  unsigned long  bfSize;
  unsigned short bfReserved1;
  unsigned short bfReserved2;
  unsigned long  bfOffBits;
} BITMAPFILEHEADER;

// BITMAPINFOHEADER 40bytes
typedef struct tagBITMAPINFOHEADER{
    unsigned long  biSize;
    long           biWidth;
    long           biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned long  biCompression;
    unsigned long  biSizeImage;
    long           biXPixPerMeter;
    long           biYPixPerMeter;
    unsigned long  biClrUsed;
    unsigned long  biClrImporant;
} BITMAPINFOHEADER;

typedef struct BMP24bitsFORMAT {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
} BMP24FORMAT;

void readBMPHeader(FILE *fp, tagBITMAPFILEHEADER &bmpfh, tagBITMAPINFOHEADER &bmpih, unsigned char *&offset);
void writeBMPHeader(FILE *fp, tagBITMAPFILEHEADER &bmpfh, tagBITMAPINFOHEADER &bmpih, unsigned char *&offset);

#endif /* BMP_HEADER_H_ */
