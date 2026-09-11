#include "stm32f446ze.h"
#include "utils.h"
#include "nvic_helper.h"

#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

#define MSEC_PER_FRAME 300
#define APPLE_TOGGLE_RATE 100
#define LED1_PIN_NUMBER 0
#define LED2_PIN_NUMBER 7
#define LED3_PIN_NUMBER 14

void _init() {
  // stub
}

u_int32_t msTick = 0;
u_int32_t appleRandomSeed = 0;
Position apple_pos = {5, 2};
volatile bool apple_visible = true;

volatile bool game_start = false;

volatile enum Direction last_direction = RIGHT;
volatile enum Direction next_direction = NONE;
volatile bool next_frame_flag = false;
volatile bool apple_toggle_flag = false;
volatile bool game_over = false;

/* clang-format off */
Position snake[MAX_SNAKE_SIZE] = { 
  {1, 6}, 
  {2, 6}, 
  {3, 6}
};
/* clang-format on */

uint8_t tail_idx = 0;
uint8_t head_idx = 2;

/* clang-format off */
uint8_t snake_lightmap[SCREEN_SIZE] = {
  0b00000000, 
  0b00000000, 
  0b00000000, 
  0b00000000,
  0b00000000, 
  0b00000000, 
  0b00000000, 
  0b00000000
};
/* clang-format on */

/* clang-format off */
uint8_t frame_lightmap[SCREEN_SIZE] = {
  0b00000000, 
  0b00000000, 
  0b00000000, 
  0b00000000,
  0b00000000, 
  0b00000000, 
  0b00000000, 
  0b00000000
};
/* clang-format on */

/* clang-format off */
const uint16_t STARTUP_COMMANDS[13] = {
    0x0100, // Clear display
    0x0200, 
    0x0300, 
    0x0400, 
    0x0500, 
    0x0600, 
    0x0700, 
    0x0800,

    0x0900, // Decode mode disabled
    0x0B07, // Scan-Limit : Display 8 digits (0-7)
   0x0A00, // Light Intensity : (1/16)
   0x0C01, // Shutdown reg : Normal Operation
   0x0F00, // Disable display test
};
/* clang-format on */

/* clang-format off */
const uint8_t GAME_OVER_FRAME[SCREEN_SIZE] = {
  0b10000001, 
  0b01000010, 
  0b00100100,                                  
  0b00011000, 
  0b00011000, 
  0b00100100,
  0b01000010, 
  0b10000001
};
/* clang-format on */

void move_snake(const Position new_pos, const bool eat_apple) {
  head_idx = (head_idx + 1U) % MAX_SNAKE_SIZE;
  snake[head_idx] = new_pos;
  if (!eat_apple) {
    // after apple eaten, snake is 1 unit longer, tail pos doesn't change
    tail_idx = (tail_idx + 1) % MAX_SNAKE_SIZE;
  }
}

