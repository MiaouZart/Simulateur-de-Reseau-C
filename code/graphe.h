#pragma once
#include <stddef.h>
#include <stdlib.h> 
#include <machine.h>


typedef size_t poids;

#define NONE ((poids)0)


typedef struct sommet
{
    int index;
    machine* machine;
}sommet;

typedef struct graphe
{
    sommet** sommets;
    int ordre;
    size_t nbSwitch;
    size_t nbStation;
}graphe;

void init_graphe(graphe *m,int nbSommet);
void init_sommet(sommet * s, int index,machine * machine);
void desinit_graphe(graphe *g);
void desinit_sommet(sommet *s);

void add_sommet(graphe *g,sommet *s,int index);
