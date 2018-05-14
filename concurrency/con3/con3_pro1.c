#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h> 
 
pthread_mutex_t mutexVal1, mutexVal2;  
int curNum = 0; 

int randNumber(int min, int max)
{
    return rand() % (max + 1 - min) + min;
}

void *Process()
{
    while (1)
    {
        pthread_mutex_lock(&mutexVal1);
        curNum++; 
        int val = randNumber(3, 7);
        printf("Process running!! executing time %d!!\n", val);

        //If pthread == 3 then 
        //      unlock when every threads complete
        if (curNum != 3)
            pthread_mutex_unlock(&mutexVal1);
        else
            printf("Process Full!!\n");

        //Do some thing
        sleep(val);

        pthread_mutex_lock(&mutexVal2);
        curNum--;
        if (curNum == 0)
        {
            printf("Process Clear!!\n");
            pthread_mutex_unlock(&mutexVal1);
        }
        pthread_mutex_unlock(&mutexVal2);

        sleep(randNumber(2, 5));
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("COMMAND LINE$ con3_pro1 <Tthreads Number> \n");
        return -1;
    }

    int procNum = atoi(argv[1]);

    srand(time(NULL)); 

    pthread_t proc[procNum];
    pthread_mutex_init(&mutexVal1, NULL);
    pthread_mutex_init(&mutexVal2, NULL); 

    int i;
    for (i = 0; i < procNum; i++)
        pthread_create(&proc[i], NULL, Process, NULL);

    for (i = 0; i < procNum; i++)
        pthread_join(proc[i], NULL);

    return 0;
}
