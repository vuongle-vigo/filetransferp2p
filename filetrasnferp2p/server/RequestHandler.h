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

const char* getRequestName(enum RequestCode requestcode);
int checkOnlineClient(uint32_t ip, struct sockaddr_in caList[], struct pollfd fds[]);
int requestHandler(int code, int fd, FileShareInfor *fsInfor, struct sockaddr_in caList[], struct pollfd fds[]);


int requestHandler(int code, int fd, FileShareInfor *fsInfor, struct sockaddr_in caList[], struct pollfd fds[]){
    if(code == NONE_REQUEST_){
        int new_code;
        recvDataStruct(fd, &new_code, sizeof(int));
        printf("New response: %s\n", getRequestName(new_code));
        code = requestHandler(new_code, fd, fsInfor, caList, fds);
    }
    else{
        if(code == ALLFILE_REQUEST_){
            FileShareInfor tmp = *fsInfor;
            while(tmp != NULL){
                if(checkOnlineClient(tmp->ip, caList, fds)){
                    sendDataStruct(fd, tmp, sizeof(struct fileShareInfor));
                    sleep(0.1);
                }
                tmp = tmp->next;
            }
            int codeResponse = SUCCESS_RESPONSE_;
            sendDataStruct(fd, &codeResponse, sizeof(int));
            code = NONE_REQUEST_;
        }
        else if(code == OWN_FILE_REQUEST_){
            FileShareInfor tmp = *fsInfor;
            while(tmp!= NULL){
                if(tmp->ip == caList[fd].sin_addr.s_addr){
                    sendDataStruct(fd, tmp, sizeof(struct fileShareInfor));
                    sleep(0.1);
                }
                tmp = tmp->next;
            }
            int codeResponse = SUCCESS_RESPONSE_;
            sendDataStruct(fd, &codeResponse, sizeof(int));
            code = NONE_REQUEST_;
        }
        else if(code == DEL_FILE_REQUEST_){
            int idFile;
            int recv = recvDataStruct(fd, &idFile, sizeof(int));
            FileShareInfor tmp = *fsInfor;
            int checkOwner = 0;
            while(tmp!=NULL){
                if(tmp->id == idFile && tmp->ip == caList[fd].sin_addr.s_addr){
                    checkOwner = 1;
                    break;
                }
                tmp = tmp->next;
            }
            if(checkOwner == 1){
                *fsInfor = dellFileInfor(*fsInfor, idFile);
                int codeResponse;
                if(writeDataBase(*fsInfor)){
                    codeResponse = SUCCESS_RESPONSE_;
                }
                else{
                    codeResponse = FAIL_RESPONSE_;
                }
                sleep(1);
                sendDataStruct(fd, &codeResponse, sizeof(int));
            }else{
                int codeResponse = FAIL_RESPONSE_;
                sleep(1);
                sendDataStruct(fd, &codeResponse, sizeof(int));
            }
            code = NONE_REQUEST_;
        }
        else if(code == DOWNLOAD_FILE_REQUEST_){
            int idFile;
            FileShareInfor tmp = *fsInfor;
            int recv = recvDataStruct(fd, &idFile, sizeof(int));
            while(tmp!= NULL){
                if(tmp->id==idFile){
                    break;
                }
                tmp = tmp->next;
            }
            if(checkOnlineClient(tmp->ip, caList, fds)){
                sleep(1);
                sendDataStruct(fd, tmp, sizeof(struct fileShareInfor));
            }
            else{
                sleep(1);
                int codeResponse = ERROR_RESPONSE_;
                sendDataStruct(fd, &codeResponse, sizeof(codeResponse)); 
            }
            // int codeResponse = NONE_REQUEST_;
            // sendDataStruct(fd, &codeResponse, sizeof(int));
            code = NONE_REQUEST_;
        }
        else if(code == UPLOAD_FILE_REQUEST_){
            send(fd, "NOP", 3, 0);
            FileShareInfor tmp = (FileShareInfor)malloc(sizeof(struct fileShareInfor));
            int recvCount = recvDataStruct(fd, tmp, sizeof(struct fileShareInfor));
            printf("recvCount = %d\n", recvCount);
            time_t rawtime;
            struct tm * timeinfo;
            time(&rawtime);
            timeinfo = localtime ( &rawtime );
            tmp->timeinfo = *timeinfo;
            FileShareInfor fileupdate = fileShareInit(-1, tmp->filepath, tmp->timeinfo, tmp->size_file, caList[fd].sin_addr.s_addr, tmp->port);
            printf("filename = %s\n", fileupdate->filepath);
            addFileInfor(*fsInfor, fileupdate);
            int codeResponse;
            int stt = writeDataBase(*fsInfor);
            if(stt){
                codeResponse = SUCCESS_RESPONSE_;
            }else{
                codeResponse = FAIL_RESPONSE_;
            }
            sleep(1);
            free(tmp);
            sendDataStruct(fd, &codeResponse, sizeof(int));
            code = NONE_REQUEST_;
        }
    }
    return code;
}


int checkOnlineClient(uint32_t ip, struct sockaddr_in caList[], struct pollfd fds[]){
    int i = 1;
    while(fds[i].fd != 0){
        if(caList[fds[i].fd].sin_addr.s_addr == ip){
            return 1;
        }
        i++;
    }
    return 0;
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
      default: return "UNKNOWN_REQUEST_";
   }
}

