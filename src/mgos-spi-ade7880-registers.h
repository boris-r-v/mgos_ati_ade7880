//
// Created by boris on 26.10.2021.
//
/**
 * Driver for at45db021d
 * Bibliography:
 * [1] https://www.farnell.com/datasheets/388169.pdf
 */
#ifndef MGOS_SPI_ADE7880_REGISTERS_H
#define MGOS_SPI_ADE7800_REGISTERS_H
/**
 * Регистры ADE7880
 */
enum ADE_REGISTERS {
    /*Registers Located in DSP DATA Memory RAM*/
    AIGAIN=0x4380,      /*Phase A current gain adjust*/
    AVGAIN=0x4381,      /*Phase A voltage gain adjust*/
    BIGAIN=0x4382,      /*Phase B current gain adjust*/
    BVGAIN=0x4383,      /*Phase B voltage gain adjust*/
    CIGAIN=0x4384,      /*Phase C current gain adjust*/
    CVGAIN=0x4385,      /*Phase C voltage gain adjust*/
    NIGAIN=0x4386,       /*Neutral current gain adjust*/

    DICOEFF=0x4388,     /*Register used in the digital integration algo. It is transmitted 0xFFF8000, but must be 0xFF8000 if integrator is turned on*/

    AIRMSOS=0x438F,     /*Phase A current rms offset*/
    AVRMSOS=0x4390,     /*Phase A voltage rms offset*/
    BIRMSOS=0x4391,     /*Phase B current rms offset*/
    BVRMSOS=0x4392,     /*Phase B voltage rms offset*/
    CIRMSOS=0x4393,     /*Phase C current rms offset*/
    CVRMSOS=0x4394,     /*Phase C voltage rms offset*/
    NIRMSOS=0x4393,     /*Neutral current rms offset*/

    ISUMLVL=0x4399,     /*Threshold between the sum of phase currents and the neutral current */

    HXIRMSOS=0x43B4,    /*Current RMS offset on harmonic X*/
    HYIRMSOS=0x43B5,    /*Current RMS offset on harmonic Y*/
    HZIRMSOS=0x43B6,    /*Current RMS offset on harmonic Z*/

    HXVRMSOS=0x43B7,    /*Voltage RMS offset on harmonic X*/
    HYVRMSOS=0x43B8,    /*Voltage RMS offset on harmonic Y*/
    HZVRMSOS=0x43B9,    /*Voltage RMS offset on harmonic Z*/

    AIRMS=0x43C0,       /*Current RMS phase A*/
    AVRMS=0x43C1,       /*Voltage RMS phase A*/
    BIRMS=0x43C2,       /*Current RMS phase B*/
    BVRMS=0x43C3,       /*Voltage RMS phase B*/
    CIRMS=0x43C4,       /*Current RMS phase C*/
    CVRMS=0x43C5,       /*Voltage RMS phase C*/
    NIRMS=0x43C6,       /*Current RMS neutral*/
    NISUM=0x43C7,       /*Summ of IAWV+IBWV+ICWV*/

    /*Internal DSP Memory RAM Registers*/
    RUN=0xE228,          /*Run register starts or stop the DSP*/

    /*Configuration and Power Quality Registers*/
    IPEAK=0xE500,       /*Current PEAK register. See Fig. 58 and tab. 33*/
    VPEAK=0xE501,       /*Voltage PEAK register. See Fig. 58 and tab. 34*/

    STATUS0=0xE502,     /*Interrupt Status Register 0, R, See table 35*/
    STATUS1=0xE503,     /*Interrupt Status Register 1, R, See table 36*/

    AIMAV=0xE504,       /*Phase A current mean absolute value, R*/
    BIMAV=0xE505,       /*Phase B current mean absolute value, R*/
    CIMAV=0xE506,       /*Phase C current mean absolute value, R*/

    OILVL=0xE507,       /*OverCurrent threshold, R/W*/
    OVLVL=0xE508,       /*OverVoltage threshold, R/W*/
    SAGLVL=0xE509,      /*Voltage SAG level threshold, R/W*/

    MASK0=0xE50A,       /*Interrupt Enable Register 0, R/W, Tab.37*/
    MASK1=0xE50B,       /*Interrupt Enable Register 1, R/W, Tab.38*/
    
    CHECKSUM=0xE51F,     /*Checksum of configuration register, R*/

    LAST_RWDATA32=0xE5FF,   /*The data from last successful 32-bit register commutation,R */
    LAST_RWDATA16=0xE7FD,   /*The data from last successful 16-bit register commutation,R */
    LAST_RWDATA8=0xE7FD,    /*The data from last successful 8-bit register commutation,R */
    LAST_ADD=0xE9FE,        /*The address register from the last successful 16bit register communication*/
    PHSTATUS=0xE600,     /*Phase peak register, R,Tab.39 */

    ANGLE0=0xE601,      /*Time Delay 0, see Time Interval Between Phases, R */
    ANGLE1=0xE602,      /*Time Delay 1, see Time Interval Between Phases, R */
    ANGLE2=0xE603,      /*Time Delay 2, see Time Interval Between Phases, R */

    PHNOLOAD=0xE608,    /*Phase no load register, R, Tab. 40*/

    VTHDN=0xE886,       /*Total harmonic distortion plus noise of phase voltage, R*/
    ITHDN=0xE887,       /*Total harmonic distortion plus noise of phase current, R*/

    HXVRMS=0xE888,      /*The rms value of the phase voltage harmonic X, R*/
    HXIRMS=0xE889,      /*The rms value of the phase current harmonic X, R*/
    HXVHD=0xE88E,       /*Harmonic distortion of the phase voltage harmonic X relative to the fundamental, R*/
    HXIHD=0xE88F,       /*Harmonic distortion of the phase voltage harmonic X relative to the fundamental, R*/

    HYVRMS=0xE890,      /*The rms value of the phase voltage harmonic Y, R*/
    HYIRMS=0xE891,      /*The rms value of the phase current harmonic Y, R*/
    HYVHD=0xE896,       /*Harmonic distortion of the phase voltage harmonic X relative to the fundamental, R*/
    HYIHD=0xE897,       /*Harmonic distortion of the phase voltage harmonic X relative to the fundamental, R*/

    HZVRMS=0xE898,      /*The rms value of the phase voltage harmonic Z, R*/
    HZIRMS=0xE899,      /*The rms value of the phase current harmonic Z, R*/
    HZVHD=0xE89E,       /*Harmonic distortion of the phase voltage harmonic X relative to the fundamental, R*/
    HZIHD=0xE89F,       /*Harmonic distortion of the phase voltage harmonic X relative to the fundamental, R*/

    CONFIG2=0xEC01,     /*Configuration reg, R/W see Tab.55*/

    APERIOD=0xE905,     /*Line period of Phase A voltage, R, see Period Measurement */
    BPERIOD=0xE906,     /*Line period of Phase A voltage, R, see Period Measurement */
    CPERIOD=0xE907,     /*Line period of Phase A voltage, R, see Period Measurement */

    COMPMODE=0xE60E,    /**/


};

#endif //MGOS_SPI_ADE7880_REGISTERS_H
