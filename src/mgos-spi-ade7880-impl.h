
//
// Created by boris on 01.11.2021.
//

#ifndef MGOS_SPI_ADE7880_IMPL_H
#define MGOS_SPI_ADE7880_IMPL_H

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
};
/**
 *
 * @param _dev:
 * @param _on_off: - if TRUE - protection is ON, FALSE - id OFF
 */
void ati_spi_ade7880_dps_reg_write_protection(struct ati_spi_ade7880* _dev, bool _on_off);

/**
 * Write some data block to ADE7880
 * @param _dev: ADE7880 spi device
 * @param _data: storage for read data
 * @param _data_len: read data len
 * @param _addr: 16bits register address
 * @return: ADE7880_OK if operation successful, or error code in overwise
 */
uint8_t ati_spi_ade7880_write_block(struct ati_spi_ade7880* _dev, uint8_t* _data, size_t _data_len, uint16_t _addr );

/**
 * Read some data block from ADE7880
 * @param _dev: ADE7880 spi device
 * @param _data: storage for read data
 * @param _data_len: read data len
 * @param _addr: 16bits register address
 * @return: ADE7880_OK if operation successful, or error code in overwise
 */
uint8_t ati_spi_ade7880_read_block( struct ati_spi_ade7880* _dev, uint8_t* _data, size_t _data_len, uint16_t _addr );


#endif //MGOS_SPI_ADE7880_IMPL_H
