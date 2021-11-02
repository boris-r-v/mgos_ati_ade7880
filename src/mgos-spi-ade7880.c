//
// Created by boris on 26.10.2021.
//
#include "mgos_system.h"
#include "mgos_debug.h"
#include "mgos_gpio.h"
#include "mgos-spi-ade7880.h"
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
    /*Confirm SPI mode on ADE7880. detail see: */
    ati_spi_ade7880_write8(_dev, 0x00, CONFIG2);

    /*Check RSTDONE bit in STATUS1 reg*/
    uint32_t rstdone=0;
    ati_spi_ade7880_read32(_dev, &rstdone, STATUS1);
    while( 0 == (rstdone & (0x01<<15)) ){
        LOG (LL_INFO, ("UNS-PA SPI ADE7880 wait for reset complete" ));
        ati_spi_ade7880_read32(_dev, &rstdone, STATUS1);
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
