#include<bits/stdc++.h>
using namespace std;


int main(int argc,char* argv[])
{
    string s;
    cin>>s;
    for(int i=0;i<s.length();i++)
    {
            s[i]='c';
    }
    cout<<s<<" "<<argv[1]<<endl;
    return 0;
}
