
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


int recvDataStruct(int fd, void *buffer, int maxlenBuff){
    int recvCount = 0;
    int maxSizeRead = 1024;
    while (1){
        int size_read = read(fd, buffer+recvCount, maxSizeRead);
        recvCount += size_read;
        if(size_read <= 0 || recvCount >= maxlenBuff || size_read < maxSizeRead){
            break;
        }
    }
    return recvCount;
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


int sendDataStruct(int fd, void *buffer, int lenBuff){
    int sent = 0;
    while(1){
        int size_write = write(fd, buffer+sent, (lenBuff-sent));
        if(size_write <= 0)
            break;
        sent += size_write;
    }
    return sent;
}
