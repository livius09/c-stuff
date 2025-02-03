

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

void write_page(uint8_t eeprom_addr, uint16_t memAddr, int8_t* arr, int len) {
  while (len > 0) {
    int bytesToWrite = min(128 - (memAddr % 128), len);  // Don't cross page boundary

    while (bytesToWrite > 0) {
      int chunkSize = min(30, bytesToWrite);  // Max chunk size due to Wire buffer limit

      Wire.beginTransmission(eeprom_addr);
      Wire.write((memAddr >> 8) & 0xFF);
      Wire.write(memAddr & 0xFF);

      for (int i = 0; i < chunkSize; i++) {
        Wire.write(arr[i]);
      }

      Wire.endTransmission();
      delay(10);  // Allow EEPROM write cycle

      memAddr += chunkSize;
      arr += chunkSize;
      len -= chunkSize;
      bytesToWrite -= chunkSize;
    }
  }
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

void readSequential(uint8_t eeprom_addr,uint16_t startAddr, int8_t* out, int len) {
  Wire.beginTransmission(eeprom_addr);

  // Send the start memory address (two bytes: high byte and low byte)
  Wire.write((startAddr >> 8) & 0xFF);  // High byte
  Wire.write(startAddr & 0xFF);         // Low byte

  Wire.endTransmission();

  delay(5);
  // Request the number of bytes to read (1 bytes per uint8_t)

  while (len > 0) {
    int chunkSize=min(32,len);

    Wire.requestFrom(eeprom_addr, chunkSize);

    for (int i = 0; i < chunkSize; i++) {
      if (Wire.available()>=1) {  // Ensure byte are available
        *out = Wire.read();  // Read byte
        out++; //move to next byte in out buffer
      }
    }
    len-=chunkSize;
  }
  
}
