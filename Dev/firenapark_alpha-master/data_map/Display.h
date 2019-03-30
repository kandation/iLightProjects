#ifndef DISPLAY_H
#define DISPLAY_H
#include <Arduino.h>
#include "Display.h"
#include "Globalconfig.h"
#include "RingColor.h"

class Display{
  static Globalconfig globalconfig;
  //globalconfig.MAP_SIZE = 16;
  byte rotation_left_grid[globalconfig.MAP_SIZE] = {12,8,4,0,13,9,5,1,14,10,6,2,15,11,7,3};
  byte rotation_right_grid[globalconfig.MAP_SIZE] = {3,7,11,15,2,6,10,14,1,5,9,13,0,4,8,12};
public:
  byte pos[globalconfig.MAP_SIZE];
  int color[globalconfig.MAP_SIZE];
  char type[globalconfig.MAP_SIZE];
  RingColor ring[globalconfig.MAP_SIZE]; 


  Display();
  void add_pixel(byte[], int);
  void add_pixel(byte[], int[], int);
  void add_pixel_clear(byte value[]);
  void clear_pixel();
  void rotate_left();
  void rotate_right();
  int int2hex(int,int,int);
  int *get_color(){
    return color;
  }
  int getColorAt(int i){
    return color[i];
  }
  char *get_type(){
    return type;  
  }

private:
  template <typename rotate_arr_t>
  void rotation(rotate_arr_t arrValue);
  

};

#endif
