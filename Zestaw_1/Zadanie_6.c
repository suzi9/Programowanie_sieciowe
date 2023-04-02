#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// Po kompilacji, podajemy programowi argumenty w przykładowy sposób: ./a.out zad6_to_copy.txt zad6_to_paste.txt
// Ważna jest kolejność!

int main(int argc, char* argv[])
{
    int deskryptor_pliku_1;
    int deskryptor_pliku_2;
    char bufor[64];
    int odczyt;
    
    // otwieramy plik z którego dane odczytamy
    deskryptor_pliku_1 = open(argv[1], O_RDONLY, 0664);

    if(deskryptor_pliku_1 == -1)
    {
        perror("Open error");
        exit(EXIT_FAILURE);
    }
    
    // otwieramy plik do którego dane wczytamy
    deskryptor_pliku_2 = open(argv[2], O_WRONLY | O_CREAT, 0664);
    
    if(deskryptor_pliku_2 == -1)
    {
        perror("Open error");
        exit(EXIT_FAILURE);
    }

    
    // robimy nieskończoną pętle while, po to aby odczytywać dane po kawałku, a nie wszytskie na raz
    while(1)
    {
        // odczytujemy dane z pliku zad6_to_copy.txt i zapisujemy do bufora, 
        // funkcja read zwróci nam liczbę pobranych bajtów
        odczyt = read(deskryptor_pliku_1, bufor, sizeof(bufor)); // podanie nazwy tablicy w postaci samego "bufor" to tak naprawdę podanie adresu tablicy

        if(odczyt == -1)
        {
            perror("Read error");
            exit(EXIT_FAILURE);
        }
        
        // jeżeli odczytano zero bajtów, oznacza to że już wszystko z danego pliku zostało odczytane
        // i nie ma nic więcej, a więc wtedy wychodzimy z pętli
        if(odczyt == 0)
        {
            break;
        }
        
        // z bufora bierzemy te dane i zapisujemy do pliku zad6_to_paste.txt
        if(write(deskryptor_pliku_2, bufor, odczyt) == -1)
        {
            perror("Write error");
            exit(EXIT_FAILURE);
        }

    }

    if(close(deskryptor_pliku_1) == -1)
    {
        perror("Close error");
        exit(EXIT_FAILURE);
    }

    if(close(deskryptor_pliku_2) == -1)
    {
        perror("Close error");
        exit(EXIT_FAILURE);
    }

    return 0;
}