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

// odrwacanie łańcucha znaków
string odwroc_string(string& str)
{
    // pobieramy długość łańcucha
    int n = str.length();

    for (int i = 0; i < n / 2; i++)
    {
        swap(str[i], str[n - i - 1]);
    }

    return str;
}
 

void obliczanie_wyniku(int gniazdko_klienta)
{
    char bufor[ROZMIAR_BUFORA];
    int liczba = 0;
    int wynik = 0;
    int digit = 0;
    int znak = 1;
    bool blad = false;
    int odebrano=0;
    char zwracany_wynik[ROZMIAR_BUFORA];
    string ostateczny_wynik;
    bool end = false;
    string do_odwrocenia;
    string tymczasowy_buf;
    int rozmiar;
    char bufor_1[ROZMIAR_BUFORA];


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

        
        // jeżeli na końcu bufora nie ma znaków to end bedzie nam sie równał false
        if(bufor[odebrano-1] != '\r' && bufor[odebrano] != '\n')
        {
            // i wtedy wykonujemy pętlę idąc od tyłu odebranej paczki do napotkania pierwszego znaka \r\n (np. mamy: "2+2\r\n10+1")
            for(int j=odebrano; j > 0; j--)
            { 
                // kiedy napotkamy pierwszy terminator \r\n np. ten znajdujący się przed 10, to 
                if(bufor[j-1] == '\r' && bufor[j] == '\n')
                {
                    // wychodzimy z pętli
                    break;
                }
                else
                {
                    // a tutaj zapiszą się znaki (np. "1+01")
                    do_odwrocenia += bufor[j];
                }
            }
            // odwracamy zapisane znaki aby mieć np. 10+1
            odwroc_string(do_odwrocenia);
            
        }// dzięki temu powyższemu warunkowi wynik zostanie obliczony tylko dla 2+2, a przy kolejnej pętli gdzie np. będzie "1+2\r\n"
        // to później w programie ta część co została zostanie zlepiona z tym co teraz odebraliśmy i powstanie wyrażenie "10+11+2\r\n"
        // które zostanie poprawnie obłużone
        else
        { // a jeśli są znaki \r\n to end będzie się równał true
            end = true;

            if(bufor[odebrano-2] == '-' || bufor[odebrano-2] == '+')
            {
            cout << "BLAD 1 "<< endl;
            blad = true;
            }
        }
        
        // pętla która przepisuje mi dane z tablicy znaków do stringa który robi za tymczasowy bufor
        for(int k = 0; k < odebrano; k++)
        {
            tymczasowy_buf += bufor[k];
        }
        
        // jeśli end==true czyli jeśli na końcu naszej odebranej treści jest terminator to
        if(end)
        {
            // reszte wczesniej odebranej treści łączymy z aktualnie odebraną treścią żeby mieć np. wyrażenie "10+11+2\r\n"
            do_odwrocenia = do_odwrocenia + tymczasowy_buf;
            
            // cały string daje do tablicy
            strcpy(bufor_1, do_odwrocenia.c_str());
        }
        else // natomiast jeżeli end == false
        {
            // to po prostu zostawiamy naszą aktualnie odebraną treść bez zmian
            strcpy(bufor_1, tymczasowy_buf.c_str());
        }
        
        // walidacja danych
        if(bufor_1[0] == '\r' && bufor_1[1] == '\n')
        {
            blad = true;
        }
        
        // walidacja danych
        if(bufor_1[0] == '-' || bufor_1[0] == '+')
        {
            blad = true;
        }
        

        for(int i=0; i < odebrano; i++)
        {
            // walidacja danych, czy po sobie czasem ne występuje ++, --, -+, +-
            if((bufor_1[i] == '+' && bufor_1[i + 1] == '+') || (bufor_1[i] == '-' && bufor_1[i + 1] == '-') || (bufor_1[i] == '-' && bufor_1[i+1] == '+') || (bufor_1[i] == '+' && bufor_1[i+1] == '-'))
            {
                blad = true;
            }
             
            // walidacja danych, jeżeli odebrana treść nie jest liczbą, znakiem + ani -, oraz \r i \n to sygnalizujemy o błędzie
            if((bufor_1[i] < 48 || bufor_1[i] > 57) && bufor_1[i] != '+' && bufor_1[i] != '-' && bufor_1[i] != '\r' && bufor_1[i] != '\n')
            {
                blad = true;
            }

           // jeśli dane są poprawne to zostaje obliczony wynik
           if(!blad)
           { 
               
                if (bufor_1[i] >= '0' && bufor_1[i] <= '9') 
                {
					digit = bufor_1[i] - '0';

					// aby zachować dziesiętność liczb, mnożymy ją przez 10
					liczba = liczba * 10 + digit;

                    // sprawdzamy czy liczby podane przez klienta sie przepelniaja
					if(liczba < -1)
					{
						printf(" PRZEPELNIENIE LICZBY \n");
						blad = true;
					}
                } 
                else if (bufor_1[i] == '+' || bufor_1[i] == '-') 
                {
				    wynik += liczba * znak;
					liczba = 0;
					if (bufor_1[i] == '+')
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
            
            // jeśli dotarliśmy do terminatora \r\n 
           if(bufor_1[i] == '\r' && bufor_1[i+1] == '\n')
           {
                // i jeżeli nie wystąpiły żadne błędne dane to zapisujemy wynik do stringa ostateczny_wynik
                if(!blad)
                {
                    wynik += liczba * znak;
                    cout<< "WYNIK 2 : "<<wynik<< endl;
                    sprintf(zwracany_wynik, "%d", wynik);
                    ostateczny_wynik += string(zwracany_wynik)+"\r\n";

                }
                else // w przeciwnym wypadku do stringa ostateczny_wynik wpisujemy ERROR która informuje o błędnych danych
                {
                    ostateczny_wynik += "ERROR\r\n";
                    blad = false; 
                }
                 
                // czyszczenie zmiennych
                liczba = 0;
                digit = 0;
                znak = 1;
                wynik = 0;

           }

        }  

        // jeżeli przeszliśmy całą odebraną treść to odsyłamy wynik           
        if(write(gniazdko_klienta, ostateczny_wynik.c_str(), ostateczny_wynik.length()) == -1)
        {
            perror("Write error");
            exit(EXIT_FAILURE);
        }
            
        // czyszczenie zmiennych
        ostateczny_wynik.clear();
        tymczasowy_buf.clear();
        do_odwrocenia.clear();
        end = false;
        rozmiar = 0;
        
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
    if(signal(SIGCHLD, SIG_IGN) == SIG_ERR)// dajemy znać jądru że nie będziemy prosić o statusy dzieci i od razu po zakończeniu działania można usunąć ich wpisy w tablicy procesów
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