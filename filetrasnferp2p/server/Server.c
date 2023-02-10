
#define _GNU_SOURCE
#define TIMEOUT 500000  /*timeout 500000 giây*/

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

# include "FileShareInfor.h"
# include "DataBaseHandler.h"
# include "DataHanlder.h"
# include "RequestHandler.h"

#define HOST_NAME "127.0.0.1"
#define PORT_NUMBER 8080
#define MAX_CONNECTIONS 1024

int g_count = 1;

int deleteConnect(int fd, struct pollfd fds[], struct sockaddr_in caList[]){
    for(int i = 1; i < g_count; i++){
        if(fds[i].fd == fd){
            fds[i].revents = 0;
            for(int j = i;j<g_count-1;j++){
                fds[j] = fds[j+1];
            }
            memset(&caList[fds[i].fd], 0, sizeof(struct sockaddr_in));
            g_count--;
            break;
        }
    }
}

int main(){
    FileShareInfor fsInfor = NULL;
    fsInfor = readDataBase(fsInfor);
    struct sockaddr_in clientList[MAX_CONNECTIONS];
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sockfd < 0){
        perror("socket");
        return -1;
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) == -1) {
        perror("setsockopt() error...\n");
        exit(1);
    }   
    struct sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(HOST_NAME);
    servAddr.sin_port = htons(PORT_NUMBER);
    if(bind(sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) == -1) {
        perror("bind() error...\n");
        exit(1);
    }
    listen(sockfd, 10);
    struct pollfd fds[MAX_CONNECTIONS] = {0};
    fds[0].fd = sockfd;               //fds[0] để nắm bắt thông tin ở sockfd, fd của client được lưu từ fds[1] trở đi
    fds[0].events = POLLIN | POLLPRI;
    for(int i = 1; i < MAX_CONNECTIONS; i++){
        fds[i].events = POLLRDHUP | POLLIN;
    }
    int codeRequest[MAX_CONNECTIONS] = {0};
    while(1){
        int ret = -1;
        ret = poll(fds, g_count, TIMEOUT*1000);
        if(ret == -1){
            perror("poll() error...\n");
            exit(1);
        }
        if(ret == 0){  
            printf("Connection timeout: %d\n", TIMEOUT);
            return 0;
        }
        if(fds[0].revents & POLLIN){ //có kết nối mới đến sockfd, chấp nhận và add fd vào fds để theo dõi
            struct sockaddr_in clientAddr = {0};
            int clen = sizeof(struct sockaddr_in);
            fds[g_count].fd = accept(sockfd, (struct sockaddr *)&clientAddr, &clen);
            clientList[fds[g_count].fd] = clientAddr;
            g_count++;
        }
        for(int i = 1; i < g_count; i++){
            if(fds[i].revents & POLLRDHUP){ //duyệt xem có kết nối từ client đóng không, nếu có thì xóa khỏi fds
                printf("disconnect:%d\n", fds[i].fd);
                deleteConnect(fds[i].fd, fds, clientList);
            }
            else if(fds[i].revents & POLLIN){ //duyệt xem có dữ liệu có thể đọc được đến từ client không
                //gọi request hanlder để xử lý dữ liệu được gửi đến
                codeRequest[fds[i].fd] = requestHandler(codeRequest[fds[i].fd], fds[i].fd, &fsInfor, clientList, fds);
                printf("Set code: %s\n", getRequestName(codeRequest[fds[i].fd]));
            }
        } 
    }
}