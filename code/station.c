#include <station.h>
#include <stdlib.h>
#include <stdio.h>


void init_station(station * sta,ipv4 ip){
    // Procédure qui initialise une station.
    sta->addr_ip = ip;
}

void desinit_station(station *sta){
    // Procédure qui libère la station.
    sta->addr_ip = 0;
}


