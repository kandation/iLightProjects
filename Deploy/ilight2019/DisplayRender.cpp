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

void DisplayRender::updateMap4x4()
{
    byte row = 0;
    byte four_couter = 0;
    for (int i = 0; i < 16; i++)
    {
        if (i % 4 == 0 && i != 0)
        {
            row += 1;
            four_couter = 0;
        }
        openSingle4x4(row, four_couter, display_map.getColorAt(i));
        four_couter += 1;
    }
    pixelsRow0.show();
    pixelsRow1.show();
    pixelsRow2.show();
    pixelsRow3.show();
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

void DisplayRender::openSingle4x4(byte row, int num, uint32_t RGB)
{
    for (int i = num * NUM_RING_ONE_PIX; i < ((num + 1) * NUM_RING_ONE_PIX); i++)
    {
        if (row == 0)
        {
            pixelsRow0.setPixelColor(i, RGB);
        }
        if (row == 1)
        {
            pixelsRow1.setPixelColor(i, RGB);
        }
        if (row == 2)
        {
            pixelsRow2.setPixelColor(i, RGB);
        }
        if (row == 3)
        {
            pixelsRow3.setPixelColor(i, RGB);
        }
    }
}

void DisplayRender::openSingleRGB(int num, byte r, byte g, byte b)
{
    for (int i = num * NUM_RING_ONE_PIX; i < ((num + 1) * NUM_RING_ONE_PIX); i++)
    {
        pixels.setPixelColor(i, r, g, b);
    }
}