void SPI3_Init() {

  // PA4 : NSS pin
  GPIOA->MODER &= ~(0b11 << (2 * 4));   // clear PA4 mode reg
  GPIOA->MODER |= (0b01 << (2 * 4));    // set PA4 mode as output
  GPIOA->OTYPER &= ~(1 << 4);           // set PA4 output type to push-pull
  GPIOA->OSPEEDR &= ~(0b11 << (2 * 4)); // clear PA4 OSPEEDR reg
  GPIOA->OSPEEDR |= (0b11 << (2 * 4));  // set PA4 output speed to high
  GPIOA->PUPDR &= ~(0b11 << (2 * 4));   // clear PA4 PUPDR reg
  GPIOA->PUPDR |= (0b10 << (2 * 4));    // set PA4 pull-down
  GPIOA->ODR |= (0b1 << 4);             // set PA4 bit

  GPIOA->MODER &= ~(0b11U << (2 * 15));   // clear PA15 mode reg
  GPIOA->MODER |= (0b01U << (2 * 15));    // set PA15 mode as output
  GPIOA->OTYPER |= (1U << 15);            // set PA15 output type to open-drain
  GPIOA->OSPEEDR &= ~(0b11U << (2 * 15)); // clear PA15 OSPEEDR reg
  GPIOA->OSPEEDR |= (0b11U << (2 * 15));  // set PA15 output speed to high
  GPIOA->PUPDR &=
      ~(0b11U << (2 * 15)); // clear PA15 PUPDR reg, set PA15 without PU or PD
  GPIOA->ODR |= (1U << 15); // set PA15 output value

  GPIOB->MODER &= ~(0b11 << (2 * 3));  // clear PB3 mode reg
  GPIOB->MODER |= (0b10 << (2 * 3));   // set PB3 mode as AF
  GPIOB->AFRL &= ~(0b1111 << (4 * 3)); // clear PB3 AFLR
  GPIOB->AFRL |= (0b0110 << (4 * 3));  // set PB3 AF as AF6

  GPIOB->MODER &= ~(0b11 << (2 * 4));  // clear PB4 mode reg
  GPIOB->MODER |= (0b10 << (2 * 4));   // set PB4 mode as AF
  GPIOB->AFRL &= ~(0b1111 << (4 * 4)); // clear PB4 AFLR
  GPIOB->AFRL |= (0b0110 << (4 * 4));  // set PB4 AF as AF6

  GPIOB->MODER &= ~(0b11 << (2 * 5));  // clear PB5 mode reg
  GPIOB->MODER |= (0b10 << (2 * 5));   // set PB5 mode as AF
  GPIOB->AFRL &= ~(0b1111 << (4 * 5)); // clear PB5 AFLR
  GPIOB->AFRL |= (0b0110 << (4 * 5));  // set PB5 AF as AF6

  SPI3->CR1 |= (0b111 << 3);            // baud rate = fPCLK/256
  SPI3->CR1 &= ~((1 << 0) | (1 << 1));  // CPHA = 0, CPOL = 0
  SPI3->CR1 |= ((1 << 14) | (1 << 15)); // Simplex transmit-only
  SPI3->CR1 |= (1 << 2);                // Master configuration
  SPI3->CR1 &= ~(1 << 7);               // MSB first
  SPI3->CR1 &= ~(1 << 13);              // CRC disabled
  SPI3->CR1 |= (1 << 9) | (1 << 8);     // SSM = 1 and SSI = 1
  SPI3->CR1 |= (1 << 2);                // Master configuration
  SPI3->CR1 |= (1 << 11);               // 16-bits frame format
  SPI3->CR2 &= ~(1 << 2);               // disable SS output
  SPI3->CR2 &= ~(1 << 7);               // mask TXE interrupt

  SPI3->CR1 |= (1 << 6); // enable SPI3
}

void SPI_Transmit(const uint16_t *data) {
  GPIOA->ODR &= ~(1 << 4); // CS LOW
  while (!((SPI3->SR) & (1 << 1))) {
  } // waiting for TXE to be set

  SPI3->DR = *data;
  while (SPI3->SR & (1 << 7)) {
  } // Attendre la fin du transfert (BSY)
  GPIOA->ODR |= (1 << 4); // CS HIGH (Latching)
}

void MAX7219_Init() {
  for (int i = 0; i < 13; i++) {
    uint16_t buffer = STARTUP_COMMANDS[i];
    SPI_Transmit(&buffer);
  }
}

void MAX7219_Display_Frame(const uint8_t *frame, const uint32_t start_digit,
                           const uint32_t frame_size) {
  for (uint32_t i = 0b0U; i < frame_size; i++) {
    uint16_t buffer = ((i + 1U + start_digit) << SCREEN_SIZE);
    buffer += frame[i];
    SPI_Transmit(&buffer);
  }
}

bool check_new_pos(const Position next_pos) {

  uint8_t next_pos_x = next_pos.X;
  uint8_t next_pos_y = next_pos.Y;
  uint8_t tail_pos_x = snake[tail_idx].X;
  uint8_t tail_pos_y = snake[tail_idx].Y;
  // if snake on the edge of the screen
  // 7 + 1 = 8 = 0b111 : for example if going down while on the botton edge
  // 0 - 1 = 0b11111111 (overflow) : for example if going up while on the upper
  // edge in both cases, the 4th bit is set
  if (((next_pos_x) & (1 << 3)) | ((next_pos_y) & (1 << 3))) {
    return false;
  }

  // if next_pos dot is already turned on, it means the snake is running into
  // itself except if this dot is snake[tail_idx], because this dot will be
  // turned off after the call of move_snake()
  if (snake_lightmap[next_pos_y] & (0b10000000U >> next_pos_x)) {
    if ((next_pos_x == tail_pos_x) & (next_pos_y == tail_pos_y)) {
      return true;
    }
    return false;
  }
  return true;
}

