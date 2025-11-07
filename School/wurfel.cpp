#include <Adafruit_NeoPixel.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <Adafruit_SSD1306.h>
#include <splash.h>
#include <Adafruit_GFX.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_ADDR 0x3C
#define LDR_PIN A1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
int numberToDisplay = 1;
 int hoo = 2;
 int homi = 3;
 int hou = 4;

int veli = 5;
int vemi = 6;
int vere = 7;
int blue = 9;
  int red = 10;
int green = 11;

int led28 = 8 ;
int brightness = 1;
int myDelay = 333;
Adafruit_NeoPixel ledModule = Adafruit_NeoPixel(3, led28, NEO_GRB + NEO_KHZ800);



void setup() {
   Serial.begin(9600);
if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("Display konnte nicht gestartet werden");
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(8);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(45, 7);

ledModule.begin();
ledModule.clear();
ledModule.show();
pinMode(LDR_PIN, INPUT);
 pinMode(hoo, OUTPUT);
 pinMode(homi, OUTPUT);
 pinMode(hou, OUTPUT);
 pinMode(veli, OUTPUT);
 pinMode(vemi, OUTPUT);
 pinMode(vere ,OUTPUT);
 randomSeed(analogRead(0));
 pinMode(blue,OUTPUT);
pinMode(red,OUTPUT);
pinMode(green,OUTPUT);
}

void loop() {
  wuerfel();
  RGB();
  led2812();
  zeigeLumen();
  blinken();
}

void zeigeLumen() {
  for (int i = 0; i < 5; i++) {
    int ldrWert = analogRead(LDR_PIN);



    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 10);
    display.print("Licht:");
    display.setCursor(0, 35);
    display.print(ldrWert);
    display.display();

    delay(1000);
  }

  display.clearDisplay();
  display.display();
}
void wuerfel() {
  display.setTextSize(8);
  reset();
  display.clearDisplay();
  display.setCursor(45, 7);
  display.print("1");
  display.display();
  eins();
  delay(500);
  reset();
  display.clearDisplay();
  delay(500);

  display.setCursor(45, 7);
  display.print("2");
  display.display();
  zwei();

  reset();
  display.clearDisplay();
  delay(500);

  display.setCursor(45, 7);
  display.print("3");
  display.display();
  drei();
    delay(250);
  reset();
  display.clearDisplay();
  delay(500);

  display.setCursor(45, 7);
  display.print("4");
  display.display();
  vier();
  delay(500);
  reset();
  display.clearDisplay();
  delay(500);

  display.setCursor(45, 7);
  display.print("5");
  display.display();
  fuenf();
  delay(500);
  reset();
  display.clearDisplay();
  delay(500);

  display.setCursor(45, 7);
  display.print("6");
  display.display();
  sechs();
  delay(500);
  display.clearDisplay();

  numberToDisplay = 1;
  reset();
}
void reset(){
 digitalWrite(hoo, LOW);
 digitalWrite(homi, LOW);
 digitalWrite(hou, LOW);
digitalWrite(vere, HIGH);
digitalWrite(vemi, HIGH);
digitalWrite(veli, HIGH);
digitalWrite(red,LOW);
digitalWrite(green,LOW);
digitalWrite(blue,LOW);
}



