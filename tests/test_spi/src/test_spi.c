#include "test_spi.h"
#include "gpio_driver.h"
#include "rcc_helper.h"
#include "spi_driver.h"
#include "stm32f446ze.h"
#include "unity.h"
#include "unity_internals.h"
#include <stdint.h>

#define SPI_CR1_CPHA_MSK (1U << SPI_CR1_CPHA_Pos)
#define SPI_CR1_CPOL_MSK (1U << SPI_CR1_CPOL_Pos)
#define SPI_CR1_MSTR_MSK (1U << SPI_CR1_MSTR_Pos)
#define SPI_CR1_BR_MSK (7U << SPI_CR1_BR_Pos)
#define SPI_CR1_SPE_MSK (1U << SPI_CR1_SPE_Pos)
#define SPI_CR1_LSBFIRST_MSK (1U << SPI_CR1_LSBFIRST_Pos)
#define SPI_CR1_SSI_MSK (1U << SPI_CR1_SSI_Pos)
#define SPI_CR1_SSM_MSK (1U << SPI_CR1_SSM_Pos)
#define SPI_CR1_RXONLY_MSK (1U << SPI_CR1_RXONLY_Pos)
#define SPI_CR1_DFF_MSK (1U << SPI_CR1_DFF_Pos)
#define SPI_CR1_BIDIOE_MSK (1U << SPI_CR1_BIDIOE_Pos)
#define SPI_CR1_BIDIMODE_MSK (1U << SPI_CR1_BIDIMODE_Pos)

extern void (*g_current_suite_setUp)(void);
extern void (*g_current_suite_tearDown)(void);

static void spi_suite_setUp(void) {
  RCC_Enable_Clock_GPIOx(SPI_MOSI_PORT);
  if (SPI_MISO_PORT != SPI_MOSI_PORT) {
    RCC_Enable_Clock_GPIOx(SPI_MISO_PORT);
  }

  GPIO_Init(SPI_MOSI_PORT,
            &(GPIO_Init_t){.Pin_Mask = SPI_MOSI_PIN,
                           .Mode = GPIO_MODE_ALTFN,
                           .Alternate_Func = SPI_MOSI_PIN_ALTFN});
  GPIO_Init(SPI_MISO_PORT,
            &(GPIO_Init_t){.Pin_Mask = SPI_MISO_PIN,
                           .Mode = GPIO_MODE_ALTFN,
                           .Alternate_Func = SPI_MISO_PIN_ALTFN});
  GPIO_Init(SPI_SCK_PORT, &(GPIO_Init_t){.Pin_Mask = SPI_SCK_PIN,
                                         .Mode = GPIO_MODE_ALTFN,
                                         .Alternate_Func = SPI_SCK_PIN_ALTFN});
  RCC_Enable_Clock_SPIx(TEST_SPI);
  TEST_SPI->CR1 = 0;
  TEST_SPI->CR2 = 0;
}

static void spi_suite_tearDown(void) {
  TEST_SPI->CR1 = 0;
  TEST_SPI->CR2 = 0;
  RCC_Disable_Clock_SPIx(TEST_SPI);

  RCC_Disable_Clock_GPIOx(SPI_MOSI_PORT);
  if (SPI_MISO_PORT != SPI_MOSI_PORT) {
    RCC_Disable_Clock_GPIOx(SPI_MISO_PORT);
  }
}

static void helper_spi_init_loopback(SPI_DataSize_t data_size) {
  SPI_Init(TEST_SPI, &(SPI_Init_t){.Mode = SPI_MODE_MASTER,
                                   .Direction = SPI_DIRECTION_2LINES,
                                   .DataSize = data_size,
                                   .CPOL = SPI_CPOL_LOW,
                                   .CPHA = SPI_CPHA_1EDGE,
                                   .BaudRate = SPI_BAUDRATE_DIV64,
                                   .FirstBit = SPI_FIRSTBIT_LSB});
  SPI_Enable(TEST_SPI);
}