bool is_apple_eaten(const Position next_pos) {
  if (next_pos.X == apple_pos.X && next_pos.Y == apple_pos.Y) {
    return true;
  }
  return false;
}

void generate_new_apple_pos() {
  uint8_t randomX = appleRandomSeed & 0b111U;
  uint8_t randomY = (appleRandomSeed >> 3) & 0b111U;

  // if random apple position is already occupied by the snake
  // new apple pos is the pos where the snake's tail used to be
  if (snake_lightmap[randomY] & (0b10000000U >> randomX)) {
    Position last_tail_pos =
        snake[(tail_idx - 1) & 0b111111U]; // snake[(tail_idx -1) % 64]
    apple_pos.X = last_tail_pos.X;
    apple_pos.Y = last_tail_pos.Y;
  } else {
    apple_pos.X = randomX;
    apple_pos.Y = randomY;
  }
}

void update_lightmap() {
  // if next_direction (e.g UP) is not compatible with last_direction (e.g DOWN)
  if (!(next_direction & last_direction)) {
    next_direction = last_direction;
  }

  Position next_pos = snake[head_idx];
  switch (next_direction) {
  case UP:
    next_pos.Y--;
    break;
  case DOWN:
    next_pos.Y++;
    break;
  case LEFT:
    next_pos.X--;
    break;
  case RIGHT:
  default:
    next_pos.X++;
    break;
  }

  if (!check_new_pos(next_pos)) {
    game_over = true;
    MAX7219_Display_Frame(&GAME_OVER_FRAME[0], 0, SCREEN_SIZE);
    return;
  }

  bool apple_eaten = is_apple_eaten(next_pos);

  move_snake(next_pos, apple_eaten);
  Position new_head_pos = snake[head_idx];
  Position old_tail_pos = snake[(tail_idx - 1) % MAX_SNAKE_SIZE];

  snake_lightmap[old_tail_pos.Y] &= ~(0b10000000U >> old_tail_pos.X);
  // the head may now be where the tail used to be, so we draw head's dot after
  // erasing the tail's one
  snake_lightmap[new_head_pos.Y] |= (0b10000000U >> new_head_pos.X);

  MAX7219_Display_Frame(&snake_lightmap[0], 0, SCREEN_SIZE);

  if (apple_eaten) {
    generate_new_apple_pos();
    uint8_t apple_dot = snake_lightmap[apple_pos.Y] | (0b10000000U >> apple_pos.X);
    MAX7219_Display_Frame(&apple_dot, apple_pos.Y, 1);
  }

  last_direction = next_direction;
  next_direction = NONE;
}

void Game_Init() {
  MAX7219_Init();
  for (int i = tail_idx; i <= head_idx; i++) {
    snake_lightmap[snake[i].Y] |= (0b10000000U >> snake[i].X);
  }

  MAX7219_Display_Frame(&snake_lightmap[0], 0, SCREEN_SIZE);

  uint8_t apple_dot =
      snake_lightmap[apple_pos.Y] | (0b10000000U >> apple_pos.X);

  MAX7219_Display_Frame(&apple_dot, apple_pos.Y, 1);
}

void SysTick_Init() {
  STK->LOAD = 0x3E80;       // 16000 in hexadecimal -> 1 ms @ 16 MHz
  STK->VAL = 0x0;           // Clear current value
  STK->CTRL |= (0b1U << 0); // enable counter
  STK->CTRL |= (0b1U << 1); // enable SysTick exception request
  STK->CTRL |= (0b1U << 2); // use CPU clock (16MHz)
}

