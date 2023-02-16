Blaga Ana-Maria-Andreea, 344C5

Tema aleasa de mine este Bitonic Sort. 

Am inceput implementarea de la varianta recursiva, 
https://www.geeksforgeeks.org/bitonic-sort/ , si am realizat la paralelizare
ca aceasta varianta determina crearea unui numar exponential de threaduri care 
asteptau sa se execute si determinau un timp de executie mare, respectiv 
implementarea nu scala. 

Din aceste motiv am trecut la o varianta imperativa: 
https://en.wikipedia.org/wiki/Bitonic_sorter , care s-a dovedit mai buna fata 
de cea recursiva.

In toate cele 4 variante am ales paralelizez bucla cea mai interna a 
algoritmului.

Probleme am intampinat pe partea de sincronizare si comunicare intre threaduri,
acestea necesitand sa stie in fiecare moment cum sunt ordonate elementele in 
array la nivel global pentru a face interschimbarile de elemente corecte.

O constrangere a algoritmului Bitonic Sort este ca numarull de elemente de 
sortat sa fie o putere a lui 2, altfel acesta va esua deoarece nu se executa 
comparatiile corecte.

Tema se compileaza si ruleaza astfel:
make all
make run_all

make <nume_versiune>
make run_<nume_versiune>

Parametrii N (numarul de elemente al array-ului de sortat) si P (numarul de 
threaduri de folosit) au fost setati by default in Makefile la 32, respectiv 4.
Acestia pot fi schimbati prin precizarea la rulare a altor valori pentru 
acestia:
make run_all N=<valoare_N> P=<valoare_P>
make run_<nume_versiune> N=<valoare_N> P=<valoare_P>

Fiecare varianta este rulata si cu time pentru a evidentia care din ele este 
mai rapida.

In cazul varaintei hibride se pornesc P/2 workeri si P/2 threaduri. Am reusit 
sa testez doar cu P=4 si P=2 datorita resurselor limitate ale calculatorului.

Pentru a facilita testarea se genereaza automat un array de lungime N.

Dupa testarea pe mai multe date am remarcat ca varianta OpenMP obtine cele mai 
bune rezultate pe array-uri de dimensiuni mari, urmata de cea cu Pthreads. Cele 
mai lente au fost variantele MPI si hibrid MPI cu OpenMP datorita sincronizarii 
mai dificile a arrayului de sortat la nivelul workerilor, sincronizare 
realizata printr-un shared window.

In cazul array-urilor de mici dimensiuni varianta seriala este mai potrivita
deoarece nu introduce overhead de alocare a structurii threadurilor, memorie 
suplimentara, sincronizare etc. Insa pe arrayuri de mari dimensiuni varianta 
OpenMP a obtinut cele mai bune rezultate.
