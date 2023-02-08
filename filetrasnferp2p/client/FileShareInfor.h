# include <stdint.h>

typedef struct fileShareInfor{
    int id;
    char filepath[100];
    long size_file;
    struct tm timeinfo;
    uint32_t ip;
    int port;
    struct fileShareInfor *next;
}*FileShareInfor;