void oli() {
  digitalWrite(veli, LOW);
  digitalWrite(hoo, HIGH);
}
void omi() {
 digitalWrite(vemi, LOW);
 digitalWrite(hoo, HIGH);
}
void ore() {
digitalWrite(vere, LOW);
 digitalWrite(hoo, HIGH);
}
void mili() {
  digitalWrite(veli, LOW);
   digitalWrite(homi, HIGH);

}
void mimi() {
 digitalWrite(vemi, LOW);
   digitalWrite(homi, HIGH);
}void mire() {
  digitalWrite(vere, LOW);
   digitalWrite(homi, HIGH);
}void uli() {
 digitalWrite(veli, LOW);
  digitalWrite(hou, HIGH);

}
void umi() {
 digitalWrite(vemi, LOW);
  digitalWrite(hou, HIGH);

}void ure() {
 digitalWrite(vere, LOW);
  digitalWrite(hou, HIGH);
}
void eins(){
  digitalWrite(vemi, LOW);
   digitalWrite(homi, HIGH);
   delay(500);
}
void zwei(){
for(int i = 0; i < 100; i++){
uli();
delay(5);
reset();
ore();
delay(5);
reset();
}
}
void drei(){
  for(int i = 0; i < 50; i++){
uli();
delay(5);
reset();
mimi();
delay(5);
reset();
ore();
delay(5);
reset();
  }
  reset();
}
void vier(){
  
digitalWrite(veli, LOW);
digitalWrite(vere, LOW);
digitalWrite(hoo, HIGH);
digitalWrite(hou, HIGH);
delay(700);
   
reset();
}
void fuenf(){
  for(int i = 0; i < 50; i++){
digitalWrite(veli, LOW);
digitalWrite(vere, LOW);
digitalWrite(hoo, HIGH);
digitalWrite(hou, HIGH);
delay(5);
reset();
mimi();
delay(5);
reset();
  }
}
void sechs(){
 digitalWrite(veli, LOW);
 digitalWrite(vere, LOW);
 digitalWrite(hoo, HIGH);
 digitalWrite(homi, HIGH);
 digitalWrite(hou, HIGH);
}
void dim_red(int x){
int red = 10;
analogWrite(red,x);
} 
void dim_blue(int x) {
  int blue = 9;
analogWrite(blue,x);
}
void dim_green(int x){
  int green = 11;
analogWrite(green,x);

}
void RGB(){


  display.setTextSize(5);
  display.setCursor(20, 10);
  display.print("RGB");
  display.display();
  delay(500);
  display.clearDisplay();
for(int i = 0; i < 250; i++) {
 Serial.print(") for(1) : (");
  Serial.print(i);
  dim_red(i);
dim_blue(250-i);
delay(5);
}
for(int i = 0; i < 250; i++) {
   Serial.print(") for(2) : (");
  Serial.print(i);
  dim_green(i);
dim_red(250-i);
delay(5);
}
for(int i = 0; i < 250; i++) {
    Serial.print(") for(3) : (");
  Serial.print(i);
  dim_blue(i);
dim_green(250-i);
delay(5);
}
digitalWrite(red, LOW);
digitalWrite(green, LOW);
digitalWrite(blue, LOW);
}
void led2812(){
  display.setTextSize(4);
  display.setCursor(7, 20);
  display.print("AMPEL");
  display.display();
  delay(500);
  display.clearDisplay();
  for(int i = 0; i < 3; i++){
  ledModule.setPixelColor(0,ledModule.Color(100,0,0));
ledModule.show();
delay(myDelay);

delay(myDelay);
 ledModule.setPixelColor(1,ledModule.Color(100,100,0));
ledModule.show();
delay(myDelay);

delay(myDelay);
 ledModule.setPixelColor(2,ledModule.Color(0,100,0));
ledModule.show();
delay(2 * myDelay);
ledModule.clear();
ledModule.show();
delay(2 * myDelay);
}
}
void blinken() {
  for (int i = 0; i < 5; i++) {
    
    digitalWrite(hoo, HIGH);
    digitalWrite(homi, HIGH);
    digitalWrite(hou, HIGH);
    digitalWrite(veli, LOW);
    digitalWrite(vemi, LOW);
    digitalWrite(vere, LOW);
    dim_red(255);
   

    ledModule.setPixelColor(0, ledModule.Color(100, 0, 0));
    ledModule.setPixelColor(1, ledModule.Color(100, 0, 0));
    ledModule.setPixelColor(2, ledModule.Color(100, 0, 0));
    ledModule.show();

    delay(250);

  
    reset();
    ledModule.clear();
    ledModule.show();

    delay(250);
  }
}
