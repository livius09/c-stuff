/* HTL-Wels
 * Levi Wimroither
 * 1CHIT
 * 18.6.2025
 * 
 * durch interupt von bewegungs sensor geweck
 * Spielt dann eine animation auf den bildschirm und erleuchtet die LED streifen in 3 sekündigen abständen
 * geht dann wieder in eine ruhe modus um energie zu sparen
 */
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <FastLED.h>
#include <Wire.h>

// How many leds are in the strip?
#define NUM_LEDS 9

// Data pin that led data will be written out over

// This is an array of leds.  One item for each led in your strip.
CRGB redLeds[NUM_LEDS];
CRGB greenLeds[NUM_LEDS];
CRGB pinkLeds[NUM_LEDS];

namespace CMD {
  constexpr uint8_t CMD                 = 0x00; //prefix befor sending a comand
  constexpr uint8_t DAT                 = 0x40; //prefix befor sending data

  constexpr uint8_t DISPLAY_OFF         = 0xAE;
  constexpr uint8_t DISPLAY_ON          = 0xAF;
  constexpr uint8_t INVERT_DISPLAY      = 0xA7; //White becomes black and vice versa
  constexpr uint8_t NORMAL_DISPLAY      = 0xA6; 
  constexpr uint8_t ALL_PIXELS_ON       = 0xA5; //All pixels on regardless of memory
  constexpr uint8_t RESUME_TO_RAM       = 0xA4; //Resume RAM content display

  constexpr uint8_t SET_MEMORY_MODE     = 0x20; //Followed by mode: 0x00 Horizontal (recommended), 0x01 Vertical, 0x02 Page addressing
  constexpr uint8_t MEM_MODE_HORIZONTAL = 0x00; //normal
  constexpr uint8_t MEM_MODE_VERTICAL   = 0x01;
  constexpr uint8_t MEM_MODE_PAGE       = 0x02;

  constexpr uint8_t SET_COLUMN_ADDR     = 0x21; //Followed by start and end column
  constexpr uint8_t SET_PAGE_ADDR       = 0x22; //Followed by start and end page

  constexpr uint8_t SET_START_LINE      = 0x40;  // + 0-63 value
  constexpr uint8_t SET_SEGMENT_REMAP_ltr = 0xA0; //left to right normal
  constexpr uint8_t SET_SEGMENT_REMAP_rtl = 0xA1; //right to left 
  constexpr uint8_t Set_Contrast        = 0x81;   //Range 0x00–0xF

  constexpr uint8_t SET_COM_OUT_NORMAL  = 0xC0;
  constexpr uint8_t SET_COM_OUT_FLIPPED = 0xC8;

  constexpr uint8_t SET_MULTIPLEX_RATIO = 0xA8;  // + value (0x3F for 64 lines)
  constexpr uint8_t SET_DISPLAY_OFFSET  = 0xD3;  //Followed by offset byte
  constexpr uint8_t SET_COM_PINS_CONFIG = 0xDA;  // usually 0x12 for 128x64

  // Timing & Power
  constexpr uint8_t SET_CLOCK_DIVIDE    = 0xD5; //Followed by value (usually 0x80)
  constexpr uint8_t SET_PRECHARGE       = 0xD9; //Followed by value (usually 0xF1)
  constexpr uint8_t SET_VCOM_DETECT     = 0xDB; //Followed by value (usually 0x40)

  constexpr uint8_t CHARGE_PUMP_CONTROL = 0x8D; //Followed by 0x14 (enable), 0x10 (off)
  constexpr uint8_t CHARGE_PUMP_ON      = 0x14;
  constexpr uint8_t CHARGE_PUMP_OFF     = 0x10;
}


class Oled_obj{
  public:
    bool on = false;
    bool inverted = false;
    bool initalized = false;
    bool charge_pump_on = false;
    const uint8_t height = 64;
    const uint8_t width = 128;
    const uint8_t pages = 8;
    const uint8_t OLED_ADR = 0x3c;

    uint8_t framebuffer[8][128];


    enum cur_mode{
      RAM,
      ALL_ON
    }mode;

