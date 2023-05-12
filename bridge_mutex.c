#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

volatile int cityA;
volatile int cityB;
volatile int cityAWaiting;
volatile int cityBWaiting;
int carAmount;
int bridge= 0;

pthread_mutex_t mutexA;

void* watek(void* numer)
{
	while(1)
	{
		if(cityA + cityAWaiting + cityB + cityBWaiting + bridge == carAmount)
		{
			cityA--;
			cityAWaiting++;
			pthread_mutex_lock(&mutexA);
			if(bridge == 0)
			{
				bridge = 1;
				cityAWaiting--;
				printf("A-%d %d >>> [>> %d >>] <<<%d %d-B\n", cityA, cityAWaiting, (int)numer, cityBWaiting, cityB);
				usleep(rand()%10000);
			}
			bridge = 0;
			cityB++;
			pthread_mutex_unlock(&mutexA);
			

			
			

			usleep(rand()%100000);

			cityB--;
			cityBWaiting++;
			pthread_mutex_lock(&mutexA);
			if(bridge == 0)
			{
				bridge = 1;
				cityBWaiting--;
				printf("A-%d %d >>> [<< %d <<] <<<%d %d-B\n", cityA, cityAWaiting, (int)numer, cityBWaiting, cityB);
				usleep(rand()%10000);
			}
			bridge = 0;
			cityA++;
			pthread_mutex_unlock(&mutexA);
			

			
			
			
			usleep(rand()%100000);
		}
		else{
		printf("Cóś się pokiepściło %d\n", (int)numer);
		break;}
		
	}
}

int main(int argc, char** argv)
{
	srand(time(0));
	if(argc == 2)
	{
		carAmount = atoi(argv[1]);
		printf("AAa %d\n", carAmount);
	}
	else
	{
		printf("Zła ilość parametrów\n");
		return 0;
	}
	cityA = carAmount;
	cityB = 0;
	cityAWaiting = 0;
	cityBWaiting = 0;
	pthread_mutex_init(&mutexA, NULL);
	pthread_t cars[carAmount];
	puts("początek programu");

	/* utworzenie wątków */
	for (int i=0; i < carAmount; ++i) 
		pthread_create(&cars[i], NULL, watek, (void*)(i+1));
		
	for (int i=0; i < carAmount; i++)
			pthread_join(cars[i], NULL);
	return 0;
}


