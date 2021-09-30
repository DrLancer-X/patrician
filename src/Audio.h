#pragma once
#include <stdint.h>

extern void initmus();
extern void musplay(const void *inst_data, const void *mus_data);
extern void mustick();
extern void muspause();
extern void musresume();
extern void playsfx(uint8_t sfx, uint8_t tone);
extern void stopsfx();
