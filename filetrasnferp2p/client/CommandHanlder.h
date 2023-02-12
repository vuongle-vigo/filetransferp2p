# include <string.h>
const char* getallfiles = "/getallfiles";
const char* getownfiles = "/getownfiles";
const char* deletefile  = "/deletefile";
const char* downloadfile = "/downloadfile";
const char* uploadfile  = "/uploadfile";
const char* quit = "/quit";
int commandHanlder(char *command, int fd){
    if(strcmp(command, getallfiles) == 0){
        int code = ALLFILE_REQUEST_;
        int sent = sendDataStruct(fd, &code, sizeof(code));
        return ALLFILE_REQUEST_;
    }
    else if(strcmp(command, getownfiles) == 0){
        int code = OWN_FILE_REQUEST_;
        int sent = sendDataStruct(fd, &code, sizeof(code));
        return OWN_FILE_REQUEST_;
    }
    else if(strncmp(command, deletefile, strlen(deletefile)) == 0){
        int code = DEL_FILE_REQUEST_;
        int sent = sendDataStruct(fd, &code, sizeof(code));
        char *p = command + strlen(deletefile) + 1;
        int idFile = atoi(p);
        printf("delete file id = %d\n", idFile);
        sleep(0.1);
        sent = sendDataStruct(fd, &idFile, sizeof(idFile));
        return DEL_FILE_REQUEST_;
    }
    else if(strncmp(command, downloadfile, strlen(downloadfile)) == 0){
        int code = DOWNLOAD_FILE_REQUEST_;
        int sent = sendDataStruct(fd, &code, sizeof(code));
        char *p = command + strlen(downloadfile) + 1;
        int idFile = atoi(p);
        printf("download file id = %d\n", idFile);
        sent = sendDataStruct(fd, &idFile, sizeof(idFile));
        return DOWNLOAD_FILE_REQUEST_;
    }
    else if(strncmp(command, uploadfile, strlen(uploadfile)) == 0){
        int code = UPLOAD_FILE_REQUEST_;
        int sent = sendDataStruct(fd, &code, sizeof(code));
        sleep(1);
        return UPLOAD_FILE_REQUEST_;
    }
    else if(strcmp(command, quit)==0){
        printf("Exiting...\n");
        exit(0);
    }
    else{
        printf("Command: '%s' invalid\n", command);
    }
    return NONE_REQUEST_;
}