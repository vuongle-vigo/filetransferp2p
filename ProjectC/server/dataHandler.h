# include <stdlib.h>
# include <errno.h>

const char* packageFile = "package.tmp";
#define SIZE_RESPONSE 4
int packingData(FileShareInfor fsInfor){
    FILE *fp = fopen(packageFile, "w");
    if(!fp){
        perror("Couldn't open packageFile to write\n");
        return 0;
    }
    FileShareInfor temp = fsInfor;
    while(temp!=NULL){
        fprintf(fp, "%d %s %s\n", temp->id, temp->filePath, temp->filename);
        temp = temp->next;
    }
    fclose(fp);
    return 1;
}


int packingDataByHost(FileShareInfor fsInfor, long ip){
    FILE *fp = fopen(packageFile, "w");
    if(!fp){
        perror("Couldn't open packageFile to write\n");
        return 0;
    }
    FileShareInfor temp = fsInfor;
    while(temp!=NULL){
        if(temp->ip==ip){
            fprintf(fp, "%d %s %s\n", temp->id, temp->filePath, temp->filename);
        }
        temp = temp->next;
    }
    fclose(fp);
    return 1;   
}

int packingDataById(FileShareInfor fsInfor, int id){
    FILE *fp = fopen(packageFile, "w");
    if(!fp){
        perror("Couldn't open packageFile to write\n");
        return 0;
    }
    FileShareInfor temp = fsInfor;
    while(temp!=NULL){
        if(temp->id==id){
            fprintf(fp, "%s %s %u %d\n", temp->filePath, temp->filename, temp->ip, temp->port);
            break;
        }
        temp = temp->next;
    }
    fclose(fp);
    return 1;   
}

int sendData(int fd, char *buffer, int lenBuff){
    int sent = 0;
    while(1){
        int size_write = send(fd, buffer+sent, (lenBuff-sent), 0);
        if(size_write <= 0)
            break;
        sent += size_write;
    }
    return sent;
}

int recvData(int fd, char *buffer, int maxlenBuff){
    int recvCount = 0;
    int maxSizeRead = 1024;
    while (1){
        int size_read = recv(fd, buffer+recvCount, maxSizeRead, 0);
        recvCount += size_read;
        if(size_read <= 0 || recvCount >= maxlenBuff || size_read < maxSizeRead){
            break;
        }
    }
    return recvCount;
}

int sendPackage(int fd, int responseCode){
    char responseCodeStr[SIZE_RESPONSE];
    sprintf(responseCodeStr, "%d", responseCode);
    char buffer[1024] = "$$";
    strcat(buffer, responseCodeStr);
    sendData(fd, buffer, strlen(buffer));
    sleep(1);
    FILE *fp = fopen(packageFile, "r");
    if(!fp){
        perror("Can't open package.tmp to send\n");
        return 1;
    }
    while(!feof(fp)){
        char buffer[1024] = {0};
        fgets(buffer, 1024, fp);
        int sent = sendData(fd, buffer, strlen(buffer));
    }
}
