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
#define BUNNY_LEDS 3

// LED arrays
CRGB redLeds[NUM_LEDS];     //ET
CRGB greenLeds[NUM_LEDS];   //MT
CRGB pinkLeds[NUM_LEDS];    //IT
CRGB flagLeds[BUNNY_LEDS];           //that bunny

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

  pinMode(6, OUTPUT); //control Et board
  pinMode(7, OUTPUT); //control IT board

  pinMode(8, OUTPUT);   //red
  pinMode(9, OUTPUT);   //green
  pinMode(10, OUTPUT);  //pink
  pinMode(11, OUTPUT);  //bunny

  FastLED.addLeds<WS2811, 8, GRB>(redLeds, NUM_LEDS);
  FastLED.addLeds<WS2811, 9, GRB>(greenLeds, NUM_LEDS);
  FastLED.addLeds<WS2811, 10, GRB>(pinkLeds, NUM_LEDS);
  FastLED.addLeds<WS2811, 11, GRB>(flagLeds, BUNNY_LEDS);

  attachInterrupt(digitalPinToInterrupt(2), wakeUp, RISING);
}

// --- MAIN LOOP ---
void loop() {
  if (state) {
    do_ordeal();
    state = false;
  }

  
  delay(50);  // short, low-cost idle delay
}

// --- LIGHT SHOW + SOUND ROUTINE ---
void do_ordeal() {

  fill_solid(flagLeds, BUNNY_LEDS , CRGB::Red);
  flagLeds[1] = CRGB::White;
  FastLED.show();

  // simple visual + sound sequence
  digitalWrite(4, HIGH);
  delay(500);
  digitalWrite(4, LOW);

  tone(5, 3000, 1000); // 3000 Hz tone for 1 second

  digitalWrite(3, HIGH);
  delay(500);
  digitalWrite(3, LOW);


  //control ET board
  digitalWrite(6, HIGH);
  delay(500);
  digitalWrite(6, LOW);

  // Red strip on
  fill_solid(redLeds, NUM_LEDS, CRGB::Green);
  FastLED.show();
  delay(3000);


  // Switch to green strip
  fill_solid(greenLeds, NUM_LEDS, CRGB(150, 0, 0));
  fill_solid(redLeds, NUM_LEDS, CRGB::Black);
  FastLED.show();
  delay(3000);

  //control IT board
  digitalWrite(7, HIGH);
  delay(500);
  digitalWrite(7, LOW);

  // Switch to pink strip
  fill_solid(pinkLeds, NUM_LEDS, CRGB(128, 0, 128));
  fill_solid(greenLeds, NUM_LEDS, CRGB::Black);
  FastLED.show();
  delay(3000);

  // Turn off everything
  FastLED.clear();
  FastLED.show();
 
}
