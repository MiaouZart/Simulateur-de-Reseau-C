#include <bpdu.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


void init_root_id(root_id* root_id, size_t priorite, mac addr_mac){
    // Procédure qui initialise un root id.
    root_id->priorite = priorite;
    root_id->addr_mac = addr_mac;
}

void init_bpdu(bpdu* bpdu, size_t priorite, mac addr_mac){
    // Procédure qui initialise un bpdu.
    bpdu->root = (root_id*) malloc(sizeof(root_id));
    init_root_id(bpdu->root, priorite, addr_mac);
    bpdu->cost = 0;
    bpdu->transmitting_id = addr_mac;
}

void desinit_bpdu(bpdu * bpdu){
    // Procédure qui libère la mémoire allouée par le bpdu.
    free(bpdu->root);
    bpdu->transmitting_id =0;
    bpdu->root = NULL;
}

void set_cost(bpdu* bpdu, size_t cost){
    // Procédure qui redéfinit le coût du bpdu.
    bpdu->cost = cost;
}

void copy_bpdu(bpdu* src, bpdu* dest) {
    // Procédure qui effectue une copie profonde du bpdu source dans le bpdu destination.
    dest->root = malloc(sizeof(root_id));
    memcpy(dest->root, src->root, sizeof(root_id));
    dest->cost = src->cost;
    memcpy(&dest->transmitting_id, &src->transmitting_id, sizeof(mac));
}

bool is_root_id_inferior_to(root_id* r1, root_id* r2){
    // Fonction qui retourne vrai si le root id 1 est strictement inférieur au root id 2, retourne faux sinon.
    if (r1->priorite < r2->priorite || (r1->priorite == r2->priorite && is_mac_inferior_to(r1->addr_mac, r2->addr_mac))){
        return true;
    }
    else{
        return false;
    }
}

bool root_id_equals(root_id* r1, root_id* r2){
    // Fonction qui retourne vrai si les 2 root id en paramètre sont égaux, retourne faux sinon.
    return (r1->priorite == r2->priorite && mac_equals(r1->addr_mac, r2->addr_mac));
}

bool is_bpdu_better(bpdu* received, bpdu* current) {
    // Fonction qui retourne vrai si le bpdu reçu est meilleur que le bpdu actuel, retourne faux sinon.
    if (!received || !current || !received->root || !current->root) return false;
    if (received->root->priorite < current->root->priorite) return true;
    if (received->root->priorite > current->root->priorite) return false;
    if (is_mac_inferior_to(received->root->addr_mac, current->root->addr_mac)) return true;
    if (is_mac_inferior_to(current->root->addr_mac, received->root->addr_mac)) return false;
    if (received->cost < current->cost) return true;
    if (received->cost > current->cost) return false;
    return is_mac_inferior_to(received->transmitting_id, current->transmitting_id);
    /*if (!root_id_equals(received->root, current->root)) {
        return is_root_id_inferior_to(received->root, current->root);
    }
    if (received->cost != current->cost) {
        return received->cost < current->cost;
    }
    return is_mac_inferior_to(received->transmitting_id, current->transmitting_id);*/
}

void set_transmit(bpdu* tochange, mac addr){
    // Procédure qui met à jour l'id du transmetteur du bpdu.
    tochange->transmitting_id = addr;
}

char* to_string_bpdu(bpdu* bpdu, char* buffer) {
    // Fonction qui convertit un bpdu en chaine de caractères puis la renvoie.
    if (!buffer || !bpdu || !bpdu->root) return NULL;
    
    char mac_root[20];
    char mac_transmitter[20];
    
    sprintf(buffer, "[%d + %s   ,  %zu   ,%s]", 
            bpdu->root->priorite,
           to_string_mac(&bpdu->root->addr_mac, mac_root),
           bpdu->cost,
           to_string_mac(&bpdu->transmitting_id, mac_transmitter));
    
    return buffer;
}



void afficher_bpdu(bpdu* b) {
    // Procédure qui affiche le bpdu à l'écran.
    if (!b || !b->root) {
        printf("BPDU invalide.\n");
        return;
    }

    char buffer[100];  
    if (to_string_bpdu(b, buffer)) {
        printf("BPDU: %s\n", buffer);
    } else {
        printf("Erreur lors de la conversion du BPDU en chaîne.\n");
    }
}
