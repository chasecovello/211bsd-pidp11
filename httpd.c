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

#define BUF_SIZE 4096
#define REQ_LEN 512
#define WWW_ROOT "/var/www/"
#define LOGFILE "/usr/adm/httpd.log"

#define HTTP_200 "HTTP/1.1 200 OK"
#define HTTP_403 "HTTP/1.1 403 Forbidden"
#define HTTP_404 "HTTP/1.1 404 Not Found"
#define HTTP_500 "HTTP/1.1 500 Internal Server Error"

int main(argc, argv)
int argc;
char *argv[];
{
    FILE *log;
    char *host;
    long secs;
    char *logtime;

    struct hostent *hp;
    struct sockaddr_in sin;
    int sval;
    
    char buf[BUF_SIZE];
    char path[REQ_LEN];
    char req_verb[5];
    char *ext;
    
    struct stat st;
    FILE *fd;
    int pos;
    
    int pid;
    union wait status;

    log = fopen(LOGFILE, "a");
    if (!log) {
        printf("%s\r\n", HTTP_500);
        exit(1);
    }

    /* Log requesting host address */
    sval = sizeof(sin);
    if (getpeername(0, (struct sockaddr *)&sin, &sval) < 0)
        fprintf("getpeername: %s ", strerror(errno));
    if (hp = gethostbyaddr((char *)&sin.sin_addr.s_addr, 
                sizeof(sin.sin_addr.s_addr), AF_INET))
        host = hp->h_name;
    else
        host = inet_ntoa(sin.sin_addr);
    fprintf(log, "%s ", host);
    
    /* Log request time */
    time(&secs);
    logtime = ctime(&secs);
    /* Strip newline from ctime string */
    logtime[strlen(logtime)-1] = '\0';
    fprintf(log, "[%s] ", logtime);
    
    /* Path starts with WWW_ROOT */
    strncpy(path, WWW_ROOT, sizeof(path));

    while (fgets(buf, sizeof(buf), stdin)) {
        
        /* If present, remove trailing newline left by fgets() */
        pos = strcspn(buf, "\r\n");
        if (pos)
            buf[pos] = '\0';
        
        /* Detect the double line break to end req header */
        if (strlen(buf) < 5) {
            fprintf(log, "No GET or POST in request\n");
            fclose(log);
            exit(1);
        }

        /* Get the path from the GET or POST request */
        if (strstr(buf, "GET ") == buf ||
	        strstr(buf, "POST ") == buf) {
	        sscanf(buf, "%s %s", req_verb, &path[strlen(path)]);
            fprintf(log, "\"%s\" ", buf);
            break;
        }
    }

    /* Check for parent directories in path */
    if (strstr(path, "/..")) {
        printf("%s\r\n", HTTP_403);
        fprintf(log, "403 Request contains ..\n");
        fclose(log);
        exit(1);
    }

    /* Check that we are not going to dump an inode */
    if (path[strlen(path)-1] == '/')
        strncat(path, "index.html", REQ_LEN-strlen(path));
    
    /* Request information about the file and handle errors */
    if (stat(path, &st) != 0) {
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
    
    /* Only serve regular files */
    if (!(st.st_mode & S_IFREG)) {
        printf("%s\r\n", HTTP_403);
        fprintf(log, "403 Not a regular file\n");
        fclose(log);
        exit(1);
    }

    /* Check if a CGI program has been requested */
    if (strstr(path, "/cgi-bin/")) {
        
        /* CGI program must be executable and not setuid/setgid */
        if ((st.st_mode & (S_ISUID | S_ISGID)) ||
                !(st.st_mode & S_IEXEC)) {
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

        fclose(log);
        exit(0);

    } else {

        /* Open file */
        fd = fopen(path, "r");
        if (!fd) {
            /* stat() above should have caught any errors, so we shouldn't
             * get here unless the file changed after the call */
            fprintf(log, "500 %s\n", strerror(errno));
            printf("%s\r\n", HTTP_500);
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
