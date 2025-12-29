#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

    printf("Device opened for writing: %s\n", DRIVER_NAME);

    struct data d;
    d.length = 3;
    d.data = malloc(d.length);
    memcpy(d.data, "xyz", d.length);

    int ret = ioctl(fd, PUSH_DATA, &d);
    if (ret < 0) {
        perror("PUSH_DATA failed");
    } else {
        printf("Pushed %d bytes: %.*s\n", d.length, d.length, d.data);
    }

    free(d.data);
    close(fd);
    printf("Device closed\n");

    return ret;
}
