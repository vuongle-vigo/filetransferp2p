# include <stdlib.h>
# include <errno.h>
// # include "fileInforHandler.h"

const char* filename = "databaseFileInfor.txt";

FileShareInfor readDataBase(FileShareInfor fsInfor);
int writeDataBase(FileShareInfor fsInfor);


FileShareInfor readDataBase(FileShareInfor fsInfor){
    FILE *fp = fopen(filename, "r");
    if(!fp){
        perror("Couldn't open database file to read\n");
        return NULL;
    }
    while(!feof(fp)){
        FileShareInfor tmp;
        int id;
        char filePath[100];
        char filename[50];
        long ip;
        int port;
        fscanf(fp, "%d %s %s %u %d\n", &id, filePath, filename, &ip, &port);
        tmp = fileShareInit(filePath, filename, ip, port);
        fsInfor = addFileInfor(fsInfor, tmp);
    }
    fclose(fp);
    return fsInfor;
}


int writeDataBase(FileShareInfor fsInfor){
    FileShareInfor temp;
    FILE *fp = fopen(filename, "w");
    if(!fp){
        perror("Couldn't open database file to write\n");
        return 1;
    }
    temp = fsInfor;
    while(temp!=NULL){
        fprintf(fp, "%d %s %s %u %d\n", temp->id, temp->filePath, temp->filename, temp->ip, temp->port);
        temp = temp->next;
    }
    fclose(fp);
    return 1;
}