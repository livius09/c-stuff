#include <Wire.h>

// EEPROM I2C address with A0 = 1 (A1 and A2 = 0, 0)
#define EEPROM_ADDRESS 0x51  // Base address is 0x50, A0=1 adds 1

// EEPROM memory address to write to (choose an address within the 64KB range)
#define EEPROM_MEM_ADDR 0x0000

int8_t n_bloks =40;
int8_t fat[40];//lets you look up if a block is in use
int8_t block_size = 1;//the size of each block

//meanings in the fat:
//-1 is free
//-2 end of file
//-3 is alocated but point to no file
// >=0: used and links to the next block




void setup() {
  Serial.begin(9600);

  for (int i = 0; i < n_bloks; i++) {
        fat[i] = -1;  // Initialize FAT: -1 means the block is free
    }

  pinMode(2, OUTPUT);
  Wire.begin();
  

  bool wp = 0;
  

  // Set write protect pin
  digitalWrite(2, wp ? HIGH : LOW);

  char txt[] = "abcdefghijklmnopqrstuvwxyz1234567890"; 
  int len=sizeof(txt)/sizeof(txt[0]);
  int abc = write_file(txt,len); //get the id on abc and write the file
  char buff[len];
  read_file(abc, buff, len); //read the file into the buffer

  for (int i = 0; i < len; i++) {
    Serial.print(buff[i]);
  }
  Serial.println(" ");



}

void loop() {
  // Empty loop
}
void test(){
  //uint8_t ar[] =  {1, 2, 3, 4, 5,18,19,20,21,22,23,14};
  //uint8_t ar2[] = {6,7,8,9,10,11,12,13,14,15,16,17};
  uint8_t ar[] =  {1, 2};  // Array of numbers to write
  uint8_t ar2[] = {6,7};  // Array of numbers to write
  int len = sizeof(ar) / sizeof(ar[0]);

  Serial.println("initial FAT:");
  for (int u = 0; u < n_bloks; u++) {
    Serial.println(fat[u]);
  }

  int file_one_id=write_file(ar,len);
  int file_two_id=write_file(ar2,len);

  uint8_t datam[30];  // Array to store the read data
  readSequential(0x00,datam,30);

  Serial.println("mem read from EEPROM:");
  for (int t = 0; t < 30; t++) {
    Serial.println(datam[t]);
  }

  Serial.println("updated FAT:");
  for (int j = 0; j < n_bloks; j++) {
    Serial.println(fat[j]);
  }

  uint8_t data[len];  // Array to store the read data
  read_file(file_one_id,data,len);

  Serial.println("file read from EEPROM:");
  for (int i = 0; i < len; i++) {
    Serial.println(data[i]);
  }

  delet_file(file_one_id);

  Serial.println("deleted file FAT:");
  for (int e = 0; e < n_bloks; e++) {
    Serial.println(fat[e]);
  }
}

void writeRecursively(uint16_t memAddr, uint8_t* arr, int len) {
  if (len == 0) {
    return;
  }
  int pageOffset = memAddr % 128;
  int bytesToWrite = min(128 - pageOffset, len * 2);

  Wire.beginTransmission(EEPROM_ADDRESS);
  // Send the memory address (two bytes: high byte and low byte)
  Wire.write((memAddr >> 8) & 0xFF);  // High byte
  Wire.write(memAddr & 0xFF);         // Low byte

  for (int i = 0; i < bytesToWrite; i++) {
    // Send the number
    Wire.write(arr[i]);
  }

  Wire.endTransmission();
  delay(10);  // Wait for EEPROM to complete the write process

  // Recursively write the remaining data
  writeRecursively(memAddr + bytesToWrite, arr + bytesToWrite, len - bytesToWrite);
}

void readSequential(uint16_t startAddr, uint8_t* out, int len) {
  Wire.beginTransmission(EEPROM_ADDRESS);

  // Send the start memory address (two bytes: high byte and low byte)
  Wire.write((startAddr >> 8) & 0xFF);  // High byte
  Wire.write(startAddr & 0xFF);         // Low byte

  Wire.endTransmission();

  // Request the number of bytes to read (1 bytes per uint8_t)
  Wire.requestFrom(EEPROM_ADDRESS, len);

  for (int i = 0; i < len; i++) {
    if (Wire.available()) {  // Ensure 2 bytes are available
      uint16_t number = Wire.read();  // Read byte
      out[i] = number;
    }
  }
}

void writeNumber(uint16_t memAddr, uint8_t number) {
  Wire.beginTransmission(EEPROM_ADDRESS);

  // Send the memory address (two bytes: high byte and low byte)
  Wire.write((memAddr >> 8) & 0xFF);  // High byte
  Wire.write(memAddr & 0xFF);         // Low byte

  // Send the number 
  Wire.write(number);   

  Wire.endTransmission();
  delay(10);  // Wait for EEPROM to complete the write process
}

uint8_t readNumber(uint16_t memAddr) {
  Wire.beginTransmission(EEPROM_ADDRESS);

  // Send the memory address (two bytes: high byte and low byte)
  Wire.write((memAddr >> 8) & 0xFF);  // High byte
  Wire.write(memAddr & 0xFF);         // Low byte
  Wire.endTransmission();

  // Request byte from EEPROM
  Wire.requestFrom(EEPROM_ADDRESS, 1);
  uint8_t number=0;
  if (Wire.available()) {
    number = Wire.read();
  }

  return number;
}

int block_aloc(){
  for(int i=0; i < n_bloks; i++){
    if (fat[i]==-1){
      fat[i]=-3;
      return i;
    }
  }
  return-1;
}

int block_free(int id){
  if (id < n_bloks){
    fat[id]=-1;
    return 1;
  }else {
    return -1;
  }
}

uint8_t write_file(uint8_t* data, int size) {
  int prev_block = -1;
  int block = block_aloc();  // Allocate the first block
  int id = block;            // File ID is the first block
  int offset = 0;            // Offset to track the remaining data to write

  if (block == -1) {
    Serial.println("No more free blocks!");
    return -1;  // No free blocks available
  }

  for (int i = 0; i < size; i++) {
    // Calculate the EEPROM address for this block
    uint16_t addr = block * block_size;

    // Check if there's still data to write before proceeding
    if (block == -1) {
      Serial.println("No more free blocks!");
      return -1;  // No more free blocks available
    }

    // Perform the write operation
    writeNumber(addr, data[i]);

    // Link the previous block to this one (if necessary)
    if (prev_block != -1) {
      fat[prev_block] = block;
    }

    prev_block = block;
    
    // Only allocate a new block if there's still data left to write
    if (i < size-1) {
      block = block_aloc();  // Allocate the next block
    } else {
      block = -1;  // No more blocks needed; mark as finished
    }
  }

  // Mark the end of the file in the FAT
  fat[prev_block] = -2;  // End of the file

  return id;  // Return the file ID
}




void read_file(int id, uint8_t* data,int size){

  int block=id;
  int index =0;

  while(block != -2 && index<size){
    
    uint16_t addr =block*block_size;

    
    data[index++] = readNumber(addr);
  
    block=fat[block];
  }
}

void delet_file(int id){
  //go through every block in the linked list and free them
  int block = id;
  while (block != -2){
    int next_block = fat[block]; //get the foloing block before it get overwriten
    block_free(block);
    block =next_block;
  }

}


