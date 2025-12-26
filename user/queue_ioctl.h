#ifndef QUEUE_IOCTL_H
#define QUEUE_IOCTL_H

#include <sys/ioctl.h>   /* user-space ioctl header */

/* Structure to pass data to/from the kernel */
struct data {
    int length;    /* number of bytes to push/pop */
    char *data;    /* pointer to user-space buffer */
};

/* IOCTL command definitions */
#define SET_SIZE_OF_QUEUE _IOW('a', 'a', int *)
#define PUSH_DATA         _IOW('a', 'b', struct data *)
#define POP_DATA          _IOWR('a', 'c', struct data *)

#endif

