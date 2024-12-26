#include<bits/stdc++.h>
using namespace std;

int main()
{
    int i=1;
    while(1)
    {
        if(i%10==0)cout<<"9876 News from R3 "<<i<<"th line\n";
        else if(i%5==0)cout<<"/d News from R3 "<<i<<"th line\n";
        else cout<<"News from R3 "<<i<<"th line\n";
        i++;
    }
    return 0;
}
