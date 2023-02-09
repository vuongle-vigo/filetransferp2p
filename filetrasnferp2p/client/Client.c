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
# include <sys/stat.h>
# include <time.h>
# include <stdint.h>

# include "FileShareInfor.h"
# include "DataHanlder.h"
# include "CodeHanlder.h"
# include "CommandHanlder.h"


# define HOST_NAME "127.0.0.1"
# define BUFFER_SIZE 1024
// char filepathupload[BUFFER_SIZE];
int main(int argc, char *argv[]){
    if(argc < 4){
        printf("Usage: %s <host> <port> <port_sharefile>\n", argv[0]);
        exit(0);
    }
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int sockfd_sharefile = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in client_addr;
    struct sockaddr_in clientListen_addr;
    struct sockaddr_in server_addr;
    clientListen_addr.sin_family = AF_INET;
    clientListen_addr.sin_addr.s_addr = inet_addr(HOST_NAME);
    clientListen_addr.sin_port = htons(atoi(argv[3]));
    if(bind(sockfd_sharefile, (struct sockaddr *)&clientListen_addr, sizeof(clientListen_addr)) < 0){
        perror("bind client listen error");
        exit(0);
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) == -1) {
        perror("setsockopt() error...\n");
        exit(1);
    } 
    listen(sockfd_sharefile, 1);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    int cfd = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if(cfd < 0){
        perror("connect to server error");
        exit(0);
    }
    struct pollfd fds[4];
    fds[0].fd = 0;    //stdin
    fds[0].events = POLLIN; // check if have any data from stdin
    fds[1].fd = sockfd; //fd connect to server
    fds[1].events = POLLIN | POLLPRI; // check if have any data from server
    fds[2].fd = sockfd_sharefile; // fd wait connect from client want download file
    fds[2].events = POLLIN | POLLPRI; // check if have any data from client
    int codeResponse = NONE_REQUEST_;
    char cmd[1024];
    FileShareInfor temp = (FileShareInfor)malloc(sizeof(struct fileShareInfor));
    while(1){
        int ret = poll(fds, 4, TIMEOUT*1000);
        if(ret == -1){
            perror("poll");
            return -1;
        }
        if(ret == 0){
            perror("poll timeout");
            return 0;
        }
        if(fds[0].revents & POLLIN){//stdin 
            char buf[BUFFER_SIZE];
            gets(buf);
            strcpy(cmd, buf);
            //command hanlder
            codeResponse = commandHanlder(buf ,fds[1].fd);
        }
        if(fds[1].revents & (POLLIN)){//server data
            char buf[BUFFER_SIZE];
            codeResponse = resposeHanlder(fds[1].fd, codeResponse, temp, atoi(argv[3]), cmd);
            // printf("response: %d\n", codeResponse);
        }
        if(fds[2].revents & POLLIN){//client data
            struct sockaddr_in client_download_addr;
            printf("incoming connect\n");
            int clen = sizeof(client_download_addr);
            fds[3].fd = accept(fds[2].fd, (struct sockaddr *)&client_download_addr, &clen);
            fds[3].events = POLLIN;
            printf("accept to send file: %d\n", fds[3].fd);
        }
        if(fds[3].fd != 0 && (fds[3].revents & POLLIN)){
            char buf[BUFFER_SIZE] = {0};
            int recv = recvData(fds[3].fd, buf, BUFFER_SIZE);
            char *filepath = buf;
            FILE *fp = fopen(filepath, "rb");
            if(!fp){
                perror("open file to send");
                exit(-1);
            }
            char ch;
            int count = 0;
            while (!feof(fp))
            {   
                ch = fgetc(fp);
                int sent = send(fds[3].fd, &ch, 1, 0);
                count += sent;
            }
            printf("send file: %d\n", count);
            fclose(fp);
            fds[3].fd = 0;
        }
    }
}