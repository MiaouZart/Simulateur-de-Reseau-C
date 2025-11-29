#pragma once 
#include <adress.h>
#include <stddef.h>
#include <bpdu.h>

typedef enum{
    PING = 1,
    BPDU = 2,
}trame_type;

typedef struct trame{
    mac source;
    mac dest;
    void* message;
    trame_type type;
}trame;

typedef size_t ping;

void init_trame(trame* tra, mac source, mac dest, void* message, trame_type type);

char* to_string_message(const trame* tra, char* buffer);

void desinit_trame(trame* tra);

void copy_trame(trame* dest, const trame* src);
