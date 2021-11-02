//
// Created by boris on 26.10.2021.
//
#include "mgos_system.h"
#include "mgos_debug.h"
#include "mgos_gpio.h"
#include "mgos-spi-ade7880-impl.h"
static uint8_t ati_spi_ade7880_write_block(struct ati_spi_ade7880* _dev, void const* _data, size_t _data_len, uint16_t _addr );
static uint8_t ati_spi_ade7880_read_block( struct ati_spi_ade7880* _dev, void* _data, size_t _data_len, uint16_t _addr );


void ati_spi_ade7880_dps_reg_write_protection(struct ati_spi_ade7880* _dev, bool _on_off){
    if ( _on_off){
        ati_spi_ade7880_write8( _dev, 0xAD, 0xE7FE );
        ati_spi_ade7880_write8( _dev, 0x80, 0xE7E3 );
    }
    else{
        ati_spi_ade7880_write8( _dev, 0xAD, 0xE7FE );
        ati_spi_ade7880_write8( _dev, 0x00, 0xE7E3 );
    }
}

uint8_t ati_spi_ade7880_write8(struct ati_spi_ade7880* _dev, uint8_t _data, uint16_t _addr ){
    return ati_spi_ade7880_write_block(_dev, &_data, sizeof(uint8_t), _addr );
}
uint8_t ati_spi_ade7880_write16(struct ati_spi_ade7880* _dev, uint16_t _data, uint16_t _addr ){
    uint16_t r = _data>>8 | (_data &0xFF) << 8;
    return ati_spi_ade7880_write_block(_dev, &r, sizeof(uint16_t), _addr );
}
uint8_t ati_spi_ade7880_write32(struct ati_spi_ade7880* _dev, uint32_t _data, uint16_t _addr ){
    uint32_t r = ((_data & 0xFF) << 24)|((_data & 0xFF00) << 8)|((_data & 0xFF0000) >> 8)|((_data & 0xFF000000) >> 24);
    return ati_spi_ade7880_write_block(_dev, &r, sizeof(uint32_t), _addr );
}

uint8_t ati_spi_ade7880_read8( struct ati_spi_ade7880* _dev, uint8_t* _data, uint16_t _addr ){
    return ati_spi_ade7880_read_block( _dev, _data, sizeof(uint8_t), _addr );
}

uint8_t ati_spi_ade7880_read16( struct ati_spi_ade7880* _dev, uint16_t* _data, uint16_t _addr ){
    return ati_spi_ade7880_read_block( _dev, _data, sizeof(uint16_t), _addr );
}

uint8_t ati_spi_ade7880_read24( struct ati_spi_ade7880* _dev, uint32_t* _data, uint16_t _addr ){
    return ati_spi_ade7880_read_block( _dev, _data, sizeof(uint32_t), _addr );
}

uint8_t ati_spi_ade7880_read32( struct ati_spi_ade7880* _dev, uint32_t* _data, uint16_t _addr ){
    return ati_spi_ade7880_read_block( _dev, _data, sizeof(uint32_t), _addr );
}



/**
 * Write some data block to ADE7880
 * @param _dev: ADE7880 spi device
 * @param _data: storage for read data
 * @param _data_len: read data len
 * @param _addr: 16bits register address
 * @return: ADE7880_OK if operation successful, or error code in overwise
 */

static uint8_t ati_spi_ade7880_write_block(struct ati_spi_ade7880* _dev, void const* _data, size_t _data_len, uint16_t _addr ){
    _dev->txn.cs = -1;

    mgos_gpio_write( _dev->isol_pin, 1 );
    mgos_gpio_write( _dev->cs_pin, 0 );
    mgos_usleep(10);

    uint8_t tx_data[3 + _data_len];
    tx_data[0] = 0x00;
    tx_data[1] = (_addr >> 8) & 0xff;
    tx_data[2] = _addr & 0xff;
    memcpy(tx_data+3, _data, _data_len );

    _dev->txn.hd.tx_len = sizeof( tx_data );
    _dev->txn.hd.tx_data = tx_data;
    _dev->txn.hd.dummy_len = 0;
    _dev->txn.hd.rx_len = 0;
    _dev->txn.hd.rx_data = NULL;

    uint8_t ret = ADE7880_OK;
    if (! mgos_spi_run_txn(_dev->spi, false/*half_duplex*/, &_dev->txn) )
    {
        LOG(LL_ERROR, ("ati_spi_ade7880_write_block") );
        ret = ADE7880_FAIL;
    }
    mgos_gpio_write( _dev->isol_pin, 0 );
    mgos_gpio_write( _dev->cs_pin, 1 );
    return ret;
}
/**
 * Read some data block from ADE7880
 * @param _dev: ADE7880 spi device
 * @param _data: storage for read data
 * @param _data_len: read data len
 * @param _addr: 16bits register address
 * @return: ADE7880_OK if operation successful, or error code in overwise
 */
static uint8_t ati_spi_ade7880_read_block( struct ati_spi_ade7880* _dev, void* _data, size_t _data_len, uint16_t _addr ){
    _dev->txn.cs = -1;

    mgos_gpio_write( _dev->isol_pin, 1 );
    mgos_gpio_write( _dev->cs_pin, 0 );
    mgos_usleep(10);
    uint8_t tx_data[3];
    tx_data[0] = 0x01;
    tx_data[1] = (_addr >> 8) & 0xff;
    tx_data[2] = _addr & 0xff;

    _dev->txn.hd.tx_len = sizeof( tx_data );
    _dev->txn.hd.tx_data = tx_data;
    _dev->txn.hd.dummy_len = 0;
    _dev->txn.hd.rx_len = _data_len;
    uint8_t rx_data[_data_len];
    _dev->txn.hd.rx_data = rx_data;

    uint8_t ret = ADE7880_OK;
    if (! mgos_spi_run_txn(_dev->spi, false/*half_duplex*/, &_dev->txn) )
    {
        LOG(LL_ERROR, ("ati_spi_ade7880_read_block") );
        ret = ADE7880_FAIL;
    }
    mgos_gpio_write( _dev->isol_pin, 0 );
    mgos_gpio_write( _dev->cs_pin, 1 );
    /**
     * Data shifts into the ADE7880 at the MOSI logic input on the falling edge of SCLK and the ADE7880 samples it on the rising edge of SCLK.
     * Data shifts out of the ADE7880 at the MISO logic output on a falling edge of SCLK and can be sampled by the master device on the raising edge of SCLK.
     * The most significant bit of the word is shifted in and out first.
     * The maximum serial clock frequency supported by this interface is 2.5 MHz.
     * Т.к. esp32 - это little-endian - то нужно перевернуть полученные байты
     */
    int i;
    uint8_t* r = _data;
    for (i=0; i < _data_len; ++i){
        *(r+i) = rx_data[_data_len-1-i];
    }
    return ret;
}
/*-----------------*/
