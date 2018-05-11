/*
CS544
933295313
Webster Cheng
Refence from :
https://www.geeksforgeeks.org/operating-system-dining-philosopher-problem-using-semaphores/

gcc -o concurrency2 concurrency2.c -lpthread
*/

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

#define N 5
#define THINKING 1
#define HUNGRY 2
#define EATING 3
#define LEFT (phnum + 4) % N
#define RIGHT (phnum + 1) % N

int phiState[N]; 
char *phi[N] = {"Confucius", "Plato", "Buddha", "Aristotle", "Laozi"};

/*Only one Philosopher can take/put the fork at the time*/
sem_t mutexFork;
/*control the philosopher thread*/
sem_t S[N];

/*Check beside Philosopher is not Eatting than the phi start Eating*/
void checkBesidePhi(int phnum)
{
    if (phiState[phnum] == HUNGRY && phiState[LEFT] != EATING && phiState[RIGHT] != EATING)
    {
        phiState[phnum] = EATING; 
        sem_post(&S[phnum]);
    }
}

/*Take up the fork*/
void takeFork(int phnum)
{
    sem_wait(&mutexFork);

    phiState[phnum] = HUNGRY;
    checkBesidePhi(phnum);

    sem_post(&mutexFork);
    /*Control hungry Phi waiting other phis give them fork*/
    sem_wait(&S[phnum]);
}

/* Put down the fork*/
void putFork(int phnum)
{
    sem_wait(&mutexFork);
    phiState[phnum] = THINKING;

    /*Let beside phi start eating*/
    checkBesidePhi(LEFT);
    checkBesidePhi(RIGHT);

    sem_post(&mutexFork);
}

void *philospher(void *num)
{
    int *i = num;
    while (1)
    {
        /*1-20 seconds.*/
        printf("Philosopher %10s is thinking\n", phi[*i]);
        sleep((rand() % 20 + 1));

        takeFork(*i);

        /*2-9 seconds.*/
        printf("Philosopher %10s is Eating!! takes fork %d and %d \n"
            , phi[*i]
            , ((*i + 4) % N) + 1
            , *i + 1);
        sleep((rand() % 8 + 2));

        putFork(*i);
    }
}

int main()
{
    int i;
    pthread_t pthid[N];
    srand(time(NULL));

    sem_init(&mutexFork, 0, 1);

    for (i = 0; i < N; i++)
        sem_init(&S[i], 0, 0);

    /*Philosopher Idx */
    int phiIdx[N] = {0, 1, 2, 3, 4};
    for (i = 0; i < N; i++)
        pthread_create(&pthid[i], NULL, philospher, &phiIdx[i]);

    for (i = 0; i < N; i++)
        pthread_join(pthid[i], NULL);
}