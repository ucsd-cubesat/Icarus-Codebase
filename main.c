/* 
 * File:     main.c
 * Author:   Christopher Madrigal
 * Modified: 20 February 2020
 */

/**
 * Section: Included Files
 */
#include "mcc_generated_files/system.h"
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
    
    if( !lora_init() ) while( 1 );
    
    unsigned int packet = 0;
    while( 1 ) {
      packet++;
      sprintf( payload,"Hello, World! I am packet %d", packet);
      
      //set SPI access to fifo to the Tx base reg
      lora_write_reg( REG_FIFO_ADDR_PTR, /*lora_read_reg( REG_FIFO_TX_BASE_ADDR )*/ 0 );
    
      //load payload into FIFO
      size_t i;
      for( i = 0; i < sizeof( payload ); i++ ){
        lora_write_reg( REG_FIFO, payload[i] );
      }
    
      //commence Tx
      lora_write_reg( REG_OP_MODE, LORA_TX );
    
      //wait for Tx to end
      while( (lora_read_reg( REG_IRQ_FLAGS ) & FLAG_TX_DONE) == 0 ) {}
    
      //confirm Tx
      if( lora_read_reg( REG_OP_MODE ) == LORA_STANDBY ) {
        lora_write_reg( REG_IRQ_FLAGS, CLEAR_IRQ_FLAGS );
        printf( "Transmitted payload.\r\n" );
      }
      else {
        //reinit lora module
        lora_init();
         printf( "LoRa was reinitialized.\r\n" );
      }
    }

    return 1;
}
/**
 End of File
*/