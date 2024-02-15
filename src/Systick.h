#pragma once

#define SYSTICK systick_hw->cvr
#define SYSTICK_RELOAD systick_hw->rvr
#define SYSTICK_CONTROL systick_hw->csr
/** 
 * @brief Resets the SysTick timer with a new reload value.
 * @param reload_value The new reload value for the SysTick timer.
 */
static inline void reset_systick_timer(uint32_t reload_value) {
    // Disable SysTick timer
    SYSTICK_CONTROL &= ~0x5;
    // Set reload value
    SYSTICK_RELOAD = reload_value;
    // Reset the current value
    SYSTICK = 0;
    // Enable SysTick timer
    SYSTICK_CONTROL |= 0x5;
}