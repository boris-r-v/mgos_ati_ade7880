//
// Created by boris on 26.10.2021.
//
#include "mgos_system.h"
#include "mgos_timers.h"
#include "mgos_debug.h"
#include "mgos_gpio.h"
#include "mgos-spi-ade7880.h"
#include "mgos-spi-ade7880-impl.h"
#include "mgos-uns-pa-data.h"
#include "mgos-uns-pa-sizing-algo.h"

#define _PERIOD_MS_ 10

static void ati_spi_ade7880_periodic( void* _ptr );

struct ati_spi_ade7880* ati_spi_ade7880_create(struct ati_spi_ade7880_config const* _conf){
    struct ati_spi_ade7880* c = (struct ati_spi_ade7880*) calloc(1, sizeof(*c));
    if (NULL != c ) {
        c->data = (struct ati_spi_ade7880_sizing_data*) calloc(1, sizeof(struct ati_spi_ade7880_sizing_data));
        memset(c->data, 0x00, sizeof(struct ati_spi_ade7880_sizing_data) );
    }
    if (NULL != c && NULL != c->data){
        c->reset_pin = _conf->reset_pin;
        c->cs_pin = _conf->cs_pin;
        c->isol_pin = _conf->isol_pin;
        c->spi = _conf->spi;
        c->coef = _conf->coef;
        c->txn.cs = -1; /*CS from over place*/
        c->txn.freq = _conf->freq;
        c->txn.mode = _conf->mode;

        mgos_gpio_set_mode(c->reset_pin, MGOS_GPIO_MODE_OUTPUT );
        mgos_gpio_set_mode(c->cs_pin, MGOS_GPIO_MODE_OUTPUT );
        mgos_gpio_set_mode(c->isol_pin, MGOS_GPIO_MODE_OUTPUT );
        mgos_gpio_write( c->isol_pin, 0 );
        mgos_gpio_write( c->cs_pin, 1 );
        /*Hard Reset the ADE7880*/
        ati_spi_ade7880_reset( c );

        LOG (LL_INFO, ("UNS-PA SPI ADE7880 init ok (CS/RESET/ISOL: %d/%d/%d)",
                c->cs_pin, c->reset_pin, c->isol_pin ));

        c->angle_seq = create_angle_sizing_algo();
        mgos_set_timer(_PERIOD_MS_, true, ati_spi_ade7880_periodic, c );
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
    /*Write config to DSP*/
    ati_spi_ade7880_dsp_write_config( _dev );
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

static void ati_spi_ade7880_periodic( void* _ptr ){
    struct ati_spi_ade7880* _dev = (struct ati_spi_ade7880*)_ptr;
    uint16_t run;
    ati_spi_ade7880_read16( _dev, &run, RUN);
    if ( 1 != run ){
        LOG (LL_ERROR, ("UNS-PA SPI ADE7880 not work - reset it" ) );
        ati_spi_ade7880_reset( _dev );
        mgos_msleep(2000);
    }

    float VGain = _dev->coef->Coefficients_esp[0];
    float VOffset = _dev->coef->Coefficients_esp[1];
    float IGain = _dev->coef->Coefficients_esp[2];
    float IOffset = _dev->coef->Coefficients_esp[3];

    _dev->data->Vrms[0] = ati_spi_ade7880_get_float( _dev, AVRMS) * VGain + VOffset;
    _dev->data->Vrms[1] = ati_spi_ade7880_get_float( _dev, BVRMS) * VGain + VOffset;
    _dev->data->Vrms[2] = ati_spi_ade7880_get_float( _dev, CVRMS) * VGain + VOffset;

    _dev->data->Irms[0] = ati_spi_ade7880_get_float( _dev, AIRMS) * IGain + IOffset;
    _dev->data->Irms[1] = ati_spi_ade7880_get_float( _dev, BIRMS) * IGain + IOffset;
    _dev->data->Irms[2] = ati_spi_ade7880_get_float( _dev, CVRMS) * IGain + IOffset;
    /*Read frequecy*/
    uint16_t tmp;
    ati_spi_ade7880_read16(_dev, &tmp, APERIOD );
    _dev->data->Freq[0] = 256000.0 / tmp;
    ati_spi_ade7880_read16(_dev, &tmp, BPERIOD );
    _dev->data->Freq[1] = 256000.0 / tmp;
    ati_spi_ade7880_read16(_dev, &tmp, CPERIOD );
    _dev->data->Freq[2] = 256000.0 / tmp;

    /*Handle Angle algo*/
    if ( _dev->angle_seq->func(_dev, _dev->angle_seq )){
        _dev->angle_seq = _dev->angle_seq->next;
    }
}

struct ati_spi_ade7880_sizing_data const* ati_spi_ade7880_get_data(struct ati_spi_ade7880* _dev ){
    return _dev->data;
}




/**-------------PRIVATE SECTION-------------*/

void ati_spi_ade7880_dsp_write_config( struct ati_spi_ade7880* _dev ){
    struct ati_spi_ade7880_calibration_data* c = _dev->coef;

    ati_spi_ade7880_write32( _dev, c->VGO_ade[0],  AVGAIN);
    ati_spi_ade7880_write32( _dev, c->VGO_ade[1],  BVGAIN);
    ati_spi_ade7880_write32( _dev, c->VGO_ade[2],  CVGAIN);
    ati_spi_ade7880_write32( _dev, c->VGO_ade[3],  AVRMSOS);
    ati_spi_ade7880_write32( _dev, c->VGO_ade[4],  BVRMSOS);
    ati_spi_ade7880_write32( _dev, c->VGO_ade[5],  CVRMSOS);

    ati_spi_ade7880_write32( _dev, c->IGO_ade[0],  AIGAIN);
    ati_spi_ade7880_write32( _dev, c->IGO_ade[1],  BIGAIN);
    ati_spi_ade7880_write32( _dev, c->IGO_ade[2],  CIGAIN);
    ati_spi_ade7880_write32( _dev, c->IGO_ade[3],  AIRMSOS);
    ati_spi_ade7880_write32( _dev, c->IGO_ade[4],  BIRMSOS);
    ati_spi_ade7880_write32( _dev, c->IGO_ade[5],  CIRMSOS);

    ati_spi_ade7880_write32( _dev, c->NIGO_ade[0],  NIGAIN);
    ati_spi_ade7880_write32( _dev, c->NIGO_ade[1],  NIRMSOS);

    ati_spi_ade7880_write32( _dev, c->Thresholds_ade[0],   OILVL);
    ati_spi_ade7880_write32( _dev, c->Thresholds_ade[1],   OVLVL);
    ati_spi_ade7880_write32( _dev, c->Thresholds_ade[2],   SAGLVL);

    /*Write ther times to frees pipeline*/
    ati_spi_ade7880_write32( _dev, c->Thresholds_ade[2],   SAGLVL);
    ati_spi_ade7880_write32( _dev, c->Thresholds_ade[2],   SAGLVL);
    ati_spi_ade7880_write32( _dev, c->Thresholds_ade[2],   SAGLVL);

}
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

static uint8_t ati_spi_ade7880_write_block(struct ati_spi_ade7880* _dev, void const* _data, size_t _data_len, uint16_t _addr );
static uint8_t ati_spi_ade7880_read_block( struct ati_spi_ade7880* _dev, void* _data, size_t _data_len, uint16_t _addr );

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

uint8_t ati_spi_ade7880_read32( struct ati_spi_ade7880* _dev, uint32_t* _data, uint16_t _addr ){
    return ati_spi_ade7880_read_block( _dev, _data, sizeof(uint32_t), _addr );
}
int32_t ati_spi_ade7880_get_int32( struct ati_spi_ade7880* _dev, uint16_t _addr ){
    uint32_t tmp;
    ati_spi_ade7880_read32( _dev, &tmp, _addr );
    return (int32_t)tmp;
}
uint32_t ati_spi_ade7880_get_uint32( struct ati_spi_ade7880* _dev, uint16_t _addr ){
    uint32_t tmp;
    ati_spi_ade7880_read32( _dev, &tmp, _addr );
    return tmp;
}
float ati_spi_ade7880_get_float( struct ati_spi_ade7880* _dev, uint16_t _addr ){
    uint32_t tmp;
    ati_spi_ade7880_read32( _dev, &tmp, _addr );
    return ((float)tmp)/0x200000;
}
uint16_t ati_spi_ade7880_get_uint16( struct ati_spi_ade7880* _dev, uint16_t _addr ){
    uint16_t tmp;
    ati_spi_ade7880_read16( _dev, &tmp, _addr );
    return tmp;
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
