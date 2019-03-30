// OTA LIBRARY
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>


#include "HTTPSRedirect.h"
#include "DebugMacros.h"

// InClass LIBRARY
#include <string.h> //Memcpy
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
// Local Library
#include "Display.h"
#include "RGB.h"
#include <math.h>

#include <stdlib.h>

#ifdef __AVR__
#include <avr/power.h>
#endif
/* ------ Configuration ------- */
#define LED_PIN D1
#define NUM_RING_ALL_PIX 384
const int NUM_RING_ONE_PIX = 24;
int DELAY_VAL = 1000;
int sensorPin = A0;
int sensorValue;
int count = 0;
int pm25_counter = 0;
int pm25_blink_time = 1000;
unsigned long pm25_timeUpdate = 0;
int senSorSampling;
int pm25_lastColor = 0;

int app_pm25_level = -99;
static int pm25_updatetinInSec = 60;

int * pm25_color_rgb;


/* ----------------- PM25 WIFI -------------------*/
// Fill ssid and password with your network credentials
const char* ssid = "";
const char* password = "";

const char* host = "script.google.com";
// Replace with your own script id to make server side changes
const char *GScriptId = "AKfycbx7cup7-dfKPlmfDQVlyXO6Co0QbBm2ae9cJgcyaKcPfk0hMoM";
const int httpsPort = 443;

// echo | openssl s_client -connect script.google.com:443 |& openssl x509 -fingerprint -noout
const char* fingerprint = "C2:00:62:1D:F7:ED:AF:8B:D1:D5:1D:24:D6:F0:A1:3A:EB:F1:B4:92";


// Write to Google Spreadsheet
String url = String("/macros/s/") + GScriptId + "/exec";
HTTPSRedirect* client = nullptr;


void accessWifi() {
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);
  Serial.flush();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void networkStep_01hostConnection() {
  // Use HTTPSRedirect class to create a new TLS connection
  Serial.println("Start CLientCall");
  client = new HTTPSRedirect(httpsPort);
  client->setInsecure();
  client->setPrintResponseBody(false);
  client->setContentTypeHeader("application/text");
  Serial.println("Client Call OK");
}

void networkStep_02CheckHost() {
  // Try to connect for a maximum of 5 times
  Serial.println("Start Check Host");
  bool flag = false;
  int retval =0;
  for (int i = 0; i < 5; i++) {
    
    
    retval = client->connect(host, httpsPort);
    
    Serial.print("retval=");
    Serial.println(retval);
    if (retval == 1) {
      flag = true;
      break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }
  Serial.println("Start Flag");

  if (!flag) {
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    Serial.println("Exiting...");
    return;
  }
}

void network_getValue() {
  Serial.println("Start GetValue");
  client->GET(url, host);
  String line = client->getResponseBody();
  Serial.println("--------");
  Serial.println(line);
}

int getPM25Level() {
  
  String line = client->getResponseBody();
  Serial.println("Line2Int");
  int val = line.toInt();
  Serial.println(val);
  
  return val;
}

int network_secureGetPM25Lavel() {
  static int error_count = 0;
  static int connect_count = 0;
  const unsigned int MAX_CONNECT = 5;
  static bool flag = false;
  int value = -2;

  if (!flag) {
    client = new HTTPSRedirect(httpsPort);
    client->setInsecure();
    flag = true;
    client->setPrintResponseBody(false);
    client->setContentTypeHeader("application/text");
  }

  if (client != nullptr) {
    if (!client->connected()) {
      client->connect(host, httpsPort);
    }
  }
  else {
    DPRINTLN("Error creating client object!");
    error_count = 5;
  }



  /*if (connect_count > MAX_CONNECT) {
    //error_count = 5;
    connect_count = 0;
    flag = false;
    delete client;
    return -99;
    }*/


  if (client->GET(url, host)) {
    ++connect_count;
    value = getPM25Level();

  }
  else {
    ++error_count;
    DPRINT("Error-count while connecting: ");
    DPRINTLN(error_count);
  }

  if (error_count > 3) {
    Serial.println("Halting processor...");
    delete client;
    client = nullptr;
    Serial.flush();
    //ESP.deepSleep(0);
  }

  return value;
}
/*------------- END PM25 WIFI ------------------*/

/* ------ Intitialize --------*/
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_RING_ALL_PIX, LED_PIN, NEO_GRB + NEO_KHZ800);
void openSingle(int num, uint32_t RGB) {
  int broken_led = 3;
  int temp_num = num;
  //Skip broken led
  if (num > broken_led) {
    temp_num -= 1;
  }

  if (num != broken_led) {
    for (int i = temp_num * NUM_RING_ONE_PIX ; i < ((temp_num + 1)*NUM_RING_ONE_PIX) ; i++) {
      pixels.setPixelColor(i, RGB);
    }
  }



  //pixels.show();
}

