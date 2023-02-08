
enum RequestCode { //size of request : 4 bytes : int
    NONE_REQUEST_ = 0,
    ALLFILE_REQUEST_ = 1,
    OWN_FILE_REQUEST_ = 2,
    DEL_FILE_REQUEST_ = 3,
    DOWNLOAD_FILE_REQUEST_ = 5,
    EXIT_REQUEST_ = 6
};


int requestHandler(int code, int fd, FileShareInfor *fsInfor, struct sockaddr_in ca){
    if(code == NONE_REQUEST_){
        int new_code;
        recvDataStruct(fd, &new_code, sizeof(int));
        printf("new_code = %d\n", new_code);
        code = requestHandler(new_code, fd, fsInfor, ca);
    }
    else{
        if(code == ALLFILE_REQUEST_){
            FileShareInfor tmp = *fsInfor;
            while(tmp != NULL){
                sendDataStruct(fd, tmp, sizeof(struct fileShareInfor));
                sleep(0.1);
                tmp = tmp->next;
            }
            int codeResponse = NONE_REQUEST_;
            sendDataStruct(fd, &codeResponse, sizeof(int));
            code = NONE_REQUEST_;
        }
        else if(code == OWN_FILE_REQUEST_){
            FileShareInfor tmp = *fsInfor;
            while(tmp!= NULL){
                if(tmp->ip == ca.sin_addr.s_addr){
                    sendDataStruct(fd, tmp, sizeof(struct fileShareInfor));
                    sleep(0.1);
                }
                tmp = tmp->next;
            }
            int codeResponse = NONE_REQUEST_;
            sendDataStruct(fd, &codeResponse, sizeof(int));
            code = NONE_REQUEST_;
        }
        else if(code == DEL_FILE_REQUEST_){
            int idFile;
            int recv = recvDataStruct(fd, &idFile, sizeof(int));
            *fsInfor = dellFileInfor(*fsInfor, idFile);
            writeDataBase(*fsInfor);
            int codeResponse = NONE_REQUEST_;
            sendDataStruct(fd, &codeResponse, sizeof(int));
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
            sendDataStruct(fd, tmp, sizeof(struct fileShareInfor));
            sleep(0.1);
            int codeResponse = NONE_REQUEST_;
            sendDataStruct(fd, &codeResponse, sizeof(int));
            code = NONE_REQUEST_;
        }
    }
    return code;
}