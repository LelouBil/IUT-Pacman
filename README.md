# IUT-Pacman
Projet S1 IUT Velizy 2020

Auteurs:
A1 - Bilel Medimegh
A1 - Celeste Casalotti

---

## Le jeu

Un jeu de pacman classique

## Compilation

placez vous dans le dossier et faites
`make clean all`

Le fichier compilé est `resultat`
Au lancement il s'attend a avoir le plateau dans `data/test.txt`

### Instruction de lancement

`./resultat`

### Regles du jeux


Vous controllez Pacman qui doit manger les pac gommes sur le plateau tout en évitant les fantomes

Vous avez 3 vies pour faire le plus de niveaux possibles.
Il y a un tunnel a votre disposition pour aller d'un coté a l'autre du plateau rapidement.
Vous pouvez aussi manger des bonus, et vous aurez 5 secondes pour aller manger des fantomes. 
Les fantomes mangés se regenerent a leur base en 3 secondes

Controles : 

 - Pacman : Fleches du clavier
 - Valider les textes a l'ecran : Entrée
 - Quitter : Echap

Score : 

 - Manger une Pac gomme : + 1 
 - Manger un Bonus : 50 points
 - Manger un fantome : 100 points
 - Faire un niveau : 500 points