void openSingleRGB(int num, byte r, byte g, byte b) {
  for (int i = num * NUM_RING_ONE_PIX ; i < ((num + 1)*NUM_RING_ONE_PIX) ; i++) {
    pixels.setPixelColor(i, r, g, b);
  }
  //pixels.show();
}
class DisplayRender {
    const static int map_size = 16;
  public:
    byte pos[map_size];
    int color[map_size];
    char type[map_size];
    Display display_map;

    DisplayRender(Display);
    void setDisplay(Display dis) {
      display_map = dis;
    }
    void updateMap();
};

DisplayRender::DisplayRender(Display disp) {
  display_map = disp;
}


void DisplayRender::updateMap() {
  for (int i = 0; i < 16; i++) {
    openSingle(i, display_map.getColorAt(i));
    //Serial.print(display_map.getColorAt(i));
    //Serial.print(";");
  }
  //Serial.println();
  pixels.show();
}
void FadingLED(int s, int f, int d) {
  RGB rgb = RGB(0, 0, 0);
  int x1 = (s <= f) ? s : f;
  int x2 = (s <= f) ? f : s;
  int x3 = (s <= f) ? d / (f - s) : d / (s - f);
  for ( int i = x1 ; i <= x2 ; i++) {
    rgb = rgb.hsl2rgb(i, 100, 50);
    for (int k = 0; k < 16; k++) {
      openSingle(k, rgb.toInt32());
    }
    delay(x3);
    pixels.show();
  }
}


void ledIntitialize() {

}

void ota_setup() {
  const char* ssid = "itim";
  const char* password = "ilovecpe";
  const char* host = "iLight_host";
  Serial.println("Booting");

  Serial.println("Booting");
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);

  Display display_ota = Display();
  DisplayRender displayRender_ota = DisplayRender(display_ota);
  byte box[] = {6, 9, 10};
  int box_color[] = {255, 120, 255};
  byte box2[] = {5};
  int box2_color[] = {0, 0, 255};
  int ota_counter = 0;
  display_ota.add_pixel(box, box_color, 3);
  display_ota.add_pixel(box2, box2_color, 1);
  displayRender_ota.setDisplay(display_ota);
  displayRender_ota.updateMap();
  pixels.show();
  delay(500);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    Serial.println("Retrying connection...");
    if (millis() % 3 == 0) {
      display_ota.rotate_left();
    }
    displayRender_ota.setDisplay(display_ota);
    displayRender_ota.updateMap();
    pixels.show();

  }


  /* configure dimmers, and OTA server events */
  ArduinoOTA.setHostname(host);
  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    //ESP.restart();
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");

  });

  /* setup the OTA server */
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

