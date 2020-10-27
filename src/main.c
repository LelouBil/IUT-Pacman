/******************************************************************************/
/* MAIN.c                                                                     */
/******************************************************************************/
#include "./main.h"
#include "../lib/libgraphique.h"

//definition de variables
#define PLATEAU_BLOCK_TAILLE 32

#define PACMAN_VITESSE 4

typedef struct pacman{
	Point pos;
	int vie;
}Pacman;

/******************************************************************************/
/* Fonctions graphiques						                                  */
/******************************************************************************/
    

void dessiner_plateau(Partie p);
void dessiner_pacman(Pacman p);

Pacman pacman_initialisation(Partie p);

/******************************************************************************/
/* MAIN                                                                       */
/******************************************************************************/
int main(int argc, char **argv)
/* argc indique le nombre d'arguments,
   argv est un tableau de mots contenant ces arguments. */
    {
    Partie  p;      // la structure contenant les données d'une partie
    int i,j;

    /** Traitement des arguments **************************************************/
    if(argc!=2) // Si le nombre d'arguments est différent de 2
        {
        printf("Usage: %s fichier_plan\n", argv[0]); // mode d'emploi et sortie
        return 0;
        }
    /******************************************************************************/

    /* Chargement du plan à partir du fichier fourni en paramètre                 */
    printf("Chargement du plan...\n");
    p = charge_plan(argv[1]);
    /* Si problème lors du chargement du plan...                                  */
    if(p.plateau == NULL)
        return 1;

#if DEBUG==1 //Ceci est un drapeau à activer/désactiver dans main.h
    /* Affichage du plan lu                                                       */
    printf("Affichage du plan...\n");
    for(i=0;i!=p.L;i++)
        {
        for(j=0;j!=p.C;j++)
            printf("%c",p.plateau[i][j]);
        printf("\n");
        }
#endif

/******************************************************************************/
/* A PARTIR D'ICI...C'EST A VOUS DE JOUER!                                    */
/******************************************************************************/
    
    ouvrir_fenetre(p.C * PLATEAU_BLOCK_TAILLE,p.L * PLATEAU_BLOCK_TAILLE);
    int EXIT_FLAG = 0;
    Point curseur;
    
    Pacman pac = pacman_initialisation(p);
    
    //while(!EXIT_FLAG){
		int evenement = attendre_touche_duree(30);
		
		if(evenement == SDLK_RIGHT){
			pac.pos.x += PACMAN_VITESSE;
		}
		
		if(evenement == SDLK_DOWN){
			EXIT_FLAG = 1;
		}
		
		curseur.x = 0;
		curseur.y = 0;
		dessiner_rectangle(curseur,p.C * PLATEAU_BLOCK_TAILLE,p.L * PLATEAU_BLOCK_TAILLE,black);
		dessiner_plateau(p);
		dessiner_pacman(pac);
		
	//}
    
	actualiser();
    
    attendre_clic();
    fermer_fenetre();
    return 0;
    }
    
    
    

void dessiner_plateau(Partie p){
//Parcour tout le tableau du plateau et dessine un rectangle pour chaque character '*'    
    Point curseur;
    
    for(int i=0;i<p.L;i++){
		for(int j=0;j<p.C;j++){
			if(p.plateau[i][j] == '*'){
				curseur.x = j * 32;
				curseur.y = i * 32;
				
				dessiner_rectangle(curseur,PLATEAU_BLOCK_TAILLE,PLATEAU_BLOCK_TAILLE,blue);
				}
			}
		}
	}

void dessiner_pacman(Pacman p){
//Dessine pacman selon sa position
	dessiner_cercle(p.pos,PLATEAU_BLOCK_TAILLE,yellow);
	}
	
Pacman pacman_initialisation(Partie p){
//Initialise pacman    
	Pacman pac;
	
    for(int i=0;i<p.L;i++){
		for(int j=0;j<p.C;j++){
			if(p.plateau[i][j] == 'P'){
				pac.pos.x = i * 32;
				pac.pos.y = j * 32;
				return pac;
				}
			}
		}
	printf("Structure de Plateau Invalide : pas de Debut pour Pacman (P)");
	pac.pos.x = 0;
	pac.pos.y = 0;
	return pac;
	}