void EXTI_Init() {
  SYSCFG->EXTICR4 |= (0b0011U << (4 * 2)); // PD14 -> EXTI14
  SYSCFG->EXTICR4 |= (0b0011U << (4 * 3)); // PD15 -> EXTI15

  EXTI->RTSR |= (1U << 14); // Rising edge trigger on EXTI14
  EXTI->RTSR |= (1U << 15); // Rising edge trigger on EXTI15
  EXTI->PR = (1U << 14) |
             (1U << 15); // clearing pending interrupts triggered during init
  EXTI->IMR |= (1U << 14) | (1U << 15); // enable EXTI14 and EXTI15

  NVIC->ISER[1] |= (1 << (40 - 32)); // enable EXTI15_10

  SYSCFG->EXTICR2 &= ~(0b1111U << (4 * (6 - 4))); // PA6 -> EXTI6
  SYSCFG->EXTICR2 &= ~(0b1111U << (4 * (7 - 4))); // PA7 -> EXTI7

  EXTI->RTSR |= (1U << 6); // Rising edge trigger on EXTI6
  EXTI->RTSR |= (1U << 7); // Rising edge trigger on EXTI7
  EXTI->PR = (1U << 6) |
             (1U << 7); // clearing pending interrupts triggered during init
  EXTI->IMR |= (1U << 6) | (1U << 7); // enable EXTI14 and EXTI15

  NVIC->ISER[0] |= (1 << (23 - 0)); // enable EXTI9_5
}

void EXTI9_5_IRQHandler() {
  if (next_direction == NONE) {
    appleRandomSeed = STK->VAL;
  }
  if ((EXTI->PR) & (1U << 6)) {
    next_direction = RIGHT; // PA6
    EXTI->PR = (1U << 6);
  }
  if ((EXTI->PR) & (1U << 7)) {
    next_direction = LEFT; // PA7
    EXTI->PR = (1U << 7);
  }
}

void EXTI15_10_IRQHandler() {
  if (next_direction == NONE) {
    appleRandomSeed = STK->VAL;
  }
  if ((EXTI->PR) & (1U << 14)) {
    next_direction = DOWN;
    EXTI->PR = (1U << 14);
  }
  if ((EXTI->PR) & (1U << 15)) {
    next_direction = UP;
    EXTI->PR = (1U << 15);
  }
}

void SysTick_Handler() {
  msTick++;
  if (((msTick % MSEC_PER_FRAME) == 0)) {
    next_frame_flag = true;
  }
  if (((msTick % APPLE_TOGGLE_RATE) == 0)) {
    apple_toggle_flag = true;
  }
}

void RCC_Init() {
  // enable GPIOA, GPIOB and GPIOD
  RCC_AHB1ENR |= (1U << 0) | (1U << 1) | (1U << 3);
  RCC_APB2ENR |= (1U << 14); // enable SYS_CFG clock
  RCC_APB1ENR |= (1 << 15);  // power SPI3 peripheral
}

int main() {
  RCC_Init();

  GPIOB->MODER |= (1 << (2 * LED1_PIN_NUMBER)) | (1 << (2 * LED2_PIN_NUMBER)) |
                  (1 << (2 * LED3_PIN_NUMBER));

  GPIOD->MODER &=
      ~((0b11U << (2 * 14)) | (0b11U << (2 * 15))); // PD14 and PD15 are inputs

  GPIOD->PUPDR &= ~(0b1111U << (2 * 14));
  GPIOD->PUPDR |=
      ((0b10U << (2 * 14)) | (0b10U << (2 * 15))); // pull-down on PD14 and PD15

  GPIOA->MODER &=
      ~((0b11U << (2 * 6)) | (0b11U << (2 * 7))); // PA6 and PA7 are inputs

  GPIOA->PUPDR &= ~(0b1111U << (2 * 6));
  GPIOA->PUPDR |=
      ((0b10U << (2 * 6)) | (0b10U << (2 * 7))); // pull-down on PA6 and PA7
  
  SysTick_Init();
  EXTI_Init();
  SPI3_Init();
  Game_Init();

  while (!(next_direction & last_direction)) {
    (void)42;
  }

  while (!game_over) {
    if (apple_toggle_flag) {
      uint8_t buffer;
      if (apple_visible) {
        buffer = snake_lightmap[apple_pos.Y] | (0b10000000U >> apple_pos.X);
        apple_visible = false;
      } else {
        buffer = snake_lightmap[apple_pos.Y];
        apple_visible = true;
      }
      MAX7219_Display_Frame(&buffer, apple_pos.Y, 1);
      apple_toggle_flag = false;
    }

    if (next_frame_flag) {
      __disable_irq();
      GPIOB->ODR |= (1 << LED3_PIN_NUMBER);
      update_lightmap();
      GPIOB->ODR &= ~(1 << LED3_PIN_NUMBER);
      next_frame_flag = false;
      __enable_irq();
    }
  }
  return 0;
}