#include <stdlib.h>
#include <stdio.h>

void drukuj_alt( int *tablica, int ilosc_elementow)
{
    int j;

    for(j=0; j < ilosc_elementow; j++)
    {
        // wskaźnik *tablica wskazuje na wartość pod adresem liczby[]
        if(*tablica > 10 && *tablica < 100)
        {
            printf("%d\n", *tablica);
        }
        
        // za pomocą tablica++ przechodzimy na kolejny indeks, np. z indeksu 0 na 1 itd
        tablica++;
    }
}

int main()
{
    int liczby[50];
    int i;
    int liczba=0;

    for(i=0; i < 50; i++)
    {
        scanf("%d",&liczba);

        if(liczba == 0)
        {
            break;
        }

        liczby[i] = liczba;
    }

    drukuj_alt(liczby, i);

    return 0;
}
