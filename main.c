/* 
 * File:     main.c
 * Author:   Christopher Madrigal
 * Modified: 20 February 2020
 */

/**
 * Section: Included Files
 */
#include "mcc_generated_files/system.h"
#include "i2c.h"
#include "gps.h"
#include "spi.h"
#include <stdio.h>

/*
                         Main application
 */
int main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    
    //gps_init();
    char buff;

    while( 1 )
    {
      /*if( gps_get_nmea( line_buff, sizeof( line_buff ) ) )
        printf( "Got Line: %s\r\n", line_buff );
      else printf( "No Data\r\n" );*/
      I2C_block_read( 0x42, &buff, 1 );
      if( buff != 0xFF )
        printf( "%c", buff );
    }

    return 1;
}
/**
 End of File
*/

