#include "test_spi.h"
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

static void spi_setUp(void) {
  RCC_Enable_Clock_SPIx(TEST_SPI);
  TEST_SPI->CR1 = 0;
  TEST_SPI->CR2 = 0;
}
static void spi_tearDown(void) {
  TEST_SPI->CR1 = 0;
  TEST_SPI->CR2 = 0;
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
  SPI_Init_t config = {
    .Mode      = SPI_MODE_MASTER,
    .Direction = SPI_DIRECTION_2LINES,
    .DataSize  = SPI_DATASIZE_16BIT,
    .CPOL      = SPI_CPOL_HIGH,
    .CPHA      = SPI_CPHA_2EDGE,
    .BaudRate  = SPI_BAUDRATE_DIV64,
    .FirstBit  = SPI_FIRSTBIT_LSB
  };
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

void test_SPI_main(void) {
  g_current_suite_setUp = spi_setUp;
  g_current_suite_tearDown = spi_tearDown;
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
}