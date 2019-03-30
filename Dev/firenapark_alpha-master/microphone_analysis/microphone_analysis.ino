#include <stdlib.h>
#include <math.h>

class GraphList{
  public:
  int *data;
  unsigned int sampling=0;
  GraphList(unsigned int sampling);
  void push(int);  
  bool check_peek();
  bool what_peek(); 
};

GraphList::GraphList(unsigned int _sampling){
  unsigned int smp=0;
  if(_sampling > 3){
    if(_sampling%2==0){
      smp = _sampling+1;
    }else{
      smp = _sampling;
    }
    data = (int*)malloc(sizeof(int)*smp);
    for(int i=0;i<smp;i++){
      data[i]=0;
    }
    sampling = smp;
  }
}

void GraphList::push(int _data){
  for(int i=1;i<sampling;i++){
    data[i] = data[i-1];
  }
  data[0] = _data;
}

bool GraphList::check_peek(){
  int mid_data_index = (sampling+1)/2;
  
  short int left_hand_slope = 0;
  short int right_hand_slope = 0;

  for(int i=0;i<mid_data_index;i++){
    // half loop
    if(abs(data[mid_data_index-i]) > abs(data[mid_data_index-i-1])){
      //center - left = +slope
      left_hand_slope += 1;
    }else{
      left_hand_slope = 0;
    }
    if(abs(data[mid_data_index+i]) > abs(data[mid_data_index+i+1])){
      //center - right = -slope
      right_hand_slope += 1;
    }else{
      right_hand_slope = 0;
    }   
  }

  if(left_hand_slope==right_hand_slope){
    //has peek
    return true;
  }return false;
  
}

bool GraphList::what_peek(){
  int mid_data_index = (sampling+1)/2;
  if(data[mid_data_index] - data[0] > 0){
    //positive peek
    return true;
  }return false;
  // negative peek
}




//--- Sensor LED ---
int sensorValue=0;
#define sensorPin A0


//--- Sensor LED END ---


//--- Sound Statistic Begin
// Threshole
int microphone_min_threshole = 20;
int microphone_max_threshole = 620;
int microphone_range = microphone_max_threshole - microphone_min_threshole;
int sound_clap_threshole_high = 580;
int sound_clap_threshole_low = 50;


// Sound sensor Variable
int analog_read_once = 0;
int sound_realtime_sampling = 10;
int sound_realtime_mean = 0;
int sound_realtime_sum = 0;

int sound_long_sum = 0;
int sound_long_mean = 0;
int sound_long_mean_prev = 0;

int sound_max_peek = 0;
int sound_max_peek_sum = 0;
byte sound_max_peek_time_counter = 0;

byte sound_clap_time_counter = 0;

//peek analysis [in sample peek not global]
int sound_peek_prev = 0;
int sound_raw_peek_counter;
int fade_divider_value=1;

int sound_global_sum = 0;
int sound_global_mean = 0;
int sound_global_mean_prev =0;

/*---- Timer ----*/
int sound_timer_counter[] = {0,0,0};
// t[1] is clap
// t[2] is 

int sound_timer_realtime_prev[] = {0,0};
// t[1] is realtime average
// t[2] is realtime  long average


// Graph statistic
int sound_graph_min_avr = 0;
int sound_graph_max_avr = 0;
int sound_graph_long_mean = 0;
int sound_graph_long_sum = 0;
int sound_graph_long_count = 0;



//--- Sound Statistic End

/*------ Music NN model ------*/
// Scoring Music
float music_type_score[] = {0,0,0,0};
/*  [0] คือ เพลงร้องที่มี พิน กลอง เป็นจังหวะหลัก ไม่มีเสียงอื่น ex [A song for the season]
 *  [1] คือ 
* 
*
*/

float music_tempo_score[] = {0,0,0,0};
/* [0] high peek per realtime_sampling
 * [1] high percentage (vol > 50) per long_mean
 */



