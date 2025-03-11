#include <Wire.h>

// EEPROM I2C address with A0 = 1 (A1 and A2 = 0, 0)
#define EEPROM_ADDRESS 0x51  // Base address is 0x50, A0=1 adds 1

// EEPROM memory address to write to (choose an address within the 64KB range)
#define EEPROM_MEM_ADDR 0x0000

const int8_t n_bloks =40;
int8_t fat[n_bloks];//lets you look up if a block is in use
uint8_t block_size = 5;//the size of each block
uint8_t used_len[n_bloks]; //bytes used of a block //wildly ineficient 

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

  uint8_t tt[] = {1,2,3,4,5,6,7,8,9,10,11,12};
  uint8_t file_id = write_file(tt, 12);
  Serial.println("read file size:");
  Serial.println(get_file_size(file_id));

  uint8_t re[12];
  uint8_t lig[15];
  readSequential(0, lig, 15);
  read_file(file_id, re, 12);

  Serial.println("mem read:");
  for (int i = 0; i < 15; i++) {
    Serial.println(lig[i]);
  }

  Serial.println("file read:");
  for (int i = 0; i < 12; i++) {
    Serial.println(re[i]);
  }
  Serial.println("fat:");
  for (int i = 0; i < 15; i++) {
    Serial.println(fat[i]);
  }

}

void loop() {
  // Empty loop
}

void write_page(uint16_t memAddr, int8_t* arr, int len) {
  while (len > 0) {
    int bytesToWrite = min(128 - (memAddr % 128), len);  // Don't cross page boundary

    while (bytesToWrite > 0) {
      int chunkSize = min(30, bytesToWrite);  // Max chunk size due to Wire buffer limit

      Wire.beginTransmission(EEPROM_ADDRESS);
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

void readSequential(uint16_t startAddr, int8_t* out, int len) {
  Wire.beginTransmission(EEPROM_ADDRESS);

  // Send the start memory address (two bytes: high byte and low byte)
  Wire.write((startAddr >> 8) & 0xFF);  // High byte
  Wire.write(startAddr & 0xFF);         // Low byte

  Wire.endTransmission();

  delay(5);
  // Request the number of bytes to read (1 bytes per uint8_t)

  while (len > 0) {
    int chunkSize=min(32,len);

    Wire.requestFrom(EEPROM_ADDRESS, chunkSize);

    for (int i = 0; i < chunkSize; i++) {
      if (Wire.available()>=1) {  // Ensure byte are available
        *out = Wire.read();  // Read byte
        out++; //move to next byte in out buffer
      }
    }
    len-=chunkSize;
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

int aloc_rand_block() {
  int ra;
  for (int i = 0; i < 5; i++) {
    ra = rand() % (n_bloks + 1);  // Get a random block
    
    if (fat[ra] == -1) {  // Check if it's free
      fat[ra] = -3;  // Mark the block as allocated
      return ra;  // Return the random block
    }
  }

  // If the random search didn't work, check every block
  for (int i = 0; i < n_bloks; i++) {
    if (fat[i] == -1) {
      fat[i] = -3;  // Mark the block as allocated
      return i;  // Return the block
    }
  }

  return -1;  // No free block found
}


int block_free(int id){
  if (id < n_bloks){
    fat[id]=-1;
    return 1;
  }else {
    return -1;
  }
}


uint8_t write_file(uint8_t* data, uint16_t size) {
  int prev_block = -1;
  int block = block_aloc();  // Allocate the first block
  int id = block;            // File ID is the first block

  if (block == -1) {
    Serial.println("No more free blocks!");
    return -1;  // No free blocks available
  }
  uint16_t addr = block * block_size;
  

  writeNumber(addr,(size >> 8) & 0xFF);   // High byte
  writeNumber(addr+1,size & 0xFF);        // Low byte
  Serial.print("size: ");
  Serial.println(size);

  uint16_t len =min(size,block_size-2);

  write_page(addr+2, data, len);

  data+=len;
  size-=len;

  prev_block = block;

  block = block_aloc();

  if (block == -1) {
    Serial.println("No more free blocks!");
    return -1;  // No free blocks available
  }


  if (prev_block != -1) {
      fat[prev_block] = block;
  }

  

  while(size > 0){
    // Calculate the EEPROM address for this block
    addr = block * block_size;

    // Check if there's still data to write before proceeding
    if (block == -1) {
      Serial.println("No more free blocks!");
      return -1;  // No more free blocks available
    }

    len =min(size,block_size);

    // Perform the write operation
    write_page(addr, data, len);

    data+=len;
    size-=len;

    //do you suffer from ligma ?

    // Link the previous block to this one (if necessary)
    if (prev_block != -1) {
      fat[prev_block] = block;
    }

    prev_block = block;
    
    // Only allocate a new block if there's still data left to write
    if (0<size) {
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
  uint16_t addr = block*block_size;

  uint16_t read_len = (readNumber(addr)<< 8);  // Read high byte
  read_len |= readNumber(addr+1);    // Read low byte
  Serial.print("read_len: ");
  Serial.println(read_len);

  size = min(size,read_len); //to make shure we dont read more then posible
  
  uint8_t len = min(block_size-2, size); //read rest of block or if user requested is smaler read that

  readSequential(addr+2, data , len);

  data+=len;
  size-=len;

  block=fat[block];

  while(0<size){
    addr=block*block_size;
    
    len=min(block_size,size);

    readSequential(addr, data , len);

    data+=len;
    size-=len;

    block=fat[block];

    if (block==-2){
      break;
    }
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

uint16_t get_file_size(int id){
  uint16_t addr = id*block_size;

  uint16_t len = (readNumber(addr)<< 8);
  len |= readNumber(addr+1);

  return len;
}
