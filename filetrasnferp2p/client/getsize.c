/* localtime example */
#include <limits.h> /* PATH_MAX */
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
int main ()
{
  char buf[PATH_MAX];
  char *res = realpath("../client/test.jpeg", buf);
  if(res==NULL){
    printf("%sx\n", buf);
  }
  else{
    struct stat st;
    stat(buf, &st);
    printf("%d %s\n", st.st_size, buf);
  }
  
}