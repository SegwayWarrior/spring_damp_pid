#include <stdint.h>
#include "utils/uartstdio.h"

uint16_t getSensorState(void){
// Gets the encoder count.  Blocks until there is a new count.
    uint8_t terminator = 0xFF;

    while(1){
        // Recieve encoder position data
        uint8_t encoderPositionLSB = UARTgetc();
        uint8_t encoderPositionMSB = UARTgetc();
        terminator = UARTgetc();

        if(terminator == 0){
            return encoderPositionLSB + (encoderPositionMSB << 8);
        }
    }
}

void sendTorqueCommand(uint16_t torque){
    char commandArray[] = {torque & 0xFF, torque >> 8, 0};
    UARTwrite(commandArray, 3);  // Write command with null terminator
}
