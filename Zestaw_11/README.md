# Zestaw 11

## Zadanie 1
Uwaga: nie wolno Państwu używać bibliotek dedykowanych bazie Discogs, które są wymienione na początku strony https://www.discogs.com/developers (pod nagłówkiem „Quickstart”). To podkopałoby dydaktyczny cel tego zadania, którym jest sprawdzenie czy przy pomocy zwykłej biblioteki klienckiej HTTP potraficie pobrać dane z serwera REST. Innymi słowy, zadanie sprawdza, czy potraficie przeczytać ze zrozumieniem dokumentację serwera REST i napisać kod konstruujący odpowiednie zapytania HTTP.

Napisz program, który w oparciu o informacje z Discogs sprawdza, czy członkowie (aktualni bądź byli) danego zespołu grali razem w jakichś innych zespołach. Wyświetl nazwy tych innych zespołów oraz nazwiska muzyków. Załóż, że numeryczny identyfikator zespołu podawany jest jako argv[1].

Dla przykładu: 359282 to Budka Suflera, według Discogs grali w niej Mieczysław Jurecki (id 702387) i Marek Stefankiewicz (id 532854), którzy grali też w Perfekcie (id 669348).

Zadbaj aby drukowane wyniki były posortowane po nazwie zespołu, oraz aby w przypadku więcej niż dwóch osób grających razem w innym zespole kompresować wyniki (np. w wypadku czterech osób grających w innym zespole X należy je wypisać jako pojedynczą czwórkę, a nie jako sześć par).

## Zadanie 2 (NIEOBOWIĄZKOWE)
Rozszerz powyższy program tak, aby jako argument można było podawać nie tylko numer, lecz również nazwę zespołu. Najprawdopodobniej będzie wymagać to zarejestrowania się na witrynie Discogs w celu uzyskania tokena dla Państwa aplikacji, bo operacji wyszukiwania zespołu po nazwie zdaje się nie można wywoływać anonimowo.

## Źródła
Autor zadań: Dr. Wojciech Palacz\
Link do strony: https://users.uj.edu.pl/~palacz/ 