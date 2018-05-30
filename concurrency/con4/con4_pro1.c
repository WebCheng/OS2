/*
 * CS544 : Concurrency project #4 - Barber, customers, waiting chairs and barber chair.
 * Name  : Webster Cheng
 * OSUID : 933-295-317
 */

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

pthread_mutex_t access_mutex;
pthread_mutex_t barber_chair_mutex;
pthread_mutex_t hair_cut_mutex;
sem_t hairComplete;
sem_t waitingChairs;
int waitChairs;

int randNumber(int min, int max)
{
    return rand() % (max + 1 - min) + min;
}

void *barber()
{
    while (1)
    {
        pthread_mutex_lock(&hair_cut_mutex);

        pthread_mutex_lock(&access_mutex);
        waitChairs++;
        pthread_mutex_unlock(&access_mutex);

        /*Hair Cut 3 to 6 Secs*/
        sleep(randNumber(3, 6));
        printf("Barber FINISH cuting hair\n");
        sem_post(&hairComplete);
        pthread_mutex_unlock(&barber_chair_mutex);
    }
}

void *customer(void *arg)
{
    int cust_no = *((int *)arg);
    while (1)
    {
        pthread_mutex_lock(&access_mutex);
        if (waitChairs > 0)
        {
            waitChairs--;
            printf("Customer No.%d sit on the waiting chair.\n", cust_no);
            pthread_mutex_unlock(&access_mutex);

            pthread_mutex_lock(&barber_chair_mutex);
            pthread_mutex_unlock(&hair_cut_mutex);
            printf("Barber START cuting No.%d hair.\n", cust_no);

            sem_wait(&hairComplete);
            //printf("Customer No.%d FINISH cutting.\n", cust_no);
            sleep(randNumber(3, 6));
        }
        else
        {
            printf("Customer No.%d had no waiting chair and left.\n", cust_no);
            pthread_mutex_unlock(&access_mutex);
            sleep(randNumber(3, 6));
        }
    }
    return 0;
}

void checkArgNum(int argc)
{
    if (argc != 3)
    {
        printf("CMD$ con4 <NUM WAITING CHAIRS> <NUM CUSTOMERS>\n");
        exit(0);
    }
}

int main(int argc, char *argv[])
{
    checkArgNum(argc);

    int chairsNum = atoi(argv[1]);
    int custNum = atoi(argv[2]);

    srand(time(NULL));

    pthread_mutex_init(&barber_chair_mutex, NULL);

    pthread_mutex_init(&hair_cut_mutex, NULL);
    pthread_mutex_lock(&hair_cut_mutex);

    pthread_mutex_init(&access_mutex, NULL);
    waitChairs = chairsNum;
    sem_init(&hairComplete, 0, 0);

    pthread_t thread_barber;
    pthread_t thread_customers[custNum];

    int i, j;
    int args[custNum];
    for (i = 0; i < custNum; i++)
        args[i] = i;

    pthread_create(&thread_barber, NULL, barber, NULL);
    for (i = 0; i < custNum; i++)
    {
        //printf("%d\n", args[i]);
        pthread_create(&thread_customers[i], NULL, customer, (void *)&args[i]);
    }

    pthread_join(thread_barber, NULL);
    for (i = 0; i < custNum; i++)
        pthread_join(thread_customers[i], NULL);

    return 0;
}