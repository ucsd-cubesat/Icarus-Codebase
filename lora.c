/* 
 * File:     lora.c
 * Author:   Christopher Madrigal
 * Modified: 20 February 2020
 */

#include "lora.h"
#include "spi.h"
#include "mcc_generated_files/pin_manager.h"
#include <stdio.h>

uint8_t lora_init() {
  
  // Reset LoRa
  LORA_RST_SetHigh();
  
  // Get current mode
  uint8_t bootmode = 0;
  bootmode = lora_read_reg( REG_OP_MODE );
  
  printf( "LoRa initial bootmode = 0x%x\r\n", bootmode );

  // If not in LoRa mode, switch
  if( !(bootmode & LORA_MODE) ) {
    lora_write_reg( REG_OP_MODE, FSK_SLEEP );
  }
  
  // Switch to LoRa sleep mode
  lora_write_reg( REG_OP_MODE, LORA_SLEEP );
  
  printf( "LoRa sleep bootmode = 0x%x\r\n", bootmode );
  
  // Set the frequency
  uint64_t freq = DEFAULT_LORA_FREQ;
  lora_write_reg( REG_RF_FREQ_MSB, (uint8_t)(freq >> 16) );
  lora_write_reg( REG_RF_FREQ_MID, (uint8_t)(freq >> 8) );
  lora_write_reg( REG_RF_FREQ_LSB, (uint8_t)(freq >> 0) );
  
  // Initialize RX/TX stack
  lora_write_reg( REG_FIFO_TX_BASE_ADDR, 0 );
  lora_write_reg( REG_FIFO_RX_BASE_ADDR, 0 );
  
  // Set the payload size
  lora_write_reg( REG_PAYLOAD_LEN, BUFFER_SIZE );
  lora_write_reg( REG_MAX_PAYLOAD_LEN, BUFFER_SIZE );
  
  // Set LNA boost
  //lora_write_reg( REG_LNA, read_reg( REG_LNA ) | DEFAULT_LORA_LNA );

  // Set auto AGC
  //lora_write_reg( REG_MODEM_CONFIG3, DEFAULT_LORA_AGC );
  
  // Set SYNC word
  lora_write_reg( REG_SYNC_WORD, DEFAULT_LORA_SYNC_WORD );

  // Set Power (TODO fix magic numbers)
  uint8_t level = DEFAULT_LORA_LEVEL;
  uint8_t dac;
  uint8_t mA;

  if( level > 20 ) level = 20;
  else if( level < 2 ) level = 2;

  if( level > 17 ) {
    level -= 3;
    dac = 0x87;
    mA = 140;
  }
  else {
    dac = 0x84;
    mA = 100;
  }

  uint8_t ocp = 27;
  if( mA <= 120 ) ocp = (mA - 45)/5;
  else if( mA <= 240 ) ocp = (mA + 30)/10;

  //lora_write_reg( REG_PA_DAC, dac );
  //lora_write_reg( REG_OCP, 0x20 | (0x1F & ocp) );
  lora_write_reg( REG_PA_CONFIG, PA_BOOST | (level - 2) );

  // Switch to LoRa standby mode
  lora_write_reg( REG_OP_MODE, LORA_STANDBY );
  
  //check that the module is in standby mode
  bootmode = lora_read_reg( REG_OP_MODE );
  if( bootmode != LORA_STANDBY ) {
    //printf( "Could not initialize LORA\r\nBoot mode is 0x%x\r\n", bootmode );
    return 0;
  }
  
  //printf( "LORA on standby with frequency %llu and power level %hhu dBm\r\n", freq, level );
  return 1;
}

void lora_close() {
	lora_write_reg( REG_OP_MODE, LORA_SLEEP );
}

uint8_t lora_read_reg( const uint8_t reg ){
  
  // For this transaction, the first byte is sent by us and the second is received
  uint8_t txbuf[2] = { reg, 0 };
  uint8_t rxbuf[2] = { 0, 0 };
  
  // Chip select low indicates active
  //LORA_CS_SetLow();
  SPI_block_exchange( rxbuf, txbuf, 2 );
  //LORA_CS_SetHigh();
  
  return rxbuf[1];
}

uint8_t lora_write_reg( const uint8_t reg, const uint8_t data ) {
  
  // We have to set reg MSB high to indicate write
  // For this transaction, we send two bytes and receive at byte two
  uint8_t txbuf[2] = { reg | 0x80, data};
  uint8_t rxbuf[2] = { 0, 0 };
  
  // Chip select low indicates active
  //LORA_CS_SetLow();
  SPI_block_exchange( rxbuf, txbuf, 2 );
  //LORA_CS_SetHigh();
  
  return rxbuf[1];
}