#ifndef _SPI_DRIVER_H_
#define _SPI_DRIVER_H_

#include "stm32f446ze.h"
#include "stdbool.h"
#include <stddef.h>

typedef enum {
  SPI_MODE_SLAVE,
  SPI_MODE_MASTER
} SPI_Mode_t;

typedef enum {
    SPI_DIRECTION_2LINES,         /** 2-line Full Duplex (BIDIMODE=0, RXONLY=0) */
    SPI_DIRECTION_2LINES_RXONLY,  /** 2-line Receive Only (BIDIMODE=0, RXONLY=1) */
    SPI_DIRECTION_1LINE           /** 1-line Half Duplex bidirectional (BIDIMODE=1) */
} SPI_Direction_t;

typedef enum {
  SPI_DATASIZE_8BIT,
  SPI_DATASIZE_16BIT
} SPI_DataSize_t;

typedef enum {
  SPI_CPOL_LOW, /** CLK to 0 when idle */
  SPI_CPOL_HIGH /** CLK to 1 when idle */
} SPI_CPOL_t;

typedef enum {
  SPI_CPHA_1EDGE, /** The 1st clock transition is the first data capture edge */
  SPI_CPHA_2EDGE  /** The 2nd clock transition is the first data capture edge */
} SPI_CPHA_t;

typedef enum {
  SPI_BAUDRATE_DIV2   = 0,
  SPI_BAUDRATE_DIV4   = 1,
  SPI_BAUDRATE_DIV8   = 2,
  SPI_BAUDRATE_DIV16  = 3,
  SPI_BAUDRATE_DIV32  = 4,
  SPI_BAUDRATE_DIV64  = 5,
  SPI_BAUDRATE_DIV128 = 6,
  SPI_BAUDRATE_DIV256 = 7
} SPI_BaudRate_t;

typedef enum {
  SPI_FIRSTBIT_MSB,
  SPI_FIRSTBIT_LSB
} SPI_FirstBit_t;

typedef struct {
  SPI_Mode_t       Mode;
  SPI_Direction_t  Direction;
  SPI_DataSize_t   DataSize;
  SPI_CPOL_t       CPOL;
  SPI_CPHA_t       CPHA;
  SPI_BaudRate_t   BaudRate;
  SPI_FirstBit_t   FirstBit;
} SPI_Init_t;

typedef enum {
  SPI_OK = 0,
  SPI_ERR_PARAM   = -1,
  SPI_ERR_TIMEOUT = -2
} SPI_ReturnCode_t;

void SPI_Init(SPIx_RegDef_t *p_spi, const SPI_Init_t *p_config);
void SPI_Enable(SPIx_RegDef_t *p_spi);
void SPI_Disable(SPIx_RegDef_t *p_spi);

SPI_ReturnCode_t SPI_TransmitReceive(SPIx_RegDef_t *p_spi, const void *p_TX_data, void *p_RX_data, size_t len_bytes, size_t timeout_ms);

#endif // _SPI_DRIVER_H_