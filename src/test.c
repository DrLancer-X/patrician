#include <gb/gb.h>
#include <gb/cgb.h>
#include <gb/bgb_emu.h>
#include <stdint.h>
#include <stdio.h>
#include <rand.h>

#include "Sprites.h"
#include "Exploration.h"

#include "Audio.h"
#include "MusicInformation.h"
#include "GenData.h"
#include "Globals.h"

const void *all_music[] = {
  Inst_noname4, Music_noname4,
  Inst_noname5, Music_noname5,
  Inst_noname6, Music_noname6,
  Inst_noname7, Music_noname7,
  Inst_noname8, Music_noname8,
  Inst_noname9, Music_noname9,
  Inst_noname10, Music_noname10,
  Inst_noname11, Music_noname11,
  Inst_noname12, Music_noname12,
  Inst_noname13, Music_noname13,
  Inst_noname14, Music_noname14,
  Inst_noname15, Music_noname15,
  Inst_noname16, Music_noname16,
  Inst_noname17, Music_noname17,
  Inst_noname18, Music_noname18,
  Inst_name, Music_name,
  Inst_name3, Music_name3,
  Inst_named, Music_named,
  Inst_noname, Music_noname,
  Inst_noname3, Music_noname3,
  Inst_noname19, Music_noname19 };
