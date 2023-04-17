#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <sys/time.h>
#include <signal.h>
#include <climits>

#define ROZMIAR_BUFORA 1024

using namespace std;

void obliczanie_wyniku(int gniazdko_klienta)
{
    char bufor[ROZMIAR_BUFORA];
    int liczba = 0;
    int wynik = 0;
    int digit = 0;
    int znak = 1;
    bool blad = false;
    int odebrano=0;
    int zwrot = 0;
    char zwracany_wynik[ROZMIAR_BUFORA];
    string ostateczny_wynik;


    while(true)
    {
        
        odebrano = read(gniazdko_klienta, bufor, ROZMIAR_BUFORA);

        if (odebrano == -1) 
        {
			perror("Read error");
			break;
		}
		
		if (odebrano == 0)
			break;
        
        // walidacja danych
        if(bufor[0] == '-' || bufor[0] == '+')
        {
            blad = true;
        }

        if(bufor[odebrano-2] == '-' || bufor[odebrano-2] == '+')
        {
            blad = true;
        }

        if(bufor[0] == '\r' && bufor[1] == '\n')
        {
            blad = true;
        }
        

        for(int i=0; i < odebrano; i++)
        {
            // walidacja danych
            if((bufor[i] == '+' && bufor[i + 1] == '+') || (bufor[i] == '-' && bufor[i + 1] == '-') || (bufor[i] == '-' && bufor[i+1] == '+') || (bufor[i] == '+' && bufor[i+1] == '-'))
            {
                blad = true;
            }
             
            
            if((bufor[i] < 48 || bufor[i] > 57) && bufor[i] != '+' && bufor[i] != '-' && bufor[i] != '\r' && bufor[i] != '\n')
            {
                blad = true;
            }

           // jeśli dane są poprawne to zostaje obliczony wynik
           if(!blad)
           { 
               
                if (bufor[i] >= '0' && bufor[i] <= '9') 
                {
					digit = bufor[i] - '0';

					// aby zachować dziesiętność liczb, mnożymy ją przez 10
					liczba = liczba * 10 + digit;

                    // sprawdzamy czy liczby podane przez klienta sie przepelniaja
					if(liczba < -1)
					{
						printf(" PRZEPELNIENIE LICZBY \n");
						blad = true;
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
           
           // jeżeli napotkaliśmy na terminator \r\n to wpisujemy nasz wynik lub błąd do zmiennej ostateczny_wynik
           if(bufor[i] == '\r' && bufor[i+1] == '\n')
           {
                if(!blad)
                {
                    wynik += liczba * znak;
                    cout<< "WYNIK 2 : "<<wynik<< endl;
                    zwrot = sprintf(zwracany_wynik, "%d", wynik);
                    ostateczny_wynik += string(zwracany_wynik)+"\r\n";

                }
                else
                {
                    ostateczny_wynik += "ERROR\r\n";
                    blad = false; 
                }

                liczba = 0;
                digit = 0;
                znak = 1;
                wynik = 0;

           }

        }  
            // odsyłamy odpowiedź    
            if(write(gniazdko_klienta, ostateczny_wynik.c_str(), ostateczny_wynik.length()) == -1)
            {
                perror("Write error");
                exit(EXIT_FAILURE);
            }

            ostateczny_wynik.clear();
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
    
    // SIGCHLD oznacza zakończenie procesu potomnego
    // SIG_IN oznacza ignorowanie sygnału
    // w funkcji signal pierwszy argument to numer sygnału który ma zostać obsłużony
    // a drugi argument to wskaźnik do funkcji która ma być wywowałana w chwili przybycia sygnału
    if(signal(SIGCHLD, SIG_IGN) == SIG_ERR) // dajemy znać jądru że nie będziemy prosić o statusy dzieci i od razu po zakończeniu działania można usunąć ich wpisy w tablicy procesów
    {
        // zwracanie 1 oznacza że wystąpił błąd
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

        // funkcja accept zwraca nam utworzone gniazdko dla klienta
        gniazdko_klienta = accept(gniazdko, (struct sockaddr*)&adres_klienta, &adres_klienta_rozmiar);

        if(gniazdko_klienta == -1)
        {
            perror("Accept error");
            exit(EXIT_FAILURE);
        }
        else
        {
            // INET_ADDRSTRLEN najwiekszy możliwy rozmiar string adresu IPv4 -> więc rezerwuje w pamięci 16 bajtów
            char buf[INET_ADDRSTRLEN];

			// komunikat diagnostyczny ------------------------------------------------

            // funkcja inet_ntop konwertuje internetowy adres binarny na tekstowy
            // i umieszczka go w tablicy znaków 'buf'
            if (inet_ntop(AF_INET, &adres.sin_addr, buf, sizeof(buf)) == NULL) {
				perror("inet_ntop error");
				strcpy(buf, "???");
			}

			cout << "Nowe połączenie " << buf << ":" << (unsigned int) ntohs(adres.sin_port) << endl;

        }

        // ustawiam timeout na połączenie serwera z klientem
        struct timeval timeout;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        // jako pierwszy arguemnt przyjmujemy gniazdko którego ten timeout ma dotyczyć
        if(setsockopt(gniazdko_klienta, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1)
        {
            perror("Setsockopt error");
            exit(EXIT_FAILURE);
        }

        // tworzymy proces potomny który jest kopią procesu macierzystego
        pid_t pid = fork();

        if(pid == -1)
        {
            perror("Fork error");
            exit(EXIT_FAILURE);
        }
        else if(pid == 0)
        {
            // akcja dla procesu potomnego
            obliczanie_wyniku(gniazdko_klienta);

            // zamykamy kopie gniazdka klienta które znajduję się w procesie potomnym
            if(close(gniazdko_klienta) == -1)
            {
                perror("Close error");
                exit(EXIT_FAILURE);
            }

            // zamykamy kopie gniazdka połączeniowego które znajduję się w procesie potomnym
            if(close(gniazdko) == -1)
            {
                perror("Close error");
                exit(EXIT_FAILURE);
            }

            // kończymy proces potomny
            exit(0);
        }

        // zamykamy gniazdko klienta w procesie macierzystym
        if(close(gniazdko_klienta) == -1)
        {
            perror("Close error");
            exit(EXIT_FAILURE);
        }

    }

}