#include <stdlib.h>
#include <stdio.h>

void drukuj( int tablica[], int ilosc_elementow)
{
    int j;
    
    for(j=0; j < ilosc_elementow; j++)
    {
        if(tablica[j] > 10 && tablica[j] < 100)
        {
            printf("%d\n", tablica[j]);
        }

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

    drukuj(liczby, i);

    return 0;
}
