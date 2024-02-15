#include <stdint.h>
#include <stdbool.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"

#include "CSyncInjection.h"
#include "CSync.pio.h"

#include "../HardwareDefines.h"

PIO CSyncInjectionPIO;
uint CSyncInjectionProgramOffset;
uint CSyncInjectionSm;

void initializeCSyncInjector(PIO pio){
    CSyncInjectionPIO = pio;
       //Get PIO Offsets
    CSyncInjectionProgramOffset = pio_add_program(CSyncInjectionPIO, &CSyncFix_program);
    //Get State machines index for each PIO program
    CSyncInjectionSm  = pio_claim_unused_sm(CSyncInjectionPIO, true);
}

void enableCSyncInjector(void){
    CSyncFix_program_init(CSyncInjectionPIO, CSyncInjectionSm, CSyncInjectionProgramOffset, HSYNC_IN_PIN, CSYNC_OUT_PIN);
}

void disableCSyncInjector(void){
    pio_sm_set_enabled(CSyncInjectionPIO, CSyncInjectionSm, false);
}

/** 
 * @brief Applies a clock signal to fill in missing H-Sync pulses
 * @param lastState The last state of the clock signal to toggle from.
 */
void applyClock(bool lastState){     
    pio_sm_exec(CSyncInjectionPIO, CSyncInjectionSm,0xc000);//Raise Flag      
    pio_sm_exec(CSyncInjectionPIO, CSyncInjectionSm,0xe000 | !lastState);  //  2: set    pins, 1     
    busy_wait_at_least_cycles(125 * FREQ_MUL);   
    pio_sm_exec(CSyncInjectionPIO, CSyncInjectionSm,0xe000 | lastState); //  2: set    pins, 0  
    busy_wait_at_least_cycles(125 * FREQ_MUL);   
    pio_sm_exec(CSyncInjectionPIO, CSyncInjectionSm,0xc040);//Lower Flag
}

void CombineSyncs(void);

func_ptr getCSyncInjector(void){
    return CombineSyncs;
}