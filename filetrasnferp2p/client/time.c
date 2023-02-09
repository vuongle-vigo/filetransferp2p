/* localtime example */
#include <stdio.h>
#include <time.h>

int main ()
{
  time_t rawtime;
  struct tm * timeinfo;

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  char *tmp = asctime ( timeinfo );
  printf("%s\n", tmp );
  struct tm birth_struct;
  strptime(tmp, "%a %b %d %H:%M:%S %Y", &birth_struct);
  printf ( "Current local time and date: %s", asctime (&birth_struct));
  
  return 0;
}