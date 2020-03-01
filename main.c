/* 
 * File:     main.c
 * Author:   Christopher Madrigal
 * Modified: 20 February 2020
 */

/**
 * Section: Included Files
 */
#include "mcc_generated_files/system.h"
#include "gps.h"
#include "lora.h"
#include <stdio.h>

/*
                         Main application
 */
char payload[BUFFER_SIZE];

int main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    
    while( 1 ) {
        // TODO main program
    }

    return 1;
}
/**
 End of File
*/