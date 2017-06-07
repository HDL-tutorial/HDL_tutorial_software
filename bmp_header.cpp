/*
 * bmp_header.cpp
 *
 *  Created on: 2017/05/24
 *      Author: Komatsu
 */
#include "bmp_header.h"

void readBMPHeader(FILE *fp, tagBITMAPFILEHEADER &bmpfh, tagBITMAPINFOHEADER &bmpih, unsigned char *&offset) {
	// Read BITMAP File Header
	fread(&bmpfh.bfType, sizeof(char), 2, fp);
	fread(&bmpfh.bfSize, sizeof(unsigned int), 1, fp);
	fread(&bmpfh.bfReserved1, sizeof(unsigned short), 1, fp);
	fread(&bmpfh.bfReserved2, sizeof(unsigned short), 1, fp);
	fread(&bmpfh.bfOffBits, sizeof(unsigned int), 1, fp);

	// Read BITMAP Information Header
	fread(&bmpih.biSize, sizeof(unsigned int), 1, fp);
	fread(&bmpih.biWidth, sizeof(int), 1, fp);
	fread(&bmpih.biHeight, sizeof(int), 1, fp);
	fread(&bmpih.biPlanes, sizeof(unsigned short), 1, fp);
	fread(&bmpih.biBitCount, sizeof(unsigned short), 1, fp);
	fread(&bmpih.biCompression, sizeof(unsigned int), 1, fp);
	fread(&bmpih.biSizeImage, sizeof(unsigned int), 1, fp);
	fread(&bmpih.biXPixPerMeter, sizeof(int), 1, fp);
	fread(&bmpih.biYPixPerMeter, sizeof(int), 1, fp);
	fread(&bmpih.biClrUsed, sizeof(unsigned int), 1, fp);
	fread(&bmpih.biClrImporant, sizeof(unsigned int), 1, fp);

	// Read offset
	offset = new unsigned char[bmpfh.bfOffBits - BITMAPFILEHEADERSIZE - BITMAPINFOHEADERSIZE]();
	fread(offset, sizeof(unsigned char), bmpfh.bfOffBits - BITMAPFILEHEADERSIZE - BITMAPINFOHEADERSIZE, fp);
}

void writeBMPHeader(FILE *fp, tagBITMAPFILEHEADER &bmpfh, tagBITMAPINFOHEADER &bmpih, unsigned char *&offset) {
	// Write BITMAP File Header
	fwrite(&bmpfh.bfType, sizeof(char), 2, fp);
	fwrite(&bmpfh.bfSize, sizeof(unsigned int), 1, fp);
	fwrite(&bmpfh.bfReserved1, sizeof(unsigned short), 1, fp);
	fwrite(&bmpfh.bfReserved2, sizeof(unsigned short), 1, fp);
	fwrite(&bmpfh.bfOffBits, sizeof(unsigned int), 1, fp);

	// Write BITMAP Information Header
	fwrite(&bmpih.biSize, sizeof(unsigned int), 1, fp);
	fwrite(&bmpih.biWidth, sizeof(int), 1, fp);
	fwrite(&bmpih.biHeight, sizeof(int), 1, fp);
	fwrite(&bmpih.biPlanes, sizeof(unsigned short), 1, fp);
	fwrite(&bmpih.biBitCount, sizeof(unsigned short), 1, fp);
	fwrite(&bmpih.biCompression, sizeof(unsigned int), 1, fp);
	fwrite(&bmpih.biSizeImage, sizeof(unsigned int), 1, fp);
	fwrite(&bmpih.biXPixPerMeter, sizeof(int), 1, fp);
	fwrite(&bmpih.biYPixPerMeter, sizeof(int), 1, fp);
	fwrite(&bmpih.biClrUsed, sizeof(unsigned int), 1, fp);
	fwrite(&bmpih.biClrImporant, sizeof(unsigned int), 1, fp);

	// Write offset
	fwrite(offset, sizeof(unsigned char), bmpfh.bfOffBits - BITMAPFILEHEADERSIZE - BITMAPINFOHEADERSIZE, fp);
}