static void helper_spi_init_halfduplex(SPI_DataSize_t data_size) {
  SPI_Init(TEST_SPI, &(SPI_Init_t){.Mode = SPI_MODE_MASTER,
                                   .Direction = SPI_DIRECTION_1LINE,
                                   .DataSize = data_size,
                                   .CPOL = SPI_CPOL_LOW,
                                   .CPHA = SPI_CPHA_1EDGE,
                                   .BaudRate = SPI_BAUDRATE_DIV64,
                                   .FirstBit = SPI_FIRSTBIT_MSB});
  SPI_Enable(TEST_SPI);
}

static void helper_spi_init_2lines_rxonly(SPI_DataSize_t data_size) {
  SPI_Init(TEST_SPI, &(SPI_Init_t){.Mode = SPI_MODE_MASTER,
                                   .Direction = SPI_DIRECTION_2LINES_RXONLY,
                                   .DataSize = data_size,
                                   .CPOL = SPI_CPOL_LOW,
                                   .CPHA = SPI_CPHA_1EDGE,
                                   .BaudRate = SPI_BAUDRATE_DIV64,
                                   .FirstBit = SPI_FIRSTBIT_MSB});
  SPI_Enable(TEST_SPI);
}

void test_SPI_Init_should_enforce_safety_invariants(void) {
  SPI_Init_t config = {.Mode = SPI_MODE_MASTER,
                       .Direction = SPI_DIRECTION_2LINES,
                       .DataSize = SPI_DATASIZE_8BIT,
                       .CPOL = SPI_CPOL_LOW,
                       .CPHA = SPI_CPHA_1EDGE,
                       .BaudRate = SPI_BAUDRATE_DIV2,
                       .FirstBit = SPI_FIRSTBIT_MSB};

  SPI_Init(TEST_SPI, &config);

  TEST_ASSERT_BITS_LOW(SPI_CR1_SPE_MSK, TEST_SPI->CR1);
  TEST_ASSERT_BITS_HIGH(SPI_CR1_SSM_MSK, TEST_SPI->CR1);
  TEST_ASSERT_BITS_HIGH(SPI_CR1_SSI_MSK, TEST_SPI->CR1);
  TEST_ASSERT_EQUAL_HEX16(0x0000, TEST_SPI->CR2);
}

void test_SPI_Init_should_set_high_polarity_bits(void) {
  SPI_Init_t config = {.Mode = SPI_MODE_MASTER,
                       .Direction = SPI_DIRECTION_2LINES,
                       .DataSize = SPI_DATASIZE_16BIT,
                       .CPOL = SPI_CPOL_HIGH,
                       .CPHA = SPI_CPHA_2EDGE,
                       .BaudRate = SPI_BAUDRATE_DIV256,
                       .FirstBit = SPI_FIRSTBIT_LSB};
  SPI_Init(TEST_SPI, &config);
  uint16_t cr1 = TEST_SPI->CR1;
  TEST_ASSERT_BITS_HIGH(SPI_CR1_CPHA_MSK, cr1);
  TEST_ASSERT_BITS_HIGH(SPI_CR1_CPOL_MSK, cr1);
  TEST_ASSERT_BITS_HIGH(SPI_CR1_MSTR_MSK, cr1);
  TEST_ASSERT_BITS_HIGH(SPI_CR1_DFF_MSK, cr1);
  TEST_ASSERT_BITS_HIGH(SPI_CR1_LSBFIRST_MSK, cr1);
  TEST_ASSERT_EQUAL_HEX16((SPI_BAUDRATE_DIV256 << SPI_CR1_BR_Pos),
                          (cr1 & SPI_CR1_BR_MSK));
}

void test_SPI_Init_should_clear_low_polarity_bits(void) {
  SPI_Init_t config = {.Mode = SPI_MODE_SLAVE,
                       .Direction = SPI_DIRECTION_2LINES,
                       .DataSize = SPI_DATASIZE_8BIT,
                       .CPOL = SPI_CPOL_LOW,
                       .CPHA = SPI_CPHA_1EDGE,
                       .BaudRate = SPI_BAUDRATE_DIV2,
                       .FirstBit = SPI_FIRSTBIT_MSB};
  SPI_Init(TEST_SPI, &config);
  uint16_t cr1 = TEST_SPI->CR1;
  TEST_ASSERT_BITS_LOW(SPI_CR1_CPHA_MSK, cr1);
  TEST_ASSERT_BITS_LOW(SPI_CR1_CPOL_MSK, cr1);
  TEST_ASSERT_BITS_LOW(SPI_CR1_MSTR_MSK, cr1);
  TEST_ASSERT_BITS_LOW(SPI_CR1_DFF_MSK, cr1);
  TEST_ASSERT_BITS_LOW(SPI_CR1_LSBFIRST_MSK, cr1);
  TEST_ASSERT_EQUAL_HEX16(0, (cr1 & SPI_CR1_BR_MSK));
}

