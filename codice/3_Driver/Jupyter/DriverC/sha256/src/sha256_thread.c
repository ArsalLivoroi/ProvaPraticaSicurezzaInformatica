

#define _XOPEN_SOURCE 500
#define _REENTRANT
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <pynq_api.h>

#define BNUM 32          /* number of buffer */
#define BSIZE 32768     /* length of each line */


typedef struct shared_buffer {
    PYNQ_AXI_DMA dma;
    pthread_mutex_t lock;      /* protects the buffer */
    pthread_cond_t             /* the POSIX condition variable type */
    new_data_cond,           /* to wait when the buffer is empty */
    new_space_cond;          /* to wait when the buffer is full */
    //char c[BNUM][BSIZE];       /* an array of lines, to hold the text */
    PYNQ_SHARED_MEMORY sm[BNUM];
    unsigned char* msg[BNUM];
    int next_in,               /* next available line for input */
    next_out,              /* next available line for output */
    count, max;                 /* the number of lines occupied */
    long int length[BNUM];
    PYNQ_SHARED_MEMORY output;
    const char *file_name;
} shared_buffer_t;


void sb_finalize(shared_buffer_t *sb);
void sb_init(shared_buffer_t *sb);


void * producer(void * arg)
{

    //FILE *fp;
    //fp = fopen("../../../jupyter_notebooks/file/input.txt", "r+");

    int fd;
    struct stat s;
    int status;
    const char * mapped;
    struct timespec t_start, t_stop;
    double tot_time=0;

    shared_buffer_t *sb = (shared_buffer_t *) arg;
    fd = open (sb->file_name, O_RDONLY);
    if (fd < 0){ printf("open  failed: \n");}

    //status = fstat (fd, & s);
    //if (status < 0){ printf("status  failed: \n");}
    //size_t length = s.st_size;

    //mapped = mmap (0, length, PROT_READ, MAP_PRIVATE, fd, 0);
    //if (mapped == MAP_FAILED){ printf("mapped  failed: \n");}

    int k = 0;


    pthread_mutex_lock(&sb->lock);                           /* (1) */
    for (;;) {
        while (sb->count == BNUM)                              /* (2) */
            pthread_cond_wait(&sb->new_space_cond, &sb->lock);
        sb->max=(sb->max >= sb->count)?sb->max:sb->count;
        pthread_mutex_unlock(&sb->lock);                       /* (3) */
        k = sb->next_in;


        //sb->length[k] = fread(sb->msg[k], 1, BSIZE-1, fd);
        clock_gettime(CLOCK_REALTIME, &t_start);
        sb->length[k] = read(fd, sb->msg[k], BSIZE-1);
        clock_gettime(CLOCK_REALTIME, &t_stop);
        tot_time += (t_stop.tv_sec - t_start.tv_sec) + (t_stop.tv_nsec - t_start.tv_nsec) / (double) 1000000000UL;

        sb->next_in = (sb->next_in + 1) % BNUM;
        pthread_mutex_lock(&sb->lock);
        sb->count++;

        pthread_cond_signal(&sb->new_data_cond);               /* (6) */
        if(sb->length[k]<=0){
            pthread_mutex_unlock(&sb->lock);
            close(fd);
            printf("producer time:\t%f\n", tot_time);
            pthread_exit(NULL);
        }
    }
}

