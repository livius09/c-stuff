#include <Wire.h>  // Correct header

void setup() {
  Wire.begin();  // Initialize I2C communication
}

void loop() {
  uint8_t args[] = {0x01};  // Example byte array
  send_FCP(0x01, 0x01, args, sizeof(args));
}

int send_FCP(uint8_t address, uint16_t function, const uint8_t* args, uint8_t length) {
  if (length >= 29) {
    return -1;  // Error: args too long
  }

  Wire.beginTransmission(address);

  // Write function ID as two bytes
  Wire.write(function & 0xFF);        // Low byte
  Wire.write((function >> 8) & 0xFF); // High byte

  // Write each byte from the args array
  for (uint8_t i = 0; i < length; i++) {
    Wire.write(args[i]);
  }

  Wire.endTransmission();
  return 0;  // Success
}
