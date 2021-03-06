/* weather.c: a cool-retro weather station on PiDP11 using 2.11BSD Unix.
 *
 * Copyright (c) 2019 rricharz, Chase Covello
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INCL_PRE  "/home/www/cgi-bin/include/weather.incl-pre"
#define INCL_POST "/home/www/cgi-bin/include/weather.incl-post"
#define BUF_SIZE 256

int main(argc,argv)
int argc;
char *argv[];
{
    FILE *fd;
    char buf[BUF_SIZE];
    int pos;
    
    char heading[20];
    double val, T, P, H;

    T = -273.0;
    P = -1.0;
    H = -1.0;

    printf("HTTP/1.0 200 OK\r\n");
    printf("Content-Type: text/html\r\n\r\n");

    fd = fopen(INCL_PRE, "r");
    if (fd) {
        while (!feof(fd) &&
                (pos = fread(buf, sizeof(*buf), sizeof(buf), fd)) > 0)
            fwrite(buf, sizeof(*buf), pos, stdout);

        fclose(fd);
    } else {
        printf("<html><head><title>PDP-11 Weather Station</title></head><body>\n");
    }

    fd = popen("/usr/local/read_sensors", "r");
    if (fd) {
        while (fgets(buf, sizeof(buf), fd) &&
               !strstr(buf, "Permission denied")) {

            sscanf(buf, "%s : %f", heading, &val);

            if (strcmp(heading, "Temperature") == 0)
                T = val;
            if (strcmp(heading,"Pressure") == 0)
                P = val;
            if (strcmp(heading,"Humidity") == 0)
                H = val;
        }
        pclose(fd);

        fd = popen("/usr/local/welcome_html", "r");
        if (fd) {
            while (!feof(fd) &&
                    (pos = fread(buf, sizeof(char), sizeof(buf), fd)) > 0)
                fwrite(buf, sizeof(char), pos, stdout);

            pclose(fd);
        }

        printf("<p>The sensor reports the following data:</p><p>");

        if (T > -273.0)
            printf("Temperature:&nbsp;<b>%0.1f &deg;C</b> (inside case)<br>\n", T);
        else
            printf("Temperature data unavailable.<br>\n");

        if (P >= 0.0)
            printf("Pressure:&nbsp;&nbsp;&nbsp;&nbsp;<b>%0.1f kPa</b></p>\n", P);
        else
            printf("Pressure data unavailable.</p>\n");
        
    } else {
        printf("<p>Cannot obtain data from sensor.</p>\n");
    }

    fd = fopen(INCL_POST, "r");
    if (fd) {
        while (!feof(fd) &&
                (pos = fread(buf, sizeof(*buf), sizeof(buf), fd)) > 0)
            fwrite(buf, sizeof(*buf), pos, stdout);

        fclose(fd);
    } else {
        printf("</body></html>");
    }
}
