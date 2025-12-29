#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "queue_ioctl.h"

#define DRIVER_NAME "/dev/swaraj_testing"

int main(void)
{
    int fd = open(DRIVER_NAME, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return -1;
    }

    printf("Device opened: %s\n", DRIVER_NAME);

    int size = 100;
    int ret = ioctl(fd, SET_SIZE_OF_QUEUE, &size);
    if (ret < 0) {
        perror("SET_SIZE_OF_QUEUE failed");
    } else {
        printf("Queue initialized with size %d\n", size);
    }

    close(fd);
    printf("Device closed\n");

    return ret;
}
