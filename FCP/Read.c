#include <Wire.h>

// Declare function pointer array with 1 function (beep)
void (*function_array[1])(char[]) = {beep};
uint16 funcs = sizeof(function_array)/sizeof(function_array[0]);

void setup() {
  Wire.begin(0x01);  // Set up I2C address
  Wire.onReceive(receiveEvent);  // Set the receive event handler
  pinMode(4, OUTPUT);  // Set pin 4 as an output for the beep function
}

void loop() {
  // Nothing needed in loop for now
}

void receiveEvent(int bytes) {
  char out[30];  // Buffer to store received data
  char low = Wire.read();  // Read low byte
  char high = Wire.read();  // Read high byte
  uint16_t func = ((uint16_t)high << 8) | low;  // Combine to get function ID

  // Ensure there are enough bytes available before reading into out
  for (char i = 0; i < bytes - 2; i++) {
    if (Wire.available()) {
      out[i] = Wire.read();  // Read remaining bytes into out array
    }
  }

  // Ensure func is within bounds (0 in this case)
  if (func < funcs) {
    function_array[func](out);  // Call the function based on func index
  }
}

void beep(char onof[]) {
  // Assuming the first byte indicates ON or OFF (HIGH or LOW)
  if (onof[0] == '1') {
    digitalWrite(4, HIGH);  // Turn on the beep (pin 4 HIGH)
  } else {
    digitalWrite(4, LOW);  // Turn off the beep (pin 4 LOW)
  }
}
