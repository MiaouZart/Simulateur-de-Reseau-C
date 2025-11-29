#pragma once
#include "adress.h"
typedef enum {
    RACINE = 0,
    DESIGNE = 1,
    NONDESIGNE = 2
} port_type;

typedef struct com
{
    mac addr_mac;
    size_t index_port;
}com;



