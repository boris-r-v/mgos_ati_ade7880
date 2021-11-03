//
// Created by boris on 03.11.2021.
//

#include "mgos_system.h"
#include "mgos_debug.h"

#include "mgos-uns-pa-sizing-algo.h"
#include "mgos-spi-ade7880-impl.h"
#include "mgos-uns-pa-data.h"

struct state* create_state( bool(*_func)(struct ati_spi_ade7880* _dev, struct state* _state), uint16_t _max, struct state* next ){
    struct state* s = (struct state*) calloc(1, sizeof(*s));
    s->func = _func;
    s->cntr = 0;
    s->max = _max;
    s->next = next;
    return s;
}
/**-------------ANGLE SIZING ALGO----------------*/

static float uint2angle(uint16_t angle ){
    /**see section Time Interval Between Phases
     * 0.0703125=360Grad*50Hz/256000Hz
     * 0.084375=360Grad*60Hz/256000Hz
     * FIXME: we need read SELFREQ bit (14-bit) from COMPMODE (0xE60E) register and choose proper system frequency
     */
    return ((float)angle)*0.0703125;
}
static bool set_voltage_to_voltage_angels(struct ati_spi_ade7880* _dev, struct state* _state ){
    uint16_t compmode;
    ati_spi_ade7880_read16(_dev, &compmode, COMPMODE);
    compmode &= 0xF9FF;
    compmode |= 0x200;
    LOG(LL_DEBUG, ("set V2V angle compmode: %02X", compmode ) );
    ati_spi_ade7880_write16(_dev, compmode, COMPMODE);
    _state->next->cntr = 0;     /*Drop cntr at the next wait state*/
    return true;
}
static bool get_voltage_to_voltage_angels(struct ati_spi_ade7880* _dev, struct state* _state ){
    LOG(LL_DEBUG, ("get V2V angle" ) );

    _dev->data->Angle_v2v[0] = uint2angle(ati_spi_ade7880_get_uint16(_dev, ANGLE0) );
    _dev->data->Angle_v2v[1] = uint2angle(ati_spi_ade7880_get_uint16(_dev, ANGLE1) );
    _dev->data->Angle_v2v[1] = uint2angle(ati_spi_ade7880_get_uint16(_dev, ANGLE1) );

    return true;
}
static bool set_voltage_to_current_angels(struct ati_spi_ade7880* _dev, struct state* _state ){
    uint16_t compmode;
    ati_spi_ade7880_read16(_dev, &compmode, COMPMODE);
    compmode &= 0xF9FF;
    LOG(LL_DEBUG, ("set V2C angle compmode: %02X", compmode ) );
    ati_spi_ade7880_write16(_dev, compmode, COMPMODE);
    _state->next->cntr = 0;     /*Drop cntr at the next wait state*/
    return true;
}
static bool get_voltage_to_current_angels(struct ati_spi_ade7880* _dev, struct state* _state ){
    LOG(LL_DEBUG, ("get V2C angle " ) );
    _dev->data->Angle_v2c[0] = uint2angle(ati_spi_ade7880_get_uint16(_dev, ANGLE0) );
    _dev->data->Angle_v2c[1] = uint2angle(ati_spi_ade7880_get_uint16(_dev, ANGLE1) );
    _dev->data->Angle_v2c[1] = uint2angle(ati_spi_ade7880_get_uint16(_dev, ANGLE1) );

    return true;
}
static bool wait_tic(struct ati_spi_ade7880* _dev, struct state* _state){
    LOG(LL_DEBUG, ("wait_tic cntr: %d/%d", _state->cntr, _state->max ) );
    if (++_state->cntr > _state->max )
        return true;
    return false;
}
#define _WAIT_ 20
struct state* create_angle_sizing_algo(){

    struct state* get_v2v = create_state(&get_voltage_to_voltage_angels, 0, NULL);
    struct state* wait_v2v = create_state(&wait_tic, _WAIT_, get_v2v);
    struct state* set_v2v = create_state(&set_voltage_to_voltage_angels, 0, wait_v2v);

    struct state* get_v2c = create_state(&get_voltage_to_current_angels, 0, set_v2v );
    struct state* wait_v2c = create_state(&wait_tic, _WAIT_, get_v2c);
    struct state* set_v2c = create_state(&set_voltage_to_current_angels, 0, wait_v2c );
    get_v2v->next = set_v2c;

    return set_v2c;
}

/**-------------GARMONIC DISTORSHIN COEF SIZING ALGO----------------*/