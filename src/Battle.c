#pragma bank 5

#include <gb/gb.h>
#include <gb/cgb.h>
#include <gb/bgb_emu.h>
#include <stdint.h>
#include <string.h>
#include <rand.h>
#include <stdio.h>

#include "Battle.h"
#include "Exploration.h"

#include "Audio.h"
#include "GenData.h"
#include "MusicInformation.h"
#include "Globals.h"
#include "Sprites.h"

const uint8_t *Ships[] = {GD_Ships_pship0_c, GD_Ships_pship1_c, GD_Ships_pship2_c, GD_Ships_ship0_c, GD_Ships_ship1_c, GD_Ships_ship2_c, GD_Ships_ship3_c, GD_Ships_ship4_c, GD_Ships_ship5_c, GD_Ships_ship6_c, GD_Ships_ship7_c, GD_Ships_ship8_c, GD_Ships_ship9_c, GD_Ships_ship10_c, GD_Ships_ship11_c, GD_Ships_ship12_c, GD_Ships_ship13_c, GD_Ships_ship14_c, GD_Ships_ship15_c, GD_Ships_ship16_c, GD_Ships_ship17_c, GD_Ships_ship18_c, GD_Ships_ship19_c};

const uint8_t Ships_BANK[] = {GD_Ships_pship0_c_BANK, GD_Ships_pship1_c_BANK, GD_Ships_pship2_c_BANK, GD_Ships_ship0_c_BANK, GD_Ships_ship1_c_BANK, GD_Ships_ship2_c_BANK, GD_Ships_ship3_c_BANK, GD_Ships_ship4_c_BANK, GD_Ships_ship5_c_BANK, GD_Ships_ship6_c_BANK, GD_Ships_ship7_c_BANK, GD_Ships_ship8_c_BANK, GD_Ships_ship9_c_BANK, GD_Ships_ship10_c_BANK, GD_Ships_ship11_c_BANK, GD_Ships_ship12_c_BANK, GD_Ships_ship13_c_BANK, GD_Ships_ship14_c_BANK, GD_Ships_ship15_c_BANK, GD_Ships_ship16_c_BANK, GD_Ships_ship17_c_BANK, GD_Ships_ship18_c_BANK, GD_Ships_ship19_c_BANK};

void battleVram() NONBANKED
{
  savedbank = _current_bank;
  VBK_REG = 0;
  
  uint8_t bg_chr, bg_col;
  SWITCH_ROM_MBC5(GD_Ships_bt_BANK);
  bg_col = GD_Ships_bt[eship_model * 2 + 0];
  bg_chr = GD_Ships_bt[eship_model * 2 + 1];
  
  memset((void *)0x9800, bg_chr, 1024);
  
  SWITCH_ROM_MBC5(Ships_BANK[eship_model]);
  set_bkg_data(128, 128, Ships[eship_model]);
  SWITCH_ROM_MBC5(Ships_BANK[pship_model]);
  set_bkg_data(0, 128, Ships[pship_model] + 2048);
  SWITCH_ROM_MBC5(GD_Ships_t_BANK);
  set_bkg_tiles(0, 3, 20, 10, GD_Ships_t + (400 * pship_model) + 200);
  set_bkg_tiles(0, 19, 20, 10, GD_Ships_t + (400 * eship_model));
  SWITCH_ROM_MBC5(GD_Ships_a_BANK);
  VBK_REG = 1;
  memset((void *)0x9800, bg_col, 1024);
  set_bkg_tiles(0, 3, 20, 10, GD_Ships_a + (400 * pship_model) + 200);
  set_bkg_tiles(0, 19, 20, 10, GD_Ships_a + (400 * eship_model));
  SWITCH_ROM_MBC5(GD_Ships_p_BANK);
  set_bkg_palette(0, 4, GD_Ships_p + (64 * eship_model));
  set_bkg_palette(4, 4, GD_Ships_p + (64 * pship_model + 32));
  SET_SPRITE_Y(0, 0);
  SET_SPRITE_Y(1, 0);
  SET_SPRITE_Y(2, 0);
  SET_SPRITE_Y(3, 0);
  
  SCY_REG = 112;
  SCX_REG = 0;
  /*
  SWITCH_ROM_MBC5(GD_SpaceBG_c_BANK);
  VBK_REG = 0;
  set_bkg_data(128, 128, GD_SpaceBG_c);
  set_bkg_data(0, 128, GD_SpaceBG_c + (128 * 16));
  VBK_REG = 1;
  set_bkg_data(128, 128, GD_SpaceBG_c + (256 * 16));
  set_bkg_data(0, 70, GD_SpaceBG_c + (384 * 16));
  set_sprite_data(0, 1, GD_SpaceBG_c + (454 * 16));
  
  SWITCH_ROM_MBC5(GD_SpaceBG_p_BANK);
  set_bkg_palette(0, 8, GD_SpaceBG_p);
  SWITCH_ROM_MBC5(GD_OShips_p_BANK);
  set_sprite_palette(0, 1, GD_OShips_p + (pship_model << 3));
  
  SWITCH_ROM_MBC5(OShips_c_BANK[pship_model]);
  VBK_REG = 0;
  set_sprite_data(0, 128, OShips_c[pship_model]);
  
  musresume();
  uint8_t selsong = (pship_system & 3) << 1;
  //musplay(Inst_noname9, Music_noname9);
  musplay(planet_music[selsong], planet_music[selsong + 1]);
  */
  SWITCH_ROM_MBC5(savedbank);
}

