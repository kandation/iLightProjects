#include "RGB.h"

#ifndef RINGCOLOR_H
#define RINGCOLOR_H
class RingColor{
  enum class Type: byte{
    normal,
    fade,
    circle,
    blink
  };
  RGB rgb;
  Type type;
  public:
  
  RingColor();
  RingColor(RGB);
  RingColor(RGB,Type);
  RingColor(byte, byte, byte);
  RingColor(byte, byte, byte, Type);
  
  void update();
};
#endif
