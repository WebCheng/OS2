#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>

/*LOCK same kind of thread (DELETE, INSERT)*/
pthread_mutex_t mutexDele;
pthread_mutex_t mutexInse;

/*LOCK between two kind of threads (INSERT, DELETE) (DELET, SEARCH)*/
pthread_mutex_t mutexInsDele;
pthread_mutex_t mutexSerDele;
sem_t empty;
sem_t full;

int buff[32], bufNum = 0;

int randNumber(int min, int max)
{
    return rand() % (max + 1 - min) + min;
}

void *Search()
{
    while (1)
    {
        /* Wait for delete to finish here */ 
        pthread_mutex_lock(&mutexSerDele);
        printf("Searcher --> %d\n", bufNum);
        pthread_mutex_unlock(&mutexSerDele);
        
        sleep(randNumber(2, 5));
    }
}

void *Insert()
{
    while (1)
    {
        sem_wait(&empty);
        pthread_mutex_lock(&mutexInsDele);

        bufNum++;
        printf("Inserter --> %d\n", bufNum);

        pthread_mutex_unlock(&mutexInsDele);
        sem_post(&full);
        
        sleep(randNumber(3, 7));
    }
}

void *Delete()
{
    while (1)
    {
        sem_wait(&full);
        pthread_mutex_lock(&mutexInsDele);
        pthread_mutex_lock(&mutexSerDele);

        bufNum--;
        printf("Deleter -_-> %d\n", bufNum);

        sem_post(&empty);
        pthread_mutex_unlock(&mutexInsDele);
        pthread_mutex_unlock(&mutexSerDele);

        sleep(randNumber(2, 9));
    }
}

int main(int argc, char *argv[])
{
    int serNum = 2, insNum = 3, deleNum = 2;
    srand(time(NULL));
    pthread_t searcher[serNum], inserter[insNum], deleter[deleNum];

    pthread_mutex_init(&mutexInsDele, NULL);
    pthread_mutex_init(&mutexSerDele, NULL);

    sem_init(&empty, 0, 32);
    sem_init(&full, 0, 0);

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
