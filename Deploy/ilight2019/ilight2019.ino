// OTA LIBRARY
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define DEBUGLIGHT

// # Internet Function
#include "HTTPSRedirect.h"
#include "DebugMacros.h"

// # InClass LIBRARY
#include <string.h> //Memcpy

#include <Arduino.h>
#include <math.h>
#include <stdlib.h>

// Local Library
#include "Display.h"
#include "RGB.h"
#include "DisplayRender.h"
#include "GlobalVariable.h"

/* ------ Configuration ------- */

#define SENSOR_SOUND_PIN A0

int DELAY_VAL = 1000;

int sensorValue;
int count = 0;
int pm25_counter = 0;
int pm25_blink_time = 1000;
unsigned long pm25_timeUpdate = 0;

int pm25_lastColor = 0;

int app_pm25_level = -99;
static int pm25_updatetinInSec = 60;
static int pm25_network_error_count = 0;

byte pm25_global_r = 0;
byte pm25_global_g = 0;
byte pm25_global_b = 0;

byte *pm25_color_rgb;
static byte pm25_color_rgbColor[] = {0, 0, 0};

HTTPSRedirect *client = nullptr;

void accessWifi()
{
  _DPRINT("Connecting to wifi: ");
  _DPRINTLN(ssid);
  Serial.flush();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    _DPRINT(".");
  }
  _DPRINTLN("");
  _DPRINTLN("WiFi connected");
  _DPRINTLN("IP address: ");
  _DPRINTLN(WiFi.localIP());
}

void networkStep_01hostConnection()
{
  // Use HTTPSRedirect class to create a new TLS connection
  _DPRINTLN("Start CLientCall");
  client = new HTTPSRedirect(httpsPort);
  client->setInsecure();
  client->setPrintResponseBody(false);
  client->setContentTypeHeader("application/text");
  _DPRINTLN("Client Call OK");
}

void networkStep_02CheckHost()
{
  // Try to connect for a maximum of 5 times
  _DPRINTLN("Start Check Host");
  bool flag = false;
  int retval = 0;
  for (int i = 0; i < 5; i++)
  {

    retval = client->connect(host, httpsPort);

    _DPRINT("retval=");
    _DPRINTLN(retval);
    if (retval == 1)
    {
      flag = true;
      break;
    }
    else
      _DPRINTLN("Connection failed. Retrying...");
  }
  _DPRINTLN("Start Flag");

  if (!flag)
  {
    _DPRINT("Could not connect to server: ");
    _DPRINTLN(host);
    _DPRINTLN("Exiting...");
    return;
  }
}

void network_getValue()
{
  _DPRINTLN("Start GetValue");
  client->GET(url, host);
  String line = client->getResponseBody();
  _DPRINTLN("--------");
  _DPRINTLN(line);
}

int getPM25Level()
{

  String line = client->getResponseBody();
  _DPRINT("PM25 Level = ");
  int val = line.toInt();
  _DPRINTLN(val);

  return val;
}

int network_secureGetPM25Lavel()
{

  static int connect_count = 0;
  const unsigned int MAX_CONNECT = 5;
  static bool flag = false;
  int value = -2;
  network_show_load_logo();

  if (!flag)
  {
    client = new HTTPSRedirect(httpsPort);
    client->setInsecure();
    flag = true;
    client->setPrintResponseBody(false);
    client->setContentTypeHeader("application/text");
  }

  if (client != nullptr)
  {
    if (!client->connected())
    {
      client->connect(host, httpsPort);
    }
  }
  else
  {
    DPRINTLN("Error creating client object!");
    pm25_network_error_count += 1;
  }

  /*if (connect_count > MAX_CONNECT) {
    //error_count = 5;
    connect_count = 0;
    flag = false;
    delete client;
    return -99;
    }*/

  if (client->GET(url, host))
  {
    value = getPM25Level();
  }
  else
  {
    pm25_network_error_count += 1;
    _DPRINT("Error-count while connecting: ");
    _DPRINTLN(pm25_network_error_count);
  }

  if (pm25_network_error_count > 50)
  {
    _DPRINTLN("Halting processor...");
    delete client;
    client = nullptr;
    Serial.flush();
    ESP.reset();
    //ESP.deepSleep(0);
  }
  //clear_zero_map();

  return value;
}
/*------------- END PM25 WIFI ------------------*/

/* ------ Intitialize --------*/

