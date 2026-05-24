Cum am folosit AI la proiect

Faza 1
La acest proiect AI m-a ajutat cu functiile ajutatoare pentru constructia comenzii FILTER.

Funcțiile la care am folosit AI

promt:cum as putea separa aceste campuri?

1. parse_condition()

Mai ales la felul in care trebuiau separate ,comanda sa fie segmentata in 3 parti.
 
severity:>=:2

în trei părți separate:

- câmp
- operator
- valoare

Am folosit această idee pentru a putea prelucra ușor condițiile introduse de utilizator.

2. match_condition()

Doar pentru a verifica daca am realizat functiile de comparatie intr-un mod bun si eficient.

promt:e ok cum am facut eu?ce sa schimb?

Exemple:

- severity >= 2
- severity == 3
- category == road
- inspector != ana


3. filterCommand()

La aceasta functie am folosit AI pentru a imbina cele 2 functii,pentru a filtra corect conditile si a cauta tipare ce se potrivesc

Pașii realizați:

- citirea fiecărui raport din fișier
- apelarea parse_condition()
- apelarea match_condition()
- afișarea doar a rapoartelor care respectă toate condițiile


Faza 2
Am folosit AI pentru a afla ce inseamna fiecare cod returnat de functia predefinita pid().

Pentru fisierul monitor_reports.c am folosit acest tool si am consultat si pagina de manual pentru a afla structura de la sigaction.

Am consultat si pentru a schimba comentariile in engleza,sa fiu sigura ca sunt corecte gramatical.

Am cautat informatii si despre ce parametrii are functia kill().


Faza 3
Am folosit AI pentru a creea interfata interactiva,mai exact sa imi explice de ce trebuia sa fie bagate comenzile in acel while(1);

promt:ce face acel while si de ce nu e bine cum am facut eu?

Plus explicatii extra pentru a fi sigura ca am rezolvat in regula cerinta cu hub_mon,cu proces copil in proces copil



