#include <Wire.h>

// EEPROM I2C address with A0 = 1 (A1 and A2 = 0, 0)
#define EEPROM_ADDRESS 0x51  // Base address is 0x50, A0=1 adds 1

// EEPROM memory address to write to (choose an address within the 64KB range)
#define EEPROM_MEM_ADDR 0x0000

uint16_t nextfree = 0x0000;

void setup() {
  pinMode(2, OUTPUT);
  Serial.begin(9600);
  Wire.begin();
  

  uint16_t numberToWrite = 12344;  // The number to write to EEPROM
  bool wp = 0;
  uint16_t ar[] = {1, 2, 3, 4, 5};  // Array of numbers to write
  int len = sizeof(ar) / sizeof(ar[0]);

  // Set write protect pin
  digitalWrite(2, wp ? HIGH : LOW);

  for(int i =0; i<len; i++){
    push(ar[i]);
    Serial.print("pushed: ");
    Serial.println(ar[i]);
  }
  for(int j =0; j<len; j++){
    Serial.print("poped: ");
    Serial.println(pop());
  }
  
}

void loop() {
  // Empty loop
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

void push(uint16_t in){
  writeNumberToEEPROM( nextfree , in );
  nextfree++;
  nextfree++;

}

uint16_t pop(){
  --nextfree;
  --nextfree;
  return readNumberFromEEPROM(nextfree);
}



