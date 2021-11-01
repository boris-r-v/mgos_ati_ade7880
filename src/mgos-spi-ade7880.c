//
// Created by boris on 26.10.2021.
//
#include "mgos_system.h"
#include "mgos_debug.h"
#include "mgos_gpio.h"
#include "mgos-spi-ade7880.h"
#include "mgos-spi-ade7880-impl.h"
#include "mgos-spi-ade7880-registers.h"
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
struct ati_spi_ade7880* ati_spi_ade7880_create(struct ati_spi_ade7880_config const* _conf){
    struct ati_spi_ade7880* c = (struct ati_spi_ade7880*) calloc(1, sizeof(*c));
    if (NULL != c ){
        c->reset_pin = _conf->reset_pin;
        c->cs_pin = _conf->cs_pin;
        c->isol_pin = _conf->isol_pin;
        c->spi = _conf->spi;
        c->txn.cs = -1; /*CS from over place*/
        c->txn.freq = _conf->freq;
        c->txn.mode = _conf->mode;

        mgos_gpio_set_mode(c->reset_pin, MGOS_GPIO_MODE_OUTPUT );
        mgos_gpio_set_mode(c->cs_pin, MGOS_GPIO_MODE_OUTPUT );
        mgos_gpio_set_mode(c->isol_pin, MGOS_GPIO_MODE_OUTPUT );
        mgos_gpio_write( c->isol_pin, 0 );
        mgos_gpio_write( c->cs_pin, 1 );
        /*Reset the ADE7880*/
        ati_spi_ade7880_reset( c );

        LOG (LL_INFO, ("UNS-PA SPI ADE7880 init ok (CS/RESET/ISOL: %d/%d/%d)",
                c->cs_pin, c->reset_pin, c->isol_pin ));
    }
    else{
        free (c);
        c = NULL;
        LOG (LL_ERROR, ("UNS-PA SPI ADE7880 init FAIL (CS/RESET/ISOL: %d/%d/%d)",
                _conf->cs_pin, _conf->reset_pin, _conf->isol_pin));
    }
    return c;
}
void ati_spi_ade7880_reset(struct ati_spi_ade7880* _dev ) {
    /*Reset*/
    mgos_gpio_write(_dev->reset_pin, 0);
    mgos_msleep(1);
    mgos_gpio_write(_dev->reset_pin, 1);
    /*Choose SPI mode on ADE7880 */
    int i;
    for (i=0; i<4; ++i) {
        mgos_gpio_write(_dev->cs_pin, 0);
        mgos_msleep(1);
        mgos_gpio_write(_dev->cs_pin, 1);
        mgos_msleep(1);
    }
    /*Write protect DSP registers*/
    ati_spi_ade7880_dps_reg_write_protection(_dev, true );
    /*Run DSP */
    ati_spi_ade7880_write16(_dev, 0x0001, RUN);
}

void  ati_spi_ade7880_destroy(struct ati_spi_ade7880* _dev){
    if ( NULL != _dev ){
        uint8_t is = _dev->isol_pin;
        uint8_t res = _dev->reset_pin;
        uint8_t cs = _dev->cs_pin;
        free(_dev );
        _dev = NULL;
        LOG (LL_DEBUG, ("UNS-PA SPI ADE7880 has being destroyed (CS/RESET/ISOL: %d/%d/%d)", cs, res, is) );
    }
    else
        LOG (LL_ERROR, ("UNS-PA SPI ADE7880 invalid destroy - device not exist!!" ) );
}
uint8_t ati_spi_ade7880_write_block(struct ati_spi_ade7880* _dev, uint8_t* _data, size_t _data_len, uint16_t _addr ){
    _dev->txn.cs = -1;

    mgos_gpio_write( _dev->isol_pin, 1 );
    mgos_gpio_write( _dev->cs_pin, 0 );
    mgos_usleep(10);
    uint8_t tx_data[3];
    tx_data[0] = 0x00;
    tx_data[1] = (_addr >> 8) & 0xff;
    tx_data[2] = _addr & 0xff;

    _dev->txn.hd.tx_len = sizeof( tx_data );
    _dev->txn.hd.tx_data = tx_data;
    _dev->txn.hd.dummy_len = 0;
    _dev->txn.hd.rx_len = _data_len;
    _dev->txn.hd.rx_data = _data;

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
uint8_t ati_spi_ade7880_write8(struct ati_spi_ade7880* _dev, uint8_t _data, uint16_t _addr ){
    return ati_spi_ade7880_write_block(_dev, &_data, sizeof(uint8_t), _addr );
}
uint8_t ati_spi_ade7880_write16(struct ati_spi_ade7880* _dev, uint16_t _data, uint16_t _addr ){
    return ati_spi_ade7880_write_block(_dev, (uint8_t*)(&_data), sizeof(uint16_t), _addr );
}
uint8_t ati_spi_ade7880_write32(struct ati_spi_ade7880* _dev, uint32_t _data, uint16_t _addr ){
    return ati_spi_ade7880_write_block(_dev, (uint8_t*)(&_data), sizeof(uint32_t), _addr );
}
uint8_t ati_spi_ade7880_read_block( struct ati_spi_ade7880* _dev, uint8_t* _data, size_t _data_len, uint16_t _addr ){
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
    _dev->txn.hd.rx_data = _data;

    uint8_t ret = ADE7880_OK;
    if (! mgos_spi_run_txn(_dev->spi, false/*half_duplex*/, &_dev->txn) )
    {
        LOG(LL_ERROR, ("ati_spi_ade7880_read_block") );
        ret = ADE7880_FAIL;
    }
    mgos_gpio_write( _dev->isol_pin, 0 );
    mgos_gpio_write( _dev->cs_pin, 1 );
    return ret;
}

uint8_t ati_spi_ade7880_read8( struct ati_spi_ade7880* _dev, uint8_t* _data, uint16_t _addr ){
    return ati_spi_ade7880_read_block( _dev, _data, sizeof(uint8_t), _addr );
}

uint8_t ati_spi_ade7880_read16( struct ati_spi_ade7880* _dev, uint16_t* _data, uint16_t _addr ){
    return ati_spi_ade7880_read_block( _dev, (uint8_t*)_data, sizeof(uint16_t), _addr );
}

uint8_t ati_spi_ade7880_read24( struct ati_spi_ade7880* _dev, uint32_t* _data, uint16_t _addr ){
    return ati_spi_ade7880_read_block( _dev, (uint8_t*)_data, sizeof(uint32_t), _addr );
}

uint8_t ati_spi_ade7880_read32( struct ati_spi_ade7880* _dev, uint32_t* _data, uint16_t _addr ){
    return ati_spi_ade7880_read_block( _dev, (uint8_t*)_data, sizeof(uint32_t), _addr );
}