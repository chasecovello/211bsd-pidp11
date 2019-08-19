/* 2.11BSD httpd */
#include <sys/types.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/errno.h>

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define CGI_BIN
#define BUF_SIZE 2048
#define PATH_LEN 512
#define WWW_ROOT "/var/www/"
#define LOGFILE "/usr/adm/httpd.log"

#define HTTP_200 "HTTP/1.1 200 OK"
#define HTTP_403 "HTTP/1.1 403 Forbidden"
#define HTTP_404 "HTTP/1.1 404 Not Found"
#define HTTP_500 "HTTP/1.1 500 Internal Server Error"

FILE *log;

/* Get path information and handle errors */
void chk_path(path, st)
char *path;
struct stat *st;
{
    /* stat the path. If there's an error, log it and terminate. */ 
    if (stat(path, st) != 0) {
        if (errno & (ENOENT | ENOTDIR | EINVAL | ENAMETOOLONG)) {
            fprintf(log, "404 %s\n", strerror(errno));
            printf("%s\r\n", HTTP_404);
        } else if (errno & EACCES) {
            fprintf(log, "403 %s\n", strerror(errno));
            printf("%s\r\n", HTTP_403);
        } else {
            fprintf(log, "500 %s\n", strerror(errno));
            printf("%s\r\n", HTTP_500);
        }

        fclose(log);
        exit(1);
    }
}

int main(argc, argv)
int argc;
char *argv[];
{
    char path[PATH_LEN];
    struct stat st;
    
    /* Open log file, quit with HTTP 500 if there's an error */
    log = fopen(LOGFILE, "a");
    if (!log) {
        printf("%s\r\n", HTTP_500);
        exit(1);
    }

    /* Log requesting host address */
    {
        struct sockaddr_in sin;
        int sval;
        struct hostent *hp;
        char *host;
        
        sval = sizeof(sin);
        if (getpeername(0, (struct sockaddr *)&sin, &sval) == 0) {
            /* This is a connected socket, so get the address */
            if (hp = gethostbyaddr((char *)&sin.sin_addr.s_addr,
                        sizeof(sin.sin_addr.s_addr), AF_INET))
                host = hp->h_name;
            else
                host = inet_ntoa(sin.sin_addr);
        } else {
            /* Not a socket or address otherwise unavailable */
            host = strerror(errno);
        }
        
        fprintf(log, "%s ", host);
    }

    /* Log request time */
    {
        long secs;
        char *logtime;

        time(&secs);
        logtime = ctime(&secs);
        /* Strip trailing newline from ctime string */
        logtime[strcspn(logtime, "\r\n")] = '\0';
        fprintf(log, "[%s] ", logtime);
    }

    /* Path starts with WWW_ROOT */
    strncpy(path, WWW_ROOT, sizeof(path));

    /* Fill in path with GET/POST request */
    {
        char line[PATH_LEN];
        char *lineptr;

        while (fgets(line, sizeof(line), stdin)) {
            
            /* Remove trailing newline left by fgets() */
            line[strcspn(line, "\r\n")] = '\0';
            
            /* Detect the double line break to end req header */
            if (strlen(line) == 0)
                break;

            /* Get the path from the GET or POST request */
            if (strstr(line, "GET ") == line ||
                strstr(line, "POST ") == line) {
                
                fprintf(log, "\"%s\" ", line);

                /* Append rest of request to path */
                /* Skip request method */
                strtok(line, " ");
                /* Next token is path */
                lineptr = strtok(NULL, " ");
                if (lineptr)
                    strncat(path, lineptr, sizeof(path)-strlen(path)-1);
            }
        }
    }

    /* Check for parent directories in path */
    if (strstr(path, "/..")) {
        printf("%s\r\n", HTTP_403);
        fprintf(log, "403 Request contains \"..\"\n");
        fclose(log);
        exit(1);
    }
    
    /* stat the path and handle errors */
    chk_path(path, &st);

    /* If a directory is requested, default page is index.html */
    if (st.st_mode & S_IFDIR) {
        strncat(path, "index.html", sizeof(path)-strlen(path)-1);
        /* stat and handle errors again */
        chk_path(path, &st);
    }

    /* Only serve regular files */
    if (!(st.st_mode & S_IFREG)) {
        printf("%s\r\n", HTTP_403);
        fprintf(log, "403 Not a regular file\n");
        fclose(log);
        exit(1);
    }

#ifdef CGI_BIN
    /* Check if a CGI program has been requested */
    if (strstr(path, "/cgi-bin/")) {

        int pid;
        union wait status;
        
        /* CGI program must be executable and not setuid/setgid */
        if (!(st.st_mode & S_IEXEC) ||
                (st.st_mode & (S_ISUID | S_ISGID))) {
            printf("%s\r\n", HTTP_403);
            fprintf(log,
                    "403 File not executable and/or is setuid/setgid\n");
            fclose(log);
            exit(1);
        }
        
        /* Execute CGI program */
        if (!(pid = fork())) {
            /* Child process */
            execve(path, NULL, NULL);
            fprintf(log, "500 %s\n", strerror(errno));
            printf("%s\r\n", HTTP_500);
            fclose(log);
            exit(1);

        } else {
            /* Parent process, wait for child to exit */
            wait(&status);

            if (WIFEXITED(status))
                fprintf(log, "Exited with status %d\n",
                        status.w_retcode);
            else if (WIFSIGNALED(status))
                fprintf(log, "Terminated with signal %d\n",
                        status.w_termsig);
        }

    } else
#endif /* CGI_BIN */
    
    /* Serve the file */
    {         
        FILE *fd;
        char *ext;
        
        char buf[BUF_SIZE];
        int pos;

        /* Open file */
        fd = fopen(path, "r");
        if (!fd) {
            /* Earlier stat should have caught any errors, so we shouldn't
             * get here unless the file changed after the call */
            fprintf(log, "500 %s\n", strerror(errno));
            printf("%s\r\n", HTTP_500);
            fclose(log);
            exit(1);
        }

        printf("%s\r\n", HTTP_200);
        fprintf(log, "200 %ld\n", st.st_size);

        /* Extract file type and output content-type header */
        ext = rindex(path, '.');
        if (!ext)
            ext = "";

        if (!strcmp(ext, ".html"))
            printf("Content-Type: text/html\r\n");
        else if (!strcmp(ext, ".jpg"))
            printf("Content-Type: image/jpeg\r\n");
        else if (!strcmp(ext, ".ico"))
            printf("Content-Type: image/x-icon\r\n");
        else
            printf("Content-Type: text/plain\r\n");

        printf("Content-Length: %ld\r\n\r\n", st.st_size);
        
        while (!feof(fd) &&
                (pos = fread(buf, sizeof(*buf), sizeof(buf), fd)) > 0)
            fwrite(buf, sizeof(*buf), pos, stdout);
        
        fclose(fd);
    }

    fclose(log);
    return 0;
}
