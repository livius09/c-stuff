#include <Wire.h>


#define INPUT 1
#define OUTPUT 0

#define IODIRA 0x00  // I/O direction A (1=input, 0=output)
#define IODIRB 0x01  // I/O direction B
#define GPIOA  0x12  // Port A register
#define GPIOB  0x13  // Port B register
#define OLATA  0x14  // Output latch A
#define OLATB  0x15  // Output latch B



uint8_t num_to_seven_segment_right(uint8_t num){
  //                  0           1         2         3         4       5
 // For common anode (active LOW)
  const unsigned char seven_seg_anode[] = {
    ~0x7E, // 0 01111110
    ~0x12, // 1 00010010
    ~0xBC, // 2 10111100
    ~0xB6, // 3 10110110
    ~0xD2, // 4 11010010
    ~0xE6, // 5 11100110
    ~0xEE, // 6 11101110
    ~0x32, // 7 00110010
    ~0xFE, // 8 11111110
    ~0xF6  // 9 11110110
  };

  //1 1-dot
  //2 2-low-right
  //3 4-botom Midle
  //4 8-left-bottom
  //5 16-right-top 
  //6 32-top-midle
  //7 64-left-top
  //8 128-midle-midle

  if (num>9) {
    return seven_seg_anode[0];
  }
  return seven_seg_anode[num];
}


uint8_t num_to_seven_segment_left(uint8_t num){
  //                  0           1         2         3         4       5
 // For common anode (active LOW)
  const unsigned char seven_seg_anode[] = {
    ~0xE7, // 0 11100111
    ~0x21, // 1 00100001
    ~0xCB, // 2 11001011
    ~0x6B, // 3 01101011
    ~0x2D, // 4 00101101
    ~0x6E, // 5 01101110
    ~0xEE, // 6 11101110
    ~0x23, // 7 00100011
    ~0xEF, // 8 11101111
    ~0x6F  // 9 01101111
  };

  //1 1-top-right
  //2 2-top-mid
  //3 4-top-left
  //4 8-mid-mid
  //5 16-dot
  //6 32-low-right
  //7 64-low-mid
  //8 128-low-left

  if (num>9) {
    return seven_seg_anode[0];
  }
  return seven_seg_anode[num];
}




class i2cIO{
  public:           
    uint8_t io_adr;

    i2cIO(uint8_t in_io_adr) {     
      io_adr = in_io_adr;

    }

    void write_bank(boolean port,byte dat){

      uint8_t port_adr =(port)?0x15:0x14; //port a is false port b is true

      Wire.beginTransmission(io_adr);
      Wire.write(port_adr);
      Wire.write(dat);
      Wire.endTransmission();

    }



    void config_bank(boolean port,byte conf){
      uint8_t port_adr = (port)?0x01:0x00; //port a is false port b is true

      Wire.beginTransmission(io_adr);
      Wire.write(port_adr);
      Wire.write(conf);
      Wire.endTransmission();
    }



    byte read_bank(bool port){
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

      uint8_t read = read_bank(port);

      return (read >> bit_pos) & 1;
    }



    void update_pin(uint8_t pin,bool val){
      if (pin > 15) {  
        Serial.println("There are only 16 pins");
        return;
      }

      uint8_t port = (pin < 8) ? true : false;
      uint8_t bit_pos = pin % 8; 
      
      uint8_t cur = read_bank(port);

      if (val){
        cur |= (1 << bit_pos);  // Set bit
      }else{
        cur &= ~(1 << bit_pos); // Clear bit
      }
      write_bank(port,cur);

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
      config_bank(port,cur);  
    }

    void write_left_num(uint8_t num){
      write_bank(true, num_to_seven_segment_left(num));
    }

    void write_Right_num(uint8_t num){
      write_bank(false, num_to_seven_segment_right(num));
    }


};

i2cIO ioLeft(0x20);
i2cIO ioRight(0x21);
uint8_t counter = 0;


char incomingByte = 0; // for incoming serial data
long start;
long current;
bool counting = false;

long get_dif(){
  return current-start;
}

void update(bool print_last){
  
  long dif = get_dif();
  double seconds =  dif/ 1000.0;
  // Round to 2 decimal places
  double seconds2 = round(seconds * 100.0) / 100.0;

  int whole = (int)seconds2;              // 12
  int frac = (int)round((seconds2 - whole) * 100); // 35

  int tens = whole / 10;      // 1
  int ones = whole % 10;      // 2

  int tenths = frac / 10;     // 3
  int hundredths = frac % 10; // 5

  ioLeft.write_left_num(tens);
  
  byte val = num_to_seven_segment_right(ones);

  bitWrite(val, 0, 0);
  
  ioLeft.write_bank(false, val);

  ioRight.write_left_num(tenths);

  if(print_last){
    ioRight.write_Right_num(hundredths);
  }
}

void reset(){
  start=millis();;
  current=millis();;
  Serial.print("herer");
}


void setup() {
  Serial.begin(9600);
  Wire.begin();

  delay(1000);
  ioLeft.config_bank(false, OUTPUT);
  ioRight.config_bank(false, OUTPUT);
  ioLeft.config_bank(true, OUTPUT);
  ioRight.config_bank(true, OUTPUT);
  update(true);
}


void loop() {

  current = millis();

  if(counting){
    update(false);

    if (get_dif()>40000){
      reset();
    }
  }

  
  // send data only when you receive data:
  if (Serial.available() > 0){
    // read the incoming byte:
    incomingByte=0;
    char incomingByte = Serial.read();

    if(incomingByte!='\n'){
      incomingByte-=48;

      // say what you got:
      Serial.print("I received:");
      Serial.println(incomingByte,DEC);

      if(incomingByte==1){
        start=millis();
        counting=true;
        
      }else if (incomingByte==2) {
        counting=false;
        update(true);
      }
      
    }
  }


  

}


//test
/*
// send data only when you receive data:
  if (Serial.available() > 0){
    // read the incoming byte:
    incomingByte=0;
    char incomingByte = Serial.read();

    if(incomingByte!='\n'){
      incomingByte-=48;
     

      // say what you got:
      Serial.print("I received:");
      Serial.println(incomingByte,DEC);

      ioLeft.write_Right_num(incomingByte);
      ioLeft.write_left_num(incomingByte);
      

      //ioRight.write_bank(true , num_to_seven_segment(incomingByte));
      //ioRight.write_bank(false , num_to_seven_segment(incomingByte));
      
    }
  }*/
