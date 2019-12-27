#ifndef DISPLAY_H
#define DISPLAY_H
#include <Arduino.h>
#include "Display.h"
#include "Globalconfig.h"
#include "RingColor.h"

class Display
{
  byte rotation_left_grid[CONFIG_MAP_SIZE] = {12, 8, 4, 0, 13, 9, 5, 1, 14, 10, 6, 2, 15, 11, 7, 3};
  byte rotation_right_grid[CONFIG_MAP_SIZE] = {3, 7, 11, 15, 2, 6, 10, 14, 1, 5, 9, 13, 0, 4, 8, 12};

public:
  byte pos[CONFIG_MAP_SIZE];
  uint32_t color[CONFIG_MAP_SIZE];
  char type[CONFIG_MAP_SIZE];
  RingColor ring[CONFIG_MAP_SIZE];

  Display();
  void add_dot(byte, uint32_t);
  void add_dot(byte position, byte rgb_color[]);
  void add_pixel(byte[], int);
  void add_pixel(byte[], byte[], int);
  void add_pixel(byte value[], uint32_t rgb_hex, int arrSize);
  void add_pixel_clear(byte value[]);
  void set_color_n(uint16_t, uint32_t);
  void clear_pixel();
  void rotate_left();
  void rotate_right();
  uint32_t rgb2hex(byte, byte, byte);
  void set_fade_from_pos(byte pos, uint8_t _value, uint8_t _start, uint8_t _end);
  uint32_t *get_color()
  {
    return color;
  }
  uint32_t getColorAt(int i)
  {
    return color[i];
  }
  char *get_type()
  {
    return type;
  }

private:
  template <typename rotate_arr_t>
  void rotation(rotate_arr_t arrValue);
};

#endif
