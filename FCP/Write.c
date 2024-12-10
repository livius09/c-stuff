#include <Wire.h>  // Correct header

uint8_t args1[] = {0x01};  // Example byte array
uint8_t args0[] = {0x00};

void setup() {
  Wire.begin();  // Initialize I2C communication
  Serial.begin(9600);
}

void loop() {
  delay(3000);
  uint8_t arr[30];
  request_FCP(0x01, 0x01, args0, 1, arr);
  Serial.print("output: ");
  Serial.println(arr[0]);
}

int instuc_FCP(uint8_t address, uint16_t function, const uint8_t* args, uint8_t length) {
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

int request_FCP(uint8_t address, uint16_t function, const uint8_t* args, uint8_t length, uint8_t* out) {
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
    Serial.print("Arg ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(args[i]);
  }

  // Check for transmission errors
  if (Wire.endTransmission() != 0) {
    Serial.println("Transmission Error!");
    return -2;
  }

  // Request data from the slave
  Wire.requestFrom(address, 30);
  
  // Read the requested data into the output buffer
  for (uint8_t k = 0; k < 30 && Wire.available(); k++) {
    out[k] = Wire.read();
    Serial.print("Received ");
    Serial.print(k);
    Serial.print(": ");
    Serial.println(out[k]);
}

  return 0;  // Success
}
