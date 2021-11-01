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
enum ADE7880_OP_RET_CODE {ADE7880_OK, ADE7880_FAIL };
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
 * Write 8bit register
 * @param _dev: ADE7880 spi device
 * @param _data: one byte data to write
 * @param _addr: 16bits register address
 * @return: ADE7880_OK if operation successful, or error code in overwise
 */
uint8_t ati_spi_ade7880_write8(struct ati_spi_ade7880* _dev, uint8_t _data, uint16_t _addr );
/**
 * Write 16bit register (BitLengthDuringCommunication: 16, 16ZP)
 * @param _dev: ADE7880 spi device
 * @param _data: two byte data for write
 * @param _addr: 16bits register address
 * @return: ADE7880_OK if operation successful, or error code in overwise
 */
uint8_t ati_spi_ade7880_write16(struct ati_spi_ade7880* _dev, uint16_t _data, uint16_t _addr );
/**
 * Write 32 bit register (BitLengthDuringCommunication: 32, 32ZP, 32SE, 32ZPSE)
 * @param _dev: ADE7880 spi device
 * @param _data: four byte data for write
 * @param _addr: 16bits register address
 * @return: ADE7880_OK if operation successful, or error code in overwise
 */
uint8_t ati_spi_ade7880_write32(struct ati_spi_ade7880* _dev, uint32_t _data, uint16_t _addr );

/**
 * Read one byte from ADE7880
 * @param _dev: ADE7880 spi device
 * @param _data: 8bit variable for storage data
 * @param _addr: 16bits register address
 * @return: ADE7880_OK if operation successful, or error code in overwise
 */
uint8_t ati_spi_ade7880_read8( struct ati_spi_ade7880* _dev, uint8_t* _data, uint16_t _addr );
/**
 * Read two byte from ADE7880
 * @param _dev: ADE7880 spi device
 * @param _data: 16bit variable for storage data
 * @param _addr: 16bits register address
 * @return: ADE7880_OK if operation successful, or error code in overwise
 */
uint8_t ati_spi_ade7880_read16( struct ati_spi_ade7880* _dev, uint16_t* _data, uint16_t _addr );
/**
 * Read three byte from ADE7880
 * @param _dev: ADE7880 spi device
 * @param _data: 24bit variable for storage data, 32bit veriable where 8 significant bits are zero
 * @param _addr: 16bits register address
 * @return: ADE7880_OK if operation successful, or error code in overwise
 */
uint8_t ati_spi_ade7880_read24( struct ati_spi_ade7880* _dev, uint32_t* _data, uint16_t _addr );
/**
 * Read four byte from ADE7880
 * @param _dev: ADE7880 spi device
 * @param _data: 32bit variable for storage data
 * @param _addr: 16bits register address
 * @return: ADE7880_OK if operation successful, or error code in overwise
 */
uint8_t ati_spi_ade7880_read32( struct ati_spi_ade7880* _dev, uint32_t* _data, uint16_t _addr );

#endif //MGOS_SPI_ADE7880_H
