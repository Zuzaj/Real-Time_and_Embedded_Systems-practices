#include<semaphore.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>

sem_t x,y, queue;
pthread_t tid;
pthread_t writerthreads[3],readerthreads[5];
int readercount = 0;

void *reader(void* param)
{
    sem_wait(&queue);
    sem_wait(&x);
    readercount++;

    if(readercount==1){
        sem_wait(&y);
    }
    sem_post(&x);
    sem_post(&queue);
    printf("There is %d readers inside.\n",readercount);
    sleep(1);
    sem_wait(&x);
    readercount--;
    if(readercount==0)
    {
        sem_post(&y);
    }
    printf("Reader %d is leaving\n",readercount+1);
    sem_post(&x);
    return NULL;
}
void *writer(void* param)
{
    int arg;
    arg = *(int*) param;
    sem_wait(&queue);
    printf("Writer %d is trying to enter\n", arg);
    sem_wait(&y);
    printf("Writer %d has entered\n", arg);
    sem_post(&y);
    printf("Writer %d is leaving\n", arg);
    sem_post(&queue);
    return NULL;
}

int main()
{
    printf("\n\n");
    int i[5] = {1, 2, 3, 4, 5};
    int j[3] = {1, 2, 3};

    sem_init(&x,0,1);
    sem_init(&y,0,1);
    sem_init(&queue,0,1);

    pthread_attr_t attr1;
    pthread_attr_init(&attr1);
    pthread_attr_setschedpolicy(&attr1, SCHED_RR);

    pthread_create(&readerthreads[0],&attr1,reader,&i[0]);
    pthread_create(&writerthreads[0],&attr1,writer,&j[0]);
    pthread_create(&readerthreads[1],&attr1,reader,&i[1]);
    pthread_create(&readerthreads[2],&attr1,reader,&i[2]);
    pthread_create(&readerthreads[3],&attr1,reader,&i[3]);
    pthread_create(&writerthreads[1],&attr1,writer,&j[1]);
    pthread_create(&writerthreads[2],&attr1,writer,&j[2]);
    pthread_create(&readerthreads[4],&attr1,reader,&i[4]);

    for(int b=0;b<3;b++)
    {
        pthread_join(writerthreads[b],NULL);
    }
    for(int k=0;k<5;k++)
    {
        pthread_join(readerthreads[k],NULL);
    }
    sem_destroy(&x);
    sem_destroy(&y);
    sem_destroy(&queue);

}