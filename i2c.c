/* 
 * File:   gps.c
 * Author: Christopher Madrigal
 *
 * Created on February 11, 2020
 */

#include "mcc_generated_files/i2c2.h"
#include "i2c.h"

// I2C peripheral uses interrupts to update status
// Therefore, volatile keyword is required
volatile I2C2_MESSAGE_STATUS reqStatus;

uint8_t I2C_block_read( const uint16_t address, void *data, const uint8_t n ) {
  
  // Set status as message pending
  reqStatus = I2C2_MESSAGE_PENDING;
  
  // Preform read from the address, which updates status
  I2C2_MasterRead( (uint8_t*)data, n, address, &reqStatus );
  
  // We are stuck here (blocked) until status changes
  while( reqStatus == I2C2_MESSAGE_PENDING );
  
  // The status may signal error, check for success
  return ( reqStatus == I2C2_MESSAGE_COMPLETE );
}

uint8_t I2C_block_write( const uint16_t address, void *data, const uint8_t n ) {
  
  // Set status as message pending
  reqStatus = I2C2_MESSAGE_PENDING;
  
  // Preform write to address, which updates status
  I2C2_MasterWrite( (uint8_t*)data, n, address, &reqStatus );
  
  // We are stuck here (blocked) until status changes
  while( reqStatus == I2C2_MESSAGE_PENDING );
  
  // The status may signal error, check for success
  return ( reqStatus == I2C2_MESSAGE_COMPLETE );
}