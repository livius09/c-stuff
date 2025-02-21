#include <SPI.h>
#define cs 10

uint8_t lig[255];

void setup() {
    SPI.begin();                  // Initialize SPI
    Serial.begin(9600);           //initialize Serial
    SPI.setClockDivider(SPI_CLOCK_DIV2); // Set clock speed
    SPI.setDataMode(SPI_MODE3);   // Set SPI mode (0,1,2,3)
    SPI.setBitOrder(MSBFIRST);    // Set bit order
    pinMode(cs, OUTPUT);          // cs must be set as an OUTPUT in master mode
    digitalWrite(cs, HIGH);
    for (int i = 0; i<255; i++){
      lig[i]=i;
    }
}

void loop() {
  page_write(0x00,lig,255);

  uint8_t bal[255];
  sequential_read_eeprom(0x00, bal, 255);
  Serial.println("arr read:");
  for (int i = 0; i<255; i++){
    Serial.print(bal[i]);
    Serial.print(" ");
  }
  Serial.print("\n");
  delay(7000);
}

void write_eeprom(uint8_t adr,uint8_t dat){
  digitalWrite(cs, LOW);
  SPI.transfer(0x06);
  digitalWrite(cs, HIGH);

  digitalWrite(cs, LOW);
  SPI.transfer(0x02);
  SPI.transfer(adr);
  SPI.transfer(dat);
  digitalWrite(cs, HIGH);
  uint8_t status;
  do {
    digitalWrite(cs, LOW);
    SPI.transfer(0x06);
    status = SPI.transfer(0x00);
    digitalWrite(cs, HIGH);
  }while (status & 0x01);
  delay(5);
}

uint8_t read_eeprom(uint8_t adr){

  digitalWrite(cs, LOW);
  SPI.transfer(0x03);
  SPI.transfer(adr);
  uint8_t dat = SPI.transfer(0x00);
  digitalWrite(cs, HIGH);
  return dat;
}

void sequential_read_eeprom(uint8_t adr,uint8_t arr[],uint8_t len){
  digitalWrite(cs, LOW);
  SPI.transfer(0x03); 
  SPI.transfer(adr);
  for (int i = 0; i<len; i++){
    arr[i]=SPI.transfer(0x00);
    delay(1);
  }

  digitalWrite(cs, HIGH);
}

void page_write(uint8_t adr,uint8_t arr[],uint8_t len){
  uint8_t *ptr = arr;
  while (len > 0) {
      uint8_t byte_to_write = min(16 - (adr % 16), len);

      digitalWrite(cs, LOW);
      SPI.transfer(0x06); // Write enable
      digitalWrite(cs, HIGH);

      digitalWrite(cs, LOW);
      SPI.transfer(0x02); // Page write command
      SPI.transfer(adr);
      for (uint8_t i = 0; i < byte_to_write; i++) {
          SPI.transfer(ptr[i]);  // Use pointer indexing
      }
      digitalWrite(cs, HIGH);

      // Wait for the write to complete
      uint8_t status;
      do {
          digitalWrite(cs, LOW);
          SPI.transfer(0x05); // Read status register
          status = SPI.transfer(0x00);
          digitalWrite(cs, HIGH);
      } while (status & 0x01); // Wait for WIP to clear

      delay(5);  // Might need longer delay for stability

      len -= byte_to_write;
      adr += byte_to_write;
      ptr += byte_to_write;  // Move pointer forward
  }
}
