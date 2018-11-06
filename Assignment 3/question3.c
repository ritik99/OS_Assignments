/***********************************************************
* You can use all the programs on  www.c-program-example.com
* for personal and learning purposes. For permissions to use the
* programs for commercial purposes,
* contact info@c-program-example.com
* To find more C programs, do visit www.c-program-example.com
* and browse!
* 
*                      Happy Coding
***********************************************************/

#include<stdio.h>
#include<semaphore.h>
#include<pthread.h>
#include<unistd.h>
#include<time.h>
 
#define N 5
#define THINKING 0
#define HUNGRY 1
#define EATING 2
#define LEFT (ph_num+4)%N
#define RIGHT (ph_num+1)%N

/*Implementation of semaphores begins here.*/
typedef struct zem_t{
    int val;
    pthread_cond_t cond;
    pthread_mutex_t lock;
} zem_t;

void zem_init(zem_t *s, int val){
    s->val = val;
    pthread_cond_init(&s->cond, NULL);
    pthread_mutex_init(&s->lock, NULL);
}

void zem_wait(zem_t *s){
    pthread_mutex_lock(&s->lock);
    while(s->val <= 0){
        pthread_cond_wait(&s->cond, &s->lock);
    }
    s->val--;
    pthread_mutex_unlock(&s->lock);
}

void zem_post(zem_t *s){
    pthread_mutex_lock(&s->lock);
    s->val++;
    pthread_cond_signal(&s->cond);
    pthread_mutex_unlock(&s->lock);
}


zem_t mutex;
zem_t S[N];

void * philospher(void *num);
void take_fork(int);
void put_fork(int);
void test(int);
 
int state[N];
int phil_num[N]={0,1,2,3,4};
 
int main()
{
    int i;
    pthread_t thread_id[N];
    zem_init(&mutex, 1);
    for(i=0;i<N;i++)
        zem_init(&S[i],0);
    for(i=0;i<N;i++)
    {
        pthread_create(&thread_id[i],NULL,philospher,&phil_num[i]);
        printf("Philosopher %d is thinking\n",i+1);
    }
    for(i=0;i<N;i++)
        pthread_join(thread_id[i],NULL);
}
 
void *philospher(void *num)
{
    while(1)
    {
        int *i = num;
        sleep(1);
        take_fork(*i);
        sleep(0);
        put_fork(*i);
    }
}
 
void take_fork(int ph_num)
{
    zem_wait(&mutex);
    state[ph_num] = HUNGRY;
    printf("Philosopher %d is Hungry\n",ph_num+1);
    test(ph_num);
    zem_post(&mutex);
    zem_wait(&S[ph_num]);
    sleep(1);
}
 
void test(int ph_num)
{
    if (state[ph_num] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING)
    {
        state[ph_num] = EATING;
        sleep(2);
        printf("Philosopher %d takes fork %d and %d\n",ph_num+1,LEFT+1,ph_num+1);
        printf("Philosopher %d is Eating\n",ph_num+1);
        zem_post(&S[ph_num]);
    }
}
 
void put_fork(int ph_num)
{
    zem_wait(&mutex);
    state[ph_num] = THINKING;
    printf("Philosopher %d putting fork %d and %d down\n",ph_num+1,LEFT+1,ph_num+1);
    printf("Philosopher %d is thinking\n",ph_num+1);
    test(LEFT);
    test(RIGHT);
    zem_post(&mutex);
}