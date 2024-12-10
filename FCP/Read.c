#include <Wire.h>

// Forward declarations
void beep(uint8_t[]);
void light_meashure(uint8_t[]);

// Declare function pointer array with 2 functions
void (*function_array[])(uint8_t[]) = {beep, light_meashure};

uint8_t retquest[30];

void setup() {
  Serial.begin(9600);

  Wire.begin(0x01);  // Set up I2C address
  Wire.onReceive(receiveEvent);  // Set the receive event handler
  Wire.onRequest(requestEvent);  // Set the request event handler

  pinMode(4, OUTPUT);  // Set pin 4 as an output for the beep function
  pinMode(A1, INPUT);
}

void loop() {
  // Nothing needed in loop for now
}

void receiveEvent(int bytes) {
  uint8_t out[30];  // Buffer to store received data
  Serial.print("recived sth");

  if (bytes < 2) {
    Serial.println("Error: Not enough bytes received");
    return;  // Need at least 2 bytes for function ID
  }

  // Read function ID
  uint8_t low = Wire.read();  
  uint8_t high = Wire.read();  
  uint16_t func = ((uint16_t)high << 8) | low;  

  Serial.print("Function ID: ");
  Serial.println(func);

  // Read arguments into `out`
  for (uint8_t i = 0; i < bytes - 2 && Wire.available(); i++) {
    out[i] = Wire.read();
    Serial.print("Arg ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(out[i]);
  }

  // Ensure function ID is valid
  if (func < sizeof(function_array) / sizeof(function_array[0])) {
    function_array[func](out);  // Call the function based on func index
  } else {
    Serial.println("Error: Unknown function ID");
  }
}

void requestEvent() {
  // Send the response back
  for (uint8_t i = 0; i < sizeof(retquest); i++) {
    Wire.write(retquest[i]);
  }
}

void beep(uint8_t onof[]) {
  // Assuming the first byte indicates ON or OFF (HIGH or LOW)
  if (onof[0] == 1) {
    digitalWrite(4, HIGH);  // Turn on the beep (pin 4 HIGH)
    Serial.println("Beep ON");
  } else {
    digitalWrite(4, LOW);  // Turn off the beep (pin 4 LOW)
    Serial.println("Beep OFF");
  }
  Serial.print("On/Off: ");
  Serial.println(onof[0]);
}

void light_meashure(uint8_t[]) {
  // Simulate a light measurement from analog pin A1
  retquest[0] = analogRead(A1);
  Serial.print("Light Measurement: ");
  Serial.println(retquest[0]);
}