uint8_t pl_h = 100;
uint8_t en_h = 100;

uint8_t en_y = 16;
uint8_t pl_y = 16;
uint8_t en_c = 0;
uint8_t pl_c = 0;
uint8_t cam = 0;

uint8_t en_movd = 0;
uint8_t en_movt = 8;

uint8_t plas_x[16];
uint8_t plas_y[16];
uint8_t plas_t[16];

uint8_t elas_x[16];
uint8_t elas_y[16];
uint8_t elas_t[16];
/*
void shootbeam() NONBANKED
{
  savedbank = _current_bank;
  SWITCH_ROM_MBC5(GD_Ships_d_BANK);
  const uint8_t *data = &GD_Ships_d[pship_model * 64 + 32];
  for (;;) {
    uint8_t i = (rand() & 15) << 1;
    if (data[i+0] || data[i+1]) {
      uint8_t j = 0;
      
    } else continue;
    b--;
    if (b == 0) break;
  }
  SWITCH_ROM_MBC5(savedbank);
}
*/
void battle() BANKED
{
  eship_model = 255;
  while (eship_model > 22) {
    eship_model = rand();
  }
  
  battleVram();
  DISPLAY_ON;
  
  for (;;) {
    exploreFrame();
    wait_vbl_done();
    joystate = joypad();
    if (joystate & J_DOWN && pl_y > 0) pl_y--;
    else if (joystate & J_UP && pl_y < 32) pl_y++;
    
    if (joystate & J_LEFT) cam = 0;
    else if (joystate & J_RIGHT) cam = 1;
    
    if (en_movt > 0) {
      en_y += en_movd;
      if (en_y == 255) en_y = 0;
      else if (en_y == 33) en_y = 32;
      en_movt--;
    } else {
      switch (rand() & 3) {
        case 0:
        case 1:
          en_movd = 0;
          break;
        case 2:
          en_movd = 255;
          break;
        case 3:
          en_movd = 1;
          break;
      }
      en_movt = rand() & 15 + 4;
    }
    
    if (cam == 0) {
      SCY_REG = 232 + pl_y;
    } else {
      SCY_REG = 88 + en_y;
    }
    
    if (pl_c > 0) pl_c--;
    /*
    if (joystate & J_A) {
      if (pl_c == 0) {
        playsfx(0x01, 24);
        pl_c = 30;
        uint8_t b = rand() & 3 + 1;
        while (b > 0) {
          shootbeam();
          b--;
        }
      }
    }
    */
    
    if (joystate & J_A) break;
    
  }

  DISPLAY_OFF;
}
