#include "spi_driver.h"
#include "stm32f446ze.h"
#include <stddef.h>
#include <stdint.h>

void SPI_Init(SPIx_RegDef_t *p_spi, const SPI_Init_t *p_config) {
  if (p_spi == 0 || p_config == 0) {
    return;
  }

  uint16_t cr1_pinmask = 0;

  int spi_direction = p_config->Direction;

  cr1_pinmask |= (p_config->CPHA << SPI_CR1_CPHA_Pos);
  cr1_pinmask |= (p_config->CPOL << SPI_CR1_CPOL_Pos);
  cr1_pinmask |= (p_config->Mode << SPI_CR1_MSTR_Pos);
  cr1_pinmask |= (p_config->BaudRate << SPI_CR1_BR_Pos);

  cr1_pinmask |= (p_config->FirstBit << SPI_CR1_LSBFIRST_Pos);
  cr1_pinmask |= (1U << SPI_CR1_SSI_Pos);
  cr1_pinmask |= (1U << SPI_CR1_SSM_Pos);
  cr1_pinmask |= (p_config->DataSize << SPI_CR1_DFF_Pos);

  switch (spi_direction) {
  case SPI_DIRECTION_2LINES_RXONLY:
    cr1_pinmask |= (1U << SPI_CR1_RXONLY_Pos);
    // cr1_pinmask &= ~(1U << SPI_CR1_BIDIOE_Pos);
    // cr1_pinmask &= ~(1U << SPI_CR1_BIDIMODE_Pos);
    break;
  case SPI_DIRECTION_1LINE:
    // cr1_pinmask &= ~(1U << SPI_CR1_RXONLY_Pos);
    cr1_pinmask |= (1U << SPI_CR1_BIDIOE_Pos);
    cr1_pinmask |= (1U << SPI_CR1_BIDIMODE_Pos);
    break;
  case SPI_DIRECTION_2LINES:
  default:
    // cr1_pinmask &= ~(1U << SPI_CR1_RXONLY_Pos);
    // cr1_pinmask &= ~(1U << SPI_CR1_BIDIOE_Pos);
    // cr1_pinmask &= ~(1U << SPI_CR1_BIDIMODE_Pos);
    break;
  }

  p_spi->CR1 = cr1_pinmask;
  p_spi->CR2 = 0;
}

void SPI_Enable(SPIx_RegDef_t *p_spi) { p_spi->CR1 |= (1U << 6); }

void SPI_Disable(SPIx_RegDef_t *p_spi) {
  if (p_spi == 0) {
    return;
  }

  /* see §26.3.10 of :
   * https://www.st.com/resource/en/reference_manual/rm0390-stm32f446xx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
   * to get more details about SPI disabling procedures
   */
  if (p_spi->CR1 & (1U << SPI_CR1_RXONLY_Pos)) {

    p_spi->CR1 &= ~(1U << SPI_CR1_SPE_Pos);

    while (p_spi->SR & (1U << SPI_SR_BSY_Pos)) {
      __asm__ volatile("nop");
    }
  } else {
    while (!(p_spi->SR & (1U << SPI_SR_TXE_Pos))) {
      __asm__ volatile("nop");
    }

    while (p_spi->SR & (1U << SPI_SR_BSY_Pos)) {
      __asm__ volatile("nop");
    }

    p_spi->CR1 &= ~(1U << SPI_CR1_SPE_Pos);

    while (p_spi->SR & (1U << SPI_SR_RXNE_Pos)) {
      (void)p_spi->DR;
    }
  }
}

static inline SPI_DataSize_t SPI_getDFF(SPIx_RegDef_t *p_spi) {
  return ((SPI_DataSize_t)((p_spi->CR1 >> SPI_CR1_DFF_Pos) & (1U)));
}

static inline SPI_Direction_t SPI_getDirection(SPIx_RegDef_t *p_spi) {
  if ((p_spi->CR1 >> SPI_CR1_RXONLY_Pos) & (1U)) {
    return SPI_DIRECTION_2LINES_RXONLY;
  } else if ((p_spi->CR1 >> SPI_CR1_BIDIMODE_Pos) & (1U)) {
    return SPI_DIRECTION_1LINE;
  } else {
    return SPI_DIRECTION_2LINES;
  }
}

SPI_ReturnCode_t SPI_TransmitReceive(SPIx_RegDef_t *p_spi, const void *p_TX_data,
                                 void *p_RX_data, size_t len_bytes,
                                 size_t timeout_ms) {
  (void)timeout_ms;

  if (p_spi == NULL) {
    return SPI_ERR_PARAM;
  }
  if ((p_TX_data == NULL) || (p_RX_data == NULL)) {
    return SPI_ERR_PARAM;
  }
  if (len_bytes == 0) {
    return SPI_OK;
  }

  SPI_Direction_t spi_dir = SPI_getDirection(p_spi);

  if ((spi_dir == SPI_DIRECTION_1LINE) ||
      (spi_dir == SPI_DIRECTION_2LINES_RXONLY)) {
    return SPI_ERR_PARAM;
  }

  SPI_DataSize_t spi_dff = SPI_getDFF(p_spi);

  for (size_t i = 0; i < len_bytes; i++) {
    if (spi_dff == SPI_DATASIZE_8BIT) {
      while (!(p_spi->SR & (1U << SPI_SR_TXE_Pos))) {
        __asm__ volatile("nop");
      }
      p_spi->DR = *((uint8_t *)p_TX_data);
      while (!(p_spi->SR & (1U << SPI_SR_RXNE_Pos))) {
        __asm__ volatile("nop");
      }
      *((uint8_t *)p_RX_data) = p_spi->DR;

      p_TX_data++;
      p_RX_data++;
    } else if (spi_dff == SPI_DATASIZE_16BIT) {
      while (!(p_spi->SR & (1U << SPI_SR_TXE_Pos))) {
        __asm__ volatile("nop");
      }
      p_spi->DR = *((uint16_t *)p_TX_data);

      while (!(p_spi->SR & (1U << SPI_SR_RXNE_Pos))) {
        __asm__ volatile("nop");
      }
      *((uint16_t *)p_RX_data) = p_spi->DR;

      p_TX_data = p_TX_data + 2;
      p_RX_data = p_RX_data + 2;
    } else {
      return SPI_ERR_PARAM;
    }
  }

  return SPI_OK;
}
