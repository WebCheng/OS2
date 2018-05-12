#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#define BUFFER_SIZE 32

/*LOCK same kind of thread (DELETE, INSERT)*/
sem_t semCon;
pthread_mutex_t mutexVal1, mutexVal2;
pthread_cond_t cond;

/*Assume buffer max siz*/
int curNum = 0;
int ProcNum;

int randNumber(int min, int max)
{
    return rand() % (max + 1 - min) + min;
}

void *Process()
{
    while (1)
    {
        pthread_mutex_lock(&mutexVal1);
        if (curNum == 3)
        {
            printf("Process Full!!\n");
            sem_wait(&semCon);
        }

        /*Print here for observe simply*/
        int val = randNumber(3, 7);
        printf("Process running!! executing time %d!!\n", val);

        curNum++;
        pthread_mutex_unlock(&mutexVal1);

        sleep(val);

        pthread_mutex_lock(&mutexVal2);
        curNum--;
        if (curNum == 0)
        {
            printf("Process Clear!!\n");
            sem_post(&semCon);
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

    ProcNum = atoi(argv[1]);

    srand(time(NULL));
    // int procNum = 4;

    pthread_t proc[ProcNum];
    pthread_mutex_init(&mutexVal1, NULL);
    pthread_mutex_init(&mutexVal2, NULL);
    sem_init(&semCon, 0, 0);

    int i;
    for (i = 0; i < ProcNum; i++)
        pthread_create(&proc[i], NULL, Process, NULL);

    for (i = 0; i < ProcNum; i++)
        pthread_join(proc[i], NULL);

    return 0;
}
