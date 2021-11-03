//
// Created by boris on 26.10.2021.
//
/**
 * Driver for at45db021d
 * Bibliography:
 * [1] https://www.farnell.com/datasheets/388169.pdf
 */
#ifndef MGOS_SPI_ADE7880_H
#define MGOS_SPI_ADE7800_H
#include <stdint.h>
#include <stdbool.h>
#include "mgos_spi.h"
#include "mgos-spi-ade7880-impl.h"

enum ADE7880_OP_RET_CODE {ADE7880_OK, ADE7880_FAIL };
struct ati_spi_ade7880_calibration_data;
struct ati_spi_ade7880_sizing_data;
struct ati_spi_ade7880_config{
    /**
       * reset_pin - нога сброса микросхемы памяти, низкий уровент более чем на 10us сбрасывает все текущие операции [1] Table 18-4
       *   после сброоса нужно подождать max 20ms [1] point 16.1
       */
    uint8_t reset_pin;
    /**
     * cs_pin нога выбора микросхемы в SPI, выбор по низкому уровню
     */
    uint8_t cs_pin;
    /**
     * isol_pin - нога включения изолятора;
     */
    uint8_t isol_pin;
    /**
     * Структа для работы с SPI в которой не указаны GPIO выходы CS должны быть установлены в -1.
     */
    struct mgos_spi* spi;
    /**
     * SPI mode
     */
    uint8_t mode;
    /**
     * SPI frequency in Hz
     */
    uint32_t freq;
    /**
     * Указательна структуру с данными калибровки, данный указатель должен быть валиден всегда
     */
     struct ati_spi_ade7880_calibration_data* coef;
};
struct ati_spi_ade7880;
/**
 * Create ADE7880 SPI device
 * @param _conf: configuration struct
 * @return: Pointer to ati_spi_ade7880 struct
 */
struct ati_spi_ade7880* ati_spi_ade7880_create(struct ati_spi_ade7880_config const* _conf );
/**
 * Destroy ADE7880 SPI device
 * @param _dev: ADE7880 device for destroy
 */
void  ati_spi_ade7880_destroy(struct ati_spi_ade7880* _dev );
/**
 * Reset ADE7880, choose SPI communication, write configuration, on write-protection and run DSP
 * @param _dev: ADE7880 device
 */
void ati_spi_ade7880_reset(struct ati_spi_ade7880* _dev );
/**
 * Get sizing data from ADE
 * @param _dev: ADE7880 device
 * @return: pointer to const struct with sizing data
 */
struct ati_spi_ade7880_sizing_data const* ati_spi_ade7880_get_data(struct ati_spi_ade7880* _dev );

#endif //MGOS_SPI_ADE7880_H
