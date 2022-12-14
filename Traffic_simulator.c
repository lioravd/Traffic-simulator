#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#define N 5
#define FIN_PROB 0.4
#define MIN_INTER_ARRIVAL_IN_NS 8000000
#define MAX_INTER_ARRIVAL_IN_NS 9000000
#define INTER_MOVES_IN_NS 100000
#define SIM_TIME 2
#define MAX_NUM_OF_CARS  (int)(((SIM_TIME*1000000000)/(MIN_INTER_ARRIVAL_IN_NS))+50) //10% saftey
void* Drive(void *arg);
void* Print(void *arg);
void* Generate(void *arg);
int Die();
pthread_mutexattr_t attr;
pthread_t generator[4];
int generator_flag[4] ={0};
pthread_t print_t;
pthread_t cars[4][(int)MAX_NUM_OF_CARS];
pthread_mutex_t cell[(4*N)-4];
char print_arr[(4*N)-4]={" "} ;
int arg[4];
struct timespec start_time;



int main() {
    return 0;
}