

/**
 * main.c
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"

#include "utils/uartstdio.h"
#include "custom_io.h"


void UART_setup(void){
    // Uart set up
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    UARTStdioConfig(0, 115200, 16000000);
}

int main(void)
{
    UART_setup();
    const float kP = 22;
    const float kD = 8;
    const float kI = 1;

    int16_t setpoint = 1023;

    uint16_t prevError = 0;

    int16_t cycleIntegral = 0; // Integral summation

    int16_t maxIntegralSum = 30000;  // Pick a maximum value for the integral sum

    int16_t encoderPosition = 0; // Set initial value to kick start controller

    while(1){

        encoderPosition = getSensorState(); //Get sensor state, blocking

        int16_t error = setpoint - encoderPosition; //Compute error

        // Set the integral sum limits
        cycleIntegral = cycleIntegral + error;
        if (cycleIntegral > maxIntegralSum){
            cycleIntegral = maxIntegralSum;
        }
        if (cycleIntegral < -1*maxIntegralSum){
            cycleIntegral = -1*maxIntegralSum;
        }


        // Control effort in mNm
        int16_t controlEffort = (setpoint - encoderPosition) * kP + (error - prevError) * kD + cycleIntegral*kI;

        prevError = error;

        sendTorqueCommand(controlEffort); // Send torque command to motor controller
    }
}
