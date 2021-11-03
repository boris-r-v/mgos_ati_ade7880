
//
// Created by boris on 01.11.2021.
//

#ifndef MGOS_SPI_ADE7880_IMPL_H
#define MGOS_SPI_ADE7880_IMPL_H
#include <stdint.h>
#include <stdbool.h>
#include "mgos_spi.h"
#include "mgos-spi-ade7880-registers.h"

struct ati_spi_ade7880_calibration_data;
struct ati_spi_ade7880{
    /**
     * Тоже что и в struct ati_spi_flash_conf
     */
    uint8_t reset_pin;
    /**
     * Тоже что и в struct ati_spi_flash_conf
     */
    uint8_t cs_pin;
    /**
     * Тоже что и в struct ati_spi_flash_conf
     */
    uint8_t isol_pin;
    /**
    * Тоже что и в struct ati_spi_flash_conf
    */
    struct mgos_spi* spi;
    /**
     * Структура запроса к SPI device, see   spi/include/mgos_spi.h
     */
    struct mgos_spi_txn txn;
    /**
    * Указательна структуру с данными калибровки, данный указатель должен быть валиден всегда
    */
    struct ati_spi_ade7880_calibration_data* coef;
};
/**
 * Write gain, offset, thresholds dsp registers
 * @param _dev: ADE7880 spi device
 */
void ati_spi_ade7880_dsp_write_config( struct ati_spi_ade7880* _dev );
/**
 * Set enable or disable write protection for ADE7880 calibration registers
 * @param _dev: ADE7880 spi device
 * @param _on_off: - if TRUE - protection is ON, FALSE - id OFF
 */
void ati_spi_ade7880_dps_reg_write_protection(struct ati_spi_ade7880* _dev, bool _on_off);

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
 * Read four byte from ADE7880
 * @param _dev: ADE7880 spi device
 * @param _data: 32bit variable for storage data
 * @param _addr: 16bits register address
 * @return: ADE7880_OK if operation successful, or error code in overwise
 */
uint8_t ati_spi_ade7880_read32( struct ati_spi_ade7880* _dev, uint32_t* _data, uint16_t _addr );
/**
 * Get four byte signed number from register
 * @param _dev: ADE7880 spi device
 * @param _addr: Register address
 * @return: Value from register in signed format
 */
int32_t ati_spi_ade7880_get_int32( struct ati_spi_ade7880* _dev, uint16_t _addr );
/**
 * Get four byte unsigned number from register
 * @param _dev: ADE7880 spi device
 * @param _addr: Register address
 * @return: Value from register in signed format
 */
uint32_t ati_spi_ade7880_get_uint32( struct ati_spi_ade7880* _dev, uint16_t _addr );
/**
 *
 * @param _dev
 * @param _addr
 * @return
 */
float ati_spi_ade7880_get_float( struct ati_spi_ade7880* _dev, uint16_t _addr );



#endif //MGOS_SPI_ADE7880_IMPL_H
