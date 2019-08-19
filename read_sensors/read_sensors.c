/* read_sensors.c: read and output values of temperature and pressure
 * sensors from memory-mapped device */

#include <sys/file.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ADDR_TEMP 017776100
#define ADDR_PRES 017776102

int main(argc, argv)
int argc;
char *argv[];
{
    int mem;
    int temp;
    int pres;

    mem = open("/dev/kmem", O_RDONLY, 0);

    if (mem < 0) {
        perror("Cannot open /dev/kmem");
        exit(1);
    }

    if (lseek(mem, ADDR_TEMP, L_SET) < 0 ||
            read(mem, &temp, sizeof(temp)) < 0) {
        perror("Cannot read temperature sensor at address ADDR_TEMP");
        exit(1);
    }

    if (lseek(mem, ADDR_PRES, L_SET) < 0 ||
            read(mem, &pres, sizeof(pres)) < 0) {
        perror("Cannot read pressure sensor at address ADDR_PRES");
        exit(1);
    }

    close(mem);

    printf("Temperature : %.1f C\n", temp / 10.0);
    printf("Pressure : %.2f kPa\n", pres / 100.0);

    return 0;
}
