/**
 * Lab2 Task2 for ECE4550 Fall 2018
 *
 * Flash RED LED from Flash
 *
 * Authors: Marion Anderson, Aditya Retnanto
 * Date: 2018-09-06
 */

#include "F2837xD_device.h"

// Code from prelab
extern Uint16 RamFuncs_loadstart;
extern Uint16 RamFuncs_loadsize;
extern Uint16 RamFuncs_runstart;
#pragma CODE_SECTION(InitFlash, "RamFuncs")
void InitFlash(void)
{
    EALLOW;
    Flash0CtrlRegs.FPAC1.bit.PMPPWR = 0x1;
    Flash0CtrlRegs.FBFALLBACK.bit.BNKPWR0 = 0x3;
    Flash0CtrlRegs.FRD_INTF_CTRL.bit.DATA_CACHE_EN = 0;
    Flash0CtrlRegs.FRD_INTF_CTRL.bit.PREFETCH_EN = 0;
    Flash0CtrlRegs.FRDCNTL.bit.RWAIT = 0x3;
    Flash0CtrlRegs.FRD_INTF_CTRL.bit.DATA_CACHE_EN = 1;
    Flash0CtrlRegs.FRD_INTF_CTRL.bit.PREFETCH_EN = 1;
    Flash0EccRegs.ECC_ENABLE.bit.ENABLE = 0xA;
    EDIS;
    asm(" RPT #6 || NOP");
}

unsigned long loop_count = 0;  // loop counter
int main(void)
{
    // More from prelab
    // move program memory between flash and ram
    // put this at start of main
    memcpy(&RamFuncs_runstart, &RamFuncs_loadstart, (Uint32) &RamFuncs_loadsize);
    InitFlash();

    // Register setup
    EALLOW;
    WdRegs.WDCR.all = 0x68;

    // GPIO Setup
    // disable blue led
    GpioCtrlRegs.GPAGMUX2.bit.GPIO31 = 0;  // gpio output
    GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO31 = 1;    // output
    GpioCtrlRegs.GPAPUD.bit.GPIO31 = 0;     // enable pull-up resistor
    GpioDataRegs.GPASET.bit.GPIO31 = 1;

    // set up red led
    GpioCtrlRegs.GPBGMUX1.bit.GPIO34 = 0;  // gpio output
    GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;    // output
    GpioCtrlRegs.GPBPUD.bit.GPIO34 = 0;     // enable pull-up resistor
    GpioDataRegs.GPBSET.bit.GPIO34 = 1;

    WdRegs.WDCR.all = 0x28;
    EDIS;

    while (1) {
        // check to toggle led
        if (loop_count % 100000 == 0){
            GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;
        }
        loop_count++;

        // watchdog timer reset
        WdRegs.WDKEY.all = 0x55;
        WdRegs.WDKEY.all = 0xAA;
    }
}
