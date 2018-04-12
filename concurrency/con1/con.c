#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "mt19937ar.h"

#define Buffer_Limit 10

int Buffer_Index_Value = 0;
int isRdrand = 0;
int head = 0;
int tail = 0;
int value = 0;
char *Buffer_Queue;

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
    value++;
}

struct value *queuePopValues()
{
    if (head == 32)
        head = 0;
    value--;
    return &(queue[head++]);
}

int genmt19937(int min, int max)
{
    init_genrand(time(NULL));
    // min to max
    //1 ~ 9
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
    int i = 0;
    while (i < 33)
    {
        pthread_mutex_lock(&mutexVal);
        if (value == 0)
        {
            printf("Consumer is waiting!!\n");
            pthread_cond_wait(&condc, &mutexVal);
        }
        struct value *x = queuePopValues();
        printf("Consumer:%d\n", x->val1);
        //2-9
        sleep(x->val2);
        printf("Consumer:%d\n", x->val2);

        /* wake up consumer */
        /* release the buffer */
        pthread_cond_signal(&condp);
        pthread_mutex_unlock(&mutexVal);
        i++;
    }

    pthread_exit(0);
}

void *Producer()
{
    int i = 0;
    while (i < 33)
    {
        pthread_mutex_lock(&mutexVal);
        if (value == 32)
        {
            printf("Producer is waiting!\n");
            pthread_cond_wait(&condp, &mutexVal);
        }

        //1-9
        int val1 = randNumber(1, 10);
        //3-7 (2 sec + (1~5)sec)
        sleep(randNumber(1, 6));
        //2-9
        int val2 = randNumber(2, 9);
        queueInsValues(val1, val2);

        printf("Producer:%d, %d\n", val1, val2);
        //3-7 (2 sec + (1~5)sec)
        sleep(2);

        /* wake up consumer */
        /* release the buffer */
        pthread_cond_signal(&condc);
        pthread_mutex_unlock(&mutexVal);
        i++;
    }
    pthread_exit(0);
}

int main()
{
    srand(time(NULL));
    checkRdrand();

    pthread_mutex_init(&mutexVal, NULL);
    pthread_cond_init(&condc, NULL); /* Initialize consumer condition variable */
    pthread_cond_init(&condp, NULL); /* Initialize producer condition variable */

    pthread_t producerThread, consumerThread;
    pthread_create(&producerThread, NULL, Producer, NULL);
    pthread_create(&consumerThread, NULL, Consumer, NULL);

    pthread_join(producerThread, NULL);
    pthread_join(consumerThread, NULL);

    /* Free up the_mutex */
    /* Free up consumer condition variable */
    /* Free up producer condition variable */
    pthread_mutex_destroy(&mutexVal);
    pthread_cond_destroy(&condc);
    pthread_cond_destroy(&condp);

    return 0;
}
