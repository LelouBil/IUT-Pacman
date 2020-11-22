#include "./gameplay.h"
#include "utils.h"
#include "timings.h"
#include "utils.h"

#define pacman_px_per_s (PLATEAU_BLOCK_TAILLE * PACMAN_SPEED)

#define pacman_px_per_frame (pacman_px_per_s / framerate)


int is_pacman_aligned_dir(Pacman *pacman, direction dir) {
    Pos p = get_case_center(pacman->case_pacman);
    switch (dir) {
        case DIR_HAUT:
        case DIR_BAS:
            return pacman->position.x == p.x;
        case DIR_GAUCHE:
        case DIR_DROITE:
            return pacman->position.y == p.y;
        default:
            return 0;
    }
}


void pacman_do_move(Partie *partie, direction dir) {
    Pos vector = dir_to_vector(dir);
    vector.x *= pacman_px_per_frame;
    vector.y *= pacman_px_per_frame;
    partie->pacman.position.x += vector.x;
    partie->pacman.position.y += vector.y;
}

void pacman_deplace(Partie *partie) {

    Pacman *pacman = &partie->pacman;

    for (int key = SDLK_UP; key < SDLK_LEFT + 1; ++key) {

        int dir = dir_from_key(key);

        if (touche_a_ete_pressee(key)) {

            Case *target = get_case_at(partie, pacman->case_pacman, dir);

            if ((target == NULL) || (is_pacman_aligned_dir(pacman, dir) && target->wall == 0))
                //on autorise le changement de direction si pacman est aligné avec cette colonne/ligne et si il n'y a pas de mur a cet endroit
                // OU si il essaie d'aller hors du plateau (pour les tunnels)
                pacman->direction = dir;
        }
    }

    if (!pacman->oob) {
        Case *target = get_case_at(partie, pacman->case_pacman, pacman->direction);

        if ((target == NULL || !target->wall) ||
            !is_pacman_aligned_dir(pacman, get_perpendicular(pacman->direction))) {
            //si vers hors du plateau, ou pas de mur, on avance.
            // ou si pas aligné perpandiculairement a la direction actuelle, on continue
            pacman_do_move(partie, pacman->direction);
        }

    } else {
        pacman_do_move(partie, pacman->direction);
    }


    //on replace la position en case de pacman
    Pos p = partie->pacman.position;

    int cx = p.x / PLATEAU_BLOCK_TAILLE;
    int cy = p.y / PLATEAU_BLOCK_TAILLE;

    //si case hors du plateau, oob

    if (cx >= 0 && cx < partie->xmax && cy >= 0 && cy < partie->ymax) {
        pacman->oob = 0;
    } else {
        pacman->oob = 1;
    }


    //on gere les 4 cas de sortie du plateau et on met bien cx et cy

    int tunnel = 0;

    if (cx > partie->xmax) {
        cx = -1;
        tunnel = 1;
    } else if (cx < 0) {
        cx = partie->xmax;
        tunnel = 1;
    }

    if (cy > partie->ymax) {
        cy = -1;
        tunnel = 1;
    } else if (cy < 0) {
        cy = partie->ymax;
        tunnel = 1;
    }


    if (tunnel) {

        //on replace pacman bien, parce que cx et cy ont été modifiés au dessus
        partie->pacman.position = (Pos) {cx * PLATEAU_BLOCK_TAILLE + PLATEAU_BLOCK_TAILLE / 2,
                                         cy * PLATEAU_BLOCK_TAILLE + PLATEAU_BLOCK_TAILLE / 2};
    }

    if (!pacman->oob) {

        partie->pacman.case_pacman = &partie->plateau[cx][cy];
    }
}


