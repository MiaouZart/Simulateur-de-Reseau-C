#include <stdio.h>
#include <stdlib.h>
#include <parser.h>
#include <graphe.h>

void simulate_stp(graphe* g);

void vider_buffer();

void demande_entier(int min, int max, int* entier);

void quitter(graphe* g, FILE* file, char* lignes[], int nb_lignes);

void envoyer_trame(graphe* g);

void afficher_table(graphe* g);