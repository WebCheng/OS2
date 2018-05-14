#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#define BUFFER_SIZE 32

/*LOCK same kind of thread (DELETE, INSERT)*/
pthread_mutex_t mutexVal1;
pthread_mutex_t mutexVal2;

/*LOCK between two kind of threads (INSERT, DELETE) (DELET, SEARCH)*/
pthread_mutex_t mutexInsDele;
pthread_mutex_t mutexSerDele;
sem_t empty;
sem_t full;

/*Assume buffer max siz*/
struct link
{
    int value;
    struct link *next;
};

struct single_link
{
    struct link *head;
    struct link *tail;
    int count;
} * Link;

void InsertValue(int val)
{
    struct link *newLink;
    newLink = malloc(sizeof(struct link));
    newLink->value = val;
    newLink->next = NULL;

    if (Link->tail)
        Link->tail->next = newLink;

    Link->tail = newLink;

    if (!Link->head)
        Link->head = newLink;

    Link->count++;
}

int RemoveValue()
{
    struct link *head = Link->head;   
    int val = head->value;
    Link->head = head->next;
    Link->count++;

    if (Link->tail == head)
        Link->tail = NULL; 

    free(head); 
    return val;
}

int randNumber(int min, int max)
{
    return rand() % (max + 1 - min) + min;
}

int curNum;
char lockerName;
void *Search()
{
    int val;
    while (1)
    {
        pthread_mutex_lock(&mutexVal1);
        if (pthread_mutex_trylock(&mutexSerDele) == 0)
            lockerName = 'S';
        else
        {
            if (lockerName == 'D')
                pthread_mutex_lock(&mutexSerDele);
            else // == "S"
            {
                /*If searching threads are too many wait*/
                if (curNum == 3)
                {
                    printf("Search process Full!!\n");
                    pthread_mutex_lock(&mutexSerDele);
                }
            }
        }
        curNum++;
        pthread_mutex_unlock(&mutexVal1);

        /*Searching Execution */
        val = randNumber(2, 4);
        printf("Searching --> %d     \n", curNum);
        sleep(val);

        pthread_mutex_lock(&mutexVal2);
        curNum--;
        if (curNum == 0)
        {
            printf("Process Clear!!\n");
            lockerName = 'D';
            pthread_mutex_unlock(&mutexSerDele);
        }
        pthread_mutex_unlock(&mutexVal2);
        sleep(randNumber(5, 10));
    }
}

void *Insert()
{
    int val;
    while (1)
    {
        sem_wait(&empty);
        pthread_mutex_lock(&mutexInsDele);
        printf("Inserter ------>  Start \n");

        val = randNumber(3, 7);
        InsertValue(val);

        printf("Inserter --> %d | %d\n", Link->count, val);

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
    int val;
    while (1)
    {
        sem_wait(&full);
        pthread_mutex_lock(&mutexSerDele);
        pthread_mutex_lock(&mutexInsDele);
        printf("Deleter -------->  Start \n");

        val = RemoveValue();
        printf("Deleter ---> %d | %d\n", Link->count, val);

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

    Link = malloc(sizeof(struct single_link));
    Link->head = NULL;
    Link->tail = NULL;
    Link->count = 0;

    srand(time(NULL));
    // int serNum = 2, insNum = 3, deleNum = 2;

    pthread_t searcher[serNum], inserter[insNum], deleter[deleNum];

    pthread_mutex_init(&mutexInsDele, NULL);
    pthread_mutex_init(&mutexSerDele, NULL);
    pthread_mutex_init(&mutexVal1, NULL);
    pthread_mutex_init(&mutexVal2, NULL);

    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);

    printf("\n\n\nAction | Link Size | Link Value\n");
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
