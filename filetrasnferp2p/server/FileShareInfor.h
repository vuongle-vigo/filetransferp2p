# include <string.h>
# include <stdlib.h>
# include <time.h>
#include <stdint.h>

#ifndef STRUCT_FILESHAREINFOR
#define STRUCT_FILESHAREINFOR

int idFile = 1;

typedef struct fileShareInfor{
    int id;
    char filepath[100];
    long size_file;
    struct tm timeinfo;
    uint32_t ip;
    int port;
    struct fileShareInfor *next;
}*FileShareInfor;
#endif /* STRUCT_FILESHAREINFOR */

FileShareInfor fileShareInit(int id, char *filepath, struct tm timeinfo, long size_file, uint32_t ip, int port);
FileShareInfor addFileInfor(FileShareInfor head, FileShareInfor newFileInfor);
FileShareInfor delHead(FileShareInfor head);
FileShareInfor delTail(FileShareInfor head);
FileShareInfor dellFileInfor(FileShareInfor head, int id);


FileShareInfor fileShareInit(int id, char *filepath, struct tm timeinfo, long size_file, uint32_t ip, int port){
    FileShareInfor fsInfor;
    fsInfor = (FileShareInfor)malloc(sizeof(struct fileShareInfor));
    if(id!=-1){
        fsInfor->id = id;
        idFile = ++id;
    }
    else{
        fsInfor->id = idFile;
        idFile++;
    }
    strcpy(fsInfor->filepath, filepath);
    fsInfor->timeinfo = timeinfo;
    fsInfor->size_file = size_file;
    fsInfor->ip = ip;
    fsInfor->port = port;
    fsInfor->next = NULL;
    return fsInfor;
}

FileShareInfor addFileInfor(FileShareInfor head, FileShareInfor newFileInfor){
    FileShareInfor temp;
    if(head == NULL){
        head = newFileInfor;
    }
    else{
        temp = head;
        while(temp->next != NULL){
            temp = temp->next;
        }
        temp->next = newFileInfor;
    }
    return head;
}

FileShareInfor delHead(FileShareInfor head){
    if(head==NULL){
        return head;
    }
    head = head->next;
    return head;
}

FileShareInfor delTail(FileShareInfor head){
    if(head==NULL || head->next==NULL){
        head = delHead(head);
        return head;
    }
    FileShareInfor tmp = head;
    while(tmp->next->next!=NULL){
        tmp = tmp->next;
    }
    tmp->next = NULL;
    return head;
}

FileShareInfor dellFileInfor(FileShareInfor head, int id){
    FileShareInfor temp;
    if(head->id == id){
        head = delHead(head);
    }
    else{
        temp = head;
        if(temp->next == NULL) return head;
        while(temp->next->id != id && temp->next->next!=NULL){
            temp = temp->next;
        }
        if(temp->next->next == NULL && temp->next->id == id){
            return delTail(head);
        }
        else if(temp->next->id == id){
            temp->next = temp->next->next;
        }
    }
    return head;
}
