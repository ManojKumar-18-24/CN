#include<bits/stdc++.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
using namespace std;

void* roll_dice(void * arg) {
    int value = (rand() % 6) + 1;
    int* result =(int*) malloc(sizeof(int));
    *result = value;
    // printf("%d\n", value);
    printf("Thread result: %p\n", result);
    return (void*) result;
}

int main(int argc, char* argv[]) {
    int* res;
    srand(time(NULL));
    pthread_t th;
    if (pthread_create(&th, NULL, &roll_dice, NULL) != 0) {
        return 1;
    }
    if (pthread_join(th, (void**) &res) != 0) {
        return 2;
    }
    printf("Main res: %p\n", res);
    printf("Result: %d\n", *res);
    free(res);
    return 0;
}
