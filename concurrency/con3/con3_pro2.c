#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#define BUFFER_SIZE 32

/*LOCK same kind of thread (DELETE, INSERT)*/
pthread_mutex_t mutexDele;
pthread_mutex_t mutexInse;

/*LOCK between two kind of threads (INSERT, DELETE) (DELET, SEARCH)*/
pthread_mutex_t mutexInsDele;
pthread_mutex_t mutexSerDele;
sem_t empty;
sem_t full;

/*Assume buffer max siz*/
int bufNum = 0, buf[BUFFER_SIZE];
int headIdx = 0, tailIdx = 0;

void bufferPush(int val)
{
    buf[tailIdx] = val;
    bufNum++;
    tailIdx++;
    if (tailIdx >= BUFFER_SIZE)
        tailIdx = 0;
}

int bufferPull()
{
    int x = buf[headIdx];
    headIdx++;
    if (headIdx >= BUFFER_SIZE)
        headIdx = 0;
    bufNum--;
    return x;
}

int randNumber(int min, int max)
{
    return rand() % (max + 1 - min) + min;
}

void *Search()
{
    while (1)
    {
        /* Wait for delete to finish here 
         * Using pthread_mutex_trylock to avoid lock others searcher threads  */  
        if (pthread_mutex_trylock(&mutexSerDele) == 0)
        {
            printf("Searcher --> %d\n", bufNum);
            pthread_mutex_unlock(&mutexSerDele);
        }
        sleep(1); 
    }
}

void *Insert()
{
    while (1)
    {
        sem_wait(&empty);
        pthread_mutex_lock(&mutexInsDele);
        printf("Inserter ------>  Start \n");

        int val = randNumber(3, 7);
        bufferPush(val);

        printf("Inserter --> %d | %d\n", bufNum, val);

        /*Testing other threads*/
        sleep(1);

        printf("Inserter ------>  End \n");
        pthread_mutex_unlock(&mutexInsDele);
        sem_post(&full);

        sleep(val);
    }
}

void *Delete()
{
    while (1)
    {
        sem_wait(&full);
        pthread_mutex_lock(&mutexInsDele);
        pthread_mutex_lock(&mutexSerDele);
        printf("Deleter -------->  Start \n");

        int val = bufferPull();
        printf("Deleter ---> %d | %d\n", bufNum, val);

        /*Testing other threads*/
        sleep(1);

        printf("Deleter -------->  End \n");
        sem_post(&empty);
        pthread_mutex_unlock(&mutexInsDele);
        pthread_mutex_unlock(&mutexSerDele);

        sleep(randNumber(2, 9));
    }
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("COMMAND LINE$ con3_pro2 <Searcher threads> <Inserter threads> <Deleter threads>\n");
        return -1;
    }
    int serNum = atoi(argv[1]), insNum = atoi(argv[2]), deleNum = atoi(argv[3]);

    srand(time(NULL));
    // int serNum = 2, insNum = 3, deleNum = 2;

    pthread_t searcher[serNum], inserter[insNum], deleter[deleNum];

    pthread_mutex_init(&mutexInsDele, NULL);
    pthread_mutex_init(&mutexSerDele, NULL);

    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);

    printf("\n\n\nAction | BufSize | BufNum\n");
    int i;
    for (i = 0; i < serNum; i++)
        pthread_create(&searcher[i], NULL, Search, NULL);
    for (i = 0; i < insNum; i++)
        pthread_create(&inserter[i], NULL, Insert, NULL);
    for (i = 0; i < deleNum; i++)
        pthread_create(&deleter[i], NULL, Delete, NULL);

    for (i = 0; i < serNum; i++)
        pthread_join(searcher[i], NULL);
    for (i = 0; i < insNum; i++)
        pthread_join(inserter[i], NULL);
    for (i = 0; i < deleNum; i++)
        pthread_join(deleter[i], NULL);

    return 0;
}
