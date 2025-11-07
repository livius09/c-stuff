#define miceLED 9
#define INPIN 13


void setup() {
  pinMode(INPIN,INPUT);
  pinMode(miceLED, OUTPUT);

  Serial.begin(9600);

}

int num;

void loop() {
  num = digitalRead(INPIN);
  Serial.print(num);

  if(num==HIGH){
    digitalWrite(miceLED, HIGH);
    
  }else{
    digitalWrite(miceLED, LOW);
  }
  delay(500);

}
