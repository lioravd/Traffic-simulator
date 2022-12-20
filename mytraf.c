#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#define N 5
#define FIN_PROB 0.1
#define MIN_INTER_ARRIVAL_IN_NS 8000000
#define MAX_INTER_ARRIVAL_IN_NS 9000000
#define INTER_MOVES_IN_NS 100000
#define SIM_TIME 2
#define MAX_CARS  0xFFFFFF
int sim;
void* move_car(void* arg);

//TODO: ADD A SEED
/********************************************************
 *                                                      *
 *                  car struct                          *
 *                                                      *
 * ******************************************************/

// Struct to represent a car with a mutex, position and a thread
typedef struct {
    int pos;
    pthread_t thread_id;
} car_t;

// 2D array to represent the traffic circle
// from now, traffic_circle is a char arr
char traffic_circle[4*N - 4];
car_t cars[MAX_CARS];
pthread_mutex_t mutex[4*N - 4];
pthread_t generators[4];
pthread_t Printer;

// Function to generate a new car at one of the generators

/********************************************************
 *                                                      *
 *                  generate car                        *
 *                                                      *
 * ******************************************************/
void* generate_car(void* arg) {
    int I = *(int*)arg;
    int car_ID = I;

    // Select a generator randomly between the positions: [0,N-1, 2*(N-1), 3*(N-1)]
    int generator_pos = I*(N-1) % (4*N-4);
    while(sim){
        // Select a random time to wait before generating a new car
        int inter_arrival_time =
                rand() % (MAX_INTER_ARRIVAL_IN_NS - MIN_INTER_ARRIVAL_IN_NS) + MIN_INTER_ARRIVAL_IN_NS;

        usleep((useconds_t)(inter_arrival_time/(double)1000));

        // Create a new car struct
        //car_t *car = &cars[++car_ID];

        /**    wait 'till the square is free   */
        // Try to acquire the mutex of the next square 🐞🐞🐞🐞

        //🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞
        //HERE IS THE BUG!! the generator thread is locking it's
        //location, no one else can unlock it!! so when the car tries
        //to unlock it fails, thats why we're in a deadlock
        //🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞🐞
        int res = pthread_mutex_trylock(&mutex[generator_pos]);
        if (res == 0) {
            // Mutex was acquired successfully, move the car to the next square
            traffic_circle[generator_pos] = '*';

            // set the position of the new car
            cars[car_ID].pos = generator_pos;

            // Create a new thread for the car

            pthread_mutex_unlock(&mutex[generator_pos]);
            pthread_create(&cars[car_ID].thread_id, NULL, move_car, (void *) &car_ID);

            car_ID +=4;

        }

        if(car_ID>0xFFFFFF){
            break;
        }
    }
    pthread_exit(NULL);
    return NULL;
}

/********************************************************
 *                                                      *
 *                  move car function                   *
 *                                                      *
 * ******************************************************/

// Function to simulate the movement of a car around the traffic circle
void* move_car(void* arg) {
    int id = *(int*)arg;

    pthread_mutex_lock(&mutex[cars[id].pos]);
    int just_created = 1;
    int alive = 1;
    while(alive&&sim){
        int x = cars[id].pos;
        // Continue moving until the car reaches a sink
        while (x % (N-1) || just_created ) {
            /**     wait 'till the square is free   */
            // Try to acquire the mutex of the next square
            int res = pthread_mutex_trylock(&mutex[(x+1)%(4*(N-1))]);
            if (res == 0) {
                just_created = 0;

                // Mutex ws acquired successfully,
                //leave a blank spot behind
                traffic_circle[x] = ' ';

                // now the spot behind is free to use
                pthread_mutex_unlock(&mutex[x]);

                //move the car to the next square
                traffic_circle[(x + 1) % (4*(N-1))] = '*';
                cars[id].pos = (x + 1) % (4*(N-1));

                x = cars[id].pos;

            }
            // sleep until next step
            usleep((useconds_t)(INTER_MOVES_IN_NS/(double)1000));
        }

        /** if we reach a sink*/
        // Check if the car should disappear with probability FIN_PROB

        if (rand()%100 <= 100*FIN_PROB) {

            // Car should disappear, exit the thread
            traffic_circle[x] = ' ';
            pthread_mutex_unlock(&mutex[x]);
            pthread_exit(NULL);
            alive = 0;

        } else{
            /**     wait 'till the square is free   */
            // Try to acquire the mutex of the next square
            int res = pthread_mutex_lock(&mutex[(x+1)%(4*(N-1))]);
            if (res == 0) {
                just_created = 0;

                // Mutex was acquired successfully, move the car to the next square
                //leave a blank spot behind
                traffic_circle[x] = ' ';
                // now the spot behind is free to use
                pthread_mutex_unlock(&mutex[x]);

                traffic_circle[(x + 1) % (4*(N-1))] = '*';

                cars[id].pos = (x + 1) % (4*(N-1));




            }
            // sleep until next step
            usleep((useconds_t)(INTER_MOVES_IN_NS/(double)1000));
        }

    }
    pthread_exit(NULL);
    return NULL;
}


/********************************************************
 *                                                      *
 *                  Printer  function                   *
 *                                                      *
 * ******************************************************/

void* printer(void* arg) {
    char roadmap[N][N];


    // 10 prints
    for(int t = 0; t<10; t++){


        // wait
        usleep((useconds_t)(SIM_TIME*1000000/(double)10));

        //traffic_circle looks like [**   *  * *] of len: 4(N-1)

        /******         CREATE A PRINTABLE ROAD MAP         *******/

        // everything is blank

        /*
        for(int i =0; i<N; i++)
            for(int j =0; j<N; j++)
                roadmap[i][j] = ' ';
*/
        // everything inside is @
        for(int i =1; i<N-1; i++)
            for(int j =1; j<N-1; j++)
                roadmap[i][j] = '@';


        // everything inside is @
        for(int i = 0; i < N ; i++){
            roadmap[0][N-1-i] = traffic_circle[i];
        }
        for(int i = N; i< 2*N - 1; i++){
            roadmap[i + 1 - N][0] = traffic_circle[i];
        }
        for(int i = 0; i< N; i++){
            roadmap[ N - 1 ][i] = traffic_circle[2*N-2+i];
        }

        for(int i = 0; i< N - 2; i++){
            roadmap[N - 2 - i ][N-1] = traffic_circle[3*N-3+i];
        }

        for(int i =0; i<N; i++){
            for(int j =0; j<N; j++) {
                printf("%c", roadmap[i][j]);
            }
            printf("\n");

        }
    }
    pthread_exit(NULL);
}


int main(){
    sim=1;
    //initialize the traffic circle
    for (int i = 0; i< 4*N-4; i++ ){
        traffic_circle[i] = ' ';
        pthread_mutex_init(&mutex[i], NULL);
    }
    //initialize the generator threads
    for (int i = 0; i< 4; i++ ){
        pthread_create(&generators[i],NULL,generate_car,(void*)&i);
    }
        //create the printer thread four car generator threads
    pthread_create(&Printer,NULL, printer,NULL);
    
    

    sleep(SIM_TIME); 
    sim=0;
    //waits for the simulation to finish using pthread_join() function
    pthread_join(Printer,NULL);
    for(int i =0; i<4;i++){
        pthread_join(generators[i],NULL);
    }

    //destroy the mutexes and exit the program.
    for(int i =0; i<4*N-4;i++){
        pthread_mutex_destroy(&mutex[i]);
    }
    
  
    return 0;
}
