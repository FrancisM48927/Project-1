/********************************************************************** 
* Author: F. MAILOM
* CPEG222 Project 1, 9/14/26
* NucleoF466ZE CMSIS Sequence Nucleo LEDs with USR BTN
**********************************************************************/
#include "stm32f4xx.h"
#include <stdbool.h>

#define GREEN_LED 0
#define BLUE_LED 7
#define RED_LED 14
#define USER_BTN 13

bool BTN_LOCK = false;
unsigned int LED_STATUS = 0;

void Delay_Count(volatile uint32_t count) 
{
    while (count--) 
    {
    }
}

int main(void) 
{
    // Enables clock for GPIOB & GPIOC
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN;

    // Clear PB0, PB7, and PB14
    // 0x3 (or 0011 in binary) shifted to the location specified by 2 x (pin number) and inverted w/ ~
    GPIOB->MODER &= ~((0x3 << (GREEN_LED * 2)) | 
                      (0x3 << (BLUE_LED * 2)) | 
                      (0x3 << (RED_LED * 2)));
    // Configure PB0, PB7, and PB14 as outputs
    // 0x1 (or 0001 in binary) shifted to the location specified by 2 x (pin number)
    GPIOB->MODER |= ((0x1 << (GREEN_LED * 2)) | 
                     (0x1 << (BLUE_LED * 2)) | 
                     (0x1 << (RED_LED * 2)));

    // Clear PC13 w/ same method as before (also sets it to input)
    GPIOC->MODER &= ~(0x3 << (USER_BTN * 2));

    // Initialize all colors
    GPIOB->BSRR = (0x1 << (GREEN_LED + 16)); // Green off
    GPIOB->BSRR = (0x1 << (BLUE_LED + 16)); // Blue off
    GPIOB->BSRR = (0x1 << (RED_LED)); // Red on

    while(1) // Loops indefinitely
    {
        if ((GPIOC->IDR & (0x1 << USER_BTN)) && !BTN_LOCK) 
        { // Debounce check if button is active (button is active-high) and lock is off
            // Primary functionality for cycling through colors on clock press
            if (LED_STATUS == 0) // If red is on proceed by turning off red and turning on green
            {
                GPIOB->BSRR = (0x1 << (RED_LED + 16)); // Red off
                GPIOB->BSRR = (0x1 << (GREEN_LED)); // Green on
            }
            else if (LED_STATUS == 1)  // If green is on proceed by turning off green and turning on blue
            {
                GPIOB->BSRR = (0x1 << (GREEN_LED + 16)); // Green off
                GPIOB->BSRR = (0x1 << (BLUE_LED)); // Blue on
            }
            else if (LED_STATUS == 2)  // If blue is on proceed by turning off blue and turning on red
            {
                GPIOB->BSRR = (0x1 << (BLUE_LED + 16)); // Blue off
                GPIOB->BSRR = (0x1 << (RED_LED)); // Red on
            }
            LED_STATUS++; // Increment LED status
            // Check led status, reset if greater than 2
            if (LED_STATUS > 2) 
            {
                LED_STATUS = 0;
            }
            // Activate debounce feature
            BTN_LOCK = true;
        }
        else if (!(GPIOC->IDR & (0x1 << USER_BTN)) && BTN_LOCK) 
        { // Debounce check if button is not active lock is on
            Delay_Count(267267);
            BTN_LOCK = false;
        }
    }

    return(0);
}