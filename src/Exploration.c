#pragma bank 4

#include <gb/gb.h>
#include <gb/cgb.h>
#include <gb/bgb_emu.h>
#include <stdint.h>
#include <string.h>
#include <rand.h>
#include <stdio.h>

#include "Exploration.h"

#include "Audio.h"
#include "GenData.h"
#include "MusicInformation.h"
#include "Globals.h"
#include "Sprites.h"
#include "Battle.h"

const uint8_t *OShips_c[] = {GD_OShip0_c, GD_OShip1_c, GD_OShip2_c, GD_OShip3_c, GD_OShip4_c, GD_OShip5_c, GD_OShip6_c, GD_OShip7_c, GD_OShip8_c, GD_OShip9_c, GD_OShip10_c, GD_OShip11_c, GD_OShip12_c, GD_OShip13_c, GD_OShip14_c, GD_OShip15_c, GD_OShip16_c, GD_OShip17_c, GD_OShip18_c, GD_OShip19_c, GD_OShip20_c, GD_OShip21_c, GD_OShip22_c};
const uint8_t OShips_c_BANK[] = {GD_OShip0_c_BANK, GD_OShip1_c_BANK, GD_OShip2_c_BANK, GD_OShip3_c_BANK, GD_OShip4_c_BANK, GD_OShip5_c_BANK, GD_OShip6_c_BANK, GD_OShip7_c_BANK, GD_OShip8_c_BANK, GD_OShip9_c_BANK, GD_OShip10_c_BANK, GD_OShip11_c_BANK, GD_OShip12_c_BANK, GD_OShip13_c_BANK, GD_OShip14_c_BANK, GD_OShip15_c_BANK, GD_OShip16_c_BANK, GD_OShip17_c_BANK, GD_OShip18_c_BANK, GD_OShip19_c_BANK, GD_OShip20_c_BANK, GD_OShip21_c_BANK, GD_OShip22_c_BANK};

// Much of this is from the gbdk large_map example. MIT license.

#define bigmap_mapWidth 92
#define bigmap_mapHeight 89
#define bigmap_map (uint8_t *)0xA000
#define camera_max_y ((bigmap_mapHeight - 18) * 8) 
#define camera_max_x ((bigmap_mapWidth - 20) * 8) 

#define MIN(A,B) ((A)<(B)?(A):(B))

void getGalaxyPos() NONBANKED
{
  savedbank = _current_bank;
  SWITCH_ROM_MBC5(GD_SolarXY_BANK);
  SET_SPRITE_X(38, pship_system);
  galaxy_x = GD_SolarXY[pship_system + pship_system + 0] << 3;
  SET_SPRITE_X(39, pship_system);
  galaxy_y = GD_SolarXY[pship_system + pship_system + 1] << 3;
  //galaxy_x = GD_SolarXY[0] << 3;
  //galaxy_y = GD_SolarXY[1] << 3;
  //SET_SPRITE_X(38, galaxy_x);
  //SET_SPRITE_X(39, galaxy_y);
  SWITCH_ROM_MBC5(savedbank);
}

