#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <semaphore.h>

void* a(void* ptr) {
    for (int i=0; i< 10; i++){

        printf("1");
        printf("2");
        printf("3");
        printf("4");

    }
    return NULL;
}

void* b(void* ptr) {
    for (int i=0; i< 10; i++){
        printf("a");
        printf("b");
        printf("c");
        printf("d");

    }
    return NULL;
}

void* c(void* ptr) {
    for (int i=0; i< 10; i++){
        printf("5");
        printf("6");
        printf("7");
        printf("8");

    }
    return NULL;
}

int main() {
    printf("\n\n");
    pthread_attr_t attr1, attr2, attr3;

    pthread_attr_init(&attr1);
    pthread_attr_init(&attr2);
    pthread_attr_init(&attr3);

    pthread_t t1, t2, t3;
    // setting the policy
    struct sched_param param_a, param_b, param_c;
    param_a.sched_priority = 40;
    param_b.sched_priority = 50;
    param_c.sched_priority = 40;
    int rVal1 = pthread_attr_setschedpolicy(&attr1, SCHED_RR);
    pthread_attr_setschedparam(&attr1, &param_a);

    int rVal2 = pthread_attr_setschedpolicy(&attr2, SCHED_FIFO);
    pthread_attr_setschedparam(&attr2, &param_b);

    int rVal3 = pthread_attr_setschedpolicy(&attr3, SCHED_RR);
    pthread_attr_setschedparam(&attr3, &param_c);

    if (rVal1 != 0 || rVal2 !=0 || rVal3 !=0) {
        // Failed to set the desired scheduler policy.
        perror("pthread_attr_setschedpolicy(&attr1, SCHED_RR)");
        exit(1);
    }

    // creating tasks all with Round Robin policy
    pthread_create(&t1, &attr1, a, NULL);
    pthread_create(&t2, &attr2, b, NULL);
    pthread_create(&t3, &attr3, c, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    pthread_exit(0);

    return 0;
}
