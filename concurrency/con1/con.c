#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "mt19937ar.h"

int isRdrand = 0;
int head = 0;
int tail = 0;
int valueNum = 0; 

struct value
{
    int val1;
    int val2;
} queue[32];

pthread_mutex_t mutexVal;
pthread_cond_t condc, condp;

void queueInsValues(int val1, int val2)
{
    if (tail == 32)
        tail = 0;
    queue[tail].val1 = val1;
    queue[tail].val2 = val2;
    tail++;
    valueNum++;
}

struct value *queuePopValues()
{
    if (head == 32)
        head = 0;
    valueNum--;
    return &(queue[head++]);
}

int genmt19937(int min, int max)
{
    init_genrand(time(NULL)); 
    int x = (int)genrand_int32();
    return (abs(x) % (max - 1)) + min;
}

int rdrandNumber(int min, int max)
{
    unsigned int gen;
    unsigned char isOk;

    asm volatile("rdrand %0; setc %1"
                 : "=r"(gen), "=qm"(isOk));

    // 1 = success, 0 = underflow
    if (isOk)
        return ((gen % max) + min);
    else
        return 0;
}

void checkRdrand()
{
    unsigned int eax, ebx, ecx, edx;
    eax = 0x01;
    __asm__ __volatile__(
        "cpuid;"
        : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
        : "a"(eax));

    isRdrand = ecx & 0x40000000;
}

int randNumber(int min, int max)
{
    return isRdrand ? rdrandNumber(min, max) : genmt19937(min, max);
}

void *Consumer()
{
    while (1)
    {
        pthread_mutex_lock(&mutexVal);

        pthread_cond_signal(&condp);
        pthread_cond_wait(&condc, &mutexVal);

        if (valueNum <= 0)
        {
            printf("Consumer is waiting!!\n");
            pthread_cond_wait(&condc, &mutexVal);
        }
        struct value *x = queuePopValues();
        printf("Consumer:%d  valueNum:%d\n", x->val1, valueNum);
        printf("Consumer:%d  valueNum:%d\n", x->val2, valueNum);
        sleep(x->val2);

        pthread_mutex_unlock(&mutexVal);
    }
    pthread_exit(0);
}

void *Producer()
{
    while (1)
    {
        pthread_mutex_lock(&mutexVal);
        if (valueNum >= 32)
        {
            printf("Producer is waiting!\n");
            pthread_cond_signal(&condc);
            pthread_cond_wait(&condp, &mutexVal);
        }
        //1-9
        int val1 = randNumber(1, 10);
        sleep(1);
        int val2 = randNumber(2, 9);
        queueInsValues(val1, val2);

        printf("Producer:%d, %d\n", val1, val2);

        pthread_cond_signal(&condc);
        pthread_cond_wait(&condp, &mutexVal);

        pthread_mutex_unlock(&mutexVal);
    }
    pthread_exit(0);
}

int main(int argc, char *argv[])
{
    int threadNum, i;

    if (argc <= 1)
    {
        printf("Note: for multiple threads pairs run './concurrency1 5'\n\n");
        threadNum = 1;
    }
    else
        threadNum = atoi(argv[1]);

    checkRdrand();

    pthread_mutex_init(&mutexVal, NULL);
    pthread_cond_init(&condc, NULL); /* Initialize consumer condition variable */
    pthread_cond_init(&condp, NULL); /* Initialize producer condition variable */

    threadNum = threadNum * 2;
    pthread_t threads[threadNum];

    /*create threads*/
    for (i = 0; i < threadNum; i++)
    {
        pthread_create(&threads[i], NULL, Consumer, NULL);
        pthread_create(&threads[i++], NULL, Producer, NULL);
    }

    /*join threads*/
    for (i = 0; i < threadNum; i++)
        pthread_join(threads[i], NULL);

    /* Free up  */
    pthread_mutex_destroy(&mutexVal);
    pthread_cond_destroy(&condc);
    pthread_cond_destroy(&condp);

    return 0;
}
