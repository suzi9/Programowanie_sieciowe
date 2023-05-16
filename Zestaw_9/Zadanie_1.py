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

# pobieramy wartość tekstową z bloku div
string_temp = temperatura.text

# jeśli pierwszy znak to minus, to potem sprawdzam czy znak na pozycji 1 i 2 są liczbami
if string_temp[0] == '-':
    if string_temp[1].isnumeric() and string_temp[2].isnumeric():
        blad = False
        
        # sprawdzamy czy pobrana treść na pewno mieści się w rozsądnym przedziale Celsjusza
        sprawdzana_temp = string_temp[1] + string_temp[2]

        if int(sprawdzana_temp) < -30 and int(sprawdzana_temp) > 40:
            blad=True
    else:
        blad = True
else:
    # sprawdzam czy pierwsze dwa znaki są na pewno liczbami
    if string_temp[0].isnumeric() and string_temp[1].isnumeric():
        blad = False

        # sprawdzamy czy pobrana treść na pewno mieści się w rozsądnym przedziale Celsjusza
        sprawdzana_temp = string_temp[0] + string_temp[1]
        
        if int(sprawdzana_temp) < -30 and int(sprawdzana_temp) > 40:
            blad=True
    else:
        blad = True
    
if blad:
    print("Error")

    # zwracamy informację o porażce
    sys.exit(1)
else:
    print("\nAktualna temperatura w Krakowie to:",string_temp, "\n")

    # Zwracamy informacje o sukcesie
    sys.exit(0)