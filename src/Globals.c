#include <stdint.h>

uint8_t tempval_a;
const uint8_t *tempval_ptr;
uint8_t joystate;
uint8_t lastjoystate = 0;
uint8_t menucur;
uint8_t savedbank;
uint16_t camera_x, camera_y, old_camera_x, old_camera_y;
uint8_t map_pos_x, map_pos_y, old_map_pos_x, old_map_pos_y;
uint8_t pship_model = 0;
uint16_t pship_x = 10000;
uint16_t pship_vx = 0, pship_vy = 0;
uint16_t pship_y = 10000;
uint8_t pship_ang = 0;
uint8_t wrapped = 0;
uint8_t pship_system = 0;
uint8_t galaxy_x;
uint8_t galaxy_y;
uint16_t countdown = 256;

uint8_t eship_model = 0;