    void init(){
      mode = RAM;

      //0xAE,         // Display OFF
      turn_off();
      //0xD5, 0x80,   // Set display clock
      set_clock_divide(0x80);
      //0xA8, 0x3F,   // Multiplex
      set_multiplex_ratio(0x3F);
      //0xD3, 0x00,   // Offset
      set_offset(0x00);
      //0x40,         // Start line = 0
      set_start_line(0);
      set_colum_adr(0, 127),
      set_page_adr(0, 7);

      //0x8D, 0x14,   // Charge pump ON
      turn_charge_pump_on();
      //0x20, 0x00,   // Memory mode: horizontal
      set_mem_mode_horizontal();
      //0xA1,         // Segment remap
      set_remap_right_to_left();
      //0xC8,         // COM output scan direction
      set_com_normal();
      //0xDA, 0x12,   // COM pins
      set_com_pins_config(0x12);
      //0x81, 0xCF,   // Contrast
      set_contrast(0xCF);
      //0xD9, 0xF1,   // Pre-charge
      set_precharge(0xF1);
      //0xDB, 0x40,   // VCOM detect
      //0xA4,         // Resume display
      resume_to_ram();
      //0xA6,         // Normal display
      set_not_inverted();

      clear_dis();

      set_start_line(0);

      //0xAF          // Display ON
      turn_on();

      initalized = true;
    }


    void send_comand(uint8_t cmd){
      Wire.beginTransmission(OLED_ADR);
      Wire.write(CMD::CMD);
      Wire.write(cmd);
      int tmp = Wire.endTransmission();
      Serial.println(tmp);
    }

    void send_comand(uint8_t cmd, uint8_t arg){
      Wire.beginTransmission(OLED_ADR);
      Wire.write(CMD::CMD);
      Wire.write(cmd);
      Wire.write(arg);
      int tmp = Wire.endTransmission();
      Serial.println(tmp);
    }

    void send_comand(uint8_t cmd, uint8_t arg, uint8_t arg2){
      Wire.beginTransmission(OLED_ADR);
      Wire.write(CMD::CMD);
      Wire.write(cmd);
      Wire.write(arg);
      Wire.write(arg2);
      int tmp = Wire.endTransmission();
      Serial.println(tmp);
    }

    

    void togle_display(){
      if(on){
        turn_off();
      }else{
        turn_on();
      }
      on= !on;
    }

    void turn_on(){
      send_comand(CMD::DISPLAY_ON);
      on = true;
    }
    void turn_off(){
      send_comand(CMD::DISPLAY_OFF);
      on = false;
    }

    void set_inverted(){
      send_comand(CMD::INVERT_DISPLAY);
    }
    void set_not_inverted(){
      send_comand(CMD::NORMAL_DISPLAY);
    }
    

    void all_pixels_on(){
      mode = ALL_ON;
      send_comand(CMD::ALL_PIXELS_ON);
    }
    void resume_to_ram(){
      mode = RAM;
      send_comand(CMD::RESUME_TO_RAM);
    }


    void set_contrast(uint8_t contrast){
      send_comand(CMD::Set_Contrast, contrast);
    }


    void set_mem_mode_horizontal(){
      send_comand(CMD::SET_MEMORY_MODE, CMD::MEM_MODE_HORIZONTAL);
    }
    void set_mem_mode_vertical(){
      send_comand(CMD::SET_MEMORY_MODE, CMD::MEM_MODE_VERTICAL);
    }
    void set_mem_mode_page(){
      send_comand(CMD::SET_MEMORY_MODE, CMD::MEM_MODE_PAGE);
    }


    void turn_charge_pump_on(){
      charge_pump_on = true;
      send_comand(CMD::CHARGE_PUMP_CONTROL,CMD::CHARGE_PUMP_ON);
    }
    void turn_charge_pump_off(){
      charge_pump_on = false;
      send_comand(CMD::CHARGE_PUMP_CONTROL,CMD::CHARGE_PUMP_OFF);
    }


    void set_clock_divide(uint8_t div = 0x80){
      send_comand(CMD::SET_CLOCK_DIVIDE, div);
    }

    void set_precharge(uint8_t val = 0xF1){
      send_comand(CMD::SET_PRECHARGE,val);
    }

    void set_vcom_detect(uint8_t val = 0x40){
      send_comand(CMD::SET_VCOM_DETECT, val);
    }


    void set_multiplex_ratio(uint8_t rat = 0x3f){
      send_comand(CMD::SET_MULTIPLEX_RATIO, rat);
    }

    void set_offset(uint8_t offset = 0x00){
      send_comand(CMD::SET_DISPLAY_OFFSET,offset);
    }

    void set_com_pins_config(uint8_t conf = 0x12){
      send_comand(CMD::SET_COM_PINS_CONFIG,conf);
    }


