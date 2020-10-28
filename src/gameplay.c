#include "../lib/libgraphique.h"
#include "./gameplay.h"



void evenement_pacman(const Partie *arg_partie,Pacman *arg_pacman);

void pacman_deplace(const Partie *arg_partie,Pacman *arg_pacman,int direction);




void pacman_deplace(const Partie *arg_partie,Pacman *arg_pacman,int direction){
	
}



void pacman_evenement(const Partie *arg_partie,Pacman *arg_pacman){
	//appel de fonction pacman selon les evenements
	if(touche_a_ete_pressee(SDLK_UP)){
		pacman_deplace(arg_partie,arg_pacman,DIR_HAUT);
	}
	
	if(touche_a_ete_pressee(SDLK_DOWN)){
		pacman_deplace(arg_partie,arg_pacman,DIR_BAS);
	}
	
	if(touche_a_ete_pressee(SDLK_RIGHT)){
		pacman_deplace(arg_partie,arg_pacman,DIR_DROITE);
	}
	
	if(touche_a_ete_pressee(SDLK_LEFT)){
		pacman_deplace(arg_partie,arg_pacman,DIR_GAUCHE);
	}
}

