#pragma once
#include <stdint.h>

#define OAM_BYTE(n, i) (*((uint8_t *)( (n << 2) + 0xFE00 + i )))
#define SET_SPRITE_Y(n, v) (OAM_BYTE((n), 0) = (v))
#define SET_SPRITE_X(n, v) (OAM_BYTE((n), 1) = (v))
#define SET_SPRITE_I(n, v) (OAM_BYTE((n), 2) = (v))
#define SET_SPRITE_A(n, v) (OAM_BYTE((n), 3) = (v))
