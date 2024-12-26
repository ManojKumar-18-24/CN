#include<bits/stdc++.h>
#include<unistd.h>
#include<fcntl.h>
using namespace std;

int main()
{
   FILE* fp;
   fp=popen("p2.exe","w");
   //dup2(fp,1);
   cout<<1;
   return 0;
}
