#define s1 2
#define s2 3

bool pos;
unsigned long lastA=0;
unsigned long lastB=0;

void setup() {
  Serial.begin(9600);
  pinMode(2,INPUT);
  pinMode(3,INPUT);
  
  attachInterrupt(digitalPinToInterrupt(s1), detectA, FALLING);
  attachInterrupt(digitalPinToInterrupt(s2), detectB, FALLING);

}

void loop() {
  uint8_t val=get_direction();
  if(val==0){
    Serial.println("right");
  }else if(val==1){
    Serial.println("left");
  }else {
    Serial.println("to long");
  }
  Serial.print("A:");
  Serial.println(lastA);

  Serial.print("B:");
  Serial.println(lastB);

  delay(1000);

}

bool get_s1(){
  return digitalRead(2);;
}
bool get_s2(){
  return digitalRead(3);;
}

void detectA(){
  lastA=millis();
}

void detectB(){
  lastB=millis();
}

uint8_t get_direction(){ //0=right, 1=left, 2= dif to big, so to long of a time between the two of the gates being pased that it would count as movement
  long dif=lastA-lastB;
  if (abs(dif)>5000 || dif==0){
    return 2;
  }
  if (dif>0) {
    return 1;
  }else if(dif<0) {
    return 0;
  }
}
