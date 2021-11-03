/**
 * работа с микрсхемой памяти at45db021d
 * https://www.farnell.com/datasheets/388169.pdf
 */
#include "mgos.h"
#include "mgos_spi.h"
#include "mgos_gpio.h"
#include "mgos_timers.h"
#include "mgos-spi-ade7880.h"
#include "mgos-uns-pa-data.h"

static const uint8_t ROM_CS = 5;
static const uint8_t AD_CS = 22;
static const uint8_t ISOL_CS = 23;
static const uint8_t RESET_PIN = 15;
static const uint8_t RESET2_PIN = 4;

struct mgos_spi *spi;
struct ati_spi_ade7880* ade;

void print_unspa_data(struct ati_spi_ade7880_sizing_data const* _d ){
    LOG(LL_INFO, ("-------------------------------" ) );
    LOG(LL_INFO, ("VRMS A:%0.3f, B:%0.3f, C:%0.3f ", _d->Vrms[0], _d->Vrms[1], _d->Vrms[2] ) );
    LOG(LL_INFO, ("IRMS A:%0.3f, B:%0.3f, C:%0.3f ", _d->Irms[0], _d->Irms[1], _d->Irms[2] ) );
    LOG(LL_INFO, ("Freq A:%0.3f, B:%0.3f, C:%0.3f ", _d->Freq[0], _d->Freq[1], _d->Freq[2] ) );
    LOG(LL_INFO, ("V2C A:%0.3f, B:%0.3f, C:%0.3f ", _d->Angle_v2c[0], _d->Angle_v2c[1], _d->Angle_v2c[2] ) );
    LOG(LL_INFO, ("V2V A:%0.3f, B:%0.3f, C:%0.3f ", _d->Angle_v2v[0], _d->Angle_v2v[1], _d->Angle_v2v[2] ) );
    LOG(LL_INFO, ("-------------------------------" ) );

}
void print_system_usage(){
    LOG(LL_INFO, ("SystemUsage: heap(total/free):%d/%d, fs(total/free):%d/%d",
            mgos_get_heap_size(),
            mgos_get_free_heap_size(),
            mgos_get_fs_size(),
            mgos_get_free_fs_size()
            ) );
}
void timer_cb_ad(void *arg) {
/*
    ++_step;
    uint32_t data=0;
    ati_spi_ade7880_read32( ade, &data, CHECKSUM);
    LOG(LL_INFO, ("ADE7880 clock %d, CHECKSUM: %08X", _step, data) );

    ati_spi_ade7880_read32( ade, &data, STATUS0);
    LOG(LL_INFO, ("ADE7880 clock %d, STATUS0: %X", _step, data) );

    ati_spi_ade7880_read32( ade, &data, STATUS1);
    LOG(LL_INFO, ("ADE7880 clock %d, STATUS1: %X", _step, data) );

    uint16_t run;
    ati_spi_ade7880_read16( ade, &run, 0xE228);
    LOG(LL_INFO, ("ADE7880 clock %d, RUN: %X", _step, run) );

    uint32_t arms=0, brms=0, crms=0;
    ati_spi_ade7880_read32( ade, &arms, AVRMS);
    ati_spi_ade7880_read32( ade, &brms, BVRMS);
    ati_spi_ade7880_read32( ade, &crms, CVRMS);
    LOG(LL_INFO, ("ADE7880 READ %d, RMS AV: %X, BV: %X, CV: %X", _step, arms, brms, crms ) );
*/
    print_unspa_data( ati_spi_ade7880_get_data( ade ) );

    print_system_usage();
}

enum mgos_app_init_result mgos_app_init(void) 
{
    /*GPIO configure*/
    mgos_gpio_set_mode(RESET_PIN, MGOS_GPIO_MODE_OUTPUT );
    mgos_gpio_set_mode(ROM_CS, MGOS_GPIO_MODE_OUTPUT );
    mgos_gpio_write( ROM_CS, 1 );
    /*SPI configure*/
    struct mgos_config_spi bus_cfg = {
        .unit_no = 2,
        .miso_gpio = 21,
        .mosi_gpio = 19,
        .sclk_gpio = 18,
        .cs0_gpio = -1,
        .cs1_gpio = -1,
        .cs2_gpio = -1,
        .debug = true,
    };
    spi = mgos_spi_create(&bus_cfg);
    if ( spi == NULL) 
    {
        LOG(LL_ERROR, ("Bus init failed"));
        return MGOS_APP_INIT_ERROR;
    }
    /*falsh configure*/
    struct ati_spi_ade7880_config fl = {
            .reset_pin = RESET2_PIN,
            .cs_pin = AD_CS,
            .isol_pin = ISOL_CS,
            .spi = spi,
            .coef = create_call_coef(),
            .mode = 0,
            .freq = 1000000,    /*1MHz ask freq*/
    };
    ade = ati_spi_ade7880_create( &fl );
    if ( ade == NULL)
    {
        LOG(LL_ERROR, ("ADE7880 init failed"));
        return MGOS_APP_INIT_ERROR;
    }

    mgos_set_timer(1000, true, timer_cb_ad, NULL);

    return MGOS_APP_INIT_SUCCESS;
}