void fillSpace() NONBANKED
{
  SWITCH_RAM_MBC5(0);
  //if (((uint8_t *)0xBFFF)[0] == pship_system) return;
  //((uint8_t *)0xBFFF)[0] = pship_system;
  
  savedbank = _current_bank;
  
  SWITCH_ROM_MBC5(GD_SolarSystem_BANK);
  uint8_t *dest = (uint8_t *)0xA000;
  const uint8_t *systemdata = GD_SolarSystem + 98u * (uint16_t)pship_system;
  
  const uint8_t *src = (const uint8_t *)(((const uint16_t *)systemdata)[0]);
  SWITCH_ROM_MBC5(GD_StarrySky_BANK);
  for (uint8_t y = 0; y < bigmap_mapHeight; y++) {
    memcpy(dest, src, bigmap_mapWidth);
    src += 128;
    dest += bigmap_mapWidth;
  }
  SWITCH_RAM_MBC5(1);
  memset((uint8_t *)0xA000, 0, bigmap_mapWidth * bigmap_mapHeight);
  SWITCH_ROM_MBC5(GD_SolarSystem_BANK);
  const uint8_t *systemdata_p = systemdata + 2 + 16 + 16;
  
  for (uint8_t i = 0; i < 8; i++) {
    uint8_t x = *(systemdata_p++);
    if (x == 0) {
      systemdata_p += 7;
      continue;
    }
    uint8_t y = *(systemdata_p++);
    uint8_t sz = *(systemdata_p++);
    uint8_t vrbank = *(systemdata_p++);
    uint8_t char_pos = *(systemdata_p++);
    
    uint8_t *pt = (uint8_t *)0xA000 + (y * 92) + x;
    for (uint8_t j = 0; j < sz; j++) {
      for (uint8_t k = 0; k < sz; k++) {
        SWITCH_RAM_MBC5(0);
        *pt = char_pos++;
        SWITCH_RAM_MBC5(1);
        *pt = vrbank;
        pt++;
      }
      pt += (92 - sz);
    }
    uint8_t station_x = *(systemdata_p++);
    if (station_x == 0) {
      systemdata_p += 2;
      continue;
    }
    uint8_t station_y = *(systemdata_p++);
    uint8_t station_chr = *(systemdata_p++);
    pt = (uint8_t *)0xA000 + (station_y * 92) + station_x;
    SWITCH_RAM_MBC5(0);
    *pt = station_chr;
    SWITCH_RAM_MBC5(1);
    *pt = 7;
  }
  
  SWITCH_ROM_MBC5(savedbank);
}

const void *planet_music[] = {
  Inst_noname, Music_noname,
  Inst_noname3, Music_noname3,
  Inst_noname9, Music_noname9,
  Inst_noname10, Music_noname10,
  Inst_noname11, Music_noname11,
  Inst_noname6, Music_noname6,
  Inst_noname13, Music_noname13,
  Inst_noname16, Music_noname16,
};

void exploreVram() NONBANKED
{
  savedbank = _current_bank;
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
  
  SWITCH_ROM_MBC5(savedbank);
}

const void *battle_music[] = {
  Inst_noname4, Music_noname4,
  Inst_noname12, Music_noname12,
  Inst_noname15, Music_noname15,
  Inst_noname17, Music_noname17
};

void battlesong() NONBANKED
{
  savedbank = _current_bank;
  uint8_t selsong = (rand() & 3) << 1;
  musplay(battle_music[selsong], battle_music[selsong + 1]);
  SWITCH_ROM_MBC5(savedbank);
}

void exploreSprites()
{
  uint8_t a = pship_ang >> 3;
  for (uint8_t i = 0; i < 4; i++) {
    SET_SPRITE_I(i, (a << 2) | i);
    SET_SPRITE_A(i, 0);
  }
  SET_SPRITE_X(0, 80); SET_SPRITE_Y(0, 80);
  SET_SPRITE_X(1, 88); SET_SPRITE_Y(1, 80);
  SET_SPRITE_X(2, 80); SET_SPRITE_Y(2, 88);
  SET_SPRITE_X(3, 88); SET_SPRITE_Y(3, 88);
}

void exploreSpriteFrame()
{
  uint8_t a = pship_ang >> 3;
  SET_SPRITE_I(0, (a << 2) | 0);
  SET_SPRITE_I(1, (a << 2) | 1);
  SET_SPRITE_I(2, (a << 2) | 2);
  SET_SPRITE_I(3, (a << 2) | 3);
}

#define COS(x) (*( ((uint16_t *)GD_SinCos) + (x) + 0*512))
#define SIN(x) (*( ((uint16_t *)GD_SinCos) + 256 + (x) + 0*512))

void explodeAdvance() NONBANKED
{
  savedbank = _current_bank;
  SWITCH_ROM_MBC5(GD_SinCos_BANK);
  pship_vx += COS(pship_ang);
  pship_vy += SIN(pship_ang);
  SWITCH_ROM_MBC5(savedbank);
}

void exploreFrame() NONBANKED
{
  savedbank = _current_bank;
  mustick();
  SWITCH_ROM_MBC5(savedbank);
}

