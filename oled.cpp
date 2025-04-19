#include <Wire.h>
const uint8_t OLED_ADR = 0x3c;

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
  constexpr uint8_t SET_SEGMENT_REMAP_0 = 0xA0;
  constexpr uint8_t SET_SEGMENT_REMAP_1 = 0xA1; 
  constexpr uint8_t Set_Contrast	      = 0x81;   //Range 0x00–0xF

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

      //0x8D, 0x14,   // Charge pump ON
      turn_charge_pump_on();
      //0x20, 0x00,   // Memory mode: horizontal
      set_mem_mode_horizontal();
      //0xA1,         // Segment remap
      //0xC8,         // COM output scan direction
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

    void send_data(uint8_t dat){
      Wire.beginTransmission(OLED_ADR);
      Wire.write(CMD::DAT);
      Wire.write(dat);
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

    
    void toggle_inverted(){
      if(inverted){
        set_not_inverted();
      }else{
        send_comand(CMD::INVERT_DISPLAY);
      }
      inverted = !inverted; 
    }
    void set_inverted(){
      if(!inverted){
        toggle_inverted();
      }
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
    void toogle_charge_pump(){
      if(charge_pump_on){
        turn_charge_pump_off();
      }else{
        turn_charge_pump_on();
      }
      charge_pump_on = !charge_pump_on;
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

    

    void set_start_line(uint8_t line){
      if(line > 63){
        Serial.println("line to big only betwen 0-63");
      }else{
        send_comand(CMD::SET_START_LINE, line);
      }
    }



    
    
};

Oled_obj oled;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  
  
  oled.init();
  oled.send_data(0xFF);
}

void loop() {
  oled.toggle_inverted();
  oled.send_data(0xFF);
  delay(2000);
}

