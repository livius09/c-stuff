#include <Wire.h>

class color_obj{
  public:
    const uint8_t adr = 0x29;
    void init(){
      send_comand(0x00,0x01); //power on
      delay(3);
      send_comand(0x00,0x03); //enable ADC

      send_comand(0x01, 0xD5); // intergration

      send_comand(0x0F, 0x00); //gain 1x

      delay(750);
    }

    void read_color(uint16_t *r, uint16_t *g, uint16_t *b ){
      auto_increment(0x16);

      Wire.requestFrom(adr, 6); // read 8 bytes

      uint8_t r_low = Wire.read();
      uint8_t r_high = Wire.read();
      uint8_t g_low = Wire.read();
      uint8_t g_high = Wire.read();
      uint8_t b_low = Wire.read();
      uint8_t b_high = Wire.read();

      *r = (r_high << 8) | r_low;
      *g = (g_high << 8) | g_low;
      *b = (b_high << 8) | b_low;
    }
      
    void read_color_normalized(double *r, double *g, double *b ){
      uint16_t r_raw,g_raw,b_raw;
      read_color(&r_raw ,&g_raw ,&b_raw);
      double c = (double)read_clear();
      if(c>0){
      *r = (double) r_raw / c;
      *g = (double) g_raw / c;
      *b = (double) b_raw / c;
      }
      

    }

    uint16_t read_red(){
      auto_increment(0x16);

      Wire.requestFrom(adr, 2);
      uint8_t r_low = Wire.read();
      uint8_t r_high = Wire.read(); 
      return ((r_high << 8) | r_low);
    }

    uint16_t read_green(){
      auto_increment(0x18);

      Wire.requestFrom(adr, 2); 
      uint8_t g_low = Wire.read();
      uint8_t g_high = Wire.read(); 
      return ((g_high << 8) | g_low);
    }

    uint16_t read_blue(){
      auto_increment(0x1A);

      Wire.requestFrom(adr, 2); // read 8 bytes
      uint8_t b_low = Wire.read();
      uint8_t b_high = Wire.read(); 
      return ((b_high << 8) | b_low);
    }
    uint16_t read_clear(){
      auto_increment(0x14);

      Wire.requestFrom(adr, 2);
      uint8_t c_low = Wire.read();
      uint8_t c_high = Wire.read(); 
      return ((c_high << 8) | c_low);
    }
    

    void auto_increment(uint8_t reg){
      Wire.beginTransmission(adr);
      Wire.write(0xA0 | reg); // 0x80 + 0x20 + reg
      Wire.endTransmission();
    }

    void send_comand(uint8_t reg){
      Wire.beginTransmission(adr);
      Wire.write(0x80|reg);
      Wire.endTransmission();
    }
    void send_comand(uint8_t reg, uint8_t cmd){
      Wire.beginTransmission(adr);
      Wire.write(0x80|reg);
      Wire.write(cmd);
      Wire.endTransmission();
    }
};

color_obj sense;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  
  sense.init();
 

}

void loop() {
  double r;
  double g;
  double b;

  sense.read_color_normalized(&r ,&g ,&b );

  Serial.print("Red: ");
  Serial.print(r);
  Serial.print("  Green: ");
  Serial.print(g);
  Serial.print("  Blue: ");
  Serial.println(b);
  Serial.println(" ");


  delay(1000);



}
