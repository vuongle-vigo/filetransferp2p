# include <string.h>
const char* filename = "database.txt";

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
        int id = 0;
        char filepath[100] = {0};
        long size_file = 0;
        struct tm time = {0};
        char time_tmp[5][100] = {0};
        uint32_t ip = 0;
        int port = 0;
        fscanf(fp, "%d %s %ld %s %s %s %s %s %u %d\n", &id, filepath, &size_file, time_tmp[0], time_tmp[1], time_tmp[2], time_tmp[3], time_tmp[4], &ip, &port);
        char ttemp[100] = {0};
        for(int i=0; i < 5; i++){
            strcat(ttemp, time_tmp[i]);
            strcat(ttemp, " ");
        }
        strptime(ttemp, "%a %b %d %H:%M:%S %Y", &time);
        tmp = fileShareInit(id, filepath, time, size_file, ip, port);
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
        return 0;
    }
    temp = fsInfor;
    while(temp!=NULL){
        char time[100];
        char *p = asctime(&temp->timeinfo);
        strcpy(time, p);
        time[strlen(time)-1] = '\0';
        fprintf(fp, "%d %s %ld %s %u %d\n", temp->id, temp->filepath, temp->size_file, time, temp->ip, temp->port);
        temp = temp->next;
    }
    fclose(fp);
    return 1;
}