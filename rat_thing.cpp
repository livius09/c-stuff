#define s1 2
#define s2 3

bool pos;
volatile unsigned long lastA = 0;
volatile unsigned long lastB = 0;

void setup() {
  Serial.begin(9600);
  pinMode(2,INPUT);
  pinMode(3,INPUT);
  pinMode(4,OUTPUT);
  
  attachInterrupt(digitalPinToInterrupt(s1), detectA, FALLING);
  attachInterrupt(digitalPinToInterrupt(s2), detectB, FALLING);

}

uint8_t val=4;
long cur = 1;
void loop() {
  val=get_direction();
  cur=millis();

  if((val==0) && ((cur-lastA)< 200) && (cur-lastB)< 200)){
    digitalWrite(4, HIGH);
    delay(100);
    digitalWrite(4, LOW);
  }
  delay(100);

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
  if (abs(dif)>2000 || dif==0){
    return 2;
  }
  if (dif>0) {
    return 1;
  }else if(dif<0) {
    return 0;
  }
}
