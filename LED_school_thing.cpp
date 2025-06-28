/* HTL-Wels
 * Levi Wimroither
 * 1CHIT
 * 18.6.2025
 * 
 * durch interupt von bewegungs sensor geweck
 * erleuchtet die LED streifen in 3 sekündigen abständen
 * geht dann wieder in eine ruhe modus um energie zu sparen
 */

#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <FastLED.h>


// How many leds are in the strip?
#define NUM_LEDS 9


// This is an array of leds.  One item for each led in your strip.
CRGB redLeds[NUM_LEDS];
CRGB greenLeds[NUM_LEDS];
CRGB pinkLeds[NUM_LEDS];


void wakeUp() {
  // Just wake from sleep — keep ISR short and avoid Serial here
}

void goToSleep() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  EIFR = bit (INTF0);  // Clear external interrupt flag

  attachInterrupt(digitalPinToInterrupt(2), wakeUp, RISING);

  sei();  // Enable global interrupts
  sleep_cpu();  // Enter sleep

  //interupt hapens here
  
  sleep_disable();
  detachInterrupt(digitalPinToInterrupt(2));
}

void setup() {
  Serial.begin(9600);

  pinMode(2, INPUT);

  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);

  
  FastLED.addLeds<WS2811, 8, GRB>(redLeds, NUM_LEDS);
  FastLED.addLeds<WS2811, 9, GRB>(greenLeds, NUM_LEDS);
  FastLED.addLeds<WS2811, 10, GRB>(pinkLeds, NUM_LEDS);
  

  power_adc_disable();
  power_spi_disable();
  power_timer1_disable();
  power_timer2_disable();
  // Leave Timer0 enabled if you use delay()


  Serial.println("Going to sleep...");
  delay(100);
  goToSleep();  // First sleep
}

void loop() {
  Serial.println("Woke up!");
  delay(100);

  
  for(int i=0;i<NUM_LEDS;i++){
    redLeds[i] = CRGB::Green;
      
  }

  FastLED.show();

  delay(3000);
  
  for(int a=0;a<NUM_LEDS;a++){
    
    greenLeds[a] = CRGB(150, 0, 0);
    
    
    redLeds[a] = CRGB::Black;
  }
  FastLED.show();

  delay(3000);
  
  for(int e=0;e<NUM_LEDS;e++){
    pinkLeds[e] = CRGB(128, 0, 128);
    
    
    greenLeds[e] = CRGB::Black;
  }
  FastLED.show();
  delay(3000);
  FastLED.clear ();
  FastLED.show();
  
  
  delay(100);
  Serial.println("back to sleep!");
  goToSleep();  // Sleep again
}
