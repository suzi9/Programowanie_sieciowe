import sys
import requests
import json
from collections import defaultdict

def metodaGET(url):

    # wykonuje zapytanie HTTP GET do wpisanego URL przez użytkownika
    response = requests.get(url)

    if response.status_code == 200:
        # jeżeli nie wystąpi żaden błąd to zwracamy obiekt JSON
        return response.json()  
    else:
        None
 
def wykonanie(argv):

    blad = False

    if len(argv) != 2:
        print("Została wpisana niepoprawna ilość argumentów")
        sys.exit(1)
    
    # id zespołu podane jako argument od użytkownika
    id_zespolu = argv[1]

    # pusta lista o nazwie członkowie
    czlonkowie = []

    # tworzę domyślny słownik z kluczem jako nazwa zespołu i wartościami jako zestaw członków
    # używam set aby zapobiec powtarzaniu się członków
    wspolneZespoly = defaultdict(set) # w przeciwieństwie do dict, defaultdict 
                                      # automatycznie dostarcza domyślną wartość dla klucza, który jeszcze nie istnieje w słowniku

    # W dict próba dostępu do klucza, który nie istnieje, powoduje wystąpienie błędu KeyError

    # wywołujemy metodę GET aby pobrać informacje o zespole o podanym id
    wynik = metodaGET("https://api.discogs.com/artists/" + id_zespolu)
    
    if wynik is None:
        print("Wystąpił błąd przy pobieranu informacji o zespole")
        blad = True

    if blad != True:
        
        # przechodzimy przez listę członków zwróconych przez API
        for i in wynik["members"]:
            
            # następnie dla każdego członka w zespole pobiermay jego identyfikator oraz nazwisko
            id_wykonawcy = i["id"]
            wykonawca_nazwisko = i["name"]

            # dodaje pare (id, nazwisko) do listy "czlonkowie"
            czlonkowie.append((id_wykonawcy, wykonawca_nazwisko))

        # przechodzimy przez listę członków zespołu
        for wykonawca in czlonkowie:

            # pobieramy z API informacje na temat każdego członka zespołu
            wynik = metodaGET("https://api.discogs.com/artists/" + str(wykonawca[0]))

            if wynik is None:
                print("Błąd pobierania informacji o wykonawcy.")
                blad = True
                continue

            # przechodzimy przez listę grup do których należał dany członek
            for i in wynik["groups"]:

                # dla każdej grupy dodajemy nazwisko członka do zestawu członków zespołu -> zespół jako łańcuch
                zespol = i["name"]
                # do odpowiedniego zestawu w słowniku
                # nazwa zespołu to klucz, a członkowie to wartości
                wspolneZespoly[zespol].add(wykonawca[1])

        print('\n')
    
    if blad != True:
        # przechodzimy przez posortowane alfabetycznie zespoły
        for zespol, czlonkowie in sorted(wspolneZespoly.items()):
         
        # i jeżeli w dnym zespole grało faktycznie wiecej niz 2 członków  to wypisujemy nazwę zespołu i nazwiska członków
            if len(czlonkowie) > 1:
                print(f"{zespol}: {', '.join(sorted(czlonkowie))}")
                print('\n-----------------------------------------------\n')
    

        # Zwracamy informacje o sukcesie
        sys.exit(0)
    else:
        # A jeżeli błąd jest prawdziwy (czyli istnieje) to zwracamy odrazu Error
        print("Error")

        # zwracamy informację o porażce
        sys.exit(1)

if __name__ == "__main__":
    wykonanie(sys.argv)