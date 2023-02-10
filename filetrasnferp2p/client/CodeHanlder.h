                                                                                                                                                                                                                                                                                                        extern int recvDataStruct();
char* uploadfile1  = "/uploadfile";
enum RequestCode { //size of request : 4 bytes : int
    NONE_REQUEST_ = 0,
    ALLFILE_REQUEST_ = 1,
    OWN_FILE_REQUEST_ = 2,
    DEL_FILE_REQUEST_ = 3,
    UPLOAD_FILE_REQUEST_ = 4,
    DOWNLOAD_FILE_REQUEST_ = 5,
    EXIT_REQUEST_ = 6
};
enum ResponseCode{
    SUCCESS_RESPONSE_ = 21,
    FAIL_RESPONSE_ = 22,
    ERROR_RESPONSE_ = 23
};
// xu ly resopnse code tu server

const char* getRequestName(enum RequestCode requestcode);
const char* getResponseName(enum ResponseCode requestcode);
int resposeHanlder(int fd, int code, FileShareInfor temp, int port_share, char *cmd);

int resposeHanlder(int fd, int code, FileShareInfor temp, int port_share, char *cmd){
    int new_code = NONE_REQUEST_;
    if(code == ALLFILE_REQUEST_){
        int recvC = recvDataStruct(fd, temp, sizeof(struct fileShareInfor));
        if(recvC!= sizeof(struct fileShareInfor)){
            int* code = (int*)temp;                                                                                                                       
            new_code = *code;
        }
        else{
            char *time = asctime(&temp->timeinfo);
            time[strlen(time) - 1] = '\0';
            printf("%d %s %ld %s %u %d", temp->id, temp->filepath, temp->size_file, time , temp->ip, temp->port);
            return code;
        }
    }
    else if(code == OWN_FILE_REQUEST_){
        int recvC = recvDataStruct(fd, temp, sizeof(struct fileShareInfor));
        if(recvC!= sizeof(struct fileShareInfor)){
            int* code = (int*)temp;
            new_code = *code;
        }
        else{
            char *time = asctime(&temp->timeinfo);
            time[strlen(time) - 1] = '\0';
            printf("%d %s %ld %s %u %d", temp->id, temp->filepath, temp->size_file, time, temp->ip, temp->port);
            return code;
        }
    }
    else if(code == DEL_FILE_REQUEST_){
        int recvC = recvDataStruct(fd, &new_code, sizeof(int)); //new_code / error code
    }
    else if(code == DOWNLOAD_FILE_REQUEST_){
        int recvC = recvDataStruct(fd, temp, sizeof(struct fileShareInfor));
        if(recvC!= sizeof(struct fileShareInfor)){
            int* code = (int*)temp;
            new_code = *code;
        }
        else{
            char *time = asctime(&temp->timeinfo);
            time[strlen(time) - 1] = '\0';
            printf("%d %s %ld %s %u %d\n", temp->id, temp->filepath, temp->size_file, time, temp->ip, temp->port);
            char tmpfilepath[100] = {0};
            strcpy(tmpfilepath, temp->filepath);
            char *p = strtok(temp->filepath, "/");
            char filename[50];
            while(p!=NULL){
                p = strtok(NULL, "/");
                if(p!=NULL){strcpy(filename, p);}
            }
            printf("filename: %s\n", filename);
            FILE *f = fopen(filename, "wb");
            if(!f){
                perror("Couldn't create file to download");
            }
            else{
                struct sockaddr_in download_addr;
                download_addr.sin_family = AF_INET;
                download_addr.sin_addr.s_addr = temp->ip;
                download_addr.sin_port = htons(temp->port);
                int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                printf("Start connection to download server\n");
                int cfd = connect(sockfd, (struct sockaddr *)&download_addr, sizeof(download_addr));
                if(cfd < 0){
                    perror("connect to download");
                    fclose(f);
                    return code;
                }
                int sent = sendData(sockfd, tmpfilepath, strlen(tmpfilepath));

                struct pollfd fds;
                fds.events = POLLIN | POLLRDHUP;
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
                        int recvC = recv(sockfd, &ch, 1, 0);
                        count_size += recvC;
                        fputc(ch, f);
                    }
                    else if(fds.revents & POLLRDHUP){      
                        printf("Close connect from download: %s\n", filename);
                        break;
                    }
                }
                if(count_size == temp->size_file){
                    printf("Download complete: %s : %d\n", filename, count_size);
                }
                fclose(f);
            }
            return code;
        }
    }
    else if(code == UPLOAD_FILE_REQUEST_){
        printf("start update file\n");
        char tmp[100];
        recv(fd, tmp, sizeof(tmp), 0);
        FileShareInfor fsInforUpdate = (FileShareInfor)malloc(sizeof(struct fileShareInfor));
        char filepath[PATH_MAX];
        char *filename = cmd + strlen(uploadfile1) + 1;\
        char *res = realpath(filename, filepath);
        printf("send file: %s\n", filepath);
        if(res == NULL){
            printf("realpath failed\n");
            return NONE_REQUEST_;
        }
        else{
            strcpy(fsInforUpdate->filepath, filepath);
            struct stat st;
            stat(filepath, &st);
            fsInforUpdate->size_file = st.st_size;
            fsInforUpdate->port = port_share;

            int sent = sendDataStruct(fd, fsInforUpdate, sizeof(struct fileShareInfor));
            sleep(2);
            int code;
            recvDataStruct(fd, &code, sizeof(int));
            if(code == SUCCESS_RESPONSE_){
                printf("update file success\n");
            }  
            if(code == FAIL_RESPONSE_){
                printf("update file failed\n");
            }
        } 
        new_code = NONE_REQUEST_;
    }
    return new_code;
}

const char* getRequestName(enum RequestCode requestcode) 
{
   switch (requestcode) 
   {
      case NONE_REQUEST_: return "NONE_REQUEST_";
      case ALLFILE_REQUEST_: return "ALLFILE_REQUEST_";
      case OWN_FILE_REQUEST_: return "OWN_FILE_REQUEST_";
      case DEL_FILE_REQUEST_: return "DEL_FILE_REQUEST_";
      case DOWNLOAD_FILE_REQUEST_: return "DOWNLOAD_FILE_REQUEST_";
      case UPLOAD_FILE_REQUEST_: return "UPLOAD_FILE_REQUEST_";
   }
}

const char* getResponseName(enum ResponseCode responsecode){
    switch (responsecode) 
    {
        case SUCCESS_RESPONSE_: return "SUCCESS_RESPONSE_";
        case FAIL_RESPONSE_: return "FAIL_RESPONSE_";
        case ERROR_RESPONSE_: return "ERROR_RESPONSE_";
        default: return "";
    }
}