    void set_remap_left_to_right(){
      send_comand(CMD::SET_SEGMENT_REMAP_ltr);
    }
    void set_remap_right_to_left(){
      send_comand(CMD::SET_SEGMENT_REMAP_rtl);
    }

    
    void set_com_normal(){ //top to botom
      send_comand(CMD::SET_COM_OUT_NORMAL);
    }

    void set_com_fliped(){  //bottom to top
      send_comand(CMD::SET_COM_OUT_FLIPPED);
    }

    //constexpr uint8_t SET_COLUMN_ADDR     = 0x21; //Followed by start and end column
    //constexpr uint8_t SET_PAGE_ADDR       = 0x22; //Followed by start and end page

    void set_colum_adr(uint8_t start, uint8_t end){
      send_comand(CMD::SET_COLUMN_ADDR,start,end);
    }

    void set_page_adr(uint8_t start, uint8_t end){
      send_comand(CMD::SET_PAGE_ADDR, start, end);
    }

    
    void set_start_line(uint8_t line){
      if(line > 63){
        Serial.println("line to big only betwen 0-63");
      }else{
        send_comand(CMD::SET_START_LINE, line);
      }
    }




    void send_data(uint8_t dat){
      Wire.beginTransmission(OLED_ADR);
      Wire.write(CMD::DAT);
      Wire.write(dat);
      Wire.endTransmission();
      
    }

    void send_data(uint8_t dat[], uint16_t len){
      while(len>0){
        uint8_t chunk = min(31,len);
        Wire.beginTransmission(OLED_ADR);
        Wire.write(CMD::DAT);
        Wire.write(dat,chunk);
        Wire.endTransmission();

        len -= chunk;
        dat += chunk;
      }
    }

    void clear_dis(){
      for (int i = 0; i < 1024; i++) {
        send_data(0x00);
      }
    }
    void clear_framebuf(){
      for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 128; j++) {
          framebuffer[i][j] = 0x00;
      }
    }
   }

    void set_pixel(uint8_t x, uint8_t y,bool state){
      if(x>128 || y>64){
        Serial.println("pixels out of range");
      }else{
        
        uint8_t bit_pos = y % 8;
        uint8_t page = y/8;
        if (state) {
          framebuffer[page][x] |= (1 << bit_pos);  // Set the bit
        } else {
          framebuffer[page][x] &= ~(1 << bit_pos); // Clear the bit
        }

      }
    }
    void draw_line_x(double k, double d, uint8_t start=0, uint8_t end = 128){
      if(start < end){
        for (int i = start; i < end; i++) {
          uint8_t y = round((k*i)+d);
          if(!(y>64||y<0)){
            set_pixel(i,y,true);
          }
        }
      }else{
        for (int i = start; i > end; i--) {
          uint8_t y = round((k*i)+d);
          if(!(y>64||y<0)){
            set_pixel(i,y,true);
          }
          
        }
      }
    }
    void draw_line_y(double k, double d, uint8_t start=0, uint8_t end = 64){
      if(start < end){
        for (int i = start; i < end; i++) {
          uint8_t x = round((k*i)+d);
          if(x>128 || x<0){
            break;
          }
          set_pixel(x,i,true);
        }
      }else{
        for (int i = start; i > end; i--) {
        uint8_t x = round((k*i)+d);
        if(x>128 || x<0){
          break;
        }
        set_pixel(x,i,true);
        }
      } 
    }

    void draw_line_ptp(uint8_t x1 ,uint8_t y1,    uint8_t x2 ,uint8_t y2){ //two bytes x y
      if(x1 > 128 || y1 > 64){
        Serial.println("p1 is out of range");
      }else if(x2 > 128 || y2 > 64){
        Serial.println("p2 is out of range");
      }else{
        int16_t dx = x2-x1;
        if(dx==0){
          draw_line_y(0,x1,y1,y2);
          return;
        }
        double k = (double)(y2-y1) / (double) (dx);
        double d = y1 - (k*x1);
        draw_line_x(k,d,x1,x2);
      }
    }

  void draw_rectangle(uint8_t x,uint8_t y, uint8_t a, uint8_t b){ //only the outline
    draw_line_x(0, y, x, x+a);
    draw_line_x(0, y+b, x, x+a);

    draw_line_y(0, x, y, y+b);
    draw_line_y(0, x+a, y, y+b);

  }
  void draw_rectangle_full(uint8_t x, uint8_t y, uint8_t a, uint8_t b, const bool state = true){ //a width, b heigt
    for (uint8_t i = y; i < y+b; i++){
      for (uint8_t j = x; j < x+a; j++){
        if(x>128 || y>64){
          Serial.println("pixels out of range");
        }else{
          set_pixel(j,i,state);  
        }
      }
    }
  }

  void draw_triangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t x3, uint8_t y3){
    draw_line_ptp(x1, y1, x2, y2);
    draw_line_ptp(x2,y2,  x3,y3);
    draw_line_ptp(x3,y3,  x1,y1);
  }

  void update(){
    send_data((uint8_t*)framebuffer,1024);
  }

  void wbuff_progmem(const uint8_t* arr, const uint8_t x, const uint8_t y, const uint8_t w, const uint8_t h){ //starts from botom left toward right high corner
    
    for (uint8_t i = 0; i < h; i++){
      for (uint8_t j = 0; j < w; j++){

        uint8_t pixel_y = y + i;
        uint8_t pixel_x = x + j;
        
        if (pixel_x < width && pixel_y < height) {
          uint8_t page = i/8;      //in arr
          uint8_t pos  = i%8;     //which bit
          set_pixel(pixel_x, pixel_y, ((pgm_read_byte(&arr[page * w + j])) >> pos) & 0x01);
        }
      }
    }
  }
    
};

