#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <sys/time.h>
#include <signal.h>

// wersja 3 

#define ROZMIAR_BUFORA 1024

using namespace std;

bool walidacja_danych(char bufor[], int odebrano)
{
    int i=0;

    if(bufor[0] == '-' || bufor[0] == '+')
    {
        return true;
    }

    if(bufor[odebrano-2] == '-' || bufor[odebrano-2] == '+')
    {
        return true;
    }

    if(bufor[0] == '\r' && bufor[1] == '\n')
    {
        return true;
    }

    
    for(; bufor[i] != '\r'; i++)
    {
        if((bufor[i] == '+' && bufor[i + 1] == '+') || (bufor[i] == '-' && bufor[i + 1] == '-') || (bufor[i] == '-' && bufor[i+1] == '+') || (bufor[i] == '+' && bufor[i+1] == '-'))
        {
            return true;
            break;
        }

        if((bufor[i] < 48 || bufor[i] > 57) && bufor[i] != '+' && bufor[i] != '-')
        {
            return true;
            break;

        }

    }



    return false;

}


void obliczanie_wyniku(int gniazdko_klienta)
{
    char bufor[ROZMIAR_BUFORA];
    int liczba = 0;
    int wynik = 0;
    int digit = 0;
    int znak = 1;
    bool blad = false;


    while(true)
    {
        int odebrano;
        odebrano = read(gniazdko_klienta, bufor, ROZMIAR_BUFORA);

        if (odebrano == -1) 
        {
			perror("Read error");
			break;
		}
		
		if (odebrano == 0)
			break;

        blad = walidacja_danych(bufor, odebrano);

        for(int i=0; i < odebrano; i++)
        {
            while(bufor[i] != '\r')
            {
                if (bufor[i] >= '0' && bufor[i] <= '9') 
                {
					digit = bufor[i] - '0';

					 // aby zachować dziesiętność liczb, mnożymy ją przez 10
					liczba = liczba * 10 + digit;
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

                i++;
            }

            char zwracany_wynik[ROZMIAR_BUFORA];
            int zwrot = 0;

            if(!blad)
            {
                wynik += liczba * znak;
                zwrot = sprintf(zwracany_wynik, "%d\r\n", wynik);

            }
            else
            {
                zwrot = sprintf(zwracany_wynik, "%s", "ERROR\r\n");
            }

            if(write(gniazdko_klienta, zwracany_wynik, zwrot) == -1)
            {
                perror("Write error");
                exit(EXIT_FAILURE);
            }

            i = i + 2;

            wynik = 0;
        }
    }

}


int main()
{
    struct sockaddr_in adres;
    struct sockaddr_in adres_klienta;
    socklen_t adres_klienta_rozmiar = sizeof(adres_klienta);
    int port = 2020;
    int gniazdko;
    int gniazdko_klienta;


    adres.sin_family = AF_INET;
    adres.sin_port = htons(port);
    adres.sin_addr.s_addr = htonl(INADDR_ANY);

    // tworzymy gniazdko
    gniazdko = socket(AF_INET, SOCK_STREAM, 0);
    if(gniazdko == -1)
    {
        perror("Socket error");
        exit(EXIT_FAILURE);
    }

    if(bind(gniazdko, (struct sockaddr*)&adres, sizeof(adres)) == -1)
    {
        perror("Bind error");
        exit(EXIT_FAILURE);
    }
     
    if(signal(SIGCHLD, SIG_IGN) == SIG_ERR)
    {
        return 1;
    }

    while(true)
    {
        cout<< "Oczekiwanie na połączenie" << endl;

        if(listen(gniazdko, 10) == -1)
        {
            perror("Listen error");
            close(gniazdko);
            exit(EXIT_FAILURE);
        }

        gniazdko_klienta = accept(gniazdko, (struct sockaddr*)&adres_klienta, &adres_klienta_rozmiar);

        if(gniazdko_klienta == -1)
        {
            perror("Accept error");
            exit(EXIT_FAILURE);
        }
        else
        {
            char buf[INET_ADDRSTRLEN];

			if (inet_ntop(AF_INET, &adres.sin_addr, buf, sizeof(buf)) == NULL) {
				perror("inet_ntop error");
				strcpy(buf, "???");
			}
			cout << "Nowe połączenie " << buf << ":" << (unsigned int) ntohs(adres.sin_port) << endl;

        }

        struct timeval timeout;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        if(setsockopt(gniazdko_klienta, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1)
        {
            perror("Setsockopt error");
            exit(EXIT_FAILURE);
        }

        pid_t pid = fork();

        if(pid == -1)
        {
            perror("Fork error");
            exit(EXIT_FAILURE);
        }
        else if(pid == 0)
        {
            // moja funkcja
            obliczanie_wyniku(gniazdko_klienta);

            if(close(gniazdko_klienta) == -1)
            {
                perror("Close error");
                exit(EXIT_FAILURE);
            }

            if(close(gniazdko) == -1)
            {
                perror("Close error");
                exit(EXIT_FAILURE);
            }

            exit(0);
        }

        if(close(gniazdko_klienta) == -1)
        {
            perror("Close error");
            exit(EXIT_FAILURE);
        }


    }

}