// OTA LIBRARY
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

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

int senSorSampling;

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
DisplayRender render = DisplayRender(display);


void setup() {
  ota_setup();

#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code

  Serial.begin(115200);

  pixels.begin();
  pinMode(D7, OUTPUT);


  //logo intilail
  display.clear_pixel();
  display.add_pixel(cpe_logo_green, cpe_logo_green_rgb, 10);
  display.add_pixel(cpe_logo_blue, cpe_logo_blue_rgb, 5);
  display.add_pixel(cpe_logo_white, cpe_logo_white_rgb, 1);
  render.setDisplay(display);
  render.updateMap();
  delay(10000);

  RGB rgb = RGB(0, 0, 0);
  int hslcolor[16];
  double fadeprogress[16];
  int fadedirection[16];

  for (int i = 0; i < 16; i++) {
    fadedirection[i] = 1;
    fadeprogress[i] = random(1, 49);
    hslcolor[i] = random(0, 360);
  }

  int yyy = 0;
  // while(yyy<8){

  //--- Sound Statistic Begin
  int analog_read_once = 0;
  int sound_conf_sampling = 10;
  int sound_sampling_mean = 0;
  int sound_sum = 0;
  int sound_max_peek = 0;
  byte sound_max_peek_time_counter = 0;

  int sound_clap_threshole_high = 580;
  byte sound_clap_time_counter = 0;

  int sound_raw_peek_counter;
  int fade_divider_value = 1;

  int sound_global_sum = 0;
  int sound_global_mean = 0;


  //--- Sound Statistic End

  for (int i = 0; i <= 100000; i++) {
    ArduinoOTA.handle();
    analog_read_once = analogRead (sensorPin);
    if (1) {
      sensorValue =  analog_read_once;

      if (sensorValue > 520) {
        sensorValue = 520;
      }
    }
    sound_sum += analog_read_once;
    if (sound_max_peek < analog_read_once) {
      sound_max_peek = analog_read_once;
      sound_max_peek_time_counter = 0;
    } else {
      sound_max_peek_time_counter += 1;
      if (sound_max_peek_time_counter == (2 * sound_conf_sampling)) {
        sound_max_peek = sound_sampling_mean;
        sound_max_peek_time_counter = 0;
      }
    }
    if (i % sound_conf_sampling == 0) {
      sound_sampling_mean = sound_sum / sound_conf_sampling;
      sound_sum = 0;
    }//Sound Mod End
    if (sound_max_peek > sound_clap_threshole_high) {

      digitalWrite(D7, HIGH);
    } else {
      digitalWrite(D7, LOW);
    }

    /*if(i%500==0){
      for(int j=0; j < 16; j++){

      hslcolor[j] = random(0,360);
      }}*/
    for (int k = 0; k < 16; k++) {
      fade_divider_value = (40.00 - sensorValue * 39 / 520);

      fadeprogress[k] +=  fadedirection[k] * exp((fadeprogress[k] / fade_divider_value));
      if (fade_divider_value <= 15) {
        for (int j = 0; j < 16; j++) {

          hslcolor[j] = random(0, 360);
        }
      }

      if (fadeprogress[k] >= 47 || fadeprogress[k] <= 5) {
        fadeprogress[k] = (fadeprogress[k] >= 47) ? 47 : 5;
        fadedirection[k] *= -1;

      }
      rgb = rgb.hsl2rgb(hslcolor[k], 100, fadeprogress[k]);


      //   Serial.print(fadeprogress[0]);
      //   Serial.println();
      //  rgb = rgb.hsl2rgb(hslcolor[k],100,fadeprogress[i]);
      // fadeprogress[i] = (fadeprogress[i]++) % 50;
      openSingle(k, rgb.toInt32());
    }

    delay(33);
    Serial.print(fade_divider_value);
    Serial.print("\t");
    Serial.print(sensorValue);
    Serial.print("\t");
    Serial.print(analogRead (sensorPin));
    Serial.print("\t");
    Serial.print(sound_sampling_mean);
    Serial.print("\t");
    Serial.print(sound_max_peek);
    Serial.println();
    pixels.show();
  }
  /*for(int i=50;i>=0;i--){
    for(int k=0;k<16;k++){
    rgb = rgb.hsl2rgb(hslcolor[k],100,(i+fadeprogress[i])%50);
    //rgb = rgb.hsl2rgb(hslcolor[k],100,fadeprogress[i]--);
    //  fadeprogress[i] = (fadeprogress[i]--) % 50;
    openSingle(k,rgb.toInt32());
    }
    delay(33);
    pixels.show();
    }*/
  /* FadingLED(0,360,720);
    FadingLED(360,0,720);*/
  // yyy++;
  //}

  delay(2000);

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

int count = 0;
void loop() {
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
