#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "./cpu.h"

#ifdef _POSIX_VERSION
#include <time.h>
#include <sys/utsname.h>
#else
#include <sys/param.h>
#include <sys/sysctl.h>
#endif /* _POSIX_VERSION */

char *days[] = { "Sunday", "Monday", "Tuesday", "Wednesday",
         "Thursday", "Friday", "Saturday" };

char *mons[] = { "January", "February", "March", "April",
         "May", "June", "July", "August",
         "September", "October", "November", "December" };

char *ttime[] = { "morning ", "afternoon ", "evening ", "night " };

int main()
{
    register struct tm *det;
    double ldvec[3];
    int a, b, x, y;
    char *tty, greeting[46], hostname[20], sys_info[43], ap, date[100];

#ifdef _POSIX_VERSION
    time_t secs;
    struct utsname uname_info;
#else
    long secs;
    int mib[2];
    size_t size;
    char machine[64], model[64], ostype[64];
#endif /* _POSIX_VERSION */

    /* Load average */
    (void)getloadavg(ldvec, 3);
    
    /* TTY */
    tty = ttyname(0);

    /* Time and greeting */
    time(&secs);
    det = localtime(&secs);
    if (det->tm_hour >= 6 && det->tm_hour < 12)
        i = 0;
    else if (det->tm_hour >= 12 && det->tm_hour < 17)
        i = 1;
    else if (det->tm_hour >= 17 && det->tm_hour < 21)
        i = 2;
    else /* det->tm_hour >= 21 || det->tm_hour < 6 */
        i = 3;

    greeting[0] = '\0';

    strncat(greeting, "Good ", sizeof(greeting) - 1);

    strncat(greeting, ttime[i],
            sizeof(greeting) - strlen(greeting) - 1);

    strncat(greeting, getenv("USER"),
            sizeof(greeting) - strlen(greeting) - 1);

    strncat(greeting, ",",
            sizeof(greeting) - strlen(greeting) - 1);

#ifdef HOURS_12
    ap = "AP"[det->tm_hour >= 12];
    if ((det->tm_hour %= 12) == 0)
        det->tm_hour = 12;
#endif /* HOURS_12 */

    /* Hostname */
    gethostname(hostname, sizeof(hostname));

    /* System information */
#ifdef _POSIX_VERSION
    uname(&uname_info);
    snprintf(sys_info, sizeof(sys_info), "A %s running %s",
            uname_info.machine, uname_info.sysname);
#else
    mib[0] = CTL_HW;
    mib[1] = HW_MACHINE;
    size = sizeof (machine);
    if  (sysctl(mib, 2, machine, &size, NULL, 0) < 0) {
        printf("Can't get machine type\n");
        strcpy(machine, "?");
    }

    mib[0] = CTL_HW;
    mib[1] = HW_MODEL;
    size = sizeof (model);
    if  (sysctl(mib, 2, model, &size, NULL, 0) < 0) {
        printf("Can't get cpu type\n");
        strcpy(model, "?");
    }
    
    mib[0] = CTL_KERN;
    mib[1] = KERN_OSTYPE;
    size = sizeof (ostype);
    if  (sysctl(mib, 2, ostype, &size, NULL, 0) < 0) {
        printf("Can't get ostype\n");
        strcpy(ostype, "?");
    }

    sys_info[0] = '\0';

    strncat(sys_info, "A ", sizeof(sys_info) - 1);

    strncat(sys_info, machine,
            sizeof(sys_info) - strlen(sys_info) - 1);

    strncat(sys_info, "/",
            sizeof(sys_info) - strlen(sys_info) - 1);

    strncat(sys_info, model,
            sizeof(sys_info) - strlen(sys_info) - 1);

    strncat(sys_info, " running ",
            sizeof(sys_info) - strlen(sys_info) - 1);

    strncat(sys_info, ostype,
            sizeof(sys_info) - strlen(sys_info) - 1);
#endif /* _POSIX_VERSION */

    fflush(stdout);
    printf("\033[2J\033(0\033)0\033[m");
    printf("\033[3;11H\033(B\033)B   Load Average\033(0\033)0");
    printf("\033[4;11Hlqqqqqqqqqqqqqqqqk");
    printf("\033[5;11Hx                x");
    printf("\033[6;11Hmqqqqqqqqqqqqqqwqj");
    printf("\033[5;13H%.02f %.02f %.02f", ldvec[0], ldvec[1], ldvec[2]);
    printf("\033[7;16Hlqqqqqqqqqvqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqk");

#ifdef TTY
    printf("\033[3;51H\033(B\033)BTTY Name\033(0\033)0");
    printf("\033[4;46Hlqqqqqqqqqqqqqqqqk");
    printf("\033[5;46Hx                x");
    printf("\033[6;46Hmqwqqqqqqqqqqqqqqj");
    printf("\033[5;50H\033(B\033)B%s\033(0\033)0", tty);
    printf("\033[7;16Hlqqqqqqqqqvqqqqqqqqqqqqqqqqqqqqqvqqqqqqqqk");
#else
    printf("\033[7;16Hlqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqk");
#endif /* TTY */

    printf("\033[8;16Hx                                        x");
    printf("\033[9;13Hlqqvqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqvqqk");
    printf("\033[10;10Hlqqvqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqvqqk");
    printf("\033[11;10Hx\033[11;63Hx\033[12;10Hx\033[12;63Hx\033[13;10Hx\033[13;63Hx");
    printf("\033[14;10Hmqqwqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqwqqj");
    printf("\033[15;13Hmqqwqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqwqqj");
    printf("\033[16;16Hx                                        x");
    printf("\033[17;16Hmqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj");
    printf("\033(B\033)B");

#ifdef LIGHT
    printf("\033[11;13H\033[7m                                                ");
#endif /* LIGHT */

    printf("\033[11;13H    %s", greeting);

#ifdef LIGHT
    printf("\033[12;13H\033[7m                                                ");
    printf("\033[12;13H\033[7m");
#else
    printf("\033[12;13H");
#endif /* LIGHT */

    printf("        You have connected to %s", hostname);

#ifdef LIGHT
    printf("\033[13;13H\033[7m                                                ");
#endif /* LIGHT */

    printf("\033[m");

#ifdef LIGHT
    printf("\033[13;13H\033[7m       ");
#else
    printf("\033[13;13H       ");
#endif /* LIGHT */

    printf("%s", sys_info);

#ifdef UVERS
    fp = fopen("/etc/motd", "r");
    fscanf(fp, "%s%s", poo, doo);
    printf(" V%c.%c", doo[1], doo[3]);
    fclose(fp);
    unlink("/tmp/foo");
#endif /* UVERS */

#ifdef LIGHT
    printf("\033[8;18H                                      ");
    printf("\033[16;18H                                      ");
#endif /* LIGHT */

    x = 40 - (strlen(days[det->tm_wday]) / 2);
    y = (69 - x);
    printf("\033[8;%dH%s", y, days[det->tm_wday]);

#ifdef HOURS_12
    sprintf(date, "%s %d, %d %d:%02d %cM", mons[det->tm_mon],
            det->tm_mday, det->tm_year + 1900, det->tm_hour,
        det->tm_min, ap);
#else
    sprintf(date, "  %s %d, %d %d:%02d", mons[det->tm_mon],
            det->tm_mday, det->tm_year + 1900, det->tm_hour,
        det->tm_min);
#endif /* HOURS_12 */
    
    a = 40 -(strlen(date) / 2);
    b = (54 - a);
    printf("\033[m");

#ifdef LIGHT
    printf("\033[7m");
#endif /* LIGHT */

    printf("\033[16;%dH%s", b, date);
    printf("\033[23;1H");
    printf("\033[m");
    exit(0);
}
