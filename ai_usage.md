Cum am folosit AI la proiect

La acest proiect am folosit AI doar pentru implementarea funcționalității filter, exact partea permisă în cerință.

AI m-a ajutat în special la construirea logicii pentru interpretarea condițiilor introduse în linia de comandă și compararea lor cu datele din structura Report.

Funcțiile la care am folosit AI

1. parse_condition()

La această funcție AI m-a ajutat cu ideea de împărțire a unui text de forma:

severity:>=:2

în trei părți separate:

- câmp
- operator
- valoare

Am folosit această idee pentru a putea prelucra ușor condițiile introduse de utilizator.

2. match_condition()

La această funcție AI m-a ajutat cu logica de comparare dintre valorile raportului și condiția primită.

Exemple:

- severity >= 2
- severity == 3
- category == road
- inspector != ana

AI m-a ajutat să structurez verificările pentru numere și pentru șiruri de caractere.

3. filterCommand()

La această funcție AI m-a ajutat cu ideea generală de parcurgere a fișierului reports.dat și verificarea fiecărui raport.

Pașii realizați:

- citirea fiecărui raport din fișier
- apelarea parse_condition()
- apelarea match_condition()
- afișarea doar a rapoartelor care respectă toate condițiile

Cum am continuat eu

După ajutorul primit, eu am integrat funcțiile în proiect, le-am adaptat la structura mea Report, am testat comenzile în terminal și am corectat eventualele erori.

Concluzie

AI a fost folosit doar pentru partea de filter, adică interpretarea condițiilor și logica de filtrare. Restul proiectului a fost realizat separat de mine.
