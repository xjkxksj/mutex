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
int bridge; //Czy most jest zajęty, czy też nie
int carAmount; //Ilość samochodów jeżdżących pomiędzy miastami

//Semafor służący do unikania równoczesnego użycia wspólnych danych przez wiele wątków
pthread_mutex_t mutex;
//Zmienna warunkowa, pozwalająca na przesłanie sygnału o zajściu warunku z jednego wątku do innych, które na ten warunek oczekują
pthread_cond_t cond;

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

//Funkcja umożliwiająca podróż samochodu z miasta A do miasta B
int carFromAtoB(int numer)
{
	//Jeśli nie wystąpił żaden błąd (wszystkie wątki synchronizują się poprawnie)
	if(cityA + cityAWaiting + cityB + cityBWaiting + bridge == carAmount)
	{

		
		//Wątek próbuje zająć mutex
		pthread_mutex_lock(&mutex);
		//Samochód oczekuje na przejazd do miasta B
		if(cityA > 0)
		{
			cityA--;
			cityAWaiting++;
		}
		//Jeśli most jest zajęty
		if(bridge == 1)
		{
			//Wątek oddaje dostęp do mutexa, oczekując na sygnał o zwolnieniu mostu
			pthread_cond_wait(&cond, &mutex);
		}
		//Jeśli choć jeden samochód oczekuje na wjazd na most
		else if(cityAWaiting > 0)
		{
			//Most zostaje zajęty
			bridge = 1;
			//Samochód opuszcza miasto A
			cityAWaiting--;
			//Wyświetlony zostaje komunikat o ilości samochodów w każdym z miast oraz o numerze samochodu będącego obecnie na moście
			infoAB(numer);
		}
		//Wątek zwalnia mutex
		pthread_mutex_unlock(&mutex);
		
		//Samochód przejeżdża przez most
		usleep(rand()%10000);
		
		//Wątek ponownie próbuje zająć mutex
		pthread_mutex_lock(&mutex);
		
		//Jeśli most jest zajęty
		if(bridge == 1)
		{
			//Samochód zjawia się w mieście B
			++cityB;
			//Most zostaje zwolniony
			bridge = 0;
		}
		
		//Jeśli most jest wolny i w mieście A lub B jest samochód oczekujący na przejazd
		if(bridge == 0 && (cityAWaiting > 0 || cityBWaiting > 0))
		{
			//Wyślij sygnał do jednego z oczekujących wątków 
			pthread_cond_signal(&cond);
		}

		//Wątek zwalnia mutex
		pthread_mutex_unlock(&mutex);
	}
	//Jeśli wystąpił błąd w synchronizacji, działanie funkcji zostaje przerwane poprzez zwrócenie kodu błędu
	else
	{
		printf("Ten wątek nie zsynchronizował się: %d\n", numer);
		return 13;
	}
	//Funkcja zwraca 0, jeśli wszystko przebiegło poprawnie
	return 0;
}

//Funkcja umożliwiająca podróż samochodu z miasta B do miasta A
int carFromBtoA(int numer)
{
	if(cityA + cityAWaiting + cityB + cityBWaiting + bridge == carAmount)
	{
		
		
		//Wątek próbuje zająć mutex
		pthread_mutex_lock(&mutex);
		
		//Samochód oczekuje na przejazd do miasta A
		if(cityB > 0)
		{
			cityB--;
			cityBWaiting++;
		}
		//Jeśli most jest zajęty
		if(bridge == 1)
		{
			//Wątek oddaje dostęp do mutexa, oczekując na sygnał o zwolnieniu mostu
			pthread_cond_wait(&cond, &mutex);
		}
		//Jeśli choć jeden samochód oczekuje na wjazd na most
		else if(cityBWaiting > 0)
		{
			//Most zostaje zajęty
			bridge = 1;
			//Samochód opuszcza miasto B
			cityBWaiting--;
			//Wyświetlony zostaje komunikat o ilości samochodów w każdym z miast oraz o numerze samochodu będącego obecnie na moście
			infoBA(numer);
		}
		//Wątek zwalnia mutex
		pthread_mutex_unlock(&mutex);
		
		//Samochód przejeżdża przez most
		usleep(rand()%10000);
		
		//Wątek ponownie próbuje zająć mutex
		pthread_mutex_lock(&mutex);
		
		//Jeśli most jest zajęty
		if(bridge == 1)
		{
			//Samochód zjawia się w mieście A
			++cityA;
			//Most zostaje zwolniony
			bridge = 0;
		}

		//Jeśli most jest wolny i w mieście A lub B jest samochód oczekujący na przejazd
		if(bridge == 0 && (cityAWaiting > 0 || cityBWaiting > 0))
		{
			//Wyślij sygnał do jednego z oczekujących wątków 
			pthread_cond_signal(&cond);
		}

		//Wątek zwalnia mutex
		pthread_mutex_unlock(&mutex);
	}
	//Jeśli wystąpił błąd w synchronizacji, działanie funkcji zostaje przerwane poprzez zwrócenie kodu błędu
	else
	{
		printf("Ten wątek nie zsynchronizował się: %d\n", numer);
		return 11;
	}
	//Funkcja zwraca 0, jeśli wszystko przebiegło poprawnie
	return 0;
}

//Funkcja wykonująca w nieskończonej pętli przejazdy samochodu z miasta A do B i z powrotem
void* carBetweenCities(void* numer)
{
	//Zmienne kontrolne; jeśli któraś z nich przyjmie wartość dodatnią, będzie to oznaczało, że któryś wątek nie zsynchronizował się poprawnie, i funkcja się zakończy.
	int retAtoB = 0;
	int retBtoA = 0;
	
	while(retAtoB == 0 && retBtoA == 0)
	{	

			retAtoB = carFromAtoB((int)numer);
			//Odczekanie chwili o losowej długości; samochód znajduje się w mieście A.
			usleep(rand()%500000);
			retBtoA = carFromBtoA((int)numer);
			//Odczekanie chwili o losowej długości; samochód znajduje się w mieście B.
			usleep(rand()%500000);
	}
	return NULL;
}

int main(int argc, char** argv)
{
	//Ustawienie punktu startowego generatora pseudolosowego.
	srand(time(0));
	//Jeśli liczba argumentów podczas uruchomienia programu wynosi 2, wówczas zostanie odczytany podana liczba samochodów (wątków).
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
	cityA = carAmount/2;
	cityB = carAmount - cityA;
	cityAWaiting = 0;
	cityBWaiting = 0;
	bridge = 0;
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);
	
	//Deklaracja listy wątków (samochodów).
	pthread_t cars[carAmount];
	
	//Rozpoczęcie jazdy samochodów (wątków).
	for (int i=0; i < carAmount; ++i) 
		pthread_create(&cars[i], NULL, carBetweenCities, (void*)(i+1));
	
	//Oczekiwanie na zakończenie pracy wątków (samochodów).
	for (int i=0; i < carAmount; i++)
			pthread_join(cars[i], NULL);
	return 0;
}
