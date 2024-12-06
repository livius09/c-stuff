#include <Wire.h>

// Forward declaration of the beep function
void beep(uint8_t[]);

// Declare function pointer array with 1 function (beep)
void (*function_array[])(uint8_t[]) = {beep};

void setup() {
  Serial.begin(9600);

  Wire.begin(0x01);  // Set up I2C address
  Wire.onReceive(receiveEvent);  // Set the receive event handler
  pinMode(4, OUTPUT);  // Set pin 4 as an output for the beep function
}

void loop() {
  // Nothing needed in loop for now
}

void receiveEvent(int bytes) {
  uint8_t out[30];  // Buffer to store received data
  uint8_t low = Wire.read();  // Read low byte
  uint8_t high = Wire.read();  // Read high byte
  uint16_t func = ((uint16_t)high << 8) | low;  // Combine to get function ID
  Serial.print("function: ");
  Serial.println(func);

  // Ensure there are enough bytes available before reading into out
  for (uint8_t i = 0; i < bytes - 2; i++) {
    if (Wire.available()) {
      out[i] = Wire.read();  // Read remaining bytes into out array
      Serial.print(i);
      Serial.print(": ");
      Serial.println(out[i]);
    }
  }

  // Ensure func is within bounds (0 in this case)
  if (func < 1) {
    function_array[func](out);  // Call the function based on func index
  }
}

void beep(uint8_t onof[]) {
  // Assuming the first byte indicates ON or OFF (HIGH or LOW)
  if (onof[0] == 1) {
    digitalWrite(4, HIGH);  // Turn on the beep (pin 4 HIGH)
    Serial.println("beep on");
    Serial.print("onof: ");
    Serial.println(onof[0]);
  } else {
    digitalWrite(4, LOW);  // Turn off the beep (pin 4 LOW)
    Serial.println("beep off");
    Serial.print("onof: ");
    Serial.println(onof[0]);
  }
}
