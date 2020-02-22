/* 
 * File:     spi.h
 * Author:   Christopher Madrigal
 * Modified: 20 February 2020
 */

#ifndef SPI_H
#define	SPI_H

#include <stdint.h>

/**
 * Preforms a blocking exchange to an SPI Slave
 * 
 * @param dataIn The input buffer 
 * @param dataOut The output buffer
 * @param n The maximum bytes to exchange
 * @return The total number of bytes exchanged
 */
uint16_t SPI_block_exchange( void *dataIn, void *dataOut, const uint16_t n );

#endif	/* SPI_H */

