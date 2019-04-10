#ifndef RGB_H
#define RGB_H
#include <Arduino.h>

class RGB
{
private:
  byte _r;
  byte _g;
  byte _b;
  byte _l;

  int _hue;
  int _stu;
  int _lua;

public:
  RGB();
  RGB(byte *);
  RGB(byte, byte, byte);
  RGB(byte, byte, byte, byte);
  RGB hex2rgb(uint32_t);
  RGB hsl2rgb(uint16_t, uint8_t, uint8_t);
  uint8_t hsl_convert(float c, float t1, float t2);
  void rgb2hsl_convert();

  uint32_t toInt32();

  //Setter
  void set_r(byte r) { _r = r; }
  void set_g(byte g) { _g = g; }
  void set_b(byte b) { _b = b; }

  byte get_r() { return _r; }
  byte get_g() { return _g; }
  byte get_b() { return _b; }

  uint16_t get_h() { return _hue; }
  uint8_t get_s() { return _stu; }
  uint8_t get_l() { return _lua; }
};

#endif
