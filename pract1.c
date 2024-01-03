#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <alchemy/task.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <alchemy/timer.h>
#include <alchemy/sem.h>
#include <semaphore.h>
#include <math.h>
#define SAMP_TIME 5
#define MAX_HEIGHT 1.3

RT_TASK task1, task2, task3;
RTIME sec = 1e9;
FILE *result_file;
RT_SEM sem1,sem2;

double A[4][4] = {{0.9418, 0.0,0.0403, 0.0},
                  {0.0, 0.9337, 0, 0.0379},
                  {0.0, 0.0, 0.9585, 0.0},
                  {0.0,0.0,0.0,0.9607}

};

double B[4][2] = {{0.0135, 0.0006},
                  {0.0006, 0.0179},
                  {0.0, 0.0272},
                  {0.0318, 0.0}
};


double K[2][4] ={{0,0.0000,0.0000,-7.5016},
                 {0,0.0000,-7.4553,0.0000}
};

double height[4] ={0.627,0.636,0.652,0.633};
double flows[2]={-4.749,-4.861};


double height2[4]= {0.65,0.66,0.65,0.66};

void fun1(void* arg){
    rt_task_sleep(sec);
    while(1){
        rt_sem_p(&sem1, TM_INFINITE);
        k++;
        double h_new[4]={0.0,0.0,0.0,0.0};

        for(int i = 0; i < 4; i++){
            for(int j = 0; j < 4; j++){
                h_new[i]+=A[i][j]*height[j];
            }
            for(int j = 0; j < 2; j++){
                h_new[i]+=B[i][j]*flows[j];
            }
            if(h_new[i]> MAX_HEIGHT){
                perror("level to high");
                return;
            }
        }
        for(int i = 0; i < 4; i++){
            height[i] = h_new[i];
        }
        rt_sem_v(&sem2);
        rt_task_wait_period(NULL);
    }
}

void fun2(void *arg){
    rt_task_sleep(sec);
    while(1){
        rt_sem_p(&sem2, TM_INFINITE);
        double flows_new[2]={0.0,0.0};

        for(int i = 0; i < 2; i++){
            for(int j =0;j<4;j++){
                flows_new[i]+= K[i][j]*height[j];
            }

            if(flows_new[i] > 2.5){
                perror("flow to high");
                return;
            }
        }

        for(int i = 0; i < 2;i++){
            flows[i]= flows_new[i];
        }
        rt_task_wait_period(NULL);
    }
}
void fun3(void *arg){
    rt_task_sleep(sec);
    int val=0;
    while(1){
        double H[4] ={0.0,0.0,0.0,0.0};
        for(int i=0;i<4;i++){
            H[i]= height[i]+height2[i];

        }
        printf("Calculated Levels: h1=%f, h2=%f, h3=%f, h4=%f\n", height[0], height[1], height[2], height[3]);
        printf("Tank Levels: h1=%f, h2=%f, h3=%f, h4=%f\n", H[0], H[1], H[2], H[3]);
        printf("Control Inputs: Qa=%f, Qb=%f\n", flows[0], flows[1]);
        printf("\n");
        val++;
        rt_sem_v(&sem1);
        fprintf(result_file, "Calculated Levels: h1=%f, h2=%f, h3=%f, h4=%f\n", height[0], height[1], height[2], height[3]);
        fprintf(result_file, "Tank Levels: h1=%f, h2=%f, h3=%f, h4=%f\n",H[0], H[1], H[2], H[3]);
        fprintf(result_file, "Control Inputs: Qa=%f, Qb=%f\n", flows[0], flows[1]);

        fflush(result_file);
        rt_task_wait_period(NULL);
    }
}


void main(int agr, char* argv[]){
    result_file = fopen("results.txt", "a");
    if (result_file == NULL) {
        perror("Error opening file");
    }
    rt_task_create(&task1, "task1", 0,50,0);
    rt_task_create(&task2, "task2", 0,50,0);
    rt_task_create(&task3, "task3", 0,50,0);

    rt_task_set_periodic(&task1, TM_NOW, 5000000000);
    rt_task_set_periodic(&task2, TM_NOW, 5000000000);
    rt_task_set_periodic(&task3, TM_NOW, 5000000000);
    rt_sem_create(&sem1, "semaphore1", 0, S_FIFO);
    rt_sem_create(&sem2, "semaphore2", 0, S_FIFO);
    rt_task_start(&task1, &fun1,0);
    rt_task_start(&task2, &fun2, 0);
    rt_task_start(&task3, &fun3, 0);

    printf("End program by ctrl+C\n");
    pause();
    fclose(result_file);
    return;
}
