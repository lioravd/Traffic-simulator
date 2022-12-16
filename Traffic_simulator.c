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
#define MAX_CARS  0xFFFFFFFF


//TODO: ADD A SEED
/********************************************************
 *                                                      *
 *                  car struct                          *
 *                                                      *
 * ******************************************************/

// Struct to represent a car with a mutex, position and a thread
typedef struct {
    pthread_mutex_t mutex;
    int pos;
    pthread_t thread_id;
} car_t;

// 2D array to represent the traffic circle
car_t traffic_circle[4*N - 4];

// Function to generate a new car at one of the generators

/********************************************************
 *                                                      *
 *                  generate car                        *
 *                                                      *
 * ******************************************************/
void* generate_car(void* arg) {

    // Select a generator randomly between the positions: [0,N-1, 2*(N-1), 3*(N-1), 4*(N-1)]
    int generator_pos = (rand() % 4)*(N-1);

    // Select a random time to wait before generating a new car
    int inter_arrival_time = rand() % (MAX_INTER_ARRIVAL_IN_NS - MIN_INTER_ARRIVAL_IN_NS + 1) + MIN_INTER_ARRIVAL_IN_NS;

    //TODO: check if usleep is busy waiting
    usleep(inter_arrival_time);

    // Create a new car struct
    car_t car;
    pthread_mutex_init(&car.mutex, NULL);
    car.pos = generator;

    // Create a new thread for the car
    pthread_create(&car.thread_id, NULL, move_car, &car);
}

/********************************************************
 *                                                      *
 *                  move car function                   *
 *                                                      *
 * ******************************************************/

// Function to simulate the movement of a car around the traffic circle
void* move_car(void* arg) {
    car_t* car = (car_t*)arg;
    int x = car->pos ;

    // Continue moving until the car reaches a sink
    while (x % (N-1)) {
        // Try to acquire the mutex of the next square

        //TODO: CHANGE traffic_circle to a 1D array: traffic_circle[4*(N-1)]
        int res = pthread_mutex_trylock(&traffic_circle[x+1].mutex);
        if (res == 0) {
            // Mutex was acquired successfully, move the car to the next square
            traffic_circle[(x + 1) % N] = *car;

            //leave a blank spot behind
            traffic_circle[x] = (car_t){0};

            // now the spot behind is free to use
            pthread_mutex_unlock(&traffic_circle[x].mutex);
            x++;
        }
        usleep(INTER_MOVES_IN_NS);
    }

    /** if we reach a sink*/
    // Check if the car should disappear with probability FIN_PROB
    if (rand() / (double)RAND_MAX < FIN_PROB) {

        // Car should disappear, exit the thread
        pthread_exit(NULL);

    } else {
        // Car should continue moving, reset its position and start a new thread
        //TODO: COMPLETE THIS PART, it
        // shuld use the pthread_mutex_trylock
        // function to try to acquire a mutex to the next.🎇🎇🔥🚀
    }
}

/********************************************************
 *                                                      *
 *                  Printer  function                   *
 *                                                      *
 * ******************************************************/

void* printer(void* arg) {
    //TODO: complete printer function
}


int main(){
    //TODO complete the main function

    //initialize the traffic circle

    //create the printer thread four car generator threads

    //waits for the simulation to finish using pthread_join() function

    //destroy the mutexes and exit the program.

    return 0
}

