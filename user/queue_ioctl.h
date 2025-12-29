#ifndef QUEUE_IOCTL_H
#define QUEUE_IOCTL_H

#include <sys/ioctl.h>   


struct data {
    int length;   
    char *data;    
};


#define SET_SIZE_OF_QUEUE _IOW('a', 'a', int *)
#define PUSH_DATA         _IOW('a', 'b', struct data *)
#define POP_DATA          _IOWR('a', 'c', struct data *)

#endif
