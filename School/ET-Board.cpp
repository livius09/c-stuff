#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define CHOICE_RED      (1 << 0)
#define CHOICE_GREEN    (1 << 1)
#define CHOICE_BLUE     (1 << 2)
#define CHOICE_YELLOW   (1 << 3)
#define CHOICE_OFF      0

#define LED_RED     10
#define LED_GREEN   3
#define LED_BLUE    13
#define LED_YELLOW  5

#define BUZZER1  4
#define BUZZER2  7

byte gameBoard[32];
byte gameRound = 0;

void setup()
{
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);

  pinMode(BUZZER1, OUTPUT);
  pinMode(BUZZER2, OUTPUT);

  pinMode(8, INPUT_PULLUP);

  play_winner();
}

void loop()
{
  if (digitalRead(8) == LOW)  // DEMO_PIN
  {
    Serial.println("begining demo");
    self_play_demo();
    self_play_demo();
    self_play_demo();
    self_play_demo();
    display.clearDisplay();
    
    Serial.println("end demo");
  }
}

void self_play_demo()
{

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(8,0);
  display.println("HTL-Wels Simon Says");

  display.setTextSize(2);
  display.setCursor(18,15);
  display.println("MyBoard");
  display.display();
  
  delay(100);
  add_to_moves();
  playMoves();

  for (byte i = 0; i < gameRound; i++)
  {
    toner(gameBoard[i], 150);
    delay(150);
  }

  delay(800);

  if (gameRound > 20) gameRound = 0;
}

void playMoves(void)
{
  for (byte currentMove = 0 ; currentMove < gameRound ; currentMove++)
  {
    toner(gameBoard[currentMove], 150);
    delay(150);
  }
}

void add_to_moves(void)
{
  byte newButton = random(0, 4);
  if(newButton == 0) newButton = CHOICE_RED;
  else if(newButton == 1) newButton = CHOICE_GREEN;
  else if(newButton == 2) newButton = CHOICE_BLUE;
  else if(newButton == 3) newButton = CHOICE_YELLOW;

  gameBoard[gameRound++] = newButton;
}

void setLEDs(byte leds)
{
  digitalWrite(LED_RED,    leds & CHOICE_RED);
  digitalWrite(LED_GREEN,  leds & CHOICE_GREEN);
  digitalWrite(LED_BLUE,   leds & CHOICE_BLUE);
  digitalWrite(LED_YELLOW, leds & CHOICE_YELLOW);
}

void toner(byte which, int buzz_length_ms)
{
  setLEDs(which);
  switch(which)
  {
    case CHOICE_RED:    buzz_sound(buzz_length_ms, 1136); break;
    case CHOICE_GREEN:  buzz_sound(buzz_length_ms, 568);  break;
    case CHOICE_BLUE:   buzz_sound(buzz_length_ms, 851);  break;
    case CHOICE_YELLOW: buzz_sound(buzz_length_ms, 638);  break;
  }
  setLEDs(CHOICE_OFF);
}

void buzz_sound(int buzz_length_ms, int buzz_delay_us)
{
  long buzz_length_us = buzz_length_ms * 1000L;

  while (buzz_length_us > (buzz_delay_us * 2))
  {
    buzz_length_us -= buzz_delay_us * 2;
    digitalWrite(BUZZER1, LOW);
    digitalWrite(BUZZER2, HIGH);
    delayMicroseconds(buzz_delay_us);
    digitalWrite(BUZZER1, HIGH);
    digitalWrite(BUZZER2, LOW);
    delayMicroseconds(buzz_delay_us);
  }
}

void play_winner(void)
{
  setLEDs(CHOICE_GREEN | CHOICE_BLUE); winner_sound();
  setLEDs(CHOICE_RED | CHOICE_YELLOW); winner_sound();
  setLEDs(CHOICE_GREEN | CHOICE_BLUE); winner_sound();
  setLEDs(CHOICE_RED | CHOICE_YELLOW); winner_sound();
  setLEDs(CHOICE_OFF);
}

void winner_sound(void)
{
  for (byte x = 250 ; x > 70 ; x--)
  {
    for (byte y = 0 ; y < 3 ; y++)
    {
      digitalWrite(BUZZER2, HIGH);
      digitalWrite(BUZZER1, LOW);
      delayMicroseconds(x);
      digitalWrite(BUZZER2, LOW);
      digitalWrite(BUZZER1, HIGH);
      delayMicroseconds(x);
    }
  }
}
