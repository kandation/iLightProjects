#ifndef MYHSL_H
#define MYHSL_H
#include <Arduino.h>
#include "RGB.h"

class MYHSL
{
private:
    byte _h;
    byte _s;
    byte _l;
    uint8_t hsl_convert(float c, float t1, float t2);

public:
    MYHSL();
    MYHSL(uint16_t, uint8_t, uint8_t);
    RGB hsl2rgb(uint16_t, uint8_t, uint8_t);

    uint16_t get_h() { return _h; }
    uint8_t get_s() { return _s; }
    uint8_t get_l() { return _l; }

    uint32_t toInt32();
};

#endif
