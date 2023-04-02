#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

bool printable_buf(const void * buf, int len)
{
    // rzutowanie do typu "adres fragmentu pamięci zawierającego ciąg bajtów"
    const char *bufor = buf;
    int i;

    for(i=0; i < len; i++)
    {
        if(*bufor < 32 || *bufor > 126)
        {
            return false;
        }

        bufor++;
    }

    return true;

}

int main()
{
    bool wynik = false;
    const char bufor[8] = "test1234"; // tutaj jest przykład dla znaków które są drukowalne

    wynik = printable_buf(bufor, 8);
    
    // jesli wynik będzie miał wartość true to wykona się if
    if(wynik)
    {
        printf("\nWszystkie znaki sa drukowalne\n");
    }
    else
    {
        printf("\nNie wszystkie znaki sa drukowalne\n"); // Przykład dla znaków nie drukowalnych: "test12\n"
    }

    return 0;
}