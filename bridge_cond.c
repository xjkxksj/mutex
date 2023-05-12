#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

volatile int cityA;
volatile int cityB;
volatile int cityAWaiting;
volatile int cityBWaiting;
volatile int bridge;
int carAmount;


pthread_mutex_t mutex;
pthread_cond_t cond;

void infoAB(int numer)
{
	printf("A-%d %d >>> [>> %d >>] <<< %d %d-B\n", cityA, cityAWaiting, numer, cityBWaiting, cityB);
}

void infoBA(int numer)
{
	printf("A-%d %d >>> [<< %d <<] <<< %d %d-B\n", cityA, cityAWaiting, numer, cityBWaiting, cityB);
}

int watekAB(int numer)
{
	if(cityA + cityAWaiting + cityB + cityBWaiting + bridge == carAmount)
	{
		cityA--;
		cityAWaiting++;
		pthread_mutex_lock(&mutex);

		if(bridge == 1)
		{
			pthread_cond_wait(&cond, &mutex);
		}

		bridge = 1;
		cityAWaiting--;
		infoAB(numer);
		
		usleep(rand()%10000);
		
		bridge = 0;
		cityB++;
		
		if(bridge == 0)
			pthread_cond_signal(&cond);

		pthread_mutex_unlock(&mutex);
	}
	else
	{
		printf("Cóś się pokiepściło %d\n", (int)numer);
		return 13;
	}
	return 0;
}

int watekBA(int numer)
{
	if(cityA + cityAWaiting + cityB + cityBWaiting + bridge == carAmount)
	{
		cityB--;
		cityBWaiting++;
		pthread_mutex_lock(&mutex);
		if(bridge == 1)
		{
			pthread_cond_wait(&cond, &mutex);
		}

		bridge = 1;
		cityBWaiting--;
		infoBA(numer);
		
		usleep(rand()%10000);
		
		bridge = 0;
		cityA++;
		
		if(bridge == 0 && cityAWaiting > 0)
			pthread_cond_signal(&cond);

		pthread_mutex_unlock(&mutex);
	}
	else
	{
		printf("Cóś się pokiepściło %d\n", (int)numer);
		return 11;
	}
	return 0;
}

void* watex(void* numer)
{
	int x = 0;
	int y = 0;
	while(x == 0 && y == 0)
	{
		x = watekAB((int)numer);
		usleep(rand()%100000);
		y = watekBA((int)numer);
		usleep(rand()%100000);
	}
}

int main(int argc, char** argv)
{
	srand(time(0));
	if(argc == 2)
	{
		carAmount = atoi(argv[1]);
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
	bridge = 0;
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);
	pthread_t cars[carAmount];
	puts("początek programu");

	for (int i=0; i < carAmount; ++i) 
		pthread_create(&cars[i], NULL, watex, (void*)(i+1));
	
		
	for (int i=0; i < carAmount; i++)
			pthread_join(cars[i], NULL);
	return 0;
}


