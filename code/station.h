#pragma once
#include <adress.h>
#include <trame.h>

typedef struct station
{
    ipv4 addr_ip;

}station;


void init_station(station * sta,ipv4 ip);

void desinit_station(station *sta);