void test_SPI_Init_direction_2lines_fullduplex(void) {
  SPI_Init_t config = {.Direction = SPI_DIRECTION_2LINES,
                       .Mode = SPI_MODE_MASTER};
  SPI_Init(TEST_SPI, &config);
  uint16_t cr1 = TEST_SPI->CR1;
  TEST_ASSERT_BITS_LOW(SPI_CR1_BIDIMODE_MSK, cr1);
  TEST_ASSERT_BITS_LOW(SPI_CR1_BIDIOE_MSK, cr1);
  TEST_ASSERT_BITS_LOW(SPI_CR1_RXONLY_MSK, cr1);
}

void test_SPI_Init_direction_2lines_rxonly(void) {
  SPI_Init_t config = {.Direction = SPI_DIRECTION_2LINES_RXONLY,
                       .Mode = SPI_MODE_MASTER};
  SPI_Init(TEST_SPI, &config);
  uint16_t cr1 = TEST_SPI->CR1;
  TEST_ASSERT_BITS_LOW(SPI_CR1_BIDIMODE_MSK, cr1);
  TEST_ASSERT_BITS_LOW(SPI_CR1_BIDIOE_MSK, cr1);
  TEST_ASSERT_BITS_HIGH(SPI_CR1_RXONLY_MSK, cr1);
}

void test_SPI_Init_direction_1line_halfduplex(void) {
  SPI_Init_t config = {.Direction = SPI_DIRECTION_1LINE,
                       .Mode = SPI_MODE_MASTER};
  SPI_Init(TEST_SPI, &config);
  uint16_t cr1 = TEST_SPI->CR1;
  TEST_ASSERT_BITS_HIGH(SPI_CR1_BIDIMODE_MSK, cr1);
  TEST_ASSERT_BITS_HIGH(SPI_CR1_BIDIOE_MSK, cr1); // TX mode by default
  TEST_ASSERT_BITS_LOW(SPI_CR1_RXONLY_MSK, cr1);
}

void test_SPI_Enable_and_Disable_2lines_fullduplex(void) {
  SPI_Init_t config = {.Direction = SPI_DIRECTION_2LINES,
                       .Mode = SPI_MODE_MASTER};
  SPI_Init(TEST_SPI, &config);
  /* Enable */
  SPI_Enable(TEST_SPI);
  TEST_ASSERT_BITS_HIGH(SPI_CR1_SPE_MSK, TEST_SPI->CR1);
  /* Disable */
  SPI_Disable(TEST_SPI);
  TEST_ASSERT_BITS_LOW(SPI_CR1_SPE_MSK, TEST_SPI->CR1);
}
void test_SPI_Enable_and_Disable_2lines_rxonly(void) {
  SPI_Init_t config = {.Direction = SPI_DIRECTION_2LINES_RXONLY,
                       .Mode = SPI_MODE_MASTER};
  SPI_Init(TEST_SPI, &config);
  SPI_Enable(TEST_SPI);
  TEST_ASSERT_BITS_HIGH(SPI_CR1_SPE_MSK, TEST_SPI->CR1);

  SPI_Disable(TEST_SPI);
  TEST_ASSERT_BITS_LOW(SPI_CR1_SPE_MSK, TEST_SPI->CR1);
}

