//
// Created by boris on 03.11.2021.
//

#ifndef MGOS_UNS_PA_DATA_H
#define MGOS_UNS_PA_DATA_H
#include "stdint.h"
/**
 * Callibration data for ADE7880, should be read from flash
 */
struct ati_spi_ade7880_calibration_data {
    /**
     * Phase A,B,C voltage gains and offsets array_seq, data loads into ade7880
     * Registers array seq: [AVGAIN, BVGAIN, CVGAIN, AVRMSOS, BVRMSOS, CVRMSOS]
     */
    uint32_t VGO_ade[6];
    /**
     * Phase A,B,C current gains and offsets, data loads into ade7880
     * Registers array seq: [AIGAIN, BIGAIN, CIGAIN, AIRMSOS, BIRMSOS, CIRMSOS]
     */
    uint32_t IGO_ade[6];
    /**
     * Neutral current gains and offset, data loads into ade7880
     * Registers array seq: [NIGAIN, NIRMSOS]
     */
    uint32_t NIGO_ade[2];
    /**
     * Overcurrent, overvoltage and SAG level thresholds, loads into ade7880
     * Registers array seq: [OILVL, OVLVL, SAGLVL]
     */
    uint32_t Thresholds_ade[3];
    /**
     * Voltage and current gain and offset for all phases, not be load to ade7880, use esp32 for get real value
     * Coefficient seq: [VGain, VOffset, IGain, IOffset]
     */
    float Coefficients_esp[4];
};

struct ati_spi_ade7880_sizing_data{
    float Vrms[3];
    float Irms[3];
    float Freq[3];
    float Angle_v2c[3];
    float Angle_v2v[3];
    float Angle_c2c[3];
};

/**
 * Create default struct ati_spi_ade7880_calibration_data
 * @return: struct ati_spi_ade7880_calibration_data pointer
 */
struct ati_spi_ade7880_calibration_data* create_call_coef();


#endif //TEST_ADE7880_MGOS_UNS_PA_DATA_H
