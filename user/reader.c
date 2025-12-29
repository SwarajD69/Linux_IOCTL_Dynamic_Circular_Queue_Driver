#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
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

    printf("Device opened for reading: %s\n", DRIVER_NAME);

    struct data d;
    d.length = 3;
    d.data = malloc(d.length);

    int ret = ioctl(fd, POP_DATA, &d);
    if (ret < 0) {
        perror("POP_DATA failed");
    } else {
        printf("Popped %d bytes: %.*s\n", d.length, d.length, d.data);
    }

    free(d.data);
    close(fd);
    printf("Device closed\n");

    return ret;
}