void test_SPI_Disable_should_preserve_configuration(void) {
  SPI_Init_t config = {.Mode = SPI_MODE_MASTER,
                       .Direction = SPI_DIRECTION_2LINES,
                       .DataSize = SPI_DATASIZE_16BIT,
                       .CPOL = SPI_CPOL_HIGH,
                       .CPHA = SPI_CPHA_2EDGE,
                       .BaudRate = SPI_BAUDRATE_DIV64,
                       .FirstBit = SPI_FIRSTBIT_LSB};
  SPI_Init(TEST_SPI, &config);

  uint16_t cr1_configured = TEST_SPI->CR1;
  SPI_Enable(TEST_SPI);
  SPI_Disable(TEST_SPI);

  TEST_ASSERT_EQUAL_HEX16(cr1_configured, TEST_SPI->CR1);
}

void test_SPI_Init_with_null_pointers_should_not_crash(void) {
  SPI_Init_t config = {.Mode = SPI_MODE_MASTER};

  SPI_Init((void *)0, &config);
  SPI_Init(TEST_SPI, (void *)0);
  SPI_Enable((void *)0);
  SPI_Disable((void *)0);
}

void test_SPI_TXRX_16bit_single_word(void) {
  helper_spi_init_loopback(SPI_DATASIZE_16BIT);
  uint16_t tx_val = 0xABCD;
  uint16_t rx_val = 0x0000;
  SPI_ReturnCode_t status = SPI_TransmitReceive(TEST_SPI, &tx_val, &rx_val, 1, 100);
  TEST_ASSERT_EQUAL(SPI_OK, status);
  TEST_ASSERT_EQUAL_HEX16(tx_val, rx_val);
}

void test_SPI_TXRX_16bit_multiple_words(void) {
  helper_spi_init_loopback(SPI_DATASIZE_16BIT);
  uint16_t tx_buf[3] = {0x1234, 0x5678, 0x9ABC};
  uint16_t rx_buf[3] = {0, 0, 0};
  SPI_ReturnCode_t status = SPI_TransmitReceive(TEST_SPI, tx_buf, rx_buf, 3, 100);
  TEST_ASSERT_EQUAL(SPI_OK, status);
  TEST_ASSERT_EQUAL_HEX16_ARRAY(tx_buf, rx_buf, 3);
}

void test_SPI_TXRX_8bit_single_byte(void) {
  helper_spi_init_loopback(SPI_DATASIZE_8BIT);
  uint8_t tx_val = 0xA5;
  uint8_t rx_val = 0xFF;
  SPI_ReturnCode_t status = SPI_TransmitReceive(TEST_SPI, &tx_val, &rx_val, 1, 100);
  TEST_ASSERT_EQUAL(SPI_OK, status);
  TEST_ASSERT_EQUAL_HEX8(tx_val, rx_val);
  TEST_ASSERT_EQUAL_HEX8(0xA5, rx_val);
}

void test_SPI_TXRX_8bit_multiple_bytes(void) {
  helper_spi_init_loopback(SPI_DATASIZE_8BIT);
  uint8_t tx_buf[4] = {0x11, 0x22, 0x33, 0x44};
  uint8_t rx_buf[4] = {0};
  SPI_ReturnCode_t status = SPI_TransmitReceive(TEST_SPI, tx_buf, rx_buf, 4, 100);
  TEST_ASSERT_EQUAL(SPI_OK, status);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(tx_buf, rx_buf, 4);
}

void test_SPI_TXRX_should_return_error_when_rx_is_null(void) {
  helper_spi_init_loopback(SPI_DATASIZE_16BIT);
  uint16_t tx_val = 0xABCD;
  SPI_ReturnCode_t status =
      SPI_TransmitReceive(TEST_SPI, &tx_val, (void *)0, 1, 100);
  TEST_ASSERT_EQUAL(SPI_ERR_PARAM, status);
}

void test_SPI_TXRX_should_return_error_when_tx_is_null(void) {
  helper_spi_init_loopback(SPI_DATASIZE_16BIT);
  uint16_t rx_val = 0xABCD;
  SPI_ReturnCode_t status =
      SPI_TransmitReceive(TEST_SPI, (void *)0, &rx_val, 1, 100);
  TEST_ASSERT_EQUAL(SPI_ERR_PARAM, status);
}

