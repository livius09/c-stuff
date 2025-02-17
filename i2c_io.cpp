#include <Wire.h>

#define io_adr 0x20
#define PORTA 0
#define PORTB 1
#define INPUT 1
#define OUTPUT 0


void setup() {
  Serial.begin(9600);
  Wire.begin();

  delay(1000);
  config_pin(0,OUTPUT);
}

void loop() {
  update_pin(0,true);
  delay(1000);
  update_pin(0, false);
  delay(1000);
  

}


void write_port(boolean port,byte dat){

  uint8_t port_adr =(port)?0x15:0x14; //port a is false port b is true

  Wire.beginTransmission(io_adr);
  Wire.write(port_adr);
  Wire.write(dat);
  Wire.endTransmission();

}



void config_port(boolean port,byte conf){
  uint8_t port_adr = (port)?0x01:0x00; //port a is false port b is true

  Wire.beginTransmission(io_adr);
  Wire.write(port_adr);
  Wire.write(conf);
  Wire.endTransmission();
}



byte read_port(bool port){
  uint8_t port_adr = (port)?0x13:0x12;

  Wire.beginTransmission(io_adr);
  Wire.write(port_adr);
  Wire.endTransmission();

  Wire.requestFrom(io_adr, 1);
  return Wire.read();
}



bool read_pin(uint8_t pin) {
  if (pin > 15) {  
    Serial.println("There are only 16 pins");
    return;
  }

  uint8_t port = (pin < 8) ? true : false;
  uint8_t bit_pos = pin % 8;  // Pins 0-7 for A, 8-15 for B

  uint8_t read = read_port(port);

  return (read >> bit_pos) & 1;
}



void update_pin(uint8_t pin,bool val){
  if (pin > 15) {  
    Serial.println("There are only 16 pins");
    return;
  }

  uint8_t port = (pin < 8) ? true : false;
  uint8_t bit_pos = pin % 8; 
  
  uint8_t cur = read_port(port);

  if (val){
    cur |= (1 << bit_pos);  // Set bit
  }else{
    cur &= ~(1 << bit_pos); // Clear bit
  }
  write_port(port,cur);

}



uint8_t read_port_config(bool port){
  uint8_t port_adr = (port)?0x01:0x00;

  Wire.beginTransmission(io_adr);
  Wire.write(port_adr);
  Wire.endTransmission();

  Wire.requestFrom(io_adr, 1);
  return Wire.read();
}



void config_pin(uint8_t pin,bool conf){
  if (pin > 15) {  
    Serial.println("There are only 16 pins");
    return;
  }
  uint8_t port = (pin < 8) ? true : false; //port a is false port b is true
  uint8_t bit_pos = pin % 8;

  uint8_t cur = read_port_config(port);

  if (conf){
    cur |= (1 << bit_pos);  // Set bit
  }else{
    cur &= ~(1 << bit_pos); // Clear bit
  }
  config_port(port,cur);  
}











