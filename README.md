<br />
<div align="center">
  <img src="https://i.imgur.com/2QUSNa2.png" alt="If the image is not loading, imagine the logo here :)" width=100% height=100%>

  <p align="center">
    <br> <br>
    Proiect in cadrul materiei PA UPB.
    <br>
    Bot creat pentru jocul Halite 1
    <br>
    Cu ajutorul lui <a href="https://github.com/Alexe1289">Alex</a>

</div>

## Botul are doua stari:
* Early Game
* Mid Game

## Early Game
O sa definim *Frontiera* botul, ca marginea sa (blocurile ce au ca vecini spatii goale)
In Early game, botul salveaza toate spatiile de pe frontierea sa, si pe rand calculeaza pentru fiecare spatiu detinut, in ce directie
se poate duce pentru a ajunge cat mai rapid la frontiera.

daca blocul se afla pe frontiera, acesta va verifica daca merita sau nu sa captureze spatiile goale

daca blocul este unu interior, se va lua in calcul blocul de pe frontiera cu cea mai mica distanta

## Mid Game
Botul va intra in mid game, in momentul in care acesta se intalneste cu un inamic

In Mid game, botul salveaza toate spatiile de pe frontierea sa in doua tipuri (spatii goale, spatii inamic),
si pe rand calculeaza pentru fiecare spatiu detinut, in ce directie se poate duce pentru a ajunge cat mai rapid la frontiera.

daca blocul se afla pe frontiera, o sa prefere sa captureze spatiile inamicului. In cazul in care nu se afla in apropiere
astfel de spatii, se va folosii strategia de la early game

daca blocul este unu interior, se va lua in calcul blocul de pe frontiera cu cea mai mica distanta.
daca blocul este unu interior, poate sa inceapa drumul pana la frontiera mai devreme in cazul in care are productia mare.

<br>
<br>
<br>

