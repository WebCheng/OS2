#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>

#define Buffer_Limit 10

int Buffer_Index_Value = 0;
char *Buffer_Queue;

int queue[32] = {};
int head = 0;
int tail = 0;
int value = 0;

pthread_mutex_t mutex_variable = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t Buffer_Queue_Not_Full = PTHREAD_COND_INITIALIZER;
pthread_cond_t Buffer_Queue_Not_Empty = PTHREAD_COND_INITIALIZER;

void queue_insert_value(int i)
{
    if(tail == 32)
        tail = 0;
    queue[tail] = i;
    tail++;
    value++;
}

int queue_pop_value()
{
    if (head == 32)
        head = 0;
    value--;
    return queue[head++];
}

void *Consumer()
{
    int i = 0;
    while(i<3)
    {
        /*
        pthread_mutex_lock(&mutex_variable);
        if(Buffer_Index_Value == -1)
        {            
            pthread_cond_wait(&Buffer_Queue_Not_Empty, &mutex_variable);
        }                
        printf("Consumer:%d\t", Buffer_Index_Value--);
        pthread_mutex_unlock(&mutex_variable);
        pthread_cond_signal(&Buffer_Queue_Not_Full);
        */
        pthread_mutex_lock(&mutex_variable);
        if(value <= 0)
            pthread_cond_wait(&Buffer_Queue_Not_Empty, &mutex_variable);

        printf("Consumer:%d\n", queue_pop_value());
        pthread_mutex_unlock(&mutex_variable);
        pthread_cond_signal(&Buffer_Queue_Not_Full);
        i++;
    }    
}

void *Producer()
{    
    int i =0;
    while(i<3)
    { 
        /*
        pthread_mutex_lock(&mutex_variable);
        if(Buffer_Index_Value == Buffer_Limit)                         
            pthread_cond_wait(&Buffer_Queue_Not_Full, &mutex_variable);
                                
        Buffer_Queue[Buffer_Index_Value++] = '@';
        printf("Producer:%d\t", Buffer_Index_Value);
        pthread_mutex_unlock(&mutex_variable);
        pthread_cond_signal(&Buffer_Queue_Not_Empty);        
        */
        pthread_mutex_lock(&mutex_variable);
        if(value == 32)
            pthread_cond_wait(&Buffer_Queue_Not_Full, &mutex_variable);

        queue_insert_value(i);
        printf("Producer:%d\n", i);
        pthread_mutex_unlock(&mutex_variable);
        pthread_cond_signal(&Buffer_Queue_Not_Empty);
        i++;
    }    
}

int main()
{  
    /*
    queue_insert_value(10);
    queue_pop_value();
    */
    pthread_t producer_thread_id, consumer_thread_id;
    Buffer_Queue = (char *) malloc(sizeof(char) * Buffer_Limit);            
    pthread_create(&producer_thread_id, NULL, Producer, NULL);
    pthread_create(&consumer_thread_id, NULL, Consumer, NULL);
    pthread_join(producer_thread_id, NULL);
    pthread_join(consumer_thread_id, NULL);
    return 0;
}
