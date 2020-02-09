/* 
 * File:   gps.c
 * Author: Christopher Madrigal
 *
 * Created on February 11, 2020
 */

#include "gps.h"
#include "i2c.h"

// I2C address of the GPS device
#define GPS_ADDRESS  0x42

// The GPS keeps a count of available chars to read. It is a 16-bit wide value,
// whose high and low bytes are in the given registers.
#define REG_NUM_HIGH 0xFD
#define REG_NUM_LOW  0xFE
    
// This register is read to retrieve the next char. Returns numerical 0xFF
// when no data is available
#define REG_DATA 0xFF
#define NO_DATA  0xFF

/* Configuration Command
 * 
 * BAUD = 19200
 * Communication Port = 0 (I2C)
 * 
 * Protocol | Threshold 9-bit  | Pin 5-bit   | Polarity  1-bit | Enable 1-bit
 *
 * Input    | 000000000 (none) | 00000 (DDC) | 1  (Active Low) | 1 (Enabled)
 * Output   | 000000000 (none) | 00000 (DCC) | 1  (Active Low) | 1 (Enabled)
 * 
 * Fields are provided in hex.
 * 
 * Reference: u-blox M8 Receiver Description
 * 
 *            32.3.1.1 "Set Protocols and Baudrate", pg. 129
 *            33.20.23.5 "Port Configuration for DDC Port", pg. 209
 */
 
char CONF_PROTOCOL_BAUD[] = "$PUBX,41,0,0003,0002,19200,0*21";

// Commands that disable output from all but the GPRMC/GPGGA networks
// Notice "1" in the enabled networks, and "0" in the disabled ones
char CONF_ENABLE_GPRMC[] = "$PUBX,40,RMC,1,0,0,0,0,0*46\r\n";
char CONF_ENABLE_GPGGA[] = "$PUBX,40,GGA,1,0,0,0,0,0*5B\r\n";

char CONF_DISABLE_GPGBS[] = "$PUBX,40,GBS,0,0,0,0,0,0*4D\r\n";
char CONF_DISABLE_GPGLL[] = "$PUBX,40,GLL,0,0,0,0,0,0*5C\r\n";
char CONF_DISABLE_GPGNS[] = "$PUBX,40,GNS,0,0,0,0,0,0*41\r\n";
char CONF_DISABLE_GPGRS[] = "$PUBX,40,GRS,0,0,0,0,0,0*5D\r\n";
char CONF_DISABLE_GPGSA[] = "$PUBX,40,GSA,0,0,0,0,0,0*4E\r\n";
char CONF_DISABLE_GPGST[] = "$PUBX,40,GST,0,0,0,0,0,0*5B\r\n";
char CONF_DISABLE_GPGSV[] = "$PUBX,40,GSV,0,0,0,0,0,0*59\r\n";
char CONF_DISABLE_GPTXT[] = "$PUBX,40,TXT,0,0,0,0,0,0*43\r\n";
char CONF_DISABLE_GPVLW[] = "$PUBX,40,VLW,0,0,0,0,0,0*56\r\n";
char CONF_DISABLE_GPVTG[] = "$PUBX,40,VTG,0,0,0,0,0,0*5E\r\n";
char CONF_DISABLE_GPZDA[] = "$PUBX,40,ZDA,0,0,0,0,0,0*44\r\n";

