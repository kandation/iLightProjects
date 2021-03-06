#include "RGB.h"
#include <Arduino.h>

RGB::RGB()
{
  this->_r = 0;
  this->_g = 0;
  this->_b = 0;
  this->_l = 0;
}

RGB::RGB(byte *__rgb)
{
  this->_r = __rgb[0];
  this->_g = __rgb[1];
  this->_b = __rgb[2];
  this->_l = 50;
}

RGB::RGB(byte r, byte g, byte b)
{
  this->_r = r;
  this->_g = g;
  this->_b = b;
  this->_l = 50;
}
RGB::RGB(byte r, byte g, byte b, byte l)
{
  this->_r = r;
  this->_g = g;
  this->_b = b;
  this->_l = l;
}

RGB RGB::hex2rgb(uint32_t rgbInt)
{
  RGB _rgb = RGB();
  _rgb.set_r(rgbInt >> 16);
  _rgb.set_g((rgbInt & 0x00ff00) >> 8);
  _rgb.set_b(rgbInt & 0x0000ff);
  return _rgb;
}

RGB RGB::hsl2rgb(uint16_t ih, uint8_t is, uint8_t il)
{
  /*
     H is HUE range(0,360)
     S i stu
  */
  float h, s, l, t1, t2, tr, tg, tb;
  uint8_t r, g, b;

  h = (ih % 360) / 360.0;
  s = constrain(is, 0, 100) / 100.0;
  l = constrain(il, 0, 100) / 100.0;

  if (s == 0)
  {
    r = g = b = 255 * l;
    //return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
    return RGB(r, g, b);
  }

  if (l < 0.5)
    t1 = l * (1.0 + s);
  else
    t1 = l + s - l * s;

  t2 = 2 * l - t1;
  tr = h + 1 / 3.0;
  tg = h;
  tb = h - 1 / 3.0;

  r = hsl_convert(tr, t1, t2);
  g = hsl_convert(tg, t1, t2);
  b = hsl_convert(tb, t1, t2);

  // NeoPixel packed RGB color
  //return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
  return RGB(r, g, b);
}

void RGB::rgb2hsl_convert()
{
  float h, s, l;
  float r, g, b;

  r = (float)_r / 255.0f;
  g = (float)_g / 255.0f;
  b = (float)_b / 255.0f;

  float _min = min(min(r, g), b);
  float _max = max(max(r, g), b);
  float _delta = _max - _min;

  h = 0.0f;
  s = 0.0f;
  l = (_max + _min) / 2.0f;

  if (_delta != 0.0f)
  {
    s = (l <= 0.5f) ? (_delta / (_max + _min)) : (_delta / (2.0f - _max - _min));
    if (r == _max)
    {
      h = ((g - b) / _delta) + (g < b ? 6.0f : 0.0f);
    }
    else if (g == _max)
    {
      h = (b - r) / _delta + 2.0f;
    }
    else
    {
      h = (r - g) / _delta + 4.0f;
    }
  }
  _hue = (uint16_t)(h * 360.0f);
  _stu = (uint8_t)(s * 100.0);
  _lua = (uint8_t)(l * 100.0);

  // Serial.print(r);
  // Serial.print(" ");
  // Serial.print(g);
  // Serial.print(" ");
  // Serial.print(b);
  // Serial.print("/");
  // Serial.print(_r);
  // Serial.print(" ");
  // Serial.print(_g);
  // Serial.print(" ");
  // Serial.print(_b);
  // Serial.print("/");
  // Serial.print(h);
  // Serial.print(" ");
  // Serial.print(_hue);
  // Serial.print(" ");
  // Serial.print(_stu);
  // Serial.print(" ");
  // Serial.println(_lua);
}

/**
   HSL Convert
   Helper function
*/
uint8_t RGB::hsl_convert(float c, float t1, float t2)
{

  if (c < 0)
    c += 1;
  else if (c > 1)
    c -= 1;

  if (6 * c < 1)
    c = t2 + (t1 - t2) * 6 * c;
  else if (2 * c < 1)
    c = t1;
  else if (3 * c < 2)
    c = t2 + (t1 - t2) * (2 / 3.0 - c) * 6;
  else
    c = t2;

  return (uint8_t)(c * 255);
}

uint32_t RGB::toInt32()
{
  return (_r << 16) | (_g << 8) | _b;
}
