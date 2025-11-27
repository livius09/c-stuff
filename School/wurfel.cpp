#include <Adafruit_NeoPixel.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
#define LDR_PIN A1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

const int topPins[]    = {2, 3, 4};
const int bottomPins[] = {5, 6, 7};
const int rgbPins[]    = {9, 10, 11};

const int led28 = 8;
int myDelay = 333;
volatile bool control= false;

Adafruit_NeoPixel ledModule(3, led28, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(8);
  display.setTextColor(SSD1306_WHITE);

  ledModule.begin();
  ledModule.clear();
  ledModule.show();

  pinMode(LDR_PIN, INPUT);

  for (int p : topPins) pinMode(p, OUTPUT);
  for (int p : bottomPins) pinMode(p, OUTPUT);
  for (int p : rgbPins) pinMode(p, OUTPUT);

  randomSeed(analogRead(0));

  pinMode(13,INPUT);

  attachInterrupt(digitalPinToInterrupt(13), reciveControl , FALLING);
}

void loop() {

  if(control){
    wuerfel();
    RGBcycle();
    ampelfkt();
    showLumen();
    blinken();
    control = false;
  }
  delay(100);
}


void reciveControl(){
  control = true;
}

void reset() {
  for (int p : topPins) digitalWrite(p, LOW);
  for (int p : bottomPins) digitalWrite(p, HIGH);
  for (int p : rgbPins)  digitalWrite(p, LOW);
}

void setDot(int topIndex, int bottomIndex) {
  reset();
  if (bottomIndex >= 0) digitalWrite(bottomPins[bottomIndex], LOW);
  if (topIndex >= 0)    digitalWrite(topPins[topIndex], HIGH);
  delay(5);
}

void wuerfel() {
  int patterns[6][3][2] = {
    {{1,1},{-1,-1},{-1,-1}},                     // 1
    {{2,0},{0,2},{-1,-1}},                       // 2
    {{2,0},{1,1},{0,2}},                         // 3
    {{0,0},{2,2},{-1,-1}},                       // 4
    {{0,0},{1,1},{2,2}},                         // 5
    {{0,0},{1,1},{2,2}}                          // 6 same, but one-shot
  };

  for (int n = 0; n < 6; n++) {
    display.clearDisplay();
    display.setCursor(45, 7);
    display.print(n + 1);
    display.display();

    for (int i = 0; i < 3; i++) {
      int t = patterns[n][i][0];
      int b = patterns[n][i][1];
      if (t >= 0 || b >= 0) setDot(t, b);
    }

    delay(500);
    reset();
    display.clearDisplay();
  }
}

void RGBcycle() {
  display.clearDisplay();
  display.setTextSize(5);
  display.setCursor(20, 10);
  display.print("RGB");
  display.display();
  delay(500);
  display.clearDisplay();

  for (int i = 0; i < 250; i++) {
    analogWrite(rgbPins[1], i);
    analogWrite(rgbPins[0], 250 - i);
    delay(5);
  }
  for (int i = 0; i < 250; i++) {
    analogWrite(rgbPins[2], i);
    analogWrite(rgbPins[1], 250 - i);
    delay(5);
  }
  for (int i = 0; i < 250; i++) {
    analogWrite(rgbPins[0], i);
    analogWrite(rgbPins[2], 250 - i);
    delay(5);
  }

  for (int p : rgbPins) digitalWrite(p, LOW);
}

void ampelfkt() {
  display.setTextSize(4);
  display.setCursor(7, 20);
  display.print("AMPEL");
  display.display();
  delay(500);
  display.clearDisplay();

  for (int i = 0; i < 3; i++) {
    ledModule.setPixelColor(0, ledModule.Color(100,0,0));
    ledModule.show();
    delay(myDelay);

    ledModule.setPixelColor(1, ledModule.Color(100,100,0));
    ledModule.show();
    delay(myDelay);

    ledModule.setPixelColor(2, ledModule.Color(0,100,0));
    ledModule.show();
    delay(2 * myDelay);

    ledModule.clear();
    ledModule.show();
    delay(2 * myDelay);
  }
}

void showLumen() {
  for (int i = 0; i < 5; i++) {
    int val = analogRead(LDR_PIN);

    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 10);
    display.print("Licht:");
    display.setCursor(0, 35);
    display.print(val);
    display.display();

    delay(1000);
  }
  display.clearDisplay();
  display.display();
}

void blinken() {
  for (int i = 0; i < 5; i++) {
    for (int p : topPins) digitalWrite(p, HIGH);
    for (int p : bottomPins) digitalWrite(p, LOW);
    analogWrite(rgbPins[1], 255);

    for (int i = 0; i < 3; i++)
      ledModule.setPixelColor(i, ledModule.Color(100,0,0));
    ledModule.show();

    delay(250);

    reset();
    ledModule.clear();
    ledModule.show();
    delay(250);
  }
}
