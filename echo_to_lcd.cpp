#define rs 2
#define e 3

#define d0 4
#define d1 5
#define d2 6
#define d3 7
#define d4 8
#define d5 9
#define d6 10
#define d7 11
//rs 0 comand
//rs 1 data

#define echo 13
#define trig 12
char bals[]="Dist\nance:";

void setup() {
  Serial.begin(9600);

  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  delay(50);
  init_lcd();
  

  nl_arr_dat(bals, 10);
  comand_lcd(0xc0);
}

void loop() {
  
  delay(1000);
  
}

void init_lcd(){
  pinMode(rs, OUTPUT);
  pinMode(e, OUTPUT);

  pinMode(d0, OUTPUT);
  pinMode(d1, OUTPUT);
  pinMode(d1, OUTPUT);
  pinMode(d2, OUTPUT);
  pinMode(d3, OUTPUT);
  pinMode(d4, OUTPUT);
  pinMode(d5, OUTPUT);
  pinMode(d6, OUTPUT);
  pinMode(d7, OUTPUT);

  delay(50); // LCD power-up delay

  comand_lcd(0x38); // 8-bit mode, 2-line, 5x8 font
  comand_lcd(0x0C); // Display ON, cursor OFF
  //comand_lcd(0x06); // Entry mode: move right
  comand_lcd(0x01); // Clear display
}

void data_lcd(uint8_t dat){
  digitalWrite(rs, HIGH);

  
  digitalWrite(d0,dat & 0x01 );
  digitalWrite(d1,dat & 0x02 );
  digitalWrite(d2,dat & 0x04 );
  digitalWrite(d3,dat & 0x08 );
  digitalWrite(d4,dat & 0x10 );
  digitalWrite(d5,dat & 0x20 );
  digitalWrite(d6,dat & 0x40 );
  digitalWrite(d7,dat & 0x80 );

  digitalWrite(e, HIGH);
  delayMicroseconds(50);
  digitalWrite(e, LOW);

  delay(2);
}

void comand_lcd(uint8_t cmd){
  digitalWrite(rs, LOW);

  digitalWrite(d0,cmd & 0x01 );
  digitalWrite(d1,cmd & 0x02 );
  digitalWrite(d2,cmd & 0x04 );
  digitalWrite(d3,cmd & 0x08 );
  digitalWrite(d4,cmd & 0x10 );
  digitalWrite(d5,cmd & 0x20 );
  digitalWrite(d6,cmd & 0x40 );
  digitalWrite(d7,cmd & 0x80 );

  digitalWrite(e, HIGH);
  delayMicroseconds(50);
  digitalWrite(e, LOW);

  delay(2);
}

void arr_dat(uint8_t* arr, uint8_t len){
  for(int i=0;i<len;i++){
    data_lcd(arr[i]);
  }
}



void nl_arr_dat(uint8_t* arr,uint8_t len){
  if (arr[len-1]=='\0') len--; //cause of termination char

  if(len==0) return;
  if (len>33) {
    Serial.println("arr is bigger then screen");
    return;
  }

  uint8_t nl=0;
  uint8_t tonl=0;
  for(int i=0;i<len;i++){
    if(arr[i]=='\n'){
      nl++;
      tonl=i;
    }
  }

  if(nl>1){
    Serial.println("more then one NL");
    return;
  }

  if(tonl>16){
      Serial.println("the nl is after 16 chars");
      return;
  }
  Serial.print("chars to nl:");
  Serial.println(tonl);

  

  uint8_t first_line_len = (nl==1) ? (tonl):min(len,16);

  arr_dat(arr,first_line_len);

  
  
  len-=(nl==1)?(first_line_len+1):(first_line_len);
  arr+=(nl==1)?(first_line_len+1):(first_line_len);

  if(len>0){
    comand_lcd(0xC0); //jump to second line first char
    arr_dat(arr,len);
  }else{
    Serial.println("len is negative");
  }


  comand_lcd(0x02);
}

void ln2_data_lcd(uint8_t dat){
  comand_lcd(0xC0);
  data_lcd(dat);
  comand_lcd(0x02);
}

float get_distance(){
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long travelt=pulseIn(echo, HIGH);
  float distance = (travelt * 0.0343) / 2;
  return distance;
}

void str_lcd(String str){
  for(int i=0;i<str.length();i++){
    data_lcd(str.charAt(i));
  }
}
