from bs4 import BeautifulSoup
import urllib.request
import sys

blad = False

# adres url strony którą sprawdzamy
url = 'https://pogoda.interia.pl/prognoza-szczegolowa-krakow,cId,4970'
    
# pobieramy odpowiedź serwera
odpowiedz_serwera = urllib.request.urlopen(url)

if odpowiedz_serwera.status != 200:
    blad = True
   
# odczytujemy dokument html i go dekodujemy w utf-8
zawartosc_html = odpowiedz_serwera.read().decode('utf-8')

soup = BeautifulSoup(zawartosc_html, 'html.parser')

temperatura = soup.find('div', {'class': 'weather-currently-temp-strict'})

# Sprawzdamy czy udało się porbrać zawartość div
if temperatura is None:
    blad = True

# Jeżeli bład nie jest prawdziwy to sprawdzamy i wyświetlamy wynik
if blad != True:

    # pobieramy wartość tekstową z bloku div
    string_temp = temperatura.text
    
    # jeżeli pierwszy znak jest minusem 
    if string_temp[0] == '-':
        
        n = len(string_temp)

        # kiedy mamy do czynienia z temperaturą jednocyfrową to po odjęciu 2 znaków celsjusza będzie cyfra naszej temepratury
        string_2 = string_temp[n-3]

        # natomiast jeżeli okaże się że mamy dwucyfrową temperature
        if string_temp[n-4].isnumeric():
          
            # to dopisujemy drugą cyfrę
            string_2 = string_temp[n-4] + string_2
        
        # musimy dolepić z powrotem usunięty minus aby zrobić walidacje danych
        string_2 = '-' + string_2

        # sprawdzamy czy pobrana temperatura na pewno jest w celsjuszach i mieści się  rozsądnych granicach 
        if int(string_2) < -30:
            blad = True
    
    else: 

        n = len(string_temp)

        # kiedy mamy do czynienia z temperaturą jednocyfrową to po odjęciu 2 znaków celsjusza będzie cyfra naszej temepratury
        string_2 = string_temp[n-3]

        # natomiast jeżeli okaże się że mamy dwucyfrową temperature
        if string_temp[n-4].isnumeric():
            # to dopisujemy drugą cyfrę 
            string_2 =  string_temp[n-4] + string_2

        # sprawdzamy czy pobrana temperatura na pewno jest w celsjuszach i mieści się  rozsądnych granicach
        if int(string_2) > 40:
            blad = True
    
if blad != True:
    print("\nAktualna temperatura w Krakowie to:",string_temp, "\n")

    # Zwracamy informacje o sukcesie
    sys.exit(0)
else:
    # A jeżeli błąd jest prawdziwy (czyli istnieje) to zwracamy odrazu Error
    print("Error")

    # zwracamy informację o porażce
    sys.exit(1)