#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef _POSIX_VERSION
#include <time.h>
#include <sys/utsname.h>
#else
#include <sys/sysctl.h>
#include <sys/param.h>
#endif /* _POSIX_VERSION */

char *days[] = { "Sunday", "Monday", "Tuesday", "Wednesday",
         "Thursday", "Friday", "Saturday" };

char *mons[] = { "January", "February", "March", "April",
         "May", "June", "July", "August",
         "September", "October", "November", "December" };

char *ttime[] = { "morning ", "afternoon ", "evening ", "night " };

void print_center(text, field_width)
char *text;
int field_width;
{
    int textlen, padlen;

    textlen = strlen(text);

    padlen = (field_width - textlen) / 2;

    printf("%*s%s", padlen, "", text, padlen, "");

    if ((field_width - textlen) % 2 == 1)
        padlen++;

    printf("%*s", padlen, "");
}

void drawline(width)
int width;
{
    while (width-- > 0)
        fputs("&boxh;", stdout);
}

void fillspace(width)
int width;
{
    while (width-- > 0)
        putchar(' ');
}

int main()
{
    register struct tm *det;
    int i;
    double ldvec[3];
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
    tty = "/dev/console";

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

    strncat(greeting, "www",
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
    size = sizeof(machine);
    if (sysctl(mib, 2, machine, &size, NULL, 0) < 0) {
        printf("Can't get machine type\n");
        strcpy(machine, "?");
    }

    mib[0] = CTL_HW;
    mib[1] = HW_MODEL;
    size = sizeof(model);
    if (sysctl(mib, 2, model, &size, NULL, 0) < 0) {
        printf("Can't get cpu type\n");
        strcpy(model, "?");
    }

    mib[0] = CTL_KERN;
    mib[1] = KERN_OSTYPE;
    size = sizeof(ostype);
    if (sysctl(mib, 2, ostype, &size, NULL, 0) < 0) {
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

    /* Print output */
    printf("<pre>\n");

    fillspace(13);
    printf("Load Average");
    fillspace(25);
    printf("TTY Name\n");
    
    fillspace(10);
    printf("&boxdr;");
    drawline(16);
    printf("&boxdl;");
    fillspace(17);
    printf("&boxdr;");
    drawline(16);
    printf("&boxdl;\n");
    
    fillspace(10);
    printf("&boxv; %.02f %.02f %.02f &boxv;",
            ldvec[0], ldvec[1], ldvec[2]);
    fillspace(17);
    printf("&boxv; ");
    print_center(tty, 14);
    printf(" &boxv;\n");

    fillspace(10);
    printf("&boxur;");
    drawline(14);
    printf("&boxhd;&boxh;&boxul;");
    fillspace(17);
    printf("&boxur;&boxh;&boxhd;");
    drawline(14);
    printf("&boxul;\n");

    fillspace(15);
    printf("&boxdr;");
    drawline(9);
    printf("&boxhu;");
    drawline(21);
    printf("&boxhu;");
    drawline(8);
    printf("&boxdl;\n");

    fillspace(15);
    printf("&boxv; ");
    print_center(days[det->tm_wday], 38);
    printf(" &boxv;\n");

    fillspace(12);
    printf("&boxdr;");
    drawline(2);
    printf("&boxhu;");
    drawline(40);
    printf("&boxhu;");
    drawline(2);
    printf("&boxdl;\n");

    fillspace(9);
    printf("&boxdr;");
    drawline(2);
    printf("&boxhu;");
    drawline(46);
    printf("&boxhu;");
    drawline(2);
    printf("&boxdl;\n");

    fillspace(9);
    printf("&boxv;");
    fillspace(6);
    printf("%-45s &boxv;\n", greeting);
    
    fillspace(9);
    printf("&boxv;");
    fillspace(10);
    printf("You have connected to %-19s &boxv;\n", hostname);

    fillspace(9);
    printf("&boxv;");
    fillspace(9);
    printf("%-42s &boxv;\n", sys_info);

    fillspace(9);
    printf("&boxur;");
    drawline(2);
    printf("&boxhd;");
    drawline(46);
    printf("&boxhd;");
    drawline(2);
    printf("&boxul;\n");

    fillspace(12);
    printf("&boxur;");
    drawline(2);
    printf("&boxhd;");
    drawline(40);
    printf("&boxhd;");
    drawline(2);
    printf("&boxul;\n");
    
#ifdef HOURS_12
    sprintf(date, "%s %d, %d %d:%02d %cM", mons[det->tm_mon],
            det->tm_mday, det->tm_year + 1900, det->tm_hour,
        det->tm_min, ap);
#else
    sprintf(date, "%s %d, %d %d:%02d", mons[det->tm_mon],
            det->tm_mday, det->tm_year + 1900, det->tm_hour,
        det->tm_min);
#endif /* HOURS_12 */
    
    fillspace(15);
    printf("&boxv; ");
    print_center(date, 38);
    printf(" &boxv;\n");

    fillspace(15);
    printf("&boxur;");
    drawline(40);
    printf("&boxul;\n");
    printf("</pre>\n");

    exit(0);
}
