/*
 * fifo_api.h
 *
 * Created on: 2017/06/07
 *      Author: Hiroto Komatsu
 */

struct xillyfifo {
	unsigned long read_total;
	unsigned long write_total;
	unsigned int bytes_in_fifo;
	unsigned int read_position;
	unsigned int write_position;
	unsigned int size;
	unsigned int done;
	unsigned char *baseaddr;
	sem_t write_sem;
	sem_t read_sem;   
};

struct xillyinfo {
	int slept;
	int bytes;
	int position;
	void *addr;
};

int fifo_init(struct xillyfifo *fifo, unsigned int size);
void fifo_done(struct xillyfifo *fifo);
void fifo_destroy(struct xillyfifo *fifo);
int fifo_request_drain(struct xillyfifo *fifo, struct xillyinfo *info);
void fifo_drained(struct xillyfifo *fifo, unsigned int req_bytes);
int fifo_request_write(struct xillyfifo *fifo, struct xillyinfo *info);
void fifo_wrote(struct xillyfifo *fifo, unsigned int req_bytes);
