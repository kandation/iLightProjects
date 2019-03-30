#ifndef RGB_H
#define RGB_H
#include <Arduino.h>

class RGB{
  private:
    byte _r;
    byte _g;
    byte _b;
    byte _l;

  public:
    RGB();
    RGB(byte,byte,byte);
    RGB(byte,byte,byte,byte);
    RGB hex2rgb(uint32_t);
    RGB hsl2rgb(uint16_t, uint8_t, uint8_t);
    uint8_t hsl_convert(float c, float t1, float t2);
    
    uint32_t toInt32();

    //Setter
    void set_r(byte r){_r=r;}
    void set_g(byte g){_g=g;}
    void set_b(byte b){_b=b;}
    
};

#endif
