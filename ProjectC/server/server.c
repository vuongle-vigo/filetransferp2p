
#define _GNU_SOURCE
#define TIMEOUT 5000  /*timeout 5 giây*/

# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <unistd.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <errno.h>
# include <sys/socket.h>
# include <signal.h>
# include <wait.h>
# include <pthread.h>
#include <sys/poll.h>

# include "fileInforHandler.h"
# include "databaseHandler.h"
# include "dataHandler.h"
# include "requestHandler.h"

#define MAX_CONNECT 1024
int g_count = 1;


struct sockaddr* setsocketaddr(char *host, int port){
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
    if(bind(sockfd, addr, sizeof(struct sockaddr)) < 0 ){perror("bind() error...\n");exit(1);}
    return sockfd;
}

int deleteConnect(int fd, struct pollfd fds[]){
    for(int i = 1; i < g_count; i++){
        if(fds[i].fd == fd){
            fds[i].revents = 0;
            for(int j = i;j<g_count-1;j++){
                fds[j] = fds[j+1];
            }
            g_count--;
            break;
        }
    }
}

int main(){
    FileShareInfor fileShareInfor = readDataBase(fileShareInfor);
    struct sockaddr_in client[MAX_CONNECT] = {0};
    int port = 8080;
    int sockfd = setSock("127.0.0.1", port);
    listen(sockfd, 5);
    struct pollfd fds[100] = {0};
    fds[0].fd = sockfd;
    fds[0].events = POLLIN | POLLPRI;
    while(1){
        int ret = -1;
        for(int i = 1;i < 100; i++){
            fds[i].events = POLLRDHUP | POLLIN;
        }
        ret = poll(fds, g_count, TIMEOUT * 1000);
        if(ret == -1){
            perror("poll() error!");
            return -1;
        }
        if (0 == ret){
            printf ("poll() timeout sau %d giay.\n", TIMEOUT);
            return 0;
        }
        if(fds[0].revents & POLLIN){
            struct sockaddr_in ca =  {0};
            int addrlen = sizeof(ca);
            fds[g_count].fd = accept(sockfd, (struct sockaddr*)&ca, &addrlen);
            client[fds[g_count].fd] = ca;
            printf("new connect: %d - %d -%d\n", fds[g_count].fd, client[fds[g_count].fd].sin_port, client[fds[g_count].fd].sin_addr);
            g_count++;
        }
        for(int i = 1;i < g_count; i++){
            if(fds[i].revents & POLLRDHUP){
                printf("disconnect:%d\n", fds[i].fd);
                deleteConnect(fds[i].fd, fds);
            }
            else if(fds[i].revents & POLLIN){
                char buffer[100] = {0};
                int recv = recvData(fds[i].fd, buffer, sizeof(buffer));
                requestHandler(buffer, fds[i].fd, &fileShareInfor, client);
            }
        }
    }
}