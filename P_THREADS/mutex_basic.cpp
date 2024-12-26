#include<bits/stdc++.h>
#include<pthread.h>
using namespace std;

pthread_mutex_t mute;
int x=0;

void *routine(void * arg)
{
       for(int i=0;i<100000;i++)
       {
             pthread_mutex_lock(&mute);
             x++;
             pthread_mutex_unlock(&mute);
       }
}

int main()
{
    pthread_t th[4];
    pthread_mutex_init(&mute,NULL);
    for(int i=0;i<4;i++)
    {
         pthread_create(&th[i],NULL,&routine,NULL);
    }
    for(int i=0;i<4;i++)
    {
        pthread_join(th[i],NULL);
    }
    cout<<"value of x= "<<x<<endl;
    pthread_mutex_destroy(&mute);
    return 0;
}
