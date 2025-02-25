#include <Wire.h>

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
  for (int i=0; i<40; i++) {
    fat[i]=-1;
  }



  bool init=true;

  Serial.begin(9600);

  pinMode(2, OUTPUT);
  Wire.begin();
  Wire.setClock(10000);
  
  if(init){

    char hello[]="Hello World";
    int len = sizeof(hello)/sizeof(hello[0]);
    
    write_file(0x51, hello, len);

    write_page(0x52,0, fat , 40);

  }else {
    readSequential(0x52, 0, fat, 40);
  }

  char lig[60];

  for (int i=0; i<60; i++) {
    lig[i]="a";
  }

  read_file(0x51, 0, lig, 12);

  for (int i=0; i<15; i++) {
    Serial.print(lig[i]);
  }


    
}

void loop() {
  // Empty loop
}


void write_page(uint8_t eeprom_addr, uint16_t memAddr, int8_t* arr, int len) {
  while (len > 0) {
    int bytesToWrite = min(128 - (memAddr % 128), len);  // Don't cross page boundary

    while (bytesToWrite > 0) {
      int chunkSize = min(30, bytesToWrite);  // Max chunk size due to Wire buffer limit

      Wire.beginTransmission(eeprom_addr);
      Wire.write((memAddr >> 8) & 0xFF);
      Wire.write(memAddr & 0xFF);

      for (int i = 0; i < chunkSize; i++) {
        Wire.write(arr[i]);
      }

      Wire.endTransmission();
      delay(10);  // Allow EEPROM write cycle

      memAddr += chunkSize;
      arr += chunkSize;
      len -= chunkSize;
      bytesToWrite -= chunkSize;
    }
  }
}


void readSequential(uint8_t eeprom_addr,uint16_t startAddr, int8_t* out, int len) {
  Wire.beginTransmission(eeprom_addr);

  // Send the start memory address (two bytes: high byte and low byte)
  Wire.write((startAddr >> 8) & 0xFF);  // High byte
  Wire.write(startAddr & 0xFF);         // Low byte

  Wire.endTransmission();

  delay(5);
  // Request the number of bytes to read (1 bytes per uint8_t)

  while (len > 0) {
    int chunkSize=min(32,len);

    Wire.requestFrom(eeprom_addr, chunkSize);

    for (int i = 0; i < chunkSize; i++) {
      if (Wire.available()>=1) {  // Ensure byte are available
        *out = Wire.read();  // Read byte
        out++; //move to next byte in out buffer
      }
    }
    len-=chunkSize;
  }
}

void writeNumber(uint8_t eeprom_addr, uint16_t memAddr, uint8_t number) {
  Wire.beginTransmission(eeprom_addr);

  // Send the memory address (two bytes: high byte and low byte)
  Wire.write((memAddr >> 8) & 0xFF);  // High byte
  Wire.write(memAddr & 0xFF);         // Low byte

  // Send the number 
  Wire.write(number);   

  Wire.endTransmission();
  delay(10);  // Wait for EEPROM to complete the write process
}

uint8_t readNumber(uint8_t eeprom_addr, uint16_t memAddr) {
  Wire.beginTransmission(eeprom_addr);

  // Send the memory address (two bytes: high byte and low byte)
  Wire.write((memAddr >> 8) & 0xFF);  // High byte
  Wire.write(memAddr & 0xFF);         // Low byte
  Wire.endTransmission();

  // Request byte from EEPROM
  Wire.requestFrom(eeprom_addr, 1);
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

uint8_t write_file(uint8_t eeprom_addr,uint8_t* data, int size) {
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
    writeNumber(eeprom_addr,addr, data[i]);

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




void read_file(uint8_t eeprom_addr,int id, uint8_t* data,int size){

  int block=id;
  int index =0;

  while(block != -2 && index<size){ 
    uint16_t addr =block*block_size;
    data[index++] = readNumber(eeprom_addr,addr);
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


