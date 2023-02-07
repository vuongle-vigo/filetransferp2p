#define _GNU_SOURCE
#define TIMEOUT 50  /*timeout 5 giây*/

# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <unistd.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <errno.h>
# include <sys/poll.h>
# include <limits.h>
# include "downloadHandler.h"
# include "recodeHandler.h"


struct sockaddr* setsocketaddr(char *host, int port){
    if(port==0){return NULL;}
    struct sockaddr_in addr_in;
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(port);
    addr_in.sin_addr.s_addr = inet_addr(host);
    memset(&addr_in.sin_zero, 0, sizeof(addr_in.sin_zero));
    struct sockaddr* addr = (struct sockaddr *)&addr_in; 
    return addr;
}

int setSock(char *host, int port){
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0){
        perror("socket");
        exit(1);
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) == -1) {
        perror("setsockopt() error...\n");
        exit(1);
    }   
    struct sockaddr* addr = setsocketaddr(host, port);
    if(port == 0) return sockfd;
    if(bind(sockfd, addr, sizeof(struct sockaddr)) < 0 ){perror("bind() error...\n");exit(1);}
    return sockfd;
}

int main(int argc, char *argv[]){
    if(argc < 3){exit(1);}
    char preData[1024] = {0};
    int responseCode = 0;
    char *myhost = "127.0.0.1";
    int myport = atoi(argv[2]);
    char *myportstr = " 9090   "; //size = 8
    char *host = "127.0.0.1";
    int port = atoi(argv[1]);
    int sockfd = setSock(NULL, 0);
    int sockfd_listen = setSock(myhost, myport);
    listen(sockfd_listen, 1);
    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = inet_addr(host);
    sa.sin_port = htons(port);
    int cfd = connect(sockfd, (struct sockaddr *)&sa, sizeof(sa));
    if(cfd<0){
        perror("connect");
        exit(1);
    }
    struct pollfd fds[4];
    fds[0].fd = 0;
    fds[0].events = POLLIN;
    fds[1].fd = sockfd;
    fds[1].events = POLLIN;
    fds[2].fd = sockfd_listen;
    fds[2].events = POLLIN | POLLPRI;

    while(1){
        int ret = poll(fds, 4, TIMEOUT*1000);
        if(-1 == ret){
            perror("poll");
            return -1;
        }
        if(ret == 0){
            printf("poll() timed out: %d\n", TIMEOUT);
            return 0;
        }
        if(fds[1].revents & POLLIN){ // xu ly voi server
            char buf[1024] = {0};
            int recv = recvData(fds[1].fd, buf, sizeof(buf));
            if(decodeCode(buf)!=-1){
                responseCode = decodeCode(buf);
                printf("code:%d\n", responseCode);
            }
            codeHanlder(buf, responseCode);
        }
        if(fds[0].revents & POLLIN){
            char buf[1024] = {0};
            gets(buf);
            strcat(buf, myportstr);
            int sent = sendData(fds[1].fd, buf, strlen(buf)+1);
        }
        if(fds[2].revents & POLLIN){
            struct sockaddr_in ca =  {0};
            int addrlen = sizeof(ca);
            fds[3].fd = accept(sockfd_listen, (struct sockaddr*)&ca, &addrlen);
            fds[3].events = POLLIN;
            printf("incoming connection: %d\n", ca.sin_port);
        }
        if(fds[3].fd != 0 && (fds[3].revents & POLLIN)){
            char buf[1024] = {0};
            int recv = recvData(fds[3].fd, buf, sizeof(buf));
            char *path = realpath(buf, NULL);
            FILE *fp = fopen(path, "rb");
            if(!fp){
                perror("Open file to send");
                exit(-1);
            }
            char codeStr[10] = {0};
            encodeCode(STARTSENDFILE, codeStr);
            send(fds[3].fd, codeStr, strlen(codeStr), 0);
            int count = 0;
            char ch;    
            while(!feof(fp)){
                ch = fgetc(fp);
                int sent = send(fds[3].fd, &ch, 1, 0);
                count += sent;
            }   
            printf("sent: %d\n", count);
            fclose(fp);
            fds[3].fd = 0;
        }
    }
}