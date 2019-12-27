// OTA LIBRARY
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define DEBUGLIGHT

// # Internet Function
#include "HTTPSRedirect.h"
#include "DebugMacros.h"
#include "MYHSL.h"

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

byte menu_number = 0;

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

// byte network_logo_block[12] = {0, 1, 2, 3, 4, 7, 8, 11, 12, 13, 14, 15};
byte network_logo_block[12] = {0};
byte network_logo_random[] = {255, 255, 255};

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
  render.updateMap4x4();
}

void network_show_load_logo()
{
  display.add_pixel(network_logo_block, network_logo_random, 12);
  render.setDisplay(display);
  render.updateMap4x4();
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
  render.updateMap4x4();
  delay(temp_delay);

  display.clear_pixel();
  display.add_pixel(pm25_logo_full, pm25_logo_rgb_2, 16);
  render.setDisplay(display);
  render.updateMap4x4();
  delay(temp_delay);

  display.clear_pixel();
  display.add_pixel(pm25_logo_full, pm25_logo_rgb_3, 16);
  render.setDisplay(display);
  render.updateMap4x4();
  delay(temp_delay);

  display.clear_pixel();
  display.add_pixel(pm25_logo_full, pm25_logo_rgb_4, 16);
  render.setDisplay(display);
  render.updateMap4x4();
  delay(temp_delay);

  display.clear_pixel();
  display.add_pixel(pm25_logo_full, pm25_logo_rgb_5, 16);
  render.setDisplay(display);
  render.updateMap4x4();
  delay(temp_delay);

  display.clear_pixel();
  display.add_pixel(pm25_logo_full, pm25_logo_rgb_6, 16);
  render.setDisplay(display);
  render.updateMap4x4();
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
  render.updateMap4x4();
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
    pm25_global_g = 140;
    pm25_global_b = 0;
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
    render.updateMap4x4();
    pm25_counter += 1;
  }
  else
  {
    _DPRINTLN("COLOR is NULL");
    display.clear_pixel();
    display.add_pixel(pm25_logo_full, icon_color_yellow, 16);
    display.add_pixel(icon_x, icon_color_red, 4);
    render.setDisplay(display);
    render.updateMap4x4();
    pm25_blink_time = 200;
  }
}

void menu_02_boxRotate()
{
  display.clear_pixel();
  display.add_pixel(box, 4);
  render.setDisplay(display);
  render.updateMap4x4();
  delay(2000);

  display.clear_pixel();
  display.add_pixel(box2, 4);
  render.setDisplay(display);
  render.updateMap4x4();
  delay(2000);

  display.clear_pixel();
  display.add_pixel(cpe2, 8);
  render.setDisplay(display);
  render.updateMap4x4();
  delay(2000);

  display.clear_pixel();
  display.add_pixel(cpe, 8);
  render.setDisplay(display);
  render.updateMap4x4();
  delay(1000);

  display.clear_pixel();
  display.add_pixel(line, 4);
  render.setDisplay(display);
  render.updateMap4x4();
  delay(2000);
  _DPRINTLN("Wait");
}

