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
  for (int i = num * NUM_RING_ONE_PIX ; i < ((num + 1)*NUM_RING_ONE_PIX) ; i++) {
    pixels.setPixelColor(i, RGB);
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

RGB rgb = RGB(0, 0, 0);
int hslcolor[16];
double fadeprogress[16];
int fadedirection[16];
void setup() {
  //ota_setup();

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



  for (int i = 0; i < 16; i++) {
    fadedirection[i] = 1;
    fadeprogress[i] = random(1, 49);
    hslcolor[i] = random(0, 360);
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



}
long prev_mic_time = millis();
int prev_mic = 0;
int prev_mic_max = 0;
boolean prev_is_max = false;
boolean prev_is_slienc = false;
long prev_is_slienc_time = millis();

boolean isClap (int mic, int avr_mic_max, int avr_mic_avr) {
  digitalWrite(D7, LOW);
  long wait_time = 1000;
  int res_slience = mic - avr_mic_max ;
  if (mic > 100 ) {
    prev_is_slienc_time = millis();
    prev_is_slienc = false;
    Serial.print("noooo___________is sound\n");
  }



  if (mic >= sound_clap_threshole_high) {
    int res = mic - prev_mic_max;
    if (res > 400) {
      prev_mic_time = millis();
      prev_is_max = true;
      Serial.print("isDetect\n");
    } else {
      prev_mic_max = avr_mic_max;
      Serial.print("noo Notdetect\n");
    }
  }
  if (prev_is_max) {
    if (millis() - prev_mic_time > wait_time) {
      Serial.print("Incunction");
      if (mic <= 250) {

        if (avr_mic_avr <= 100) {
          //          prev_is_slienc = true;
          if (prev_is_slienc) {
            Serial.print("\n\n\n\nmic100000000000000000+++++++++++++++++++++++++000000___________________0000\n");
            digitalWrite(D7, HIGH);
            delay(1000);
            prev_mic_time = millis();
            prev_is_max = false;
            return true;
          }
        }
        else {
          Serial.print("prev not slien___________________________________________\n\n\n\nct=======");
          Serial.print(avr_mic_avr);
          Serial.print("\n");
          prev_is_max = false;
        }

      } else {
        Serial.print("Mic over 100\n");
        prev_is_max = false;
      }
    }

  }

  if (millis() - prev_is_slienc_time >= 2000 ) {

    if (avr_mic_avr <= 100) {
      prev_is_slienc = true;
    }
    /*
        //    Serial.print("slienc\n");
        if (prev_is_max) {
          prev_is_slienc_time = millis();
          prev_is_max = false;
        } else {
          prev_is_slienc = true;
        }*/
  }
  return false;

}

int munu_state = 0;
int stat_avr_long_time = 0;
int stat_avr_long_time_sum = 0;
int count = 0;
bool isClaper = false;
void loop() {
  if (munu_state % 4 == 0) {
    //ArduinoOTA.handle();
    analog_read_once = analogRead (sensorPin);
    if (true) {
      sensorValue =  analog_read_once;
      if (sensorValue > 520) {
        sensorValue = 520;
      }
    }
    sound_sum += analog_read_once;
    stat_avr_long_time_sum += analog_read_once;

    if (sound_max_peek < analog_read_once) {
      sound_max_peek = analog_read_once;
      sound_max_peek_time_counter = 0;
    } else {
      sound_max_peek_time_counter += 1;
      if (sound_max_peek_time_counter == (2 * sound_conf_sampling)) {
        sound_max_peek = analog_read_once;
        sound_max_peek_time_counter = 0;
      }
    }
    if (count % 100 == 0) {
      stat_avr_long_time = int(stat_avr_long_time_sum / 100);
      stat_avr_long_time_sum = 0;

    }
    if (count % sound_conf_sampling == 0) {
      sound_sampling_mean = int(sound_sum / sound_conf_sampling);
      sound_sum = 0;
    }//Sound Mod End
    /*if (sound_max_peek > sound_clap_threshole_high) {
      digitalWrite(D7, HIGH);
      } else {
      digitalWrite(D7, LOW);
      }*/
    isClaper = isClap(analog_read_once, sound_max_peek, stat_avr_long_time);

    if (isClaper) {
      munu_state += 1;
    }
    for (int k = 0; k < 16; k++) {
      fade_divider_value = (40.00 - sensorValue * 39 / 520);

      fadeprogress[k] +=  fadedirection[k] * exp((fadeprogress[k] / fade_divider_value));
      if (fade_divider_value <= 18) {
        for (int j = 0; j < 16; j++) {
          hslcolor[j] = random(0, 360);
        }
      }

      if (fadeprogress[k] >= 47 || fadeprogress[k] <= 5) {
        fadeprogress[k] = (fadeprogress[k] >= 47) ? 47 : 5;
        fadedirection[k] *= -1;

      }
      rgb = rgb.hsl2rgb(hslcolor[k], 100, fadeprogress[k]);
      openSingle(k, rgb.toInt32());
    }
    pixels.show();
    count++;
  }
  else if (munu_state % 4 == 1) {
    //logo intilail
    display.clear_pixel();
    display.add_pixel(cpe_logo_green, cpe_logo_green_rgb, 10);
    display.add_pixel(cpe_logo_blue, cpe_logo_blue_rgb, 5);
    display.add_pixel(cpe_logo_white, cpe_logo_white_rgb, 1);
    render.setDisplay(display);
    render.updateMap();
    pixels.show();
    isClaper = isClap(analog_read_once, sound_max_peek, stat_avr_long_time);

    if (isClaper) {
      munu_state += 1;
    }
  }

  delay(33);
  Serial.print(fade_divider_value);
  Serial.print("\t");
  Serial.print(sensorValue);
  Serial.print("\t");
  Serial.print(analogRead (sensorPin));
  Serial.print("\t");
  Serial.print(stat_avr_long_time);
  Serial.print("\t");
  Serial.print(sound_sampling_mean);
  Serial.print("\t");
  //    Serial.print(stat_avr_long_time_sum);
  //    Serial.print("\t");
  Serial.print(sound_max_peek);
  Serial.println();
}