uint8_t show_system_name(uint16_t idx) NONBANKED
{
  savedbank = _current_bank;
  VBK_REG = 0;
  SWITCH_ROM_MBC5(GD_Galaxy_i_BANK);
  uint8_t i = GD_Galaxy_i[idx];
  uint8_t *dest = ((uint8_t *)0x9800 + 17*32);
  if (i == 255) {
    memset(dest, 16, 20);
  } else {
    SWITCH_ROM_MBC5(GD_PlanetNames_BANK);
    const uint8_t *p = GD_PlanetNames + ((uint16_t)i << 8);
    memcpy(dest, p, 20);
  }
  SWITCH_ROM_MBC5(savedbank);
  return i;
}
void show_planet_name(uint8_t sslot) NONBANKED
{
  savedbank = _current_bank;
  SWITCH_ROM_MBC5(GD_SolarSystem_BANK);
  const uint8_t *systemdata = GD_SolarSystem + 98u * (uint16_t)pship_system;
  uint8_t body = systemdata[2 + sslot] - 1;
  SWITCH_ROM_MBC5(GD_PlanetNames_BANK);
  const uint8_t *p = GD_PlanetNames + ((uint16_t)pship_system << 8) + ((uint16_t)body << 5);
  uint8_t *dest = ((uint8_t *)0x9C00);
  
  VBK_REG = 0;
  memcpy(dest, p, 20);
  //dest[0] = body + 16;
  SHOW_WIN;
  SWITCH_ROM_MBC5(savedbank);
}

