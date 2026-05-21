#include <stdbool.h>
#pragma once
#ifndef _NETWORK_H_
#define _NETWORK_H_


struct Network {
        unsigned int layers, inputs, *neyrones;
        float ***data;
        float (*f)(float);
        float (*g)(float);
};
typedef struct Network Network;

Network create_network(unsigned int, unsigned int, unsigned int *, float (*)(float), float (*)(float));

Network create_network_default(unsigned int, unsigned int, unsigned int *, float (*)(float), float (*)(float), float);

Network create_network_random(unsigned int, unsigned int, unsigned int *, float (*)(float), float (*)(float), float, float);

Network create_network_default_random(unsigned int, unsigned int, unsigned int *, float (*)(float), float (*)(float));

Network copy_network(Network);

void delete_network(Network);

bool is_ok_network(Network);

//static void delete_trash(unsigned int, unsigned int *, int ***);

void save_network(Network, const char *);

Network load_network(const char *);

float *compute_network(Network, float *);

void compute_network_to(Network, float [], float []);

float teach_network(Network, float, float [], float []);

#endif