uint8_t next_track = 0;
uint8_t chg_timeout = 0;
void main() {
  DISABLE_OAM_DMA;
  // Init everything
  cpu_fast();
  initrand(DIV_REG);
  for (uint8_t i = 0; i < 40; i++) {
    SET_SPRITE_Y(i, 0);
  }
  ENABLE_RAM_MBC5;
  
  SPRITES_8x8;
  SHOW_SPRITES;
  
  tempval_ptr = GD_Title_p + 15 * 56;
  tempval_a = 255;
  menucur = 2;
  initmus();
  //if (menucur == 2) goto begin_game;
  SWITCH_ROM_MBC5(GD_Title_c_BANK);
  set_bkg_data(128, 128, GD_Title_c);
  set_bkg_data(0, 128, GD_Title_c + 2048);
  VBK_REG = 1;
  set_bkg_data(128, 111, GD_Title_c + 4096);
  SWITCH_ROM_MBC5(GD_Title_a_BANK);
  set_bkg_tiles(0, 0, 32, 32, GD_Title_a);
  VBK_REG = 0;
  SWITCH_ROM_MBC5(GD_Title_t_BANK);
  set_bkg_tiles(0, 0, 32, 32, GD_Title_t);
  SWITCH_ROM_MBC5(GD_Title_p_BANK);
  //set_bkg_palette(0, GD_Title_p_SIZE / 8, GD_Title_p);
  
  set_bkg_palette(0, 7, tempval_ptr);

  musplay(Inst_noname19, Music_noname19);
  
  
  
  for (;;) {
      mustick();
      wait_vbl_done();
      joystate = joypad();
      if (((joystate & (J_UP | J_SELECT)) && (menucur == 6)) || 
          ((joystate & (J_DOWN | J_SELECT)) && (menucur == 0))) {
        playsfx(0x07, 24);
        menucur = (menucur == 6 ? 0 : 6);
        SWITCH_ROM_MBC5(GD_Title_a_BANK);
        VBK_REG = 1;
        set_bkg_tiles(7, 13, 6, 1, GD_Title_a + 28*32 + menucur);
        set_bkg_tiles(7, 14, 6, 1, GD_Title_a + 29*32 + menucur);
        set_bkg_tiles(7, 15, 6, 1, GD_Title_a + 30*32 + menucur);
        set_bkg_tiles(7, 16, 6, 1, GD_Title_a + 31*32 + menucur);
        SWITCH_ROM_MBC5(GD_Title_t_BANK);
        VBK_REG = 0;
        set_bkg_tiles(7, 13, 6, 1, GD_Title_t + 28*32 + menucur);
        set_bkg_tiles(7, 14, 6, 1, GD_Title_t + 29*32 + menucur);
        set_bkg_tiles(7, 15, 6, 1, GD_Title_t + 30*32 + menucur);
        set_bkg_tiles(7, 16, 6, 1, GD_Title_t + 31*32 + menucur);
      }
      if (chg_timeout > 0) chg_timeout--;
      if (joystate & (J_START | J_A) && (menucur != 2)) {
        if (menucur == 0) {
          muspause();
          playsfx(0x11, 24);
          
          tempval_a = 0;
          break;
        } else {
          if (chg_timeout == 0) {
            musplay(all_music[next_track], all_music[next_track + 1]);
            next_track += 2;
            chg_timeout = 8;
            if (next_track == 42) next_track = 0;
          }
        }
      }
      
      
      if (tempval_a > 0) {
        move_bkg(0, tempval_a / 8);
        if (tempval_a >= 241) {
          tempval_ptr -= 56;
          SWITCH_ROM_MBC5(GD_Title_p_BANK);
          set_bkg_palette(0, 7, tempval_ptr);
        }
        
        if (tempval_a <= 15) {
          tempval_ptr += 56;
          SWITCH_ROM_MBC5(GD_Title_p_BANK);
          set_bkg_palette(0, 7, tempval_ptr);
        }
        
        tempval_a--;
        
        if (tempval_a == 0) {
          // Copy from 0,22 20x4 to 0,5
          SWITCH_ROM_MBC5(GD_Title_a_BANK);
          VBK_REG = 1;
          set_bkg_tiles(0, 5, 20, 1, GD_Title_a + 22*32 + 0);
          set_bkg_tiles(0, 6, 20, 1, GD_Title_a + 23*32 + 0);
          set_bkg_tiles(0, 7, 20, 1, GD_Title_a + 24*32 + 0);
          set_bkg_tiles(0, 8, 20, 1, GD_Title_a + 25*32 + 0);
          set_bkg_tiles(7, 0, 6, 1, GD_Title_a + 26*32 + 7);
          
          set_bkg_tiles(7, 13, 6, 1, GD_Title_a + 28*32 + 0);
          set_bkg_tiles(7, 14, 6, 1, GD_Title_a + 29*32 + 0);
          set_bkg_tiles(7, 15, 6, 1, GD_Title_a + 30*32 + 0);
          set_bkg_tiles(7, 16, 6, 1, GD_Title_a + 31*32 + 0);
          SWITCH_ROM_MBC5(GD_Title_t_BANK);
          VBK_REG = 0;
          set_bkg_tiles(0, 5, 20, 1, GD_Title_t + 22*32 + 0);
          set_bkg_tiles(0, 6, 20, 1, GD_Title_t + 23*32 + 0);
          set_bkg_tiles(0, 7, 20, 1, GD_Title_t + 24*32 + 0);
          set_bkg_tiles(0, 8, 20, 1, GD_Title_t + 25*32 + 0);
          set_bkg_tiles(7, 0, 6, 1, GD_Title_t + 26*32 + 7);
          
          set_bkg_tiles(7, 13, 6, 1, GD_Title_t + 28*32 + 0);
          set_bkg_tiles(7, 14, 6, 1, GD_Title_t + 29*32 + 0);
          set_bkg_tiles(7, 15, 6, 1, GD_Title_t + 30*32 + 0);
          set_bkg_tiles(7, 16, 6, 1, GD_Title_t + 31*32 + 0);
          menucur = 0;
        }
      } else {
        if (tempval_ptr >= GD_Title_p) {
          SWITCH_ROM_MBC5(GD_Title_p_BANK);
          set_bkg_palette(0, 7, tempval_ptr);
          tempval_ptr -= 56;
        }
      }
      
      /*
      i++;
      if (i == 20 && sizeof(GBS_Vars) != 87) {
        playsfx(8, 24);
        i = 0;
      }
      */
  }
  while (tempval_a <= 15) {
    mustick();
    wait_vbl_done();
    tempval_ptr += 56;
    SWITCH_ROM_MBC5(GD_Title_p_BANK);
    set_bkg_palette(0, 7, tempval_ptr);
    tempval_a++;
  }
  stopsfx();
  /*
  while (tempval_a <= 120) {
    mustick();
    wait_vbl_done();
    tempval_a++;
  }
  */
  
  begin_game:
  DISPLAY_OFF;
  
  exploration();
}
