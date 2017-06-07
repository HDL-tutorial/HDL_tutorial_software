#include <iostream>
#include <cstdio>
#include <stdlib.h>

#include "bmp_header.h"

int main(int argc, char *argv[]) {
	FILE *fpr, *fpw;
	tagBITMAPFILEHEADER bmpfh;
	tagBITMAPINFOHEADER bmpih;
	unsigned char *offset;
	unsigned int *src, *dst;

	if(argc != 3) {
		std::cerr << "Usage: send_bmp input output" << std::endl;
		exit(1);
	}

	// Read BMP image
	fpr = fopen(argv[1], "rb");
	if(fpr == NULL) {
		std::cerr << "send_bmp: can't open " << argv[1] << std::endl;
		exit(2);
	}
	readBMPHeader(fpr, bmpfh, bmpih, offset);

	src = new unsigned int[bmpih.biWidth * bmpih.biHeight]();
	dst = new unsigned int[bmpih.biWidth * bmpih.biHeight]();
	for(int y = 0; y < bmpih.biHeight; y++) {
		for(int x = 0; x < bmpih.biWidth; x++) {
			int r, g, b;
			r = fgetc(fpr);
			g = fgetc(fpr);
			b = fgetc(fpr);
			src[(bmpih.biHeight - y - 1) * bmpih.biWidth + x] = (r << 16) | (g << 8) | b;
		}
	}
	fclose(fpr);

	// filter
	for(int y = 0; y < bmpih.biHeight; y++) {
		for(int x = 0; x < bmpih.biWidth; x++) {
			printf("%06x ", src[y * bmpih.biHeight + x]);
			dst[y * bmpih.biHeight + x] = src[y * bmpih.biHeight + x];
		}
		std::cout << std::endl;
	}

	// Write BMP image
	fpw = fopen(argv[2], "wb");
	if(fpw == NULL) {
		std::cerr << "send_bmp: can't open " << argv[2] << std::endl;
		exit(2);
	}

	writeBMPHeader(fpw, bmpfh, bmpih, offset);
	for(int y = 0; y < bmpih.biHeight; y++) {
		for(int x = 0; x < bmpih.biWidth; x++) {
			int pix;
			pix = dst[(bmpih.biHeight - y - 1) * bmpih.biWidth + x];
			fputc((pix >> 16) & 0xff, fpw);
			fputc((pix >> 8) & 0xff, fpw);
			fputc(pix & 0xff, fpw);
		}
	}
	fclose(fpw);

	delete src;
	delete dst;

	return 0;
}
