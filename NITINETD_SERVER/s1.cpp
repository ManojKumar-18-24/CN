#include<bits/stdc++.h>
using namespace std;

int main(int argc,char* argv[])
{
     string a;
     cin>>a;
     for(int i=0;i<a.length();i++)
     {
            a[i]='a';
     }
     cout<<a<<" from "<<argv[1]<<endl;
     return 0;
}
