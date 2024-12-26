#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>

int main()
{
   FILE * fd;
   char buffer[100];
   sprintf(buffer,"hi my name is manojkumar");
   fd=popen("wc -w","w");
   fwrite(buffer,sizeof(char),strlen(buffer),fd);
   pclose(fd);
   return 0;
}