Oled_obj oled;

void wakeUp() {
  // Just wake from sleep — keep ISR short and avoid Serial here
}

void goToSleep() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  EIFR = bit (INTF0);  // Clear external interrupt flag

  attachInterrupt(digitalPinToInterrupt(2), wakeUp, RISING);

  sei();  // Enable global interrupts
  sleep_cpu();  // Enter sleep

  //interupt hapens here
  
  sleep_disable();
  detachInterrupt(digitalPinToInterrupt(2));
}

void setup() {
  Wire.begin();
  Serial.begin(9600);

  pinMode(2, INPUT);
  pinMode(8, OUTPUT);

  
  FastLED.addLeds<WS2811, 8, GRB>(redLeds, NUM_LEDS);
  FastLED.addLeds<WS2811, 9, GRB>(greenLeds, NUM_LEDS);
  FastLED.addLeds<WS2811, 10, GRB>(pinkLeds, NUM_LEDS);
  

  power_adc_disable();
  power_spi_disable();
  power_timer1_disable();
  power_timer2_disable();
  // Leave Timer0 enabled if you use delay()

  oled.init();
  
  oled.draw_rectangle_full(40,30,15,30);
  oled.draw_rectangle_full(70,30,15,30);
  
  oled.draw_rectangle_full(40,10,45,10);
  
  
  
  
  oled.update();

  Serial.println("Going to sleep...");
  delay(100);
  goToSleep();  // First sleep
}

void loop() {
  Serial.println("Woke up!");
  delay(100);
  
  oled.draw_rectangle_full(42,32,11,26,false);
  oled.draw_rectangle_full(72,32,11,26,false);
  
 
  oled.draw_rectangle_full(30,20,10,5,true);
  oled.draw_rectangle_full(85,20,10,5,true);
  
  oled.update();
  delay(2000);
  oled.draw_rectangle_full(40,32,13,28);
  oled.draw_rectangle_full(72,32,13,28);
  
  oled.draw_rectangle_full(30,20,10,5,false);
  oled.draw_rectangle_full(85,20,10,5,false);
  oled.update();

  
  for(int i=0;i<NUM_LEDS;i++){
    // Turn our current led on to white, then show the leds
      redLeds[i] = CRGB::Green;
      
  }

  FastLED.show();

  delay(3000);
  
  for(int a=0;a<NUM_LEDS;a++){
    // Turn our current led on to white, then show the leds
      greenLeds[a] = CRGB(150, 0, 0);
      
      
      redLeds[a] = CRGB::Black;
  }
  FastLED.show();

  delay(3000);
  
  for(int e=0;e<NUM_LEDS;e++){
    // Turn our current led on to white, then show the led-s
      pinkLeds[e] = CRGB(128, 0, 128);
      
      
      greenLeds[e] = CRGB::Black;
  }
  FastLED.show();
  delay(3000);
  FastLED.clear ();
  FastLED.show();
  
  
  delay(100);
  Serial.println("back to sleep!");
  goToSleep();  // Sleep again
}
