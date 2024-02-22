#  Praca magisterska

<p align="left"><i>Autor: Jakub Kaniowski</i></p>

<hr/>
<h5>Politechnika Śląska</br>
Wydział Automatyki Elektroniki i Informatyki</br>
Kierunek: Informatyka. Specjalność: Oprogramowanie Systemowe</h5>


<hr/>
<p align="center"><b>Rozwiązanie problemu Cargo Theft VRP z użyciem algorytmów heurystycznych</b> </p>
<hr/>

>Opis projektu: 

*Celem pracy dyplomowej jest zapoznanie się z istniejącymi algorytmami heurystycznymi rozwiązywania problemu CTVRP i innych wariantów VRP. Następnie należy zaimplementować wybrane algorytmy dostosowując je dla problemu CTVRP i przeprowadzić badania eksperymentalne.*

Platforma GitHub jest wykorzystywana jako miejsce kopii zapasowych oraz główne repozytorium tego projektu.
Znajdą się tutaj kody źródłowe, dokumenty wykorzystywane do wykonania projektu.

<h3>Aktualny postęp prac:</h3>
<hr/>
<h3><p align="center">Faza 1 - Początek - Research. Done (100%)</p></h3>
<h3><p align="center">Faza 2 - Implementacja - implementacja wszystkich trzech algorytmów. Done (100%)</p></h3>
<h3><p align="center">Faza 3 - Porównanie - Badanie. Done (100%)</p></h3>
<h3><p align="center">Faza 4 - Dokumentacja. Pisanie dokumentu. Done (100%)</p></h3>
<hr/>
<h3>Struktura repozytorium:</h4>
<ul>
<li>Folder <b><i>Documentation</i></b> - Znajduje się tam Praca Magisterska oraz dokumentacja<br/></li>
<li>Folder <b><i>Source</i></b> - Znajduje się tam kod źródłowy programu rozwiązującego problematyke pracy.</li>
<li>Folder <b><i>Program</i></b> - Znajduje się tam skompilowany program.</li>
</ul>

>Opis problemu:

<p align ='justify'><i> Klasyczny problem trasowania pojazdów (VRP, ang. Vehicle Routing Problem) sformułowali w 1959 roku G. B. Dantzig i J. H. Ramser. W problemie VRP dany jest skierowany graf G = (V, E) z funkcją wagową d. Graf G zawiera n wierzchołków o numerach 1, ..., n; wierzchołek 1 reprezentuje magazyn, a pozostałe wierzchołki reprezentują miasta lub klientów. Macierz D = (d_{ij}) zawiera wartości funkcji wagowej d, gdzie d_{ij} jest równe wadze krawędzi (i, j) interpretowanej jako koszt przejazdu z i do j. W magazynie znajduje się flota pojazdów o identycznej ładowności (homogeniczna flota pojazdów). Pojazdy dostarczają towar z magazynu do klientów i innych miast. Dla każdego wierzchołka i > 1 zdefiniowana jest ilość towaru q_i >= 0, która ma być do niego dostarczona. Celem VRP jest wyznaczenie zbioru tras przejazdu pojazdów minimalizując koszt przejazdu, przy założeniu: - każdy wierzchołek v (za wyjątkiem magazynu) jest odwiedzany dokładnie jeden raz, przez dokładnie jeden pojazd, - każda trasa pojazdu rozpoczyna się i kończy w magazynie, tj. w wierzchołku 1, - ilość towaru przewożonego na danej trasie nie może przekroczyć dopuszczalnej ładowności pojazdu. VRP należy do grupy problemów NP-trudnych i jest rozwiązywany z użyciem różnych algorytmów heurystycznych. Problem VRP jest rozszerzany i rozpatrywane są rożne jego warianty, które uwzględniają różne aspekty życia codziennego. Jednym z rozszerzeń VRP jest Cargo Theft (CTVRP). W CTVRP uwzględnia się transport towarów przez tereny potencjalnie uznane za niebezpieczne, a co za tym idzie ryzyko napadu na pojazd i kradzież transportowanego towaru. Celem jest wyznaczenie tras minimalizując m.in. dodatkowy koszt, który jest ponoszony w przypadku kradzieży towaru.</i></p>

<hr/>


<hr/><h5><b>Start platformy GitHub: 06.04.2023</b></h5> <h5><b>Praca obroniona: 28.11.2023</b></h5>
