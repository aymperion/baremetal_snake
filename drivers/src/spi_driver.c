#include "spi_driver.h"
#include "stm32f446ze.h"
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
