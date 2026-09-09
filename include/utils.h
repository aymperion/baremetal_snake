#include <stdint.h>
#define SCREEN_SIZE 8
#define MAX_SNAKE_SIZE 64U // screen size = 8 * 8 = 64

typedef struct {
  uint8_t X;
  uint8_t Y;
} Position;

enum Direction {
  UP    = 0b0011U,
  RIGHT = 0b1001U,
  DOWN  = 0b1100U,
  LEFT  = 0b0110U,
  NONE  = 0b0000U
}; // values are chosen so that UP & DOWN == 0b0000, LEFT & RIGHT == 0b0000
   // and NONE & X == 0b0000
