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
 * @param dataTX The output buffer
 * @param dataRX The input buffer 
 * @param n The maximum bytes to exchange
 * @return The total number of bytes exchanged
 */
uint16_t SPI_block_exchange( void *dataTX, void *dataRX, const uint16_t n );

#endif	/* SPI_H */

