/* HTL-Wels
 * Levi Wimroither
 * 1CHIT
 * 18.6.2025
 * 
 * Durch Interrupt vom Bewegungssensor geweckt
 * Leuchtet LED-Streifen in 3-sekündigen Abständen
 * Geht danach wieder in einen Ruhemodus um Energie zu sparen
 */

#include <avr/power.h>
#include <avr/interrupt.h>
#include <FastLED.h>

// Number of LEDs per strip
#define NUM_LEDS 9

// LED arrays
CRGB redLeds[NUM_LEDS];
CRGB greenLeds[NUM_LEDS];
CRGB pinkLeds[NUM_LEDS];

volatile bool state = false; // Triggered by interrupt

// --- INTERRUPT HANDLER ---
void wakeUp() {
  state = true;
}

// --- SETUP ---
void setup() {
  
  Serial.begin(9600);

  pinMode(2, INPUT);   // Motion sensor input
  pinMode(3, OUTPUT);  // LED indicator or relay
  pinMode(4, OUTPUT);  // LED indicator
  pinMode(5, OUTPUT);  // Buzzer

  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);

  FastLED.addLeds<WS2811, 8, GRB>(redLeds, NUM_LEDS);
  FastLED.addLeds<WS2811, 9, GRB>(greenLeds, NUM_LEDS);
  FastLED.addLeds<WS2811, 10, GRB>(pinkLeds, NUM_LEDS);

  attachInterrupt(digitalPinToInterrupt(2), wakeUp, RISING);
}

// --- MAIN LOOP ---
void loop() {
  if (state) {
    state = false;
    do_ordeal();
  }

  
  delay(50);  // short, low-cost idle delay
}

// --- LIGHT SHOW + SOUND ROUTINE ---
void do_ordeal() {

  // simple visual + sound sequence
  digitalWrite(4, HIGH);
  delay(500);
  digitalWrite(4, LOW);

  tone(5, 3000, 1000); // 3000 Hz tone for 1 second

  digitalWrite(3, HIGH);
  delay(500);
  digitalWrite(3, LOW);

  // Red strip on
  fill_solid(redLeds, NUM_LEDS, CRGB::Green);
  FastLED.show();
  delay(3000);

  // Switch to green strip
  fill_solid(greenLeds, NUM_LEDS, CRGB(150, 0, 0));
  fill_solid(redLeds, NUM_LEDS, CRGB::Black);
  FastLED.show();
  delay(3000);

  // Switch to pink strip
  fill_solid(pinkLeds, NUM_LEDS, CRGB(128, 0, 128));
  fill_solid(greenLeds, NUM_LEDS, CRGB::Black);
  FastLED.show();
  delay(3000);

  // Turn off everything
  FastLED.clear();
  FastLED.show();
 
}
