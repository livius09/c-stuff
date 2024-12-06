#include <Wire.h>  // Correct header

uint8_t args1[] = {0x01};  // Example byte array
uint8_t args0[] = {0x00};

void setup() {
  Wire.begin();  // Initialize I2C communication
  Serial.begin(9600);
}

void loop() {
  
  send_FCP(0x01, 0x00, args1, sizeof(args1));
  delay(3000);
  send_FCP(0x01, 0x00, args0, sizeof(args0));
  delay(3000);

}

int send_FCP(uint8_t address, uint16_t function, const uint8_t* args, uint8_t length) {
  if (length >= 29) {
    return -1;  // Error: args too long
  }

  Wire.beginTransmission(address);

  // Write function ID as two bytes
  Wire.write(function & 0xFF);        // Low byte
  Wire.write((function >> 8) & 0xFF); // High byte
  Serial.print("Function: ");
  Serial.println(function);

  // Write each byte from the args array
  for (uint8_t i = 0; i < length; i++) {
    Wire.write(args[i]);
    Serial.print(i);
    Serial.print(": ");
    Serial.println(args[i]);
  }

  Wire.endTransmission();
  return 0;  // Success
}
