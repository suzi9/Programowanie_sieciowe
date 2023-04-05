#include <sys/socket.h>
#include <limits.h>
#include <unistd.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define rozmiar_datagramu 65535

int main()
{
	
	struct sockaddr_in adres;
	int gniazdko; // deskryptor gniazdka
	
	adres.sin_family = AF_INET;
	adres.sin_port = htons(2020);
	adres.sin_addr.s_addr = htonl(INADDR_ANY);
	
	gniazdko = socket(AF_INET, SOCK_DGRAM, 0);
	if(gniazdko == -1) 
    {
		perror("Socket() error");
		exit(EXIT_FAILURE);
	}
	
	if(bind(gniazdko, (struct sockaddr*)&adres, sizeof(struct sockaddr_in)) == -1) 
    {
		perror("Bind() error");
		exit(EXIT_FAILURE);
	}
	
    while (1) 
    {
		
		struct sockaddr_in klient;
		socklen_t rozmiar_klient = sizeof(klient);
		
		char bufor[rozmiar_datagramu];
		bool blad = false;
        int wynik = 0;
        int liczba = 0;
        ssize_t wyslano_odp;
		int zwrot;

		
		printf("oczekiwanie na datagram\n");
		
		// oczekiwanie na datagram
		int odebrano = recvfrom(gniazdko, bufor, sizeof(bufor), 0, (struct sockaddr *)&klient, &rozmiar_klient);
		if (odebrano == -1) 
        {
			perror ("Recvfrom error");
			exit (EXIT_FAILURE);
		} 
        else if (odebrano == 0) 
        {
			// pusty datagram 
			printf("pusty datagram\n");
			blad = true;
		}
         else if (bufor[0] == '+' || bufor[0] == '-' || bufor[odebrano - 1] == '+' || bufor[odebrano - 1] == '-') 
         {
			// na poczatku albo koncu byl luzny znak + albo -
			printf("na poczatku albo koncu + albo -\n");
			blad = true;
		}
		
		// odebrano datagram
		for (int i = 0; i < odebrano; i++) 
		{
			if (blad)
				break;
			
			if ((bufor[i] < 48 || bufor[i] > 57) && bufor[i] != '+' && bufor[i] != '-') 
            {
				// niedozwolony znak inny niz cyfra lub + albo -
				printf("%d\n", bufor[i]);
				printf("niedozwolony znak na pozycji %d\n", i);
				blad = true;
				break;
			}
			
			if((bufor[i] == '+' && bufor[i + 1] == '+') || (bufor[i] == '-' && bufor[i + 1] == '-') || (bufor[i] == '-' && bufor[i+1] == '+') || (bufor[i] == '+' && bufor[i+1] == '-'))
			{
				// niedozwolona para ++, --, +-, -+ itd -> działa na więce znaków po sobie
				printf("niedozwolona para\n");
				blad = true;
				break;
			}

            // sprawdamy wystapienie znaku /r
            if(bufor[i] == 13)
            {   
                // i jesli mamy sekwencje /r/n to akceptujemy
                if(bufor[i+1] == 10)
                {
                    blad = false;
                }
                else
                {
                    // ale jeżeli mamy samo /r to zwracamy błąd
                    blad = true;
					break;
                }
		    }

			if(bufor[i] == '\0')
			{
				blad = true;
				break;
			}
		}

		if(!blad)
		{
			
			// kod obliczania wyniku
			int znak = 1;
			int digit;

			for (int i = 0; i < odebrano; i++) 
            {
				if (bufor[i] >= '0' && bufor[i] <= '9') 
                {
					// odejmujemy od kodu ASCII liczby, znak '0', którego wartość wynosi 48, dzięki temu
					// np. mając 4 mamy w kodzie ASCII wartość 52, czyli 52 - '0' -> 52 - 48 = 4
					// w ten sposób zamieniamy ją na liczbę dziesiętną
					digit = bufor[i] - '0';

					// aby zachować dziesiętność liczb, mnożymy ją przez 10
					liczba = liczba * 10 + digit;
                    
					// sprawdzamy czy liczby podane przez klienta sie przepelniaja
					if(liczba < -1)
					{
						printf(" PRZEPELNIENIE LICZBY \n");
						blad = true;
						break;
					}
				
                } 
                else if (bufor[i] == '+' || bufor[i] == '-') 
                {
					wynik += liczba * znak;
					liczba = 0;
					if (bufor[i] == '+')
						znak = 1;
					else
						znak = -1;
				}

				
			}
			wynik += liczba * znak;
			
		}

		// obsluga przepelnienia
        if(wynik > INT_MAX - liczba)
        {
            printf("PRZEPELNIENIE\n");
            blad = true;
        }
        else if(wynik < INT_MIN + liczba)
        {
            printf("PRZEPELNIENIE\n");
            blad = true;
        }
        
        char wysylane[6];
        
        if(!blad)
        {
            char zwracany_wynik[rozmiar_datagramu];
			
			// sprintf zwraca liczbę bajtów wpisanych do tablicy
			zwrot = sprintf(zwracany_wynik, "%d", wynik);

			// sendto zwraca liczbę znaków które zostały wysłane
			wyslano_odp = sendto(gniazdko, zwracany_wynik, zwrot, 0, (struct sockaddr *)&klient, rozmiar_klient);
			if (wyslano_odp == -1) 
			{
				perror ("Sendto error");
				exit (EXIT_FAILURE);
            }
        }
		else
		{
			printf("Komunikat bledu\n\n");
		
			sprintf(wysylane, "%s", "ERROR");
			wyslano_odp = sendto(gniazdko, wysylane, 5, 0, (struct sockaddr *)&klient, rozmiar_klient);
			if(wyslano_odp == -1) 
			{
				perror ("Sendto error");
				exit (EXIT_FAILURE);
			 }
		}

		printf("\nWyslano %zi bajtow\n", wyslano_odp);
	}

	if(close(gniazdko) == -1)
	{
		perror ("Close error");
		exit(EXIT_FAILURE);	
	}

	return 0;
		

}   