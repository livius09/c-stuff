#include <Wire.h>

// EEPROM I2C address with A0 = 1 (A1 and A2 = 0, 0)
#define EEPROM_ADDRESS 0x51  // Base address is 0x50, A0=1 adds 1

// EEPROM memory address to write to (choose an address within the 64KB range)
#define EEPROM_MEM_ADDR 0x0000


uint16_t runid = 0;
int8_t n_bloks =25;
//uint16_t lookup[100];//lets you look up block of elements by id
int8_t fat[25];//lets you look up if a block is in use
int8_t block_size = 1;//the size of each block

//meanings in the fat:
//-1 is free
//-2 end of file
// >=0: used and links to the next block




void setup() {

  for (int i = 0; i < n_bloks; i++) {
        fat[i] = -1;  // Initialize FAT: -1 means the block is free
    }

  pinMode(2, OUTPUT);
  Serial.begin(9600);
  Wire.begin();
  

  bool wp = 0;
  uint16_t ar[] =  {1, 2, 3, 4, 5,18,19,20,21,22,23,14};  // Array of numbers to write
  uint16_t ar2[] = {6,7,8,9,10,11,12,13,14,15,16,17};  // Array of numbers to write
  int len = sizeof(ar) / sizeof(ar[0]);

  // Set write protect pin
  digitalWrite(2, wp ? HIGH : LOW);

  Serial.println("initial FAT:");
  for (int u = 0; u < n_bloks; u++) {
    Serial.println(fat[u]);
  }

  int file_one_id=write_file(ar,len);
  int file_two_id=write_file(ar2,len);

  uint16_t datam[30];  // Array to store the read data
  readSequential(0x00,datam,30);

  Serial.println("mem read from EEPROM:");
  for (int t = 0; t < 30; t++) {
    Serial.println(datam[t]);
  }

  Serial.println("updated FAT:");
  for (int j = 0; j < n_bloks; j++) {
    Serial.println(fat[j]);
  }

  uint16_t data[len];  // Array to store the read data
  read_file(file_one_id,data,len);

  Serial.println("Array read from EEPROM:");
  for (int i = 0; i < len; i++) {
    Serial.println(data[i]);
  }

  delet_file(file_one_id);

  Serial.println("deleted file FAT:");
  for (int e = 0; e < n_bloks; e++) {
    Serial.println(fat[e]);
  }

}

void loop() {
  // Empty loop
}

void writeRecursively(uint16_t memAddr, uint16_t* arr, int len) {
  if (len == 0) {
    return;
  }
  int pageOffset = memAddr % 128;
  int bytesToWrite = min(128 - pageOffset, len * 2);

  Wire.beginTransmission(EEPROM_ADDRESS);
  // Send the memory address (two bytes: high byte and low byte)
  Wire.write((memAddr >> 8) & 0xFF);  // High byte
  Wire.write(memAddr & 0xFF);         // Low byte

  for (int i = 0; i < bytesToWrite / 2; i++) {
    // Send the number (two bytes: high byte and low byte)
    Wire.write((arr[i] >> 8) & 0xFF);   // High byte
    Wire.write(arr[i] & 0xFF);          // Low byte
  }

  Wire.endTransmission();
  delay(10);  // Wait for EEPROM to complete the write process

  // Recursively write the remaining data
  writeRecursively(memAddr + bytesToWrite, arr + (bytesToWrite / 2), len - (bytesToWrite / 2));
}

void readSequential(uint16_t startAddr, uint16_t* out, int len) {
  Wire.beginTransmission(EEPROM_ADDRESS);

  // Send the start memory address (two bytes: high byte and low byte)
  Wire.write((startAddr >> 8) & 0xFF);  // High byte
  Wire.write(startAddr & 0xFF);         // Low byte

  Wire.endTransmission();

  // Request the number of bytes to read (2 bytes per uint16_t)
  Wire.requestFrom(EEPROM_ADDRESS, len * 2);

  for (int i = 0; i < len; i++) {
    if (Wire.available() >= 2) {  // Ensure 2 bytes are available
      uint16_t number = (Wire.read() << 8);  // Read high byte
      number |= Wire.read();                 // Read low byte
      out[i] = number;
    }
  }
}

void writeNumber(uint16_t memAddr, uint16_t number) {
  Wire.beginTransmission(EEPROM_ADDRESS);

  // Send the memory address (two bytes: high byte and low byte)
  Wire.write((memAddr >> 8) & 0xFF);  // High byte
  Wire.write(memAddr & 0xFF);         // Low byte

  // Send the number (two bytes: high byte and low byte)
  Wire.write((number >> 8) & 0xFF);   // High byte
  Wire.write(number & 0xFF);          // Low byte

  Wire.endTransmission();
  delay(10);  // Wait for EEPROM to complete the write process
}

uint16_t readNumber(uint16_t memAddr) {
  Wire.beginTransmission(EEPROM_ADDRESS);

  // Send the memory address (two bytes: high byte and low byte)
  Wire.write((memAddr >> 8) & 0xFF);  // High byte
  Wire.write(memAddr & 0xFF);         // Low byte
  Wire.endTransmission();

  // Request 2 bytes of data from EEPROM
  Wire.requestFrom(EEPROM_ADDRESS, 2);

  // Read the 2 bytes of data and combine them into the original number
  uint16_t number = 0;
  if (Wire.available() >= 2) {
    number = (Wire.read() << 8);  // Read high byte
    number |= Wire.read();        // Read low byte
  }

  return number;
}

int block_aloc(){
  for(int i=0; i < n_bloks; i++){
    if (fat[i]==-1){
      fat[i]=0;
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

uint8_t write_file(uint16_t* data, int size) {
  int8_t blocks_needed = (size + block_size - 1) / block_size;  // Number of blocks required
  int prev_block = -1;
  int block = block_aloc();  // Allocate the first block
  int id = block;            // File ID is the first block
  int offset = 0;            // Offset to track the remaining data to write

  if (block == -1) {
    Serial.println("No more free blocks!");
    return -1;  // No free blocks available
  }

  for (int i = 0; i < blocks_needed; i++) {
    // Calculate the EEPROM address for this block
    uint16_t addr = block * block_size;

    // Check if there's still data to write before proceeding
    if (block == -1) {
      Serial.println("No more free blocks!");
      return -1;  // No more free blocks available
    }

    // Calculate how many bytes to write in this block
    int bytes_to_write = min(block_size, size - offset);  // Only write the remaining bytes

    // Perform the write operation
    writeRecursively(addr, data + offset, bytes_to_write);

    // Link the previous block to this one (if necessary)
    if (prev_block != -1) {
      fat[prev_block] = block;
    }

    prev_block = block;
    
    // Update the offset for the next block
    offset += bytes_to_write;

    // Only allocate a new block if there's still data left to write
    if (offset < size) {
      block = block_aloc();  // Allocate the next block
    } else {
      block = -1;  // No more blocks needed; mark as finished
    }
  }

  // Mark the end of the file in the FAT
  fat[prev_block] = -2;  // End of the file

  return id;  // Return the file ID
}




void read_file(int id,uint16_t* data,int size){

  int block=id;
  int index =0;

  while(block != -2 && index<size){
    
    uint16_t addr =block*block_size;

    for (int i = 0; i < block_size && index < size; i++){
      data[index++] = readNumber(addr+i);
    }
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
  block_free(block);
}