void menu_02_boxRotate_Rotate()
{
  render.updateMap4x4();
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

byte menu_04_dot_run_number = 0;
void menu_04_dot_run()
{

  for (byte i = 0; i < 16; i++)
  {
    display.add_dot(i, pm25_logo_rgb_3);
    render.setDisplay(display);
    render.updateMap4x4();
    delay(100);
  }

  for (byte i = 0; i < 16; i++)
  {
    display.add_dot(i, (uint32_t)0);
    render.setDisplay(display);
    render.updateMap4x4();
    delay(100);
  }
}

void menu_05_randomsurface()
{
  uint16_t hua_color[16];
  uint8_t stu_color = 100;
  uint8_t lua_color[16];
  MYHSL temp_hsl;

  bool direction[16];

  for (byte i = 0; i < 16; i++)
  {
    hua_color[i] = (uint16_t)random(0, 360);
    lua_color[i] = (uint8_t)random(1, 50);

    temp_hsl = MYHSL(hua_color[i], 54, lua_color[i]);
    display.add_dot(i, temp_hsl.toInt32());
    render.setDisplay(display);
    direction[i] = (bool)random(0, 1);
  }
  render.updateMap4x4();

  // Loop Fading
  while (true)
  {
    for (byte i = 0; i < 16; i++)
    {
      temp_hsl = MYHSL(hua_color[i], 54, lua_color[i]);
      display.add_dot(i, temp_hsl.toInt32());
      render.setDisplay(display);
      lua_color[i] += 1 * (direction[i]) ? 1 : -2;
      lua_color[i] = (lua_color[i] <= 0) ? 0 : lua_color[i];
      lua_color[i] = (lua_color[i] >= 50) ? 50 : lua_color[i];
      if (lua_color[i] <= 0 || lua_color[i] >= 50)
      {
        direction[i] = !direction[i];
      }
    }
    if (random(4977) == 31)
    {
      break;
    }
    render.updateMap4x4();
    delay(10);
  }
}

void menu_switching(byte num_menu)
{
  byte can_play_led[] = {0, 1, 2, 3, 7, 11, 15, 14, 13, 12, 8, 4};
  num_menu = (num_menu <= 12) ? num_menu : 12;

  for (byte i = 0; i < num_menu; i++)
  {
    display.add_dot(can_play_led[i], (uint32_t)0xffffff);
    render.setDisplay(display);
    render.updateMap4x4();
    delay(100);
  }
  for (byte i = num_menu; i < 12; i++)
  {
    display.add_dot(can_play_led[i], (uint32_t)0x1e00ff);
    render.setDisplay(display);
    render.updateMap4x4();
    delay(30);
  }
  delay(1000);

  for (int t = 0; t < 3; t++)
  {
    // Close LED
    delay(150);
    for (byte i = 0; i < 12; i++)
    {
      display.add_dot(can_play_led[i], (uint32_t)0x0);
    }
    render.setDisplay(display);
    render.updateMap4x4();
    // Open Agine
    delay(150);
    for (byte i = 0; i < num_menu; i++)
    {
      display.add_dot(can_play_led[i], (uint32_t)0xffffff);
    }
    for (byte i = num_menu; i < 12; i++)
    {
      display.add_dot(can_play_led[i], (uint32_t)0x1e00ff);
    }
    render.setDisplay(display);
    render.updateMap4x4();
  }
  delay(1500);
  display.clear_pixel();
  render.setDisplay(display);
  render.updateMap4x4();
}

void menu_06_bar()
{
  byte random_bar_history[4] = {0, 0, 0, 0};
  byte index_height[4][4] = {{12, 4, 8, 0}, {13, 9, 5, 1}, {14, 10, 6, 2}, {15, 11, 7, 3}};
  byte bar_random_now = (byte)random(0, 4);
  MYHSL color_hsl_p[4];
  for (int i = 0; i < 4; i++)
  {
    random_bar_history[i] = (byte)random(0, 4);
    color_hsl_p[i] = MYHSL((uint16_t)random(0, 360), 100, 50);
  }
  while (true)
  {
    // BarHistory
    for (byte aa = 0; aa < 3; aa++)
    {
      random_bar_history[aa] = random_bar_history[aa + 1];
      color_hsl_p[aa] = color_hsl_p[aa + 1];
    }
    random_bar_history[4] = (byte)random(0, 4);
    color_hsl_p[4] = MYHSL((uint16_t)random(0, 360), 100, 50);
    display.clear_pixel();
    for (byte i = 0; i < 16; i++)
    {
      display.add_dot(i, (uint32_t)0xffffff);
    }
    render.setDisplay(display);
    if (random(63333) == 0)
    {
      break;
    }
    //render.updateMap4x4();

    // Generate Bar
    for (byte histo = 0; histo < 4; histo++)
    {
      for (byte i = 0; i < random_bar_history[histo]; i++)
      {
        display.add_dot(index_height[histo][i], color_hsl_p[histo].toInt32());
        //render.setDisplay(display);
      }
    }
    render.setDisplay(display);
    render.updateMap4x4();
    delay(1000);
  }
}

void setup()
{

  // End of trinket special code

  Serial.begin(115200);
  render.displayBegin4x4();

  _DPRINTLN("Start MCU");

  pinMode(D7, OUTPUT);
  delay(100);
  //test_01_testColor();
  // menu_switching(1);
  //menu_00_showCPELogo();
  //delay(3000);
  //menu_switching(2);
  // delay(2000);
  //menu_04_dot_run();
  // menu_switching(3);
  //menu_05_randomsurface();
  //menu_switching(6);
  menu_06_bar();
  // menu_switching(12);

  /* ---- Wifi -----*/
  _DPRINTLN("Start Wifi");

  //accessWifi();
  //networkStep_01hostConnection();
  ////networkStep_02CheckHost();

  //delete HTTPSRedirect object;
  //delete client;
  //client = nullptr;

  /* ----- Wifi ------*/
  _DPRINTLN("Start Loop");
  // menu_04_dot_run();
  //delay(2000);
}
byte menu_index = 0;

void loop()
{
  menu_index = (byte)random(0, 4);
  if (menu_index == 0)
  {

    menu_switching(1);
    menu_00_showCPELogo();
    delay(6000);
    menu_04_dot_run();
  }
  else if (menu_index == 2)
  {
    menu_switching(3);
    menu_05_randomsurface();
    menu_04_dot_run();
  }
  else if (menu_index == 2)
  {
    int temp_delay = 500;
    display.clear_pixel();
    display.add_pixel(pm25_logo_full, pm25_logo_rgb_1, 16);
    render.setDisplay(display);
    render.updateMap4x4();
    delay(3000);
    while (true)
    {
      FadingLEDAllFromCouterValue(abs(pm25_counter), 0, 50, 255, 206, 8);
      pm25_counter += 1;
      if (pm25_counter == 50)
      {
        pm25_counter = -50;
      }
      if (random(2000) == 32)
      {
        break;
      }
    }
  }
  else
  {
    menu_04_dot_run();
  }
  //menu_03_pm25();
  //delay(pm25_blink_time);
}
