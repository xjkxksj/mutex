#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

//Zmienne globalne; 'volatile' oznacza, że zmienna może zostać zmodyfikowana w dowolnej chwili, więc kompilator musi zawsze odwoływać się nie do rejestru, lecz do pamięci
volatile int cityA; //Ile samochodów przebywa w mieście A i nie oczekuje na dostęp do mostu
volatile int cityB; //Ile samochodów przebywa w mieście B i nie oczekuje na dostęp do mostu
volatile int cityAWaiting; //Ile samochodów przebywa w mieście A i oczekuje na wjazd na most
volatile int cityBWaiting; //Ile samochodów przebywa w mieście B i oczekuje na wjazd na most
int bridge= 0; //Czy most jest zajęty, czy też nie
int carAmount; //Ilość samochodów jeżdżących pomiędzy miastami

//Semafor służący do unikania równoczesnego użycia wspólnych danych przez wiele wątków
pthread_mutex_t mutex;

//Wypisanie komunikatu, gdy samochód znajduje się na moście w drodze z miasta A do miasta B
void infoAB(int numer)
{
	printf("A-%d %d >>> [>> %d >>] <<< %d %d-B\n", cityA, cityAWaiting, numer, cityBWaiting, cityB);
}

//Wypisanie komunikatu, gdy samochód znajduje się na moście w drodze z miasta B do miasta A
void infoBA(int numer)
{
	printf("A-%d %d >>> [<< %d <<] <<< %d %d-B\n", cityA, cityAWaiting, numer, cityBWaiting, cityB);
}

//Funkcja wykonująca w nieskończonej pętli przejazdy samochodu z miasta A do B i z powrotem
void* carRunningBetweenCities(void* numer)
{
	while(1)
	{
		//Jeśli nie wystąpił żaden błąd (wszystkie wątki synchronizują się poprawnie)
		if(cityA + cityAWaiting + cityB + cityBWaiting + bridge == carAmount)
		{
			//Samochód oczekuje na przejazd do miasta B
			cityA--;
			cityAWaiting++;
			
			
			//Wątek próbuje zająć mutex (samochód wjeżdża na most, o ile nie jest on zajęty przez inny samochód) 
			pthread_mutex_lock(&mutex);
			
			//Jeśli most jest wolny
			if(bridge == 0)
			{
				//Most zostaje zajęty
				bridge = 1;
				//Samochód opuszcza miasto A
				cityAWaiting--;
				//Wyświetlony zostaje komunikat o ilości samochodów w każdym z miast oraz o numerze samochodu będącego obecnie na moście
				infoAB((int)numer);
				//Samochód przejeżdża przez most
				usleep(rand()%15000);
			}
			
			//Most zostaje zwolniony
			bridge = 0;
			//Samochód zjawia się w mieście B
			cityB++;
			
			//Wątek zwalnia mutex
			pthread_mutex_unlock(&mutex);
			
			//Samochód znajduje się w mieście B przez losową ilość czasu
			usleep(rand()%500000);
			
			//Samochód oczekuje na przejazd do miasta B
			cityB--;
			cityBWaiting++;
			
			//Wątek próbuje zająć mutex (samochód wjeżdża na most, o ile nie jest on zajęty przez inny samochód) 
			pthread_mutex_lock(&mutex);
			
			//Jeśli most jest wolny
			if(bridge == 0)
			{
				//Most zostaje zajęty
				bridge = 1;
				//Samochód opuszcza miasto B
				cityBWaiting--;
				//Wyświetlony zostaje komunikat o ilości samochodów w każdym z miast oraz o numerze samochodu będącego obecnie na moście
				infoBA((int)numer);
				//Samochód przejeżdża przez most
				usleep(rand()%15000);
			}
			
			//Most zostaje zwolniony
			bridge = 0;
			//Samochód zjawia się w mieście A
			cityA++;
			
			//Wątek zwalnia mutex
			pthread_mutex_unlock(&mutex);
			
			//Samochód znajduje się w mieście B przez losową ilość czasu
			usleep(rand()%500000);
		}
		//Jeśli wystąpił błąd w synchronizacji, działanie funkcji zostaje przerwane
		else
		{
			printf("Ten wątek nie zsynchronizował się: %d\n", (int)numer);
			break;
		}
		
	}
}

int main(int argc, char** argv)
{
	//Ustawienie punktu startowego generatora pseudolosowego.
	srand(time(0));
	//Jeśli liczba argumentów podczas uruchomienia programu wynosi 2, wówczas zostanie odczytana podana liczba samochodów (wątków).
	if(argc == 2)
	{
		carAmount = atoi(argv[1]);
	}
	//W przeciwnym wypadku zostanie wypisany komunikat.
	else
	{
		printf("Po wpisaniu nazwy programu należy podać jedynie ilość samochodów (wątków)\n");
		return 0;
	}
	//Inicjalizowanie zmiennych globalnych; wszystkie samochody (wątki) rozpoczynają w mieście A.
	cityA = carAmount;
	cityB = 0;
	cityAWaiting = 0;
	cityBWaiting = 0;
	pthread_mutex_init(&mutex, NULL);
	
	//Deklaracja listy wątków (samochodów).
	pthread_t cars[carAmount];
	
	//Rozpoczęcie jazdy samochodów (wątków).
	for (int i=0; i < carAmount; ++i) 
		pthread_create(&cars[i], NULL, carRunningBetweenCities, (void*)(i+1));
		
	//Oczekiwanie na zakończenie pracy wątków (samochodów).
	for (int i=0; i < carAmount; i++)
			pthread_join(cars[i], NULL);
	return 0;
}