void gps_init( void ) {
  I2C_block_write( GPS_ADDRESS, CONF_PROTOCOL_BAUD, sizeof( CONF_PROTOCOL_BAUD ) - 1 );
  I2C_block_write( GPS_ADDRESS, CONF_DISABLE_GPGBS, sizeof( CONF_DISABLE_GPGBS ) - 1 );
  I2C_block_write( GPS_ADDRESS, CONF_DISABLE_GPGLL, sizeof( CONF_DISABLE_GPGLL ) - 1 );
  I2C_block_write( GPS_ADDRESS, CONF_DISABLE_GPGNS, sizeof( CONF_DISABLE_GPGNS ) - 1 );
  I2C_block_write( GPS_ADDRESS, CONF_DISABLE_GPGRS, sizeof( CONF_DISABLE_GPGRS ) - 1 );
  I2C_block_write( GPS_ADDRESS, CONF_DISABLE_GPGSA, sizeof( CONF_DISABLE_GPGSA ) - 1 );
  I2C_block_write( GPS_ADDRESS, CONF_DISABLE_GPGST, sizeof( CONF_DISABLE_GPGST ) - 1 );
  I2C_block_write( GPS_ADDRESS, CONF_DISABLE_GPGSV, sizeof( CONF_DISABLE_GPGSV ) - 1 );
  I2C_block_write( GPS_ADDRESS, CONF_DISABLE_GPTXT, sizeof( CONF_DISABLE_GPTXT ) - 1 );
  I2C_block_write( GPS_ADDRESS, CONF_DISABLE_GPVLW, sizeof( CONF_DISABLE_GPVLW ) - 1 );
  I2C_block_write( GPS_ADDRESS, CONF_DISABLE_GPVTG, sizeof( CONF_DISABLE_GPVTG ) - 1 );
  I2C_block_write( GPS_ADDRESS, CONF_DISABLE_GPZDA, sizeof( CONF_DISABLE_GPZDA ) - 1 );
}

uint8_t gps_get_nmea( char *buffer, const uint8_t n ) {
  
  // keep track of the prior exchange, a $ may have been gobbled up
  static uint8_t lastChar = 0;
  
  if( n == 0 ) return 1; //nonzero size
  if( n == 1 ) { // buffer contains only null terminator
    buffer[0] = 0;
    return 1;
  }
  
  // keep track of the last character read, and the index into the line
  uint8_t i = 0;
  
  // keep skipping until the first $
  while( lastChar != '$' ) {
    
    // panic out if I2C ever errors
    if( !I2C_block_read(GPS_ADDRESS,  &lastChar, 1 ) ) return 0;
  }
  
  //now read the sentence, which ends in \r\n
  do {
    
    //skip empty data
    if( lastChar != NO_DATA ) {
      buffer[i] = lastChar;
      i++;
      buffer[i] = 0; // pad line with null at every step
    }
    
    // continue reading one char at a time
    if( !I2C_block_read( GPS_ADDRESS, &lastChar, 1 ) ) return 0;
    
  } while( lastChar != '\r' && lastChar != '$' && i < (n-1) );
  
  //either lastChar = \r, or lastChar = $, i == n
  
  return 1;
}

/**
 * Small Hex String-to-Int conversion
 * Assumes characters involved are only hex, and null-terminated
 * @param str The string to convert
 * @return The numeric value of the string
 */
uint8_t small_shtoui( const char *str, uint8_t len ) {
  
  // running total
  uint8_t val = 0;
  
  // what power of 16 are we testing
  uint8_t place = 1;
  
  // run through whole string
  uint8_t i = len - 1;
  while( i >= 0 ) {
    
    switch( str[i] ) {
    case '0': continue;
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      val += ((str[i] - '0') * place);
      break;
    case 'A':
      val += (0xA * place);
      break;
    case 'B':
      val += (0xB * place);
      break;
    case 'C':
      val += (0xC * place);
      break;
    case 'D':
      val += (0xD * place);
      break;
    case 'E':
      val += (0xE * place);
      break;
    case 'F':
      val += (0xF * place);
      break;
    default:
      return 0;
    }
      place <<= 4; // place *= 16
      i--;
  }
  
  return val;
}

uint8_t nmea_validate( const char *sentence ) {
  
  //all NMEA sentences start with $
  if( sentence[0] != '$' ) return 0;
  
  //find the checksum
  const char *s = sentence;
  while( *s != '*' && *s != 0 ) s++;
  if( *s == 0 ) return 0;
  char val[2] = { *s, *(s+1) };
  
  uint8_t checksum = small_shtoui( val, 2 );
  
  if( checksum == 0 ) return 0;
  
  //calculate the checksum from the rest of the string
  uint8_t sum = 0;
  const char *b = sentence + 1; //skip '$'
  char c;
  while( (c = *b) != 0 && c != '*' ) {
    if( c != ',' ) sum ^= c;
    b++;
  }
  
  //compare the checksums
  return ( sum == checksum );
}