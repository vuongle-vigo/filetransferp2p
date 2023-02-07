# include <stdlib.h>
# include <errno.h>
# include <string.h>
// # include "fileInforHandler.h"
// # include "dataHandler.h"

const char* SHAREREQUEST = "share";
const char* INFOALL = "info";
const char* DELLFILE = "del";
const char* GET = "get";

enum Code { 
    ALLFILEINFOR = 1,
    OWNFILEINFOR = 2,
    DELFILE = 3,
    FILEDATA2DOWNLOAD = 4,
    STARTSENDFILE = 5
};

int requestHandler(char* request, int fd, FileShareInfor *fsInfor, struct sockaddr_in client[]){
    // enum ResponseCode responseCode;
    char portstr[10];
    request[strlen(request)] = '\0';
    strcpy(portstr, (request+strlen(request) - 7));
    int portclient = atoi(portstr);
    request[strlen(request) - 8] = '\0';
    if(strcmp(request, INFOALL)==0){
        if(*fsInfor == NULL) return 1;
        printf("requestHandler: %s", request);
        packingData(*fsInfor);
        sendPackage(fd, ALLFILEINFOR);
        printf("sendPackage: x");
    }
    if(strncmp(request, DELLFILE, 3)==0){
         if(*fsInfor == NULL) return 1;
        char *p;
        char tmp[10];
        p = strtok(request, " ");
        while(p!=NULL){
            p = strtok(NULL, " ");
            if(p!=NULL){
                strcpy(tmp, p);
            }
        }
        int idFile = atoi(tmp);
        *fsInfor = dellFileInfor(*fsInfor, idFile);
    }
    if(strncmp(request, GET, 3) == 0){
        char *p;
        char tmp[10];
        p = strtok(request, " ");
        while(p!=NULL){
            p = strtok(NULL, " ");
            if(p!=NULL){
                strcpy(tmp, p);
            }
        }
        printf("%s", tmp);
        int idFile = atoi(tmp);
        char buffer[1024] = {0};
        packingDataById(*fsInfor, idFile);
        sendPackage(fd, FILEDATA2DOWNLOAD);
    }
    if(strncmp(request, SHAREREQUEST, 5)==0){
        int i = 0;
        char *p;
        char fileData[100];
        char filename[100];
        char filepath[100];
        p = strtok(request, " ");
        while(p!=NULL){
            p = strtok(NULL, " ");
            if(p!=NULL){
                strcpy(fileData, p);
                i++;
            }
        }
        strcpy(filepath, fileData);
        p = strtok(fileData, "/");
        while(p!=NULL){
            p = strtok(NULL, "/");
            if(p!=NULL){
                strcpy(filename, p);
            }
        }
        filepath[strlen(filepath) - strlen(filename)] = '\0';
        printf("File:%s - %u\n", filepath, client[fd].sin_addr.s_addr);
        FileShareInfor newFileShareInfor = fileShareInit(filepath, filename, client[fd].sin_addr.s_addr, portclient);
        *fsInfor = addFileInfor(*fsInfor, newFileShareInfor);
        writeDataBase(*fsInfor);
    }
    return 1;
}