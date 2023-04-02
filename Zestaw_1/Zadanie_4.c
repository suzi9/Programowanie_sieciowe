#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

bool printable_str(const char * buf)
{
    // znak \0 nie jest uznawany, czyli on nie należy do naszego łańcucha znaków dlatego sprawdzamy wszystko poza nim
    for(; *buf != '\0'; buf++)
    {
        if(*buf < 32 || *buf > 126)
        {
            return false;
        }
    }

    return true;

}

int main()
{
    bool wynik = false;
    const char bufor[10] = "test1234\0"; // tutaj jest przykład dla dla znaków drukowalnych
    wynik = printable_str(bufor);
    
    // jesli wynik będzie miał wartość true to wykona się if
    if(wynik)
    {
        printf("\nWszystkie znaki sa drukowalne\n");
    }
    else
    {
        printf("\nNie wszystkie znaki sa drukowalne\n"); // Przykład dla znaków niedrukowalnych: "test1234\n\0"
    }

    return 0;
}