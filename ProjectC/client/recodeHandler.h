# include <stdlib.h>
# include <stdio.h>
# include <errno.h>
# include <string.h>
#define SIZE 4 //bits

enum Code { 
    ALLFILEINFOR = 1,
    OWNFILEINFOR = 2,
    DELFILE = 3,
    FILEDATA2DOWNLOAD = 4,
    STARTSENDFILE = 5
};

void encodeCode(int code, char *codeStr){
    strcat(codeStr, "$$");
    sprintf(codeStr + strlen(codeStr), "%d", code);
    strcat(codeStr, "$$");
}

int decodeCode(char *data){
    char responseBuffer[10];
    char* start = strstr(data, "$$");
    if(start==0){
        return -1;
    }
    char* end = strstr(start+2, "$$");
    // strcpy(pointer, end+2);
    strcpy(responseBuffer, start + 2);
    return atoi(responseBuffer);
}

int codeHanlder(char *data, int code){
    char *pointer[1024] = {0};
    int responseCode = decodeCode(data);
    if(responseCode == -1 && ((code & ALLFILEINFOR) == ALLFILEINFOR)){//du lieu thuong
        printf("%s", data);
        return code;
    }
    if(responseCode == -1 && ((code & OWNFILEINFOR) == OWNFILEINFOR)){
        printf("%s", data);
        return code;
    }
    if(responseCode == -1 && ((code & FILEDATA2DOWNLOAD) == FILEDATA2DOWNLOAD)){
        int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        struct sockaddr_in ca2download;
        char filepath[100];
        char filename[100];
        uint32_t ip;
        int port;
        char * token = strtok(data, " ");
        // Lấy ra toàn bộ token
        strcpy(filepath, token);
        token = strtok(NULL, " ");
        strcpy(filename, token);
        token = strtok(NULL, " ");
        ip = atoi(token);
        token = strtok(NULL, " ");
        port = atoi(token);
        ca2download.sin_family = AF_INET;
        ca2download.sin_addr.s_addr = ip;
        ca2download.sin_port = htons(port);
        int cfd = connect(sockfd, (struct sockaddr *)&ca2download, sizeof(ca2download));
        if(cfd<0){
            perror("connect");
            exit(1);
        }
        char buf[1024] = {0};
        strcat(buf, filepath);
        strcat(buf, filename);
        sendData(sockfd, buf, strlen(buf));
        printf("buf: %s\n", buf);
        printf("Downloading\n"); 
        struct pollfd fds;
        fds.fd = sockfd;
        fds.events = POLLIN | POLLRDHUP;
        memset(buf, 0, sizeof(buf));
        int recv1 = recvData(sockfd, buf, sizeof(buf));
        int codeRecvFile = decodeCode(buf); 
        printf("recv data: %s\n", buf);
        FILE *f = fopen("test1.jpeg", "wb");
        int count = 0;
        while(1){
            int ret = poll(&fds, 1, 5*1000);
            if(-1 == ret){
                perror("poll");
                return -1;
            }
            if(ret == 0){
                printf("poll() timed out: %d\n", TIMEOUT);
                fclose(f);
                break;
            }
            if(fds.revents & POLLIN){
                int ch;
                int recvc = recv(sockfd, &ch, 1, 0);
                fputc(ch, f);
            }
            if(fds.revents & POLLRDHUP){
                fclose(f);
                break;
            }
        }
        printf("counters: %d\n", count);
        printf("download complete\n");
        close(sockfd);
        memset(&fds, 0, sizeof(fds));
    }
    return code;
}