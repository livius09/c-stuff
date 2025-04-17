#include <Wire.h>
const uint8_t OLED_ADR = 0x3c;



void setup() {
  Wire.begin();
  

}

void loop() {
  // put your main code here, to run repeatedly:

}

class oled_comand{
  public:
    
  
};


class Oled{
  public:
    bool on = false;
    bool inverted = false;
    oled_comand comnad;
    void init(){

    }
    void send_comand(uint8_t cmd){
      Wire.beginTransmission(OLED_ADR);
      Wire.write(0x00);
      Wire.write(cmd);
      Wire.endTransmission();
    }

    void togle_display{
      on= (!on);
      if(on){
        turn_on();
      }else{
        turn_off();
      }
    }

    void turn_on(){
      send_comand(0xAF);
    }
    void turn_off(){
      send_comand(0XAE);
    }

    void invert(){
      inverted = (!inverted);
      send_comand(0xA7);
    }

    void set_inverted(){
      if(!inverted){
        invert();
      }
    }

    void set_not_inverted(){
      if(inverted){
        invert();
      }
    }
    
    
};
