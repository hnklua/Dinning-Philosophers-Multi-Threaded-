#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h> 
#include <sys/syscall.h> 
//#define pthread_self() syscall(SYS_gettid)
#include "random.h"
#define PHILOSOPHERS 5
// Use a struck. Malloc the structure to stay on heap 
struct threadStruct { 
	pthread_mutex_t chopsticks[PHILOSOPHERS]; // Array of threads of chopsticks 
	pthread_t philo[PHILOSOPHERS]; // Array of threads of Philosophers
	int secondsElapsed[PHILOSOPHERS]; // Total eating time
};

// Struct for thread arguments 
struct philoArgs {
	int id;
	struct threadStruct *sharedData; 
};

// Philosopher logic function 
// Argument will probably be for number of Philosophers
static void* diningPhilosophers(void* arg) {
	struct philoArgs *philos = (struct philoArgs*) arg; 
	int id = philos->id; 
	struct threadStruct *handling = philos->sharedData; 
	free(arg);  
	
	// Cast the void array to an array of threads
	int thinkingTime = randomGaussian(11, 7); 
	int eatingTime = randomGaussian(9, 3);

	while (handling->secondsElapsed[id] < 100) {
		// Have Philosophers in thinking state 
		sleep(abs(thinkingTime)); 
		printf("Philosopher %d (TID: %lu) is currently thinking. Thinking Time: %d seconds\n", id + 1, pthread_self(), thinkingTime);

		printf("Philosopher %d (TID: %lu) is attempting to pick up chopsticks.\n", id + 1, pthread_self()); 

		// Attempt to grab chopsticks using locks
		if (id % 2 == 0) {
			pthread_mutex_lock(&handling->chopsticks[id]); // Pick up the right chopsticks 
			pthread_mutex_lock(&handling->chopsticks[(id + 1) % PHILOSOPHERS]); // Pick up the left chopstick 
		} else {
			pthread_mutex_lock(&handling->chopsticks[(id + 1) % PHILOSOPHERS]); // Pick up the left chopstick
			pthread_mutex_lock(&handling->chopsticks[id]); // Pick up the right chopsticks
		}

		// Eating state 
		sleep(abs(eatingTime)); 
		printf("Philosopher %d (TID: %lu) is currently eating.\n", id + 1, pthread_self()); 
		handling->secondsElapsed[id] += eatingTime; 

		// Put down Chopsticks
		pthread_mutex_unlock(&handling->chopsticks[(id + 1) % PHILOSOPHERS]); // Put down the left chopstick
		pthread_mutex_unlock(&handling->chopsticks[id]); // Put down the right chopsticks 

		printf("Philosopher %d (TID: %lu) put down their chopsticks. Eating Time: %d seconds\n", id + 1, pthread_self(), eatingTime); 
	}
	printf("Philosopher %d (TID: %lu) is done eating and has left the table. Total Eating Time: %d\n", id + 1, pthread_self(), handling->secondsElapsed[id]); 
	return NULL; 						
}


int main(int argc, char *argv[]){
	// Initialize struct. Malloc later 
	struct threadStruct *info = malloc(sizeof(struct threadStruct)); 
	if (info == NULL) {
		write(2, strerror(errno), strlen(strerror(errno))); 
		return errno; 
	}

	// Initialize mutexs for chopsticks 
	for (int i = 0; i < PHILOSOPHERS; i++) {
		if (pthread_mutex_init(&info->chopsticks[i], NULL) != 0) {
			write(2, strerror(errno), strlen(strerror(errno))); 
			free(info); 
			return errno;
		}
	}

	for(int i = 0; i < PHILOSOPHERS; i++) {
		struct philoArgs *args = malloc(sizeof(struct philoArgs)); 
		args->id = i;
		args->sharedData = info; 

		// Create i amount of threads representing each Philosohper
		if (pthread_create(&info->philo[i], NULL, diningPhilosophers, args)) {
			write(2, strerror(errno), strlen(strerror(errno)));
			free(args); 
			free(info); 
			return errno; 
		}
	}

	// Wait for Philosophers to finish and report eating time 
	for (int i = 0; i < PHILOSOPHERS; i++) {
		pthread_join(info->philo[i], NULL); 
	}

	// Destroy mutexs
	for (int i = 0; i < PHILOSOPHERS; i++) {
		pthread_mutex_destroy(&info->chopsticks[i]); 
	}
	free(info); 
	 
	return 0;
}
