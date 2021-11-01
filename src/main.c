/**
 * работа с микрсхемой памяти at45db021d
 * https://www.farnell.com/datasheets/388169.pdf
 */
#include "mgos.h"
#include "mgos_spi.h"
#include "mgos_gpio.h"
#include "mgos_timers.h"
#include "mgos-spi-ade7880.h"
#include "mgos-spi-ade7880-registers.h"


static const uint8_t ROM_CS = 5;
static const uint8_t AD_CS = 22;
static const uint8_t ISOL_CS = 23;
static const uint8_t RESET_PIN = 15;
static const uint8_t RESET2_PIN = 4;
struct mgos_spi *spi;
struct ati_spi_ade7880* ade;


static int _step = 0;


void timer_cb_ad(void *arg) {
    ++_step;
    uint32_t data=0;
    ati_spi_ade7880_read32( ade, &data, CHECKSUM);
    LOG(LL_INFO, ("ADE7880 clock %d, CHECKSUM: %08X", _step, data) );

    ati_spi_ade7880_read32( ade, &data, STATUS0);
    LOG(LL_INFO, ("ADE7880 clock %d, STATUS0: %08X", _step, data) );

    ati_spi_ade7880_read32( ade, &data, STATUS1);
    LOG(LL_INFO, ("ADE7880 clock %d, STATUS1: %08X", _step, data) );

/*    uint32_t bdata[8];
    ati_spi_ade7880_read( ade, (uint8_t*)bdata, sizeof(bdata), AIRMS);
    LOG(LL_INFO, ("ADE7880 clock %d, AIRMS: %08X %08X %08X %08X %08X", _step, bdata[0], bdata[1], bdata[2], bdata[3], bdata[4] ) );
*/

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
