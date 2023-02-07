#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <limits.h>
#include <stdlib.h>

int decodeResponse(char *data, char *pointer){
    char responseBuffer[10];
    char* start = strstr(data, "$$");
    if(start==0){
        return -1;
    }
    char* end = strstr(start+2, "$$");
    strcpy(pointer, end+2);
    strcpy(responseBuffer, start + 2);
    return atoi(responseBuffer);
}

int main(){
  char pointer[10] = {0};
  char *data = "$$0231a$$";
  printf("%d\n", decodeResponse(data, pointer));
  if(*pointer==NULL){
    printf("error\n");
  }
}