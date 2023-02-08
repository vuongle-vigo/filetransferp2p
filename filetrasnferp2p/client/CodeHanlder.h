extern int recvDataStruct();
enum RequestCode { //size of request : 4 bytes : int
    NONE_REQUEST_ = 0,
    ALLFILE_REQUEST_ = 1,
    OWN_FILE_REQUEST_ = 2,
    DEL_FILE_REQUEST_ = 3,
    UPDATE_FILE_REQUEST_ = 4,
    DOWNLOAD_FILE_REQUEST_ = 5,
    EXIT_REQUEST_ = 6
};
int resposeHanlder(int fd, int code, FileShareInfor temp){
    int new_code = NONE_REQUEST_;
    // if(code == NONE_REQUEST_){
    //     int recv = recvDataStruct(fd, &new_code, sizeof(int));
    //     printf("new_code = %d\n", new_code);
    // }
    if(code == ALLFILE_REQUEST_){
        int recvC = recvDataStruct(fd, temp, sizeof(struct fileShareInfor));
        if(recvC!= sizeof(struct fileShareInfor)){
            int* code = temp;
            new_code = *code;
        }
        else{
            char *time = asctime(&temp->timeinfo);
            time[strlen(time) - 1] = '\0';
            printf("%d %s %ld %s %u %d\n", temp->id, temp->filepath, temp->size_file, time , temp->ip, temp->port);
            return code;
        }
    }
    else if(code == OWN_FILE_REQUEST_){
        int recvC = recvDataStruct(fd, temp, sizeof(struct fileShareInfor));
        if(recvC!= sizeof(struct fileShareInfor)){
            int* code = temp;
            new_code = *code;
        }
        else{
            char *time = asctime(&temp->timeinfo);
            time[strlen(time) - 1] = '\0';
            printf("%d %s %ld %s %u %d\n", temp->id, temp->filepath, temp->size_file, time, temp->ip, temp->port);
            return code;
        }
    }
    else if(code == DEL_FILE_REQUEST_){
        int recvC = recvDataStruct(fd, &new_code, sizeof(int)); //new_code / error code
    }
    else if(code == DOWNLOAD_FILE_REQUEST_){
        int recvC = recvDataStruct(fd, temp, sizeof(struct fileShareInfor));
        if(recvC!= sizeof(struct fileShareInfor)){
            int* code = temp;
            new_code = *code;
        }
        else{
            char *time = asctime(&temp->timeinfo);
            time[strlen(time) - 1] = '\0';
            printf("%d %s %ld %s %u %d\n", temp->id, temp->filepath, temp->size_file, time, temp->ip, temp->port);
            char *p = strtok(temp->filepath, "/");
            char filename[50];
            while(p!=NULL){
                p = strtok(NULL, "/");
                if(p!=NULL){strcpy(filename, p);}
            }
            printf("filename: %s\n", filename);
            FILE *f = fopen("test.jpeg", "wb");
            if(!f){
                perror("Couldn't create file to download");
            }
            else{
                struct sockaddr_in download_addr;
                download_addr.sin_family = AF_INET;
                download_addr.sin_addr.s_addr = temp->ip;
                download_addr.sin_port = temp->port;
                int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                printf("Start connection to download server\n");
                int cfd = connect(sockfd, (struct sockaddr *)&download_addr, sizeof(download_addr));
                if(cfd < 0){
                    perror("connect to download");
                    fclose(f);
                    return code;
                }
                struct pollfd fds;
                fds.events = POLLRDHUP | POLLIN;
                fds.fd = sockfd;
                int count_size = 0;
                while(1){
                    int ret = poll(&fds, 1, 5 * 1000);
                    if(ret == -1){
                        perror("poll");
                        break;
                    }
                    if(ret == 0){
                        perror("timeout");
                        break;
                    }
                    if(fds.revents & POLLIN){
                        char ch;
                        int recvC = recv(fd, &ch, 1, 0);
                        printf("received");
                        count_size += recvC;
                        fputc(ch, f);
                    }
                    else if(fds.revents & POLLRDHUP){
                        // printf("Download complete: %s\n", filename);
                        break;
                    }
                }
                if(count_size == temp->size_file){
                    printf("Download complete: %s\n", filename);
                }
                fclose(f);
            }
            return code;
        }
    }
    return new_code;
}