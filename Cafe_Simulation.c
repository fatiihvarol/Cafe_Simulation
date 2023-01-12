/*             ----RESOURCES-----

https://github.com/ali-ghanem/Sleeping-Barber

https://www.geeksforgeeks.org/sleeping-barber-problem-in-process-synchronization/

https://www.youtube.com/watch?v=c0rkh30IUeE

https://docs.oracle.com/cd/E19683-01/806-6867/6jfpgdcnj/index.html

https://www.youtube.com/watch?v=4m-I1SFT_80&t=92s

*/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>

#define CUSTOMER_NUMBER 25
#define CUSTOMER_ARRIVAL_TIME_MIN 1
#define CUSTOMER_ARRIVAL_TIME_MAX 3
#define REGISTER_NUMBER 5
#define COFFEE_TIME_MIN 2
#define COFFEE_TIME_MAX 5  //Defining global variables



int customer_in_queue = 0; //it keeps customers which  is waiting in queue
int queue_capacity = 20; //maximum capacity of queue
int space_in_queue = 0; // how many space in queue
int location_in_queue = 0; //for showing customer number

int *Queue; //our customer queue
int time_for_creating_customer; //declaring random CUSTOMER_ARRIVAL_TIME
sem_t Registers; //its declaring the semaphore for registers
sem_t Customers; //its declaring the semaphore for customers
sem_t mutex; ////its declaring the semaphore for mutexes

void Register(void *num); //its declaring the fucntion for registers
void Customer(void *num,int time); //its declaring the fucntion for customers

int main(int argc, char const *argv[])
{

	space_in_queue = queue_capacity; 
	Queue = (int *)malloc(sizeof(int) * queue_capacity); //allocates the requested memory and returns a pointer to it.
	
	pthread_t counter[REGISTER_NUMBER], customerr[CUSTOMER_NUMBER]; //its using to uniquely identify the register and customer threads
	sem_init(&Registers, 0, 0);
	sem_init(&Customers, 0, 0);
	sem_init(&mutex, 0, 1); //for initialize the threads

	printf("DEU CAFE IS OPENING\n");

	for (size_t i = 0; i < REGISTER_NUMBER; i++) //creating register threads
	{
		pthread_create(&counter[i], NULL, (void *)Register, (void *)&i);
		sleep(1);
	}
	for (size_t i = 0; i < CUSTOMER_NUMBER; i++) //creating customer threads
	{
		  time_for_creating_customer= rand()%(CUSTOMER_ARRIVAL_TIME_MAX-CUSTOMER_ARRIVAL_TIME_MIN+1)+CUSTOMER_ARRIVAL_TIME_MIN;
		sleep(1);
		pthread_create(&customerr[i], NULL, (void *)Customer, (void *)i);
		
	}
	for (size_t i = 0; i < CUSTOMER_NUMBER; i++) //waiting for a thread to terminate detaches the thread then returns the threads exit status
	{
		sleep(1);
		pthread_join(customerr[i], NULL);
		
	}
	
	printf("ALL CUSTOMER ARE SERVED CAFE IS CLOSED\n");
	return EXIT_SUCCESS;

	return 0;
}
void Register(void *num) //our register function
{
	int register_id = *(int *)num + 1; //to keep it in an array by incrementing
	int next_customer, customer_id;
	printf("%d. COUNTERS ARE READY \n", register_id);
    srand(time(0)); // for gives a random function new seed
	while (1) //main loop
	{

		sem_wait(&Registers); //decrements by one the value of the register semaphore
		sem_wait(&mutex);     //its blocking the access to all others
		customer_in_queue = (++customer_in_queue) % queue_capacity; //this process for keeps value in queue
		next_customer = customer_in_queue;
		customer_id = Queue[next_customer];
		Queue[next_customer] = pthread_self(); //its returns the id of threads

		sem_post(&mutex); //its allows to access from all others 
		sem_post(&Customers); //increments by one the value of the customer semaphore
		printf(" CUSTOMER %d GOES TO REGISTER %d. \n\n", customer_id, register_id);
		
		int waiting_time=rand()%(COFFEE_TIME_MAX-COFFEE_TIME_MIN+1)+COFFEE_TIME_MIN; //calculating the random waiting time
		sleep(waiting_time);
		printf("CUSTOMER %d FINISHED BUYING FROM REGISTER %d AFTER %d SECONDS. \n\n",customer_id,register_id,waiting_time);
	}
	pthread_exit(0); //terminates the thread functions exist
}

void Customer(void *num,int time)
{
	int n = (int)num + 1; //keep the customer id
	int number_in_queue, register_num;

	sem_wait(&mutex); //its blocking the access to all others
	
	printf(" CUSTOMER %d IS CREATED AFTER %d SECONDS. \n", n,time_for_creating_customer);

	if (space_in_queue > 0) //checking maximum customer number
	{
		space_in_queue--;
		
		location_in_queue = (++location_in_queue % queue_capacity); 
		number_in_queue = location_in_queue;
		Queue[number_in_queue] = n;       //this process for keeps value in queue

		sem_post(&mutex); //its allows to access from all others 
		sem_post(&Registers); //increments by one the value of the registers semaphore

		sem_wait(&Customers); //decrements by one the value of the customer semaphore
		sem_wait(&mutex); //its blocking the access to all others

		register_num = Queue[number_in_queue];
		space_in_queue++;

		sem_post(&mutex); //its unlocking the semaphore
	}
	
	pthread_exit(0); //terminates the thread functions exist
}