void drawExploreScreen()
{
  wait_vbl_done();
  exploreSpriteFrame();

  joystate = joypad();
  
  SWITCH_RAM_MBC5(0);
  
  // update hardware scroll position
  SCY_REG = camera_y; SCX_REG = camera_x; 
  map_pos_x = (uint8_t)(camera_x >> 3u);
  map_pos_y = (uint8_t)(camera_y >> 3u);
  
  if (map_pos_x != old_map_pos_x || map_pos_y != old_map_pos_y) {
    HIDE_WIN;
    // On top of something unusual?
    uint8_t px = pship_x >> 9u;
    uint8_t py = pship_y >> 9u;
    uint8_t *p = &(((uint8_t *)0xA000)[py * 92 + px]);
    SWITCH_RAM_MBC5(1);
    if (*p) goto on_planet;
    SWITCH_RAM_MBC5(0);
    if (*p >= 128 && *p <= 170) goto not_on_planet;
    on_planet:
    ;
    
    // Render the planet name
    uint8_t sslot = (((py >> 2u) & 3u) << 2u) | ((px >> 2u) & 3u);
    show_planet_name(sslot);
    
    not_on_planet:
    ;
  }
  
  if (wrapped) {
    wrapped = 0;
    // Redraw all
    VBK_REG = 0; SWITCH_RAM_MBC5(0);
    set_bkg_submap(map_pos_x, map_pos_y, 20, 18, bigmap_map, bigmap_mapWidth);
    VBK_REG = 1; SWITCH_RAM_MBC5(1);
    set_bkg_submap(map_pos_x, map_pos_y, 20, 18, bigmap_map, bigmap_mapWidth);
    old_map_pos_x = map_pos_x;
    old_map_pos_y = map_pos_y; 
    
  } else {
    // up or down

    if (map_pos_y != old_map_pos_y) { 
      if (camera_y < old_camera_y) {
        VBK_REG = 0; SWITCH_RAM_MBC5(0);
        set_bkg_submap(map_pos_x, map_pos_y, MIN(21u, bigmap_mapWidth-map_pos_x), 1, bigmap_map, bigmap_mapWidth);
        VBK_REG = 1; SWITCH_RAM_MBC5(1);
        set_bkg_submap(map_pos_x, map_pos_y, MIN(21u, bigmap_mapWidth-map_pos_x), 1, bigmap_map, bigmap_mapWidth);
      } else {
        if ((bigmap_mapHeight - 18u) > map_pos_y) {
          VBK_REG = 0; SWITCH_RAM_MBC5(0);
          set_bkg_submap(map_pos_x, map_pos_y + 18u, MIN(21u, bigmap_mapWidth-map_pos_x), 1, bigmap_map, bigmap_mapWidth);     
          VBK_REG = 1; SWITCH_RAM_MBC5(1);
          set_bkg_submap(map_pos_x, map_pos_y + 18u, MIN(21u, bigmap_mapWidth-map_pos_x), 1, bigmap_map, bigmap_mapWidth);     
        }
      }
      old_map_pos_y = map_pos_y; 
    }
    // left or right
    
    if (map_pos_x != old_map_pos_x) {
      if (camera_x < old_camera_x) {
        VBK_REG = 0; SWITCH_RAM_MBC5(0);
        set_bkg_submap(map_pos_x, map_pos_y, 1, MIN(19u, bigmap_mapHeight - map_pos_y), bigmap_map, bigmap_mapWidth);     
        VBK_REG = 1; SWITCH_RAM_MBC5(1);
        set_bkg_submap(map_pos_x, map_pos_y, 1, MIN(19u, bigmap_mapHeight - map_pos_y), bigmap_map, bigmap_mapWidth);     
      } else {
        if ((bigmap_mapWidth - 20u) > map_pos_x) {
          VBK_REG = 0; SWITCH_RAM_MBC5(0);
          set_bkg_submap(map_pos_x + 20u, map_pos_y, 1, MIN(19u, bigmap_mapHeight - map_pos_y), bigmap_map, bigmap_mapWidth);     
          VBK_REG = 1; SWITCH_RAM_MBC5(1);
          set_bkg_submap(map_pos_x + 20u, map_pos_y, 1, MIN(19u, bigmap_mapHeight - map_pos_y), bigmap_map, bigmap_mapWidth);     
        }
      }
      old_map_pos_x = map_pos_x;
    }
  }
  // set old camera position to current camera position
  old_camera_x = camera_x, old_camera_y = camera_y;
}
void drawGalaxyMap() NONBANKED
{
  savedbank = _current_bank;
  VBK_REG = 1;
  SWITCH_ROM_MBC5(GD_Galaxy_a_BANK);
  set_bkg_tiles(0, 0, 20, 18, GD_Galaxy_a);
  VBK_REG = 0;
  SWITCH_ROM_MBC5(GD_Galaxy_t_BANK);
  set_bkg_tiles(0, 0, 20, 18, GD_Galaxy_t);

  SWITCH_ROM_MBC5(savedbank);
}
void exploration() BANKED
{
  exploring:
  camera_x = camera_y = 0;
  map_pos_x = map_pos_y = 0; 
  old_map_pos_x = old_map_pos_y = 255;
  cont_exploring:
  
  // Fills SRAM with sector data
  fillSpace();
  // Initialises VRAM
  exploreVram();
  exploreSprites();
  
  wait_vbl_done();
  camera_x = (pship_x >> 6) - 80;
  camera_y = (pship_y >> 6) - 72;
  map_pos_x = (uint8_t)(camera_x >> 3u);
  map_pos_y = (uint8_t)(camera_y >> 3u);
  VBK_REG = 0; SWITCH_RAM_MBC5(0);
  set_bkg_submap(map_pos_x, map_pos_y, 32, 32, bigmap_map, bigmap_mapWidth);
  VBK_REG = 1; SWITCH_RAM_MBC5(1);
  set_bkg_submap(map_pos_x, map_pos_y, 32, 32, bigmap_map, bigmap_mapWidth);
  LCDC_REG |= 0x40U; // Set window address area to 9C00
  move_win(7, 136);
  
  VBK_REG = 0;
  for (uint8_t i = 0; i < 20; i++) ((uint8_t *)0x9C00)[i] = 16;
  VBK_REG = 1;
  for (uint8_t i = 0; i < 20; i++) ((uint8_t *)0x9C00)[i] = 8 + 7;
  DISPLAY_ON;
  joystate = 0;
  for (;;) {
    exploreFrame();
    if (joystate & J_LEFT) pship_ang--;
    else if (joystate & J_RIGHT) pship_ang++;
    if (joystate & J_UP) {
      explodeAdvance();
    }
    if (joystate & J_SELECT) {
      pship_model++;
      if (pship_model == 23) pship_model = 0;
      DISPLAY_OFF;
      goto cont_exploring;
    }
    pship_x += pship_vx;
    pship_y += pship_vy;
    
    if (pship_x < 5120u) {
      //pship_x = 36863u+5120u;
      
      //pship_x = 5120u;
      goto galaxy;
    } else if (pship_x >= 36864u+5120u) {
      //pship_x = 5120u;
      //pship_x = 36863u+5120u;
      goto galaxy;
    }
    if (pship_y < 4608u) {
      //pship_y = 36351u+4608u;
      //pship_y = 4608u;
      goto galaxy;
    } else if (pship_y >= 36352u+4608u) {
      //pship_y = 4608u;
      //pship_y = 36351u+4608u;
      goto galaxy;
    }
    
    /*
    pship_x += 60416u;
    pship_y += 60928u;
    

    if (pship_x > 36864u) {
      pship_x -= 36864u;
    }
    if (pship_y > 36352u) {
      pship_y -= 36352u;
    }
    pship_x += 5120u;
    pship_y += 4608u;
    */
    
    pship_vx = (uint16_t)( (int16_t)pship_vx * 15 / 16 );
    pship_vy = (uint16_t)( (int16_t)pship_vy * 15 / 16 );
    
    //uint16_t before_shift = pship_vx & 0x8000;
    //pship_vx = (pship_vx >> 1) | before_shift;
    //before_shift = pship_vy & 0x8000;
    //pship_vy = (pship_vy >> 1) | before_shift;
    
    camera_x = (pship_x >> 6) - 80u;
    camera_y = (pship_y >> 6) - 72u;
    
    

    /*
    camera_x++;
    camera_y++;
    if (camera_x == camera_max_x) camera_x -= camera_max_x;
    if (camera_y == camera_max_y) camera_y -= camera_max_y;
    */
    drawExploreScreen();
    //exploreSprites();
    
    if ((joystate & J_UP)) {
      countdown--;
      if (countdown > 32767u) goto randbattle;
    }
    lastjoystate = joystate;
  }
  
  galaxy:
  playsfx(0x13, 24);
  
  DISPLAY_OFF;
  getGalaxyPos();
  HIDE_WIN;
  SCY_REG = 0; SCX_REG = 0;
  drawGalaxyMap();
  exploreFrame();
  
  SET_SPRITE_Y(1, 0);
  SET_SPRITE_Y(2, 0);
  SET_SPRITE_Y(3, 0);
  SET_SPRITE_A(0, 8 + 7);
  SET_SPRITE_I(0, 0);
  DISPLAY_ON;
  for (;;) {
    exploreFrame();
    joystate = joypad();
    
    wait_vbl_done();
    SET_SPRITE_X(0, (galaxy_x) + 8);
    SET_SPRITE_Y(0, (galaxy_y) + 16);
    
    if (joystate == 0) break;
  }
  uint8_t gx = 0, gy = 0, gt = 1;
  uint8_t over_system = 255;
  for (;;) {
    exploreFrame();
    if (gt > 0) {
      galaxy_x += gx;
      galaxy_y += gy;
      gt--;
      if (gt == 0) {
        uint16_t idx = ((uint16_t)galaxy_y >> 3u) * 20u + (galaxy_x >> 3u);
        over_system = show_system_name(idx);
      }
    } else {
      joystate = joypad();
      gx = 0; gy = 0;
      if ((joystate & J_UP) && galaxy_y > 1*8) gy = 255;
      else if ((joystate & J_DOWN) && galaxy_y < 16*8) gy = 1;
      else if ((joystate & J_LEFT) && galaxy_x > 0*8) gx = 255;
      else if ((joystate & J_RIGHT) && galaxy_x < 19*8) gx = 1;
      if (gx || gy) {
        gt = 8;
      }
      if (joystate & J_A && over_system != 255) {
        pship_system = over_system;
        pship_x = 23552u;
        pship_y = 22784u;
        DISPLAY_OFF;
        playsfx(0x12, 24);
        goto exploring;
      }
    }
    wait_vbl_done();
    SET_SPRITE_X(0, (galaxy_x) + 8);
    SET_SPRITE_Y(0, (galaxy_y) + 16);
    
    
  }
  
  randbattle:
  battlesong();
  DISPLAY_OFF;
  
  battle();
  
  countdown = rand() + 1024;
  
  goto cont_exploring;
  /*
  for (;;) {
    exploreFrame();
    wait_vbl_done();
  }
  */
}
