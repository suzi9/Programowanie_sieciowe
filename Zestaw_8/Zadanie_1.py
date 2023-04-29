import sys
import urllib.request

def pobieranie_strony():
    
    # kiedy błąd jest ustawiony na False, oznacza że nie ma błędu i wszystko jest dobrze
    blad = False
    
    # adres url strony którą sprawdzamy
    url = 'http://th.if.uj.edu.pl/'

    # ciąg znaków który mamy sprawdzić czy jest w pobranej stronie
    weryfikacja_strony = 'Institute of Theoretical Physics'
    
    # pobieramy odpowiedź serwera
    odpowiedz_serwera = urllib.request.urlopen(url)
    
    # sprawdzamy czy status wykonania jest różny od OK (200)
    if(odpowiedz_serwera.status != 200):
        # jeśli tak to świadczy o błędzie wykonania zapytania
        blad = True
    
    # sprawdzamy czy pobrany dokumnet jest inny niż HTML
    if(odpowiedz_serwera.headers['Content-Type'] != 'text/html'):
        blad = True
    
    # odczytujemy dokument html i go dekodujemy w utf-8
    zawartosc_html = odpowiedz_serwera.read().decode('utf-8')
 
    # jeśli ciągu znaków wymienionego powyżej nie ma w pobranej stronie
    if weryfikacja_strony not in zawartosc_html:
        # to zwracamy błąd
        blad = True
    
    # błąd równa się false
    if blad is False:
        print("Sukces")

        # to zwracamy informacje o sukcesie
        sys.exit(0)
        
    else:
        print("Porażka")

        # w innym przypadku zwracamy informację o porażce
        sys.exit(1)

if __name__ == '__main__':
    pobieranie_strony()