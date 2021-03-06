/* 
 * File:     i2c.h
 * Author:   Christopher Madrigal
 * Modified: 20 February 2020
 */

#ifndef I2C_H
#define	I2C_H

#include <stdint.h>

/**
 * Preforms a blocking write to an I2C Slave
 * 
 * @param address The I2C address of the slave
 * @param data The buffer 
 * @param n The maximum bytes to be read into data
 * @return 1 if the transaction was successful, 0 otherwise
 */
uint8_t I2C_block_write( const uint16_t address, void *data, const uint8_t n );

/**
 * Preforms a blocking read from an I2C Slave
 * 
 * @param address The I2C address of the slave
 * @param data The buffer 
 * @param n The maximum bytes to be read into data
 * @return 1 if the transaction was successful, 0 otherwise
 */
uint8_t I2C_block_read( const uint16_t address, void *data, const uint8_t n );

#endif	/* I2C_H */

