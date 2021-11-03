//
// Created by boris on 03.11.2021.
//

#ifndef MGOS_UNS_PA_SIZING_ALGO_H
#define MGOS_UNS_PA_SIZING_ALGO_H

#include "stdint.h"
#include "stdbool.h"

struct ati_spi_ade7880;
struct state{
    bool(*func)(struct ati_spi_ade7880* _dev, struct state* _state);
    uint16_t cntr, max;
    struct state* next;
};


struct state* create_state( bool(*_func)(struct ati_spi_ade7880* _dev, struct state* _state), uint16_t _max, struct state* next );

struct state* create_angle_sizing_algo();

#endif //MGOS_UNS_PA_SIZING_ALGO_H