void test_SPI_TXRX_should_return_error_when_direction_is_halfduplex(void) {
  helper_spi_init_halfduplex(SPI_DATASIZE_16BIT);
  uint16_t tx_val = 0xABCD;
  uint16_t rx_val = 0x0000;
  SPI_ReturnCode_t status = SPI_TransmitReceive(TEST_SPI, &tx_val, &rx_val, 1, 100);
  TEST_ASSERT_EQUAL(SPI_ERR_PARAM, status);
}

void test_SPI_TXRX_should_return_error_when_direction_is_2lines_rxonly(void) {
  helper_spi_init_2lines_rxonly(SPI_DATASIZE_16BIT);
  uint16_t tx_val = 0xABCD;
  uint16_t rx_val = 0x0000;
  SPI_ReturnCode_t status = SPI_TransmitReceive(TEST_SPI, &tx_val, &rx_val, 1, 100);
  TEST_ASSERT_EQUAL(SPI_ERR_PARAM, status);
}

void test_SPI_TXRX_zero_length_should_return_ok(void) {
  helper_spi_init_loopback(SPI_DATASIZE_8BIT);
  uint8_t dummy = 0;
  SPI_ReturnCode_t status = SPI_TransmitReceive(TEST_SPI, &dummy, &dummy, 0, 100);
  TEST_ASSERT_EQUAL(SPI_OK, status);
}

void test_SPI_TXRX_null_spi_should_return_error(void) {
  uint8_t val = 0;
  SPI_ReturnCode_t status = SPI_TransmitReceive((void *)0, &val, &val, 1, 100);
  TEST_ASSERT_EQUAL(SPI_ERR_PARAM, status);
}

void test_SPI_TXRX_should_return_timeout_when_spi_disabled(void) {
  TEST_IGNORE();
  helper_spi_init_loopback(SPI_DATASIZE_8BIT);
  SPI_Disable(TEST_SPI); // Bus éteint -> blocage simulé
  uint8_t tx = 0xAA, rx = 0x00;
  SPI_ReturnCode_t status =
      SPI_TransmitReceive(TEST_SPI, &tx, &rx, 1, 10); // 10 ms max
  TEST_ASSERT_EQUAL(SPI_ERR_TIMEOUT, status);
}

void test_SPI_main(void) {
  g_current_suite_setUp = spi_suite_setUp;
  g_current_suite_tearDown = spi_suite_tearDown;
  UnitySetTestFile(__FILE_NAME__);

  RUN_TEST(test_SPI_Init_should_enforce_safety_invariants);
  RUN_TEST(test_SPI_Init_should_set_high_polarity_bits);
  RUN_TEST(test_SPI_Init_should_clear_low_polarity_bits);
  RUN_TEST(test_SPI_Init_direction_2lines_fullduplex);
  RUN_TEST(test_SPI_Init_direction_2lines_rxonly);
  RUN_TEST(test_SPI_Init_direction_1line_halfduplex);
  RUN_TEST(test_SPI_Enable_and_Disable_2lines_fullduplex);
  RUN_TEST(test_SPI_Enable_and_Disable_2lines_rxonly);
  RUN_TEST(test_SPI_Disable_should_preserve_configuration);
  RUN_TEST(test_SPI_Init_with_null_pointers_should_not_crash);

  RUN_TEST(test_SPI_TXRX_16bit_single_word);
  RUN_TEST(test_SPI_TXRX_16bit_multiple_words);
  RUN_TEST(test_SPI_TXRX_8bit_single_byte);
  RUN_TEST(test_SPI_TXRX_8bit_multiple_bytes);
  RUN_TEST(test_SPI_TXRX_should_return_error_when_rx_is_null);
  RUN_TEST(test_SPI_TXRX_should_return_error_when_tx_is_null);
  RUN_TEST(test_SPI_TXRX_should_return_error_when_direction_is_halfduplex);
  RUN_TEST(test_SPI_TXRX_should_return_error_when_direction_is_2lines_rxonly);
  RUN_TEST(test_SPI_TXRX_zero_length_should_return_ok);
  RUN_TEST(test_SPI_TXRX_null_spi_should_return_error);
  RUN_TEST(test_SPI_TXRX_should_return_timeout_when_spi_disabled);
}