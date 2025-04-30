#include <Wire.h>
void setup() {
  Wire.begin();
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:

}

class color_obj{
  const uint8_t adr = 0x29;
  void init(){
    send_comand(0x00,0x01); //power on
    delay(3);
    send_comand(0x00,oxo3); //enable ADC

    send_comand(0x01, 0xD5); // intergration

    send_comand(0x0F, 0x00); //gain 1x

    delay(750);
    
    
  }

  void read_color(uint16_t *r, uint16_t *g, uint16_t *b ){
    
  }

  void send_comand(uint8_t cmd){
      Wire.beginTransmission(adr);
      Wire.write(0x80|reg);
      Wire.endTransmission();
   }
  void send_comand(uint8_t reg, uint8_t val){
    Wire.beginTransmission(adr);
    Wire.write(0x80|reg);
    Wire.write(cmd);
    Wire.endTransmission();
  }
}