void * consumer(void *arg)
{ int i, k = 0;
    shared_buffer_t *sb = (shared_buffer_t *) arg;
    struct timespec t_start, t_stop;
    double tot_time=0;
    pthread_mutex_lock(&sb->lock);                             /* L */
    for (;;) {                                                 /* L */
        while (sb->count == 0)                                   /* L */
            pthread_cond_wait(&sb->new_data_cond, &sb->lock);      /* L */
        pthread_mutex_unlock(&sb->lock);                         /* L */
        k = sb->next_out;

        if(sb->length[k]<=0){
            ((unsigned char*)sb->sm[k].pointer)[0]=0;

            clock_gettime(CLOCK_REALTIME, &t_start);
            PYNQ_writeDMA(&(sb->dma), &(sb->sm[k]), 0, sizeof(unsigned char) * 1);

            PYNQ_waitForDMAComplete(&(sb->dma), AXI_DMA_WRITE);



            //printf("cons1: %u\n", ((unsigned char*)(sb->sm[k]).pointer)[0]);
            PYNQ_readDMA(&(sb->dma), &(sb->output), 0, sizeof(unsigned char) * 32);

            PYNQ_waitForDMAComplete(&(sb->dma), AXI_DMA_READ);

            clock_gettime(CLOCK_REALTIME, &t_stop);
            tot_time += (t_stop.tv_sec - t_start.tv_sec) + (t_stop.tv_nsec - t_start.tv_nsec) / (double) 1000000000UL;

            printf("consumer time:\t%f\n", tot_time);
            pthread_exit(NULL);
        }
        clock_gettime(CLOCK_REALTIME, &t_start);
        PYNQ_writeDMA(&(sb->dma), &(sb->sm[k]), 0, sizeof(unsigned char) * (sb->length[k]+1));
        PYNQ_waitForDMAComplete(&(sb->dma), AXI_DMA_WRITE);
        clock_gettime(CLOCK_REALTIME, &t_stop);
        tot_time += (t_stop.tv_sec - t_start.tv_sec) + (t_stop.tv_nsec - t_start.tv_nsec) / (double) 1000000000UL;
        sb->next_out = (sb->next_out + 1) % BNUM;
        pthread_mutex_lock(&sb->lock);                           /* L */
        sb->count--;                                             /* L */
        pthread_cond_signal(&sb->new_space_cond);                /* L */
    }                                                          /* L */
}

int main(int argc, char const *argv[])
{
    pthread_t th1, th2;  /* the two thread objects */
    shared_buffer_t sb;  /* the buffer */
    struct timespec t_start, t_stop, t1, t2, t3;

    sb_init(&sb);
    sb.file_name=argv[1];
    clock_gettime(CLOCK_REALTIME, &t_start);
    pthread_create(&th1, NULL, producer, &sb );  /* (1) */
    pthread_create(&th2, NULL, consumer, &sb);
    pthread_join(th1, NULL);            /* (2) */
    pthread_join(th2, NULL);
    clock_gettime(CLOCK_REALTIME, &t_stop);
    printf("total time:\t%f\n",(t_stop.tv_sec - t_start.tv_sec)
                  + (t_stop.tv_nsec - t_start.tv_nsec)/(double)1000000000UL);

    //printf("cons1: %d\n", sb.max);

    //PYNQ_readDMA(&(sb.dma), &(sb.output), 0, sizeof(unsigned char) * 32);
    //PYNQ_waitForDMAComplete(&(sb.dma), AXI_DMA_READ);

    unsigned char * d2=(unsigned char*)(sb.output.pointer);

    for (int i=0;i<32;i++) {
        printf("%02X ", (unsigned char)d2[i]);
    }
    printf("\n\n");

    sb_finalize(&sb);

    return 0;
}

void sb_init(shared_buffer_t *sb)
{

    PYNQ_openDMA(&(sb->dma), 0x40400000);
    for(int i=0; i<BNUM; i++){
        PYNQ_allocatedSharedMemory(&(sb->sm[i]), sizeof(unsigned char)*BSIZE, 1);
        ((unsigned char*)(sb->sm[i]).pointer)[0]=1;
        //printf("init: %u\n", ((unsigned char*)(sb->sm[i]).pointer)[0]);
        sb->msg[i]=((unsigned char*)(sb->sm[i]).pointer)+1;
    }

    PYNQ_allocatedSharedMemory(&(sb->output), sizeof(unsigned char)*32, 1);



    sb->next_in = 0;
    sb->next_out = 0;
    sb->max = 0;
    sb->count = 0;    /* (1) */
    pthread_mutex_init(&sb->lock, NULL);           /* (2) */
    pthread_cond_init(&sb->new_data_cond, NULL);   /* (3) */
    pthread_cond_init(&sb->new_space_cond, NULL);
}

void sb_finalize(shared_buffer_t *sb) {
    PYNQ_closeDMA(&(sb->dma));
    for(int i=0; i<BNUM; i++){
        PYNQ_freeSharedMemory(&(sb->sm[i]));
    }
    PYNQ_freeSharedMemory(&(sb->output));
}
