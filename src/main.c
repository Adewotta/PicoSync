#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/structs/systick.h"
#include "hardware/pio.h"
#include "pico/multicore.h"
#include "HardwareDefines.h"
#include "Systick.h"
#include "CSyncInjection/CSyncInjection.h"

//Time to wait for a missing pulse before inseting one ourselves, recorded in systick ticks
#define PULSE_LATE_THRESHOLD ( 10 * FREQ_MUL)

//How large a difference in the calculated average time from the set time has to be to update it recorded in systick ticks
#define UPDATE_THRESHOLD     (FREQ_MUL)

//How many samples to average for the timing between pulses
#define NUM_SAMPLES 4096

//Systick target for timing between pulses
volatile int systickTarget = 0x00ffffff;
volatile int timingArray[NUM_SAMPLES];
volatile int timingArrayIndex = 0;
volatile uint64_t timingArraySum = 0;
volatile bool timingArrayFull = false;


/** 
 * @brief Updates the timing array with a new sample.
 * @param sample The new timing sample to add to the array.
 */
void updateTimingArraySample(int sample){
    timingArraySum += sample;                        // add sample's value to sum
    timingArraySum -= timingArray[timingArrayIndex]; // subtract overwritten value from sum
    timingArray[timingArrayIndex++] = sample;        // overwrite value with sample
    if(timingArrayIndex == NUM_SAMPLES){
        timingArrayIndex = 0;
        timingArrayFull = true;
    }
}

/** 
 * @brief Calculates the average timing from the timing array.
 * @return The average timing value.
 */
int getAverageTicksBetweenPulses(){
    if(timingArrayFull) {
        return timingArraySum / NUM_SAMPLES;
    } else if(timingArrayIndex == 0) {
        return 0;
    } else {
        return timingArraySum / timingArrayIndex;
    }
}

#define VSYNC_PRESENT true
#define VSYNC_NOT_PRESENT false

/** 
 * @brief The main loop for core 1 handling timing adjustments and Shanks reset button
 */
void core1() {
    uint64_t startTime = 0;
    while(true){
        startTime = time_us_64();
        while(abs(time_us_64() - startTime) < 32000){
            tight_loop_contents();
        }


        //Calculate a new target timing based on the average time between pulses 
        int newTarget = getAverageTicksBetweenPulses() + PULSE_LATE_THRESHOLD;
        //If the new target timing is significantly different from the old timing, use the newly calculated target timing
        if(abs(newTarget - systickTarget) > UPDATE_THRESHOLD){
            systickTarget = newTarget;
        }
    }
}

/** 
 * @brief The main loop for core 0, handles recording sync pulses and injecting them if they're missing
 */
void RecordAndFixHSync(){
    uint32_t SystickTimeElapsed = 0x00FFFFFF;
    //Match the clock, and hold low, try to keep within systickTarget
    for(int i = 0; i < 4; i++){
        while(gpio_get(CSYNC_IN_PIN) == 0){
            //If Systick is out of range
            if(SYSTICK < systickTarget){
                SystickTimeElapsed -= (0xFFFFFF - SYSTICK);
                applyClock(0);
                reset_systick_timer(0x00ffffff);
            }
        };
    }
    int timer = 128 * FREQ_MUL;
    for(int i = 0; i < 4; i++){
        //While high
        while(gpio_get(CSYNC_IN_PIN) == 1){
            if(timer-- > 1){
                if(SYSTICK < systickTarget){
                    //Reset Systick timer, but keep track in SystickTimeElapsed
                    SystickTimeElapsed -= (0xFFFFFF - SYSTICK);
                    applyClock(1);
                    reset_systick_timer(0x00ffffff);
                }
            }
        }
    }
    SystickTimeElapsed -= (0xFFFFFF - SYSTICK);
    reset_systick_timer(0x00ffffff);
    updateTimingArraySample(SystickTimeElapsed);
}

/** 
 * @brief Initializes GPIO pins used for CSync, HSync, and VSync signals.
 */
void initializeGPIO(){
    //Initialise CSync/HSync input pin
    gpio_init(CSYNC_IN_PIN);
    gpio_set_dir(CSYNC_IN_PIN, GPIO_IN);
    gpio_set_pulls(CSYNC_IN_PIN,false,true);
    gpio_set_input_hysteresis_enabled(CSYNC_IN_PIN, true);

    //Initialize CSyncOutput pin
    gpio_init(CSYNC_OUT_PIN);
    gpio_set_dir(CSYNC_OUT_PIN, GPIO_OUT);
}

/** 
 * @brief Main function, initializes system components and enters the main loop.
 * @return Returns 0 upon completion.
 */
int main(void) {    
    //Set Clockspped
    set_sys_clock_khz(125000 * FREQ_MUL, true);
    //Setup and reset Systick Timer
    reset_systick_timer(0x00ffffff);
    
    //Initialize GPIO
    initializeGPIO();

    //Initialize PIOs
    initializeCSyncInjector(pio0);

    enableCSyncInjector();

    //Launce Second Core
    multicore_launch_core1(core1);

    reset_systick_timer(0x00ffffff);
    while(true){
        RecordAndFixHSync();
    }
    return 0;
}
