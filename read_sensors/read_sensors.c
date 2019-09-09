/* read_sensors.c: read and output values of temperature and pressure
 * sensors from the BMP180 memory-mapped sensor device. See:
 *
 * https://obsolescence.wixsite.com/obsolescence/pidp-11-temp-barometer-hack
 *
 * for details on how to set this up on a PiDP-11.
 *
 * This program requires access to /dev/kmem to read the sensor data, so it
 * must be run setgid kmem, or by a user with kmem group membership.
 *
 * Copyright (c) 2019 Chase Covello
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

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
