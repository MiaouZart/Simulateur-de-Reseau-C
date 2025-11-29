#pragma once
#include <stddef.h>
#include <port.h>
#include <adress.h>
#include <trame.h>
#include "interface.h" 
#include "machine.h"   

typedef struct port {
    interface* port;
    port_type type;
    bool type_changed;
    bpdu* best_received;
} port;

typedef struct bridge {
    size_t nb_ports;
    size_t priorite;
    size_t table_length;
    size_t max_table_length;
    com* table;
    port** ports;
    bpdu* bpdu;
    mac addr_mac;
    int root_index;
} bridge;

void init_bridge(bridge* bd, size_t nb_ports, size_t priorite, mac addr_mac);
void desinit_bridge(bridge* bd);
void add_to_com_table(bridge* bd, mac addr, interface* inter);
int check_if_in_com_table(bridge* bd, mac addr);
interface* bridge_get_free_interface(bridge* bd, machine* mach);

void print_switch_table(const bridge* bd);
int retrieve_port(bridge* bd, interface* inter);
void process_trame(bridge* bd, trame* trame, interface* input_port);
void copy_bpdu(bpdu* machine1, bpdu* machine2);
void recalculate_ports(bridge* br);
void send_bdpu(bridge* br);