/*------ Algorithm Variable --------*/
unsigned long now_time_milli=0;
unsigned long prev_milli = 0;

bool is_clap = false;
int is_clap_counter=0;

float sound_vol_percentage=0;
bool is_slilence =false;

GraphList graph = GraphList(5);



void setup(){
  Serial.begin(115200);
  pinMode(sensorPin,INPUT);
  pinMode(D0,OUTPUT);
  pinMode(D1,OUTPUT);
  pinMode(D2,OUTPUT);
  pinMode(D3,OUTPUT);
  pinMode(D4,OUTPUT);
  pinMode(D5,OUTPUT);
  pinMode(D7,OUTPUT);
  pinMode(D6,OUTPUT);
}

void loop() {
  now_time_milli=millis();
  // Global Delay
  if(now_time_milli - prev_milli >= 10 ){
  analog_read_once = analogRead (sensorPin);
  graph.push(analog_read_once);
  
  //Transform real Sensor amplitude to Percentage Amplitude (Volume)
  sound_vol_percentage = ((float)analog_read_once/(float)microphone_max_threshole)*100.0;

  
  if(sound_vol_percentage >= 50){
  // เพลงมันส์ๆ ที่มักจะมีกราฟอยู่ด้านบน
    digitalWrite(D3,HIGH);
  }else{
    digitalWrite(D3,LOW);
  }

   
  //ระบบวัดเสียงเพลงจากการตรวจสอบ peek
  if(graph.check_peek()){
    float temp_peek_range_precentage = 0.08633;
    if(analog_read_once - sound_realtime_mean > microphone_range*temp_peek_range_precentage){
      digitalWrite(D7,HIGH);  
    }else if(analog_read_once - sound_realtime_mean < -(microphone_range*temp_peek_range_precentage)){
      digitalWrite(D6,HIGH);
    } 

    if(abs(analog_read_once - sound_peek_prev) > 50 && sound_peek_prev != 0){
      digitalWrite(D5,HIGH);  
    }else{
      digitalWrite(D5,LOW); 
    }
    sound_peek_prev = analog_read_once;
  }else{
    digitalWrite(D7,LOW);
    digitalWrite(D6,LOW);
  }
  
  /*--- วัดเสียงเพลง แบบ percentage [by kanoon alogorithm] -----*/
  fade_divider_value = (40.00-sensorValue*39/520);
  {
    sensorValue =  analog_read_once;    
    if(sensorValue > 520){
      sensorValue = 520;
    }
  }
  
  // ท่อนฮุก ผ่านระบบ percentage
  if(fade_divider_value < 15){
    digitalWrite(D2,HIGH);
  }else{
    digitalWrite(D2,LOW);
  }
  /* --------- End kanoon algorithm -----------------*/

  /*---system statistic---*/
  sound_realtime_sum += analog_read_once;
  if(sound_max_peek < analog_read_once){
    sound_max_peek = analog_read_once;
    sound_max_peek_time_counter = 0;
    sound_max_peek_sum += sound_max_peek;
  }else{
    sound_max_peek_time_counter += 1;
    if(sound_max_peek_time_counter == (2*sound_realtime_sampling)){
      //sound_max_peek = sound_max_peek_sum/sound_max_peek_time_counter;
      sound_max_peek = sound_realtime_mean;
      sound_max_peek_time_counter = 0;
    }
  }
  
  
  //Sound mod begin
  sound_timer_realtime_prev[0]+=1;
  sound_timer_realtime_prev[1]+=1;
  
  if(sound_timer_realtime_prev[0] >= sound_realtime_sampling){
    //หา realtime_sampling_average
    sound_realtime_mean = sound_realtime_sum/sound_realtime_sampling;
    sound_long_sum += sound_realtime_sum;
    sound_realtime_sum = 0;
    sound_timer_realtime_prev[0] = 0;
    
  }
  if(sound_timer_realtime_prev[1] >= (10*sound_realtime_sampling)){
    sound_long_mean_prev = sound_long_mean;
    sound_long_mean = sound_long_sum/(10*sound_realtime_sampling);  

    // reset Long_sum
    sound_long_sum = 0;
    sound_timer_realtime_prev[1] = 0;
    
       
  }
  //Sound Mod End

   // หาผลรวมของเพลง
    sound_graph_long_count += 1;
    sound_graph_long_sum += analog_read_once;
    sound_graph_long_mean = sound_graph_long_sum/sound_graph_long_count; 
    //Serial.print(sound_graph_long_count);
    //Serial.print("\t");

  //Min Average
  if(analog_read_once < sound_realtime_mean){
    //when sensor less than realtime average 
    if(1){
      
    }
  }
  /*------ End statistic system -----*/

  //Find Slilence
  
  //if(abs(sound_long_mean - sound_realtime_mean) <= 15){
  if(abs(sound_long_mean - sound_long_mean_prev) <= 3 && sound_realtime_mean < microphone_min_threshole+5){
    // abs(mean-prev_mean) <= ค่าคงที่ จากการทดลองดูการแกว่งของค่า ในขณะเงียบ =[fix_when_change_sensor]
    // microphone_min_threshole + (low pass filter)  สำหรับกัน noise ในขณะเงียบ
    // this condition worked when the room is really silence
    is_slilence = true;
  }else{
    is_slilence = false;
  }

  //Find Peek clap
  if(is_slilence){
    digitalWrite(D1,HIGH);
    sound_graph_long_count = 0;
    sound_graph_long_sum = 0;

    // Clap check v2
    if(analog_read_once > sound_clap_threshole_high && !is_clap){
      is_clap = true;
      sound_timer_counter[0] = 0;      
    }else{
      if(is_clap){
        /*sound_timer_counter[0] += 1;
        if((analog_read_once > sound_clap_threshole_high) && sound_timer_counter[0] >= 20){
          is_clap = false;
        }else{
          if((sound_global_mean < sound_clap_threshole_low ) && sound_timer_counter[0] >= 30){
            digitalWrite(D4,HIGH);
          }
        }*/
        digitalWrite(D4,HIGH);
        delay(200);
      }
      
    }
    
  }else{
    sound_timer_counter[0] += 1;
    if(sound_timer_counter[0] >= 2*(10*sound_realtime_sampling)+10){
      // รอจนกระทั้งเป็น silence แต่ถ้าไม่เป็น ก็ถือว่าเป็นเสียงเพลง
      is_clap = false;
    }
    
    /*if((analog_read_once > sound_clap_threshole_high) && sound_timer_counter[0] >= 20){
      // ตบมืออีกครั้ง แต่ละไว้ก่อน
      //is_clap = false;
    }else{
      if((sound_global_mean < sound_clap_threshole_low ) && sound_timer_counter[0] >= 30){
        digitalWrite(D4,HIGH);
      }
    }*/
    digitalWrite(D4,LOW);
    digitalWrite(D1,LOW);
  }

  if(graph.check_peek( ) && sound_max_peek > sound_clap_threshole_high){
    
  }

  



  
    Serial.print(fade_divider_value);
    Serial.print("\t");
    Serial.print(sensorValue);
    Serial.print("\t");
    Serial.print(analogRead (sensorPin));
    Serial.print("\t");
    Serial.print(sound_realtime_mean);
    Serial.print("\t");
    Serial.print(sound_max_peek);
    Serial.print("\t");
    Serial.print(sound_long_mean);
    Serial.print("\t");
    Serial.print(sound_vol_percentage);
    Serial.print("\t");
    //Serial.print(is_clap);
    //Serial.print("\t");
    Serial.print(sound_graph_long_mean);
    Serial.println();
    prev_milli = now_time_milli;
  }


}