class IntegerList {
  public:
    int *data;
    IntegerList(int);
    void pop(int);
};
IntegerList::IntegerList(int L) {
  data = (int*)malloc(L * sizeof(int));
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
int cpe_logo_green_rgb[] = {0, 255, 0};
byte cpe_logo_blue[] = {1, 6, 8, 9, 10};
int cpe_logo_blue_rgb[] = {0, 0, 255};
byte cpe_logo_white[] = {0};
int cpe_logo_white_rgb[] = {200, 200, 200};

byte pm25_logo_down[8] = {8, 9, 10, 11, 12, 13, 14, 15};
byte pm25_logo_up[8] = {0, 1, 2, 3, 4, 5, 6, 7};
byte pm25_logo_full[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
int pm25_logo_rgb_1[] = {0, 255, 0};
int pm25_logo_rgb_2[] = {255, 255, 0};
int pm25_logo_rgb_3[] = {255, 150, 0};
int pm25_logo_rgb_4[] = {255, 0, 0};
int pm25_logo_rgb_5[] = {255, 0, 255};
int pm25_logo_rgb_6[] = {165, 0, 70};
int pm25_logo_rgb_error[] = {10, 0, 130};
DisplayRender render = DisplayRender(display);


void FadingLEDAllFromCouter( uint8_t _value, uint8_t _start, uint8_t _end, int lastColor) {
  Serial.println("HSL2RGB CAL");
  RGB rgb = rgb.hex2rgb(lastColor);  
  uint8_t _val = constrain(_value, _start, _end);

  for ( uint16_t i = 0 ; i < 16 ; i++) {
    // TODO
    //rgb = rgb.hex2rgb(display.getColorAt(i));
    rgb.rgb2hsl_convert();
    rgb = rgb.hsl2rgb(rgb.get_h(), rgb.get_s(), _val);
    display.set_color_n(i, rgb.toInt32());
  }
}

void test_01_testColor() {
  int temp_delay = 2000;
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


void menu_00_showCPELogo() {
  //logo intilail
  display.clear_pixel();
  display.add_pixel(cpe_logo_green, cpe_logo_green_rgb, 10);
  display.add_pixel(cpe_logo_blue, cpe_logo_blue_rgb, 5);
  display.add_pixel(cpe_logo_white, cpe_logo_white_rgb, 1);
  render.setDisplay(display);
  render.updateMap();
  delay(10000);
}





int * menu_03_pm25_colorSelect(int level) {
  switch (level) {
    case 1: pm25_blink_time = 300; return  pm25_logo_rgb_1;
    case 2: pm25_blink_time = 300; return pm25_logo_rgb_2;
    case 3: pm25_blink_time = 100; return pm25_logo_rgb_3;
    case 4: pm25_blink_time = 50; return pm25_logo_rgb_4;
    case 5: pm25_blink_time = 25; return pm25_logo_rgb_5;
    case 6: pm25_blink_time = 250; return  pm25_logo_rgb_6;
    default: pm25_blink_time = 1000; return  pm25_logo_rgb_error;
  } return pm25_logo_rgb_error;
}

void menu_03_pm25() {
  //Note: Dont make delay in this function
  unsigned long pm25_diff_time = millis() - pm25_timeUpdate;
  if (pm25_diff_time > 1000*pm25_updatetinInSec || app_pm25_level == -99) {
    Serial.println("Start Update PM25");
    app_pm25_level = network_secureGetPM25Lavel();
    pm25_color_rgb = menu_03_pm25_colorSelect(app_pm25_level);
    Serial.print("Update PM25=");
    Serial.println(app_pm25_level);
    pm25_timeUpdate = millis();
    display.clear_pixel();
    display.add_pixel(pm25_logo_full, pm25_color_rgb, 16);
    pm25_lastColor = display.getColorAt(0);
  }
  if (pm25_color_rgb != nullptr) {
    if (app_pm25_level >= 6) {
      if (pm25_counter % 2 == 0) {
        Serial.println("PM25 LED RUN DOWN");
        display.clear_pixel();
        display.add_pixel(pm25_logo_down, pm25_color_rgb, 8);

      } else {
        Serial.println("PM25 LED RUN UP");
        display.clear_pixel();
        display.add_pixel(pm25_logo_up, pm25_color_rgb, 8);
      }
    } else {
      
      if (pm25_counter >= 0) {
        FadingLEDAllFromCouter(pm25_counter, 0, 50, pm25_lastColor);
      } else {
        FadingLEDAllFromCouter(abs(pm25_counter), 0, 50, pm25_lastColor);
      }
      
      if (pm25_counter == 50) {
        pm25_counter = -50;
      }

    }

    render.setDisplay(display);
    render.updateMap();
    pm25_counter += 1;
  } else {
    Serial.println("COLOR is NULL");
  }


}



void menu_02_boxRotate() {
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
  Serial.println("Wait");
}

void menu_02_boxRotate_Rotate() {
  render.updateMap();
  //pixels.show();

  if (count > 10) {
    display.rotate_left();
    render.setDisplay(display);
    // delay(1000);
  }
  if (count % 5 == 0) {
    display.add_pixel(dot, 1);
    render.setDisplay(display);
  }
  count++;


  delay(500);
  
}


void setup() {
  //ota_setup();

#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code

  Serial.begin(115200);

  pixels.begin();

  Serial.println("Start MCU");
  
  pinMode(D7, OUTPUT);
  delay(100);
  test_01_testColor();
  menu_00_showCPELogo();
  delay(100);

  /* ---- Wifi -----*/
  Serial.println("Start Wifi");

  accessWifi();
  networkStep_01hostConnection();
  networkStep_02CheckHost();



  //delete HTTPSRedirect object;
  delete client;
  client = nullptr;

  /* ----- Wifi ------*/
  Serial.println("Start Loop");
  delay(2000);


}


void loop() {
  menu_03_pm25();
  delay(pm25_blink_time);


}
