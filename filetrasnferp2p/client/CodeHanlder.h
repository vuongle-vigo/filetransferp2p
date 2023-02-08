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
        int recv = recvDataStruct(fd, temp, sizeof(struct fileShareInfor));
        if(recv!= sizeof(struct fileShareInfor)){
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
        int recv = recvDataStruct(fd, temp, sizeof(struct fileShareInfor));
        if(recv!= sizeof(struct fileShareInfor)){
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
        int recv = recvDataStruct(fd, &new_code, sizeof(int)); //new_code / error code
    }
    else if(code == DOWNLOAD_FILE_REQUEST_){
        int recv = recvDataStruct(fd, temp, sizeof(struct fileShareInfor));
        if(recv!= sizeof(struct fileShareInfor)){
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
    return new_code;
}