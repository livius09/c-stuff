

#include <Wire.h>

// EEPROM I2C address with A0 = 1 (A1 and A2 = 0, 0)
#define EEPROM_ADDRESS 0x51  // Base address is 0x50, A0=1 adds 1

// EEPROM memory address to write to (choose an address within the 64KB range)
#define EEPROM_MEM_ADDR 0x0000


void setup() {
  pinMode(2,OUTPUT);
  Serial.begin(9600);
  Wire.begin();

  uint16_t numberToWrite = 12344;  // The number to write to EEPROM
  bool wp = 0;
  int ar[]={1,2,3,4,5};
  int len=sizeof(ar)/sizeof(ar[0])

  if (wp==1){
    digitalWrite(2,HIGH);
  }
  else{
    digitalWrite(2,LOW);
  } 
  // Write the number to EEPROM
  writeNumberToEEPROM(EEPROM_MEM_ADDR, numberToWrite);
  Serial.print("Number written to EEPROM: ");
  Serial.println(numberToWrite);

  // Read the number back from EEPROM
  uint16_t readNumber = readNumberFromEEPROM(EEPROM_MEM_ADDR);
  Serial.print("Number read from EEPROM: ");
  Serial.println(readNumber);
}

void loop() {
  // Empty loop
}

void writeRecursively(uint16_t memaddr,uint16_t* arr,int len){
  if (len==0){
    return;
  }
  int pageofset = len % 128;
  int bytestowrite = min(128-pageofset,len*2)


  Wire.beginTransmission(EEPROM_ADDRESS);
  // Send the memory address (two bytes: high byte and low byte)
  Wire.write((memAddr >> 8) & 0xFF);  // High byte
  Wire.write(memAddr & 0xFF);         // low byte
  int len=sizeof(arr)/sizeof(arr[0])
  for (i=0,i<len,i++){
    // Send the number (two bytes: high byte and low byte)
    Wire.write((arr[i] >> 8) & 0xFF);   // High byte
    Wire.write(arr[i] & 0xFF);          // Low byte
  }
  Wire.endTransmission();
  delay(10);
}

void writeNumberToEEPROM(uint16_t memAddr, uint16_t number) {
  Wire.beginTransmission(EEPROM_ADDRESS);

  // Send the memory address (two bytes: high byte and low byte)
  Wire.write((memAddr >> 8) & 0xFF);  // High byte
  Wire.write(memAddr & 0xFF);         // Low byte

  // Send the number (two bytes: high byte and low byte)
  Wire.write((number >> 8) & 0xFF);   // High byte
  Wire.write(number & 0xFF);          // Low byte

  Wire.endTransmission();
  delay(10);  // Wait for EEPROM to complete the write process
}

uint16_t readNumberFromEEPROM(uint16_t memAddr) {
  Wire.beginTransmission(EEPROM_ADDRESS);

  // Send the memory address (two bytes: high byte and low byte)
  Wire.write((memAddr >> 8) & 0xFF);  // High byte
  Wire.write(memAddr & 0xFF);         // Low byte
  Wire.endTransmission();

  // Request 2 bytes of data from EEPROM
  Wire.requestFrom(EEPROM_ADDRESS, 2);

  // Read the 2 bytes of data and combine them into the original number
  uint16_t number = 0;
  if (Wire.available() >= 2) {
    number = (Wire.read() << 8);  // Read high byte
    number |= Wire.read();        // Read low byte
  }

  return number;
}

void readSequential(uint16_t startAddr, uint16_t* out, int len) {
  Wire.beginTransmission(EEPROM_ADDRESS);

  // Send the start memory address (two bytes: high byte and low byte)
  Wire.write((startAddr >> 8) & 0xFF);  // High byte
  Wire.write(startAddr & 0xFF);         // Low byte

  Wire.endTransmission();

  // Request the number of bytes to read (2 bytes per uint16_t)
  Wire.requestFrom(EEPROM_ADDRESS, len * 2);

  for (int i = 0; i < len; i++) {
    if (Wire.available() >= 2) {  // Ensure 2 bytes are available
      uint16_t number = (Wire.read() << 8);  // Read high byte
      number |= Wire.read();                 // Read low byte
      out[i] = number;
    }
  }
}

