#pragma 
#include <stdint.h>
#include <stdbool.h>
#include "hardware/pio.h"
#include "../HardwareDefines.h"

typedef void (*func_ptr)(void);

void initializeCSyncInjector(PIO pio);

void enableCSyncInjector(void);

void disableCSyncInjector(void);
void initializePIOs();
void applyClock(bool lastState);
void CombineSyncs(void);


func_ptr getCSyncInjector(void);