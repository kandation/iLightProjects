#ifndef DISPLAYRENDER_H
#define DISPLAYRENDER_H

#include <Arduino.h>
#include "Display.h"

#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
#include <avr/power.h>
#endif

#if defined(__AVR_ATtiny85__)
if (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
#endif

class DisplayRender
{
public:
    Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_RING_ALL_PIX, LED_PIN, NEO_GRB + NEO_KHZ800);
    Adafruit_NeoPixel pixelsRow0 = Adafruit_NeoPixel(NUM_RING_ALL_PIX, RING_LED_PIN_ROW_01, NEO_GRB + NEO_KHZ800);
    Adafruit_NeoPixel pixelsRow1 = Adafruit_NeoPixel(NUM_RING_ALL_PIX, RING_LED_PIN_ROW_02, NEO_GRB + NEO_KHZ800);
    Adafruit_NeoPixel pixelsRow2 = Adafruit_NeoPixel(NUM_RING_ALL_PIX, RING_LED_PIN_ROW_03, NEO_GRB + NEO_KHZ800);
    Adafruit_NeoPixel pixelsRow3 = Adafruit_NeoPixel(NUM_RING_ALL_PIX, RING_LED_PIN_ROW_04, NEO_GRB + NEO_KHZ800);

    byte pos[CONFIG_MAP_SIZE];
    int color[CONFIG_MAP_SIZE];
    char type[CONFIG_MAP_SIZE];
    Display display_map;

    DisplayRender(Display);
    void openSingle(int num, uint32_t RGB);
    void openSingleRGB(int num, byte r, byte g, byte b);
    void openSingle4x4(byte r, int num, uint32_t RGB);
    void setDisplay(Display dis)
    {
        display_map = dis;
    }
    void updateMap();
    void updateMap4x4();

    void displayBegin4x4()
    {
        pixelsRow0.begin();
        pixelsRow1.begin();
        pixelsRow2.begin();
        pixelsRow3.begin();
    }

    void displayBegin()
    {
        pixels.begin();
    }
};

#endif