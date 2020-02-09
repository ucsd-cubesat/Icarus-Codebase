/* 
 * File:   gps.h
 * Author: Christopher Madrigal
 *
 * Created on February 11, 2020
 */

#ifndef GPS_H
#define	GPS_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>
    
/*
 * The GPS unit receives GPS data and outputs NMEA Sentences
 * NMEA is simply a packet format in plaintext.
 * The general format for an NMEA Sentence is:
 * 
 * $[service],[field_i],...,[field_n]*N\r\n
 * 
 * Where [service] is the name of a GPS satellite network,
 *         or a custom command name
 *       [field_i] is the i-th field's value
 *       N is a checksum of the entire sentence between
 *         the '$' and '*' not including ','
 *       The ',' '$' '*' and "\r\n" literals are required
 *
 * The checksum N is calculated by applying a running XOR to every
 * character in the sentence. Here is a pseudocode solution:
 * 
 *   let subsentence := substring( sentence, '$', '*' );
 *   let checksum := 0;
 *   for( char c in subsentence )
 *     if( c != ',' )
 *       checksum := checksum XOR c
 * 
 * Where substring( s, c1, c2 ) gives the sequence of
 *   characters in s between c1 and c2, not counting c1 nor c2
 * 
 * Checksums are NOT mandatory, but expected. If a checksum is not present,
 *   then '*' will not appear in the sentence and the sentence ends
 *   immediately with "\r\n"
 * 
 * Any field without valid data is left empty, but is still delimited by a ','
 * This has the added benefit of ensuring any empty message, though well
 *   formatted, has a checksum of identically 0.
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 
 * The satellite networks we are using are GPGGA and GPRMC
 *   together they provide location, velocity, heading, and universal time.
 * 
 * The NMEA sentence structure for each network is as follows:
 * 
 * $GPGGA,time,lat,NS,lon,EW,quality,numSV,HDOP,alt,altUnit,sep,sepUnit,diffAge,diffStation*cs<CR><LF>
 * $GPRMC,time,status,lat,NS,lon,EW,spd,cog,date,mv,mvEW,posMode,navStatus*cs<CR><LF>
 * 
 * We will only concern ourselves with feeding only non-empty, valid
 *   sentences verbatim through telemetry.
 */

/**
 * Initialize the GPS.
 * 
 * Precondition:
 *   I2C peripheral must be initialized.
 * 
 * Postcondition:
 *   GPS will be initialized to the desired configuration.
 *   gps_get_line will return the next valid NMEA Sentence.
 */
void gps_init( void );

/**
 * Read an NMEA sentence from the GPS
 * 
 * Precondition:
 *   GPS must be initialized.
 *   buffer must be a valid memory address.
 * 
 * Postcondition:
 *   The next read will continue from the last character read
 * 
 * @param buffer A pointer to where a message will be saved
 * @param n The maximum number of bytes to read into the buffer
 * @return 1 is the transaction was successful, 0 otherwise
 */
uint8_t gps_get_nmea( char *buffer, const uint8_t n );

/**
 * Validates an NMEA sentence.
 * 
 * Precondition:
 *   sentence must point to a null-terminated string.
 * 
 * Postcondition:
 *   None.
 * 
 * @param sentence The string to validate.
 * @return 1 if the sentence is valid, 0 otherwise
 */
uint8_t nmea_validate( const char *sentence );

#ifdef	__cplusplus
}
#endif

#endif	/* GPS_H */