class IntegerList
{
public:
  int *data;
  IntegerList(int);
  void pop(int);
};
IntegerList::IntegerList(int L)
{
  data = (int *)malloc(L * sizeof(int));
}

/* ------ Main Code --------- */
Display display = Display();
byte box[4] = {5, 6, 9, 10};
byte box2[4] = {0, 1, 4, 5};
byte cpe[8] = {1, 2, 3, 5, 9, 13, 14, 15};
byte cpe2[8] = {1, 2, 3, 7, 11, 13, 14, 15};
byte line[4] = {0, 1, 2, 3};
byte dot[1] = {5};
byte cpe_logo_green[] = {2, 3, 4, 5, 7, 11, 12, 13, 14, 15};
byte cpe_logo_green_rgb[] = {0, 255, 0};
byte cpe_logo_blue[] = {1, 6, 8, 9, 10};
byte cpe_logo_blue_rgb[] = {0, 0, 255};
byte cpe_logo_white[] = {0};
byte cpe_logo_white_rgb[] = {200, 200, 200};

byte icon_sad[] = {0, 3, 9, 10, 12, 15};
byte icon_x[] = {0, 5, 10, 15};

byte icon_color_red[] = {255, 0, 0};
byte icon_color_yellow[] = {255, 255, 0};

byte pm25_logo_down[8] = {8, 9, 10, 11, 12, 13, 14, 15};
byte pm25_logo_up[8] = {0, 1, 2, 3, 4, 5, 6, 7};
byte pm25_logo_full[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
byte pm25_logo_rgb_1[] = {0, 255, 0};
byte pm25_logo_rgb_2[] = {255, 255, 0};
byte pm25_logo_rgb_3[] = {255, 150, 50};
byte pm25_logo_rgb_4[] = {255, 0, 0};
byte pm25_logo_rgb_5[] = {255, 0, 255};
byte pm25_logo_rgb_6[] = {255, 0, 255};
byte pm25_logo_rgb_error[] = {10, 0, 130};

byte network_logo_block[12] = {0, 1, 2, 3, 4, 7, 8, 11, 12, 13, 14, 15};
byte network_logo_random[] = {255, 150, 0};

DisplayRender render = DisplayRender(display);

uint32_t network_RandomColor()
{
  byte temp_random_color_number = (byte)random(150, 255);
  RGB temp_network_rgb_hex = RGB(temp_random_color_number, temp_random_color_number, temp_random_color_number);

  return temp_network_rgb_hex.toInt32();
}

void clear_zero_map()
{
  display.clear_pixel();
  render.setDisplay(display);
  render.updateMap();
}

void network_show_load_logo()
{
  display.add_pixel(network_logo_block, network_logo_random, 12);
  render.setDisplay(display);
  render.updateMap();
}

void FadingLEDAllFromCouter(uint8_t _value, uint8_t _start, uint8_t _end, byte *__color)
{
  _DPRINTLN("HSL2RGB CAL");
  Serial.print(__color[0]);
  Serial.print(" ");
  Serial.print(__color[1]);
  Serial.print(" ");
  Serial.println(__color[2]);
  RGB __rgb = RGB(__color[0], __color[1], __color[2]);
  uint8_t _val = constrain(_value, _start, _end);
  __rgb.rgb2hsl_convert();

  for (uint16_t i = 0; i < CONFIG_MAP_SIZE; i++)
  {
    // TODO
    //rgb = rgb.hex2rgb(display.getColorAt(i));

    __rgb = __rgb.hsl2rgb(__rgb.get_h(), __rgb.get_s(), _val);
    display.set_color_n(i, __rgb.toInt32());
    _DPRINT(__rgb.get_h());
    _DPRINT(" ");
    _DPRINT(__rgb.get_s());
    _DPRINT(" ");
    _DPRINTLN(__rgb.get_l());
  }
  Serial.println("---------------------");
}

void FadingLEDAllFromCouterValue(uint8_t _value, uint8_t _start, uint8_t _end, byte __r, byte __g, byte __b)
{
  _DPRINTLN("HSL2RGB CAL");
  Serial.print(__r);
  Serial.print(" ");
  Serial.print(__g);
  Serial.print(" ");
  Serial.println(__b);
  RGB __rgb = RGB(__r, __g, __b);
  uint8_t _val = constrain(_value, _start, _end);
  __rgb.rgb2hsl_convert();
  __rgb = __rgb.hsl2rgb(__rgb.get_h(), __rgb.get_s(), _val);

  for (uint16_t i = 0; i < CONFIG_MAP_SIZE; i++)
  {
    // TODO
    //rgb = rgb.hex2rgb(display.getColorAt(i));

    display.set_color_n(i, __rgb.toInt32());
    _DPRINT(__rgb.get_h());
    _DPRINT(" ");
    _DPRINT(__rgb.get_s());
    _DPRINT(" ");
    _DPRINTLN(__rgb.get_l());
  }
  Serial.println("---------------------");
}

void test_01_testColor()
{
  int temp_delay = 500;
  display.clear_pixel();
  display.add_pixel(pm25_logo_full, pm25_logo_rgb_1, 16);
  render.setDisplay(display);
  render.updateMap();
  delay(temp_delay);

  display.clear_pixel();
  display.add_pixel(pm25_logo_full, pm25_logo_rgb_2, 16);
  render.setDisplay(display);
  render.updateMap();
  delay(temp_delay);

  display.clear_pixel();
  display.add_pixel(pm25_logo_full, pm25_logo_rgb_3, 16);
  render.setDisplay(display);
  render.updateMap();
  delay(temp_delay);

  display.clear_pixel();
  display.add_pixel(pm25_logo_full, pm25_logo_rgb_4, 16);
  render.setDisplay(display);
  render.updateMap();
  delay(temp_delay);

  display.clear_pixel();
  display.add_pixel(pm25_logo_full, pm25_logo_rgb_5, 16);
  render.setDisplay(display);
  render.updateMap();
  delay(temp_delay);

  display.clear_pixel();
  display.add_pixel(pm25_logo_full, pm25_logo_rgb_6, 16);
  render.setDisplay(display);
  render.updateMap();
  delay(temp_delay);
}

void menu_00_showCPELogo()
{
  //logo intilail
  display.clear_pixel();
  display.add_pixel(cpe_logo_green, cpe_logo_green_rgb, 10);
  display.add_pixel(cpe_logo_blue, cpe_logo_blue_rgb, 5);
  display.add_pixel(cpe_logo_white, cpe_logo_white_rgb, 1);
  render.setDisplay(display);
  render.updateMap();
  delay(2000);
}

byte *menu_03_pm25_colorSelect(int level)
{
  switch (level)
  {
  case 1:
    pm25_blink_time = 300;
    return pm25_logo_rgb_1;
  case 2:
    pm25_blink_time = 300;
    return pm25_logo_rgb_2;
  case 3:
    pm25_blink_time = 100;
    return pm25_logo_rgb_3;
  case 4:
    pm25_blink_time = 50;
    return pm25_logo_rgb_4;
  case 5:
    pm25_blink_time = 25;
    return pm25_logo_rgb_5;
  case 6:
    pm25_blink_time = 250;
    return pm25_logo_rgb_6;
  default:
    pm25_blink_time = 1000;
    return pm25_logo_rgb_error;
  }
  return pm25_logo_rgb_error;
}
void menu_03_pm25_setColorSelect(int level)
{
  switch (level)
  {
  case 1:
    pm25_blink_time = 50;
    pm25_global_r = 0;
    pm25_global_g = 255;
    pm25_global_b = 0;
    break;
  case 2:
    pm25_blink_time = 50;
    pm25_global_r = 255;
    pm25_global_g = 255;
    pm25_global_b = 0;
    break;
  case 3:
    pm25_blink_time = 50;
    pm25_global_r = 255;
    pm25_global_g = 150;
    pm25_global_b = 50;
    break;
  case 4:
    pm25_blink_time = 50;
    pm25_global_r = 255;
    pm25_global_g = 0;
    pm25_global_b = 0;
    break;
  case 5:

    pm25_blink_time = 25;
    pm25_global_r = 255;
    pm25_global_g = 0;
    pm25_global_b = 255;
    break;
  case 6:
    pm25_blink_time = 250;
    pm25_global_r = 255;
    pm25_global_g = 0;
    pm25_global_b = 255;
    break;
  default:
    pm25_blink_time = 1000;
    pm25_global_r = 10;
    pm25_global_g = 0;
    pm25_global_b = 130;
    break;
  }
  pm25_color_rgbColor[0] = pm25_global_r;
  pm25_color_rgbColor[1] = pm25_global_g;
  pm25_color_rgbColor[1] = pm25_global_b;
}

void menu_03_pm25()
{
  //Note: Dont make delay in this function
  unsigned long pm25_diff_time = millis() - pm25_timeUpdate;
  if (pm25_diff_time > 1000 * pm25_updatetinInSec || app_pm25_level == -99)
  {
    _DPRINTLN("Start Update PM25");
    app_pm25_level = network_secureGetPM25Lavel();
    //pm25_color_rgb = menu_03_pm25_colorSelect(app_pm25_level);
    menu_03_pm25_setColorSelect(app_pm25_level);
    _DPRINT("Update PM25=");
    _DPRINTLN(app_pm25_level);
    pm25_timeUpdate = millis();
    display.clear_pixel();
    display.add_pixel(pm25_logo_full, pm25_color_rgbColor, 16);
  }

  if (pm25_color_rgbColor != nullptr)
  {
    if (app_pm25_level >= 6)
    {
      if (pm25_counter % 2 == 0)
      {
        _DPRINTLN("PM25 LED RUN DOWN");
        display.clear_pixel();
        display.add_pixel(pm25_logo_down, pm25_color_rgbColor, 8);
      }
      else
      {
        _DPRINTLN("PM25 LED RUN UP");
        display.clear_pixel();
        display.add_pixel(pm25_logo_up, pm25_color_rgbColor, 8);
      }
    }
    else
    {
      _DPRINT("PM25 COLOR COUTER = ");
      _DPRINTLN(pm25_counter);

      if (pm25_counter >= 0)
      {
        //FadingLEDAllFromCouter(pm25_counter, 0, 50, pm25_color_rgbColor);
        FadingLEDAllFromCouterValue(pm25_counter, 0, 50, pm25_color_rgbColor[0], pm25_color_rgbColor[1], pm25_color_rgbColor[2]);
      }
      else
      {
        //FadingLEDAllFromCouter(abs(pm25_counter), 0, 50, pm25_color_rgbColor);
        FadingLEDAllFromCouterValue(abs(pm25_counter), 0, 50, pm25_color_rgbColor[0], pm25_color_rgbColor[1], pm25_color_rgbColor[2]);
      }

      if (pm25_counter == 50)
      {
        pm25_counter = -50;
      }
    }

    render.setDisplay(display);
    render.updateMap();
    pm25_counter += 1;
  }
  else
  {
    _DPRINTLN("COLOR is NULL");
    display.clear_pixel();
    display.add_pixel(pm25_logo_full, icon_color_yellow, 16);
    display.add_pixel(icon_x, icon_color_red, 4);
    render.setDisplay(display);
    render.updateMap();
    pm25_blink_time = 200;
  }
}

void menu_02_boxRotate()
{
  display.clear_pixel();
  display.add_pixel(box, 4);
  render.setDisplay(display);
  render.updateMap();
  delay(2000);

  display.clear_pixel();
  display.add_pixel(box2, 4);
  render.setDisplay(display);
  render.updateMap();
  delay(2000);

  display.clear_pixel();
  display.add_pixel(cpe2, 8);
  render.setDisplay(display);
  render.updateMap();
  delay(2000);

  display.clear_pixel();
  display.add_pixel(cpe, 8);
  render.setDisplay(display);
  render.updateMap();
  delay(1000);

  display.clear_pixel();
  display.add_pixel(line, 4);
  render.setDisplay(display);
  render.updateMap();
  delay(2000);
  _DPRINTLN("Wait");
}

void menu_02_boxRotate_Rotate()
{
  render.updateMap();
  //pixels.show();

  if (count > 10)
  {
    display.rotate_left();
    render.setDisplay(display);
    // delay(1000);
  }
  if (count % 5 == 0)
  {
    display.add_pixel(dot, 1);
    render.setDisplay(display);
  }
  count++;

  delay(500);
}

void setup()
{

  // End of trinket special code

  Serial.begin(115200);
  render.displayBegin();

  _DPRINTLN("Start MCU");

  pinMode(D7, OUTPUT);
  delay(100);
  test_01_testColor();
  menu_00_showCPELogo();
  delay(100);

  /* ---- Wifi -----*/
  _DPRINTLN("Start Wifi");

  accessWifi();
  networkStep_01hostConnection();
  networkStep_02CheckHost();

  //delete HTTPSRedirect object;
  delete client;
  client = nullptr;

  /* ----- Wifi ------*/
  _DPRINTLN("Start Loop");
  //delay(2000);
}

void loop()
{
  menu_03_pm25();
  delay(pm25_blink_time);
}
