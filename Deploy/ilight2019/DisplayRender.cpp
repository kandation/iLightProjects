#include "DisplayRender.h"

DisplayRender::DisplayRender(Display disp)
{
    display_map = disp;
}

void DisplayRender::updateMap()
{
    for (int i = 0; i < 16; i++)
    {
        openSingle(i, display_map.getColorAt(i));
        //Serial.print(display_map.getColorAt(i));
        //Serial.print(";");
    }
    //Serial.println();
    pixels.show();
}

void DisplayRender::openSingle(int num, uint32_t RGB)
{
    int broken_led = 3;
    int temp_num = num;
    uint8_t interval = 0;
    interval = 16 / 2;
    //Skip broken led
    if (num > broken_led)
    {
        temp_num -= 1;
    }

    if (num != broken_led)
    {
        for (int i = temp_num * NUM_RING_ONE_PIX; i < ((temp_num + 1) * NUM_RING_ONE_PIX); i++)
        {
            pixels.setPixelColor(i, RGB);
        }
    }

    //pixels.show();
}

void DisplayRender::openSingle4x4(int num, uint32_t RGB)
{
    int broken_led = 3;
    int temp_num = num;
    //Skip broken led
    if (num > broken_led)
    {
        temp_num -= 1;
    }

    if (num != broken_led)
    {
        for (int i = temp_num * NUM_RING_ONE_PIX; i < ((temp_num + 1) * NUM_RING_ONE_PIX); i++)
        {
            pixels.setPixelColor(i, RGB);
        }
    }

    //pixels.show();
}

void DisplayRender::openSingleRGB(int num, byte r, byte g, byte b)
{
    for (int i = num * NUM_RING_ONE_PIX; i < ((num + 1) * NUM_RING_ONE_PIX); i++)
    {
        pixels.setPixelColor(i, r, g, b);
    }
}