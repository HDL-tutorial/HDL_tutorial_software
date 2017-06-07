#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <semaphore.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>

#include "bmp_header.h"
#include "fifo_api.h"

static const char *read_dev = "/dev/xillybus_read_32";
static const char *write_dev = "/dev/xillybus_write_32";

static int read_fd = 0;
static int write_fd = 0;

static int img_width = 0;
static int img_height = 0;

static unsigned int *src, *dst;

static timeval start;
static timeval end;
double get_sec(timeval start, timeval end) {
	return ((double)end.tv_sec - (double)start.tv_sec) + ((double)end.tv_usec - (double)start.tv_usec) * (1e-6);
}

/*** User threads ***/
// Write to FIFO, read from standard output
void *read_thread(void *arg)
{
	struct xillyfifo *fifo = (xillyfifo *)arg;
	int do_bytes, read_bytes, remain_bytes;
	int readable_bytes;
	struct xillyinfo info;
	int buf_size = 512 * 512;
	unsigned int buf[buf_size];
	int x = 0, y = 0; 
	int i, j;
	int total_read_bytes = 0;

	remain_bytes = img_width * img_height;
	while(remain_bytes > 0) {
		readable_bytes = remain_bytes < buf_size ? remain_bytes : buf_size;
		read_bytes = read(read_fd, buf, sizeof(unsigned int) * readable_bytes);
		if ((read_bytes < 0) && (errno != EINTR)) {
			std::cerr << "read() failed" << std::endl;
			return NULL;
		}

		if (read_bytes == 0) {
			// Reached EOF. Quit without complaining.
			std::cerr << "reached EOF" << std::endl;
			fifo_done(fifo);
			return NULL;
		}

		if (read_bytes < 0) { // errno is EINTR
			read_bytes = 0;
			continue;
		}

		for (i = 0; i < read_bytes / sizeof(unsigned int); i++) {
			dst[y * img_width + x] = buf[i];

			x++;
			if (x == img_width) {
				x = 0;
				y++;
			}
		}

		remain_bytes -= read_bytes / sizeof(unsigned int);
		total_read_bytes += read_bytes;
	}

	// After fill the frame buffer, calc the end time
	gettimeofday(&end, NULL);
	std::cout << "[TIME] " << get_sec(start, end) * 1000 << std::endl;
}

void all_write(int fd, unsigned int *buf, size_t len) {
	int sent = 0;
	int rc;

	while (sent < len) {
		rc = write(fd, buf + (sent >> 2), len - sent);

		if ((rc < 0) && (errno == EINTR))
			continue;

		if (rc < 0) {
			std::cerr << "allwrite() failed to write" << std::endl;
			exit(1);
		}

		if (rc == 0) {
			std::cerr << "Reached write EOF (?!)" << std::endl;
			exit(1);
		}

		sent += rc;
	}
}

void *trans_thread(void *arg) {
	int rc;
	int buf_size = 1024;
	unsigned int buf[buf_size];
	int read_pixels = 0;

	gettimeofday(&start, NULL);

	buf[0] = 0xC0000000 + ((img_width - 1) << 15) + (img_height - 1);
	all_write(write_fd, buf, sizeof(unsigned int));

	// send frame
	read_pixels = 0;
	while(read_pixels < img_width * img_height) {
		rc = img_width * img_height - read_pixels;
		rc = (buf_size < rc) ? buf_size : rc;

		for(int i = 0; i < rc; i++) {
			buf[i] = src[read_pixels + i];
		}

		all_write(write_fd, buf, sizeof(unsigned int) * rc);

		read_pixels += rc;
	}

	// send dummy data
	for(int i = 0; i < buf_size; i++) buf[i] = 0;
	for(int i = 0; i < img_width + 1; i+=read_pixels) {
		read_pixels = img_width + 1 - i;
		if(read_pixels > buf_size) read_pixels = buf_size;
		all_write(write_fd, buf, sizeof(unsigned int) * read_pixels);
	}
}

/*** main ***/
int main(int argc, char *argv[]) {
	FILE *fpr, *fpw;
	tagBITMAPFILEHEADER bmpfh;
	tagBITMAPINFOHEADER bmpih;
	unsigned char *offset;

	pthread_t tid[2];
	struct xillyfifo fifo;

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

	img_width = bmpih.biWidth;
	img_height = bmpih.biHeight;

	src = new unsigned int[img_width * img_height]();
	dst = new unsigned int[img_width * img_height]();
	for(int y = 0; y < img_height; y++) {
		for(int x = 0; x < img_width; x++) {
			int r, g, b;
			r = fgetc(fpr);
			g = fgetc(fpr);
			b = fgetc(fpr);
			src[(img_height - y - 1) * img_width + x] = (r << 16) | (g << 8) | b;
		}
	}
	fclose(fpr);

	// initialize FIFO
	if (fifo_init(&fifo, 1)) {
		std::cerr << "Failed to init" << std::endl;
		exit(1);
	}

	// open devices
	read_fd = open(read_dev, O_RDONLY);
	if (read_fd < 0) {
		if (errno == ENODEV) {
			std::cerr << "(Maybe " << read_dev << " a write-only file?)" << std::endl;
		}

		std::cerr << "Failed to open read file" << std::endl;
		exit(1);
	}

	write_fd = open(write_dev, O_WRONLY);
	if (write_fd < 0) {
		if (errno == ENODEV) {
			std::cerr << "(Maybe " << write_dev << " a read-only file?)" << std::endl;
		}

		std::cerr << "Failed to open write file" << std::endl;
		exit(1);
	}

	// filter
	if (pthread_create(&tid[0], NULL, read_thread, &fifo)) {
		std::cerr << "Failed to create thread" << std::endl;
		exit(1);
	}

	if (pthread_create(&tid[1], NULL, trans_thread, NULL)) {
		std::cerr << "Failed to create thread" << std::endl;
		exit(1);
	}

	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);

	fifo_destroy(&fifo);

	// Write BMP image
	fpw = fopen(argv[2], "wb");
	if(fpw == NULL) {
		std::cerr << "send_bmp: can't open " << argv[2] << std::endl;
		exit(2);
	}

	writeBMPHeader(fpw, bmpfh, bmpih, offset);
	for(int y = 0; y < img_height; y++) {
		for(int x = 0; x < img_width; x++) {
			int pix;
			pix = dst[(img_height - y - 1) * img_width + x];
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
