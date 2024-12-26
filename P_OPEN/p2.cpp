#include<bits/stdc++.h>
#include<unistd.h>
#include<fcntl.h>
using namespace std;

int main()
{
   int x;
   cin>>x;
   FILE* fp;
   fp=popen("p3.exe","w");
   //dup2(fp,1);
   cout<<x;
   return 0;
}
