#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sqlite3.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>

#define PORTNUM 5555
#define MAXLINE 1024
#define Q_UPLOAD 1
#define Q_DOWNLOAD 2
#define Q_LIST 3

struct Cquery {
    int command;
    char filename[64];
};

int gettime(char *tstr);

int callback(void *farg, int argc, char **argv, char **ColName);

int process(int sockfd);

int file_upload(int sockfd, char *filename);

int file_download(int sockfd, char *filename);

int file_list(int sockfd);

int main(int argc, char **argv) {
    int cli_sockfd, serv_sockfd;
    struct sockaddr_in addr, clientaddr;
    socklen_t clilen;

    if ((serv_sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        return 1;
    }
    memset(&addr, 0x00, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORTNUM);

    if (bind(serv_sockfd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("bind error");
        return 1;
    }

    if (listen(serv_sockfd, 5) == -1) {
        perror("bind error");
        return 1;
    }

    for (int i = 0; i < 5; i++) {
        clilen = sizeof(clientaddr);

        if ((cli_sockfd = accept(serv_sockfd, NULL, &clilen)) < 0) {
            perror("accept error\n");
            return 1;
        }

        int pid = fork();
        if (pid == 0) {
            process(cli_sockfd);
            close(cli_sockfd);
        }
    }

}

int process(int sockfd) {
    struct Cquery query;
    while (1) {
        if (recv(sockfd, (void *) &query, sizeof(query), 0) <= 0) {
            return -1;
        }
        printf("->>>>> %s\n", query.filename);
        query.command = ntohl(query.command);
        switch (query.command) {
            case (Q_UPLOAD):
                file_upload(sockfd, query.filename);
                break;
            case (Q_DOWNLOAD):
                file_download(sockfd, query.filename);
                break;
            case (Q_LIST):
                file_list(sockfd);
                break;
        }
        break;
    }
    return 1;
}

int file_upload(int sockfd, char *filename) {
    int fd;
    int size = 0;
    int readn;
    int writen;
    char buf[MAXLINE];
    char *errMsg;
    struct sockaddr_in addr;
    sqlite3 *db;
    time_t now = time(NULL);
    socklen_t addrlen;
    int ret;

    time(&now);
    if ((fd = open(filename, O_WRONLY | O_CREAT)) == -1) {
        perror("File open error\n");
        return 1;
    }

    memset(buf, 0x00, MAXLINE);
    while ((readn = recv(sockfd, buf, MAXLINE, 0)) > 0) {

        if (write(fd, buf, readn) < 0) {
            break;
        }
        size += readn;
        memset(buf, 0x00, MAXLINE);
    }

    addrlen = sizeof(addr);
    getpeername(sockfd, (struct sockaddr *) &addr, &addrlen);
    ret = sqlite3_open("filelist.db", &db);
    if (ret == SQLITE_OK) {
        char *up_date;
        gettime(up_date);
        char *query = sqlite3_mprintf(
                "insert into file_info(name, ip, up_date, count, size) values('%q', '%q', '%q', '%d', '%d');", filename,
                inet_ntoa(addr.sin_addr), up_date, 0, size);
        ret = sqlite3_exec(db, query, callback, 0, &errMsg);
        if (ret != SQLITE_OK) {
            printf("%s\n", errMsg);
        }
    }
    printf("File Upload %s\n", inet_ntoa(addr.sin_addr));
}

int callback(void *farg, int argc, char **argv, char **ColName) {
    int i;
    int sockfd = *(int *) farg;
    int sendn = 0;
    char buf[MAXLINE];
    for (i = 0; i < argc; i++) {
        sprintf(buf, "%10s = %s\n", ColName[i], argv[i] ? argv[i] : "NULL");
        sendn = send(sockfd, buf, strlen(buf), 0);
        printf("--> %d\n", sendn);
    }
    sprintf(buf, "%s\n", "-----------------------------");
    sendn = send(sockfd, buf, strlen(buf), 0);
    printf("--> %d\n", sendn);
    return 0;
}

int file_download(int sockfd, char *filename) {
  int fd;
  int readn;
  int sendn;
  int ret;
  sqlite3 *db;
  char *errMsg;
  char buf[MAXLINE];
  if((fd = open(filename, O_RDONLY)) == -1){
    perror("File Does not exist\n");
  }

  memset(buf, 0x00, MAXLINE);
  while((readn = read(fd, buf, MAXLINE)) > 0){
    if((sendn = send(sockfd, buf, readn, 0))< 0){
      perror("Socket send error\n");
      break;
    }
    memset(buf, 0x00, MAXLINE);
  }
  ret = sqlite3_open("filelist.db", &db);
  if(ret == SQLITE_OK){
    char *query = sqlite3_mprintf("update file_info set count = count + 1 where name = '%q';", filename);
    ret = sqlite3_exec(db, query, 0, 0, &errMsg);
    if(ret != SQLITE_OK){
      printf("%s\n",errMsg);
    }
  }
}

int file_list(int sockfd) {
    char *ErrMsg;
    sqlite3 *db;
    int ret;
    ret = sqlite3_open("filelist.db", &db);
    if(ret == SQLITE_OK){
        if((sqlite3_exec(db, "select * from file_info;", callback, (void *)&sockfd, &ErrMsg)) < 0){
            printf("%s\n", ErrMsg);
            return 0;
        }
    }
    else{
      printf("SQL OPEN ERROR\n");
    }
    sqlite3_close(db);
    return 1;
}

int gettime(char *tstr) {
    struct tm *tm_ptr;
    time_t the_time;
    time(&the_time);
    tm_ptr = localtime(&the_time);
    sprintf(tstr, "%d/%d/%d %d:%d:00",
            tm_ptr->tm_year + 1900,
            tm_ptr->tm_mon + 1,
            tm_ptr->tm_mday,
            tm_ptr->tm_hour,
            tm_ptr->tm_min
    );
}
