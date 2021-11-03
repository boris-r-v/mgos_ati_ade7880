//
// Created by boris on 03.11.2021.
//
#include "mgos_system.h"
#include "string.h"
#include "mgos-uns-pa-data.h"

struct ati_spi_ade7880_calibration_data* create_call_coef(){
    struct ati_spi_ade7880_calibration_data* c = (struct ati_spi_ade7880_calibration_data*) calloc(1, sizeof(*c));
    memset(c, 0x00,  sizeof (*c));
    c->Thresholds_ade[0] = 0xFFFFFF;
    c->Thresholds_ade[1] = 0xFFFFFF;
    //Coefficient seq: [VGain, VOffset, IGain, IOffset]
    c->Coefficients_esp[0] = 180.7692;
    c->Coefficients_esp[1] = 0.0;
    c->Coefficients_esp[2] = 1.0;
    c->Coefficients_esp[3] = 0.0;
    return c;
}

