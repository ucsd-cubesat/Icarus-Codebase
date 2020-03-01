/* 
 * File:     spi.c
 * Author:   Christopher Madrigal
 * Modified: 20 February 2020
 */

#include "spi.h"
#include "mcc_generated_files/spi1.h"

uint16_t SPI_block_exchange( void *dataTX, void *dataRX, const uint16_t n ) {
  
  // if n == 0
  if( !n ) return 0;
  
  // do exchange
  return SPI1_Exchange8bitBuffer( (uint8_t*)dataTX, n, (uint8_t*)dataRX );
}