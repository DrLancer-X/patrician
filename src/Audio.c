#include <gb/gb.h>
#include <stdint.h>
#include "SoundSystem.h"
#include "Globals.h"
#include "Audio.h"

#pragma bank 0

void initmus() {
__asm
  push bc
  push de
	call #GBS_SoundSystem_Init
	ld bc, #1
	ld de, #(_SFX_Table)
	call #GBS_SFX_Prepare
	pop bc
	pop de
__endasm;
}
void musplay(const void *inst_data, const void *mus_data) {
__asm
  push af
  push bc
  push de
  push hl
  ldhl sp, #11
  ld a, (hl-)
  ld e, (hl)
  ld d, a
  ld bc, #1
	call #GBS_Music_PrepareInst
	ldhl sp, #13
	ld a, (hl-)
	ld e, (hl)
	ld d, a
	ld bc, #1
	call #GBS_Music_Play
	pop af
	pop bc
	pop de
	pop hl
__endasm;
  // Remove warnings
  inst_data = inst_data;
  mus_data = mus_data;
}

void mustick() {
__asm
  push af
  push bc
  push de
  push hl
	call #GBS_SoundSystem_Process
  pop af
  pop bc
  pop de
  pop hl
__endasm;
  //if (!GBS_Vars.wMusicPlayState) musplay(Inst_noname9, Music_noname9);
}
void muspause() {
__asm
  push af
  push bc
  push de
  push hl
	call #GBS_Music_Pause
  pop af
  pop bc
  pop de
  pop hl
__endasm;
}
void musresume() {
__asm
  push af
  push bc
  push de
  push hl
	call #GBS_Music_Resume
  pop af
  pop bc
  pop de
  pop hl
__endasm;
}
void playsfx(uint8_t sfx, uint8_t tone) {
__asm
  push af
  push bc
  push de
  push hl
  
  ldhl sp, #10
  ld a, (hl-)
  ld c, (hl)
  ld b, a
	call #GBS_SFX_Play
  pop af
  pop bc
  pop de
  pop hl
__endasm;
  // Remove warnings
  sfx = sfx;
  tone = tone;
}
void stopsfx() {
__asm
  push af
  push bc
  push de
  push hl
	call #GBS_SFX_Stop
  pop af
  pop bc
  pop de
  pop hl
__endasm;
}
