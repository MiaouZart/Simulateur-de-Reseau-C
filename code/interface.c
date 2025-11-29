#include "interface.h"
#include "machine.h"
#include <stdio.h>
#include <stdlib.h>

void init_interface(interface* inter,struct machine* machine) {
    // Procédure qui initialise une interface.
    if (!inter) return;
    inter->machine = machine;
    inter->connected_to = NULL;
    inter->poids = -1;
}

void desinit_inter(interface* inter){
    // Procédure qui supprime la liaison de l'interface.
    inter->machine = NULL;
}

void connect_two_interface(interface* a, interface* b, size_t poids){
    // Procédure qui connecte 2 interfaces (c'est comme si on les branchaient avec un câble).
    a->connected_to = b;
    a->poids = poids;
    b->connected_to = a;
    b->poids = poids;
    printf("Connexion: %s <-> %s (Poids: %zu)\n",
       to_string_mac(&a->machine->addr_mac,  (char[20]){0}),
       to_string_mac(&b->machine->addr_mac,  (char[20]){0}),
       poids
    );
}

void send_data(interface *intf, void *tr) { //Ca envoie les données dans le "câble"
    /* Procédure qui envoie des données dans l'interface connectée à l'interface 'intf',
    c'est comme si on envoyait les données dans un câble. */
    if (intf->connected_to && intf->connected_to->machine) {
        receive_trame(intf->connected_to->machine, tr, intf->connected_to);
    }
}

void receive_data(interface *receiver, void *data) {
    // Procédure qui gère la réception des données.
    if (receiver->machine) {
        receive_trame(receiver->machine, data, receiver);
    }
}