#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <semaphore.h>

sem_t sem_a, sem_b, sem_c;
int count;
void* a(void* ptr) {
    for (int i=0; i< 10; i++){
        sem_wait(&sem_a);
        printf("1");
        printf("2");
        printf("3");
        printf("4");
        count++;
        sem_post(&sem_b);
    }
    return NULL;
}
void* b(void* ptr) {
    for (int i=0; i< 10; i++){
        sem_wait(&sem_b);
        printf("a");
        printf("b");
        printf("c");
        printf("d");
        count--;
        sem_post(&sem_c);
    }
    return NULL;
}
void* c(void* ptr) {
    for (int i=0; i< 10; i++){
        sem_wait(&sem_c);
        if(count ==1){
            printf("5");
            printf("6");
            printf("7");
            printf("8");
            sem_post(&sem_a);
        }
        else{
            sem_post(&sem_a);
        }
    }
    return NULL;
}
int main() {
    printf("\n\n");
    //initializing attributes
    pthread_attr_t attr1, attr2, attr3;
    pthread_attr_init(&attr1);
    pthread_attr_init(&attr2);
    pthread_attr_init(&attr3);

    //initializing semaphores
    sem_init(&sem_a,0,1);
    sem_init(&sem_b,0,0);
    sem_init(&sem_c,0,0);

    pthread_t t1, t2, t3;
    // setting the policy
    int rVal1 = pthread_attr_setschedpolicy(&attr1, SCHED_RR);

    int rVal2 = pthread_attr_setschedpolicy(&attr2, SCHED_RR);

    int rVal3 = pthread_attr_setschedpolicy(&attr3, SCHED_RR);

    if (rVal1 != 0 || rVal2 !=0 || rVal3 !=0) {
        // Failed to set the desired scheduler policy.
        perror("pthread_attr_setschedpolicy(&attr, SCHED_RR)");
        exit(1);
    }

// creating tasks
    pthread_create(&t1, &attr1, a, NULL);
    pthread_create(&t2, &attr2, b, NULL);
    pthread_create(&t3, &attr3, c, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    pthread_exit(0);
    sem_destroy(&sem_a);
    sem_destroy(&sem_b);
    sem_destroy(&sem_c);
    return 0;
}