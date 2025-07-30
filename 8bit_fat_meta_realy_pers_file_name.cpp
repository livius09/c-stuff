#include <Wire.h>
#include <avr/wdt.h>
#include <string.h>
#include <math.h>


// EEPROM I2C address with A0 = 1 (A1 and A2 = 0)
#define EEPROM_ADDRESS 0x51  // Base address is 0x50, A0=1 adds 1


const int8_t  n_bloks = 40;
const uint8_t max_files = 5;
const uint8_t block_size = 10; //the size of each block (min 3)
const uint8_t file_name_size = 5;
const uint8_t boot_blocks = ceil((double)(n_bloks+2)/(double)(block_size));
const uint8_t boot_byte_size = (boot_blocks*block_size);
const uint8_t file_blocks = ceil((double)(max_files*(file_name_size+1) + 1)/(double)(block_size));  //the blocks needed for storing the files struct arr  composed of:  (filename+ (1)id ) * arrsize + (1)storedfiles
const uint8_t init_sect = file_blocks + boot_blocks;


uint8_t stored_files = 0;
int8_t fat[n_bloks];//lets you look up if a block is in use


//meanings in the fat:
//-1 is free
//-2 end of file
//-3 is alocated but point to no file
// >=0: used and links to the next block

struct fil{
  char name[file_name_size]; //so 4 charactes and 1 \n
  uint8_t id;
};
  
fil files[max_files];


void setup() {

  Serial.end();
  Serial.begin(9600);

  delay(500);
  Wire.end();
  Wire.begin();
  Wire.setClock(100000); // Try lowering to 100 kHz or even 50 kHz

  bool start = false;

   

  read_test(start);

  Wire.end();
  Serial.end();
}

void loop() {
}

void read_test(bool con){
  uint8_t file_id;
  if (con) {
    writeNumber(0,(n_bloks >> 8) & 0xFF);   // High byte
    writeNumber(1,n_bloks & 0xFF);        // Low byte

    for (int i = 0; i < init_sect; i++) {
      set_fat(i, -3);  // Initialize FAT: -3 means the block is in use in this case the boot sector
    }


    for (int i = boot_blocks; i < init_sect; i++) {
      writeNumber(i + boot_byte_size, 0);   //zeros the file name and id sector
    }


    for (int i = init_sect; i < n_bloks; i++) {
      set_fat(i, -1);  // Initialize FAT: -1 means the block is free
    }


    uint8_t tt[] = {50,52,54,56,58,60,62,64, 66, 68, 70, 72};
    file_id = write_file_by_name("ha", tt, 12);
  }else {
    
    init_fat();
  }

  Serial.print("Boot blocks: ");
  Serial.println(boot_blocks);

  Serial.print("file blocks: ");
  Serial.println(file_blocks);

  Serial.print("file ID: ");
  Serial.println(file_id);

  Serial.println("read file size:");
  Serial.println(get_file_size(file_id));

  uint8_t re[12];
  uint8_t lig[60];
  readSequential(0, lig, 60);
  read_file_by_name("ha",re,12);


  Serial.println("mem read:");
  for (int i = 0; i < 60; i++) {
    Serial.println(lig[i]);
  }

  Serial.println("file read:");
  for (int i = 0; i < 12; i++) {
    Serial.println(re[i]);
  }
  
  Serial.println("fat:");
  for (int i = 0; i < 20; i++) {
    Serial.println(fat[i]);
  }

  Serial.println("strukt arr:");
  for (int i = 0; i < max_files; i++) {
    Serial.println(files[i].name);
    Serial.println(files[i].id);
  }

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
      set_fat(i,-3); //set as alocated
      Serial.println("alocated block:");
      Serial.println(i);
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
      set_fat(ra,-3);  // Mark the block as allocated
      return ra;  // Return the random block
    }
  }

  // If the random search didn't work, check every block
  for (int i = 0; i < n_bloks; i++) {
    if (fat[i] == -1) {
      set_fat(i,-3);  // Mark the block as allocated
      
      return i;  // Return the block
    }
  }

  return -1;  // No free block found
}


int block_free(int id){
  if (id < n_bloks){
    set_fat(id,-1);
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

  Serial.println("start block adr:");
  Serial.println(addr);
  
  

  writeNumber(addr,(size >> 8) & 0xFF);   // High byte
  writeNumber(addr+1,size & 0xFF);        // Low byte

  uint16_t len = min(size,block_size-2);

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
    set_fat(prev_block,block);
  }

  

  while(size > 0){
    // Calculate the EEPROM address for this block
    addr = block * block_size;

    // Check if there's still data to write before proceeding
    if (block == -1) {
      Serial.println("No more free blocks!");
      return -1;  // No more free blocks available
    }

    len = min(size,block_size);

    Serial.println("folowing block adr:");
    Serial.println(addr);

    // Perform the write operation
    write_page(addr, data, len);

    data+=len;
    size-=len;

    //do you suffer from ligma ?

    // Link the previous block to this one (if necessary)
    if (prev_block != -1) {
      set_fat(prev_block,block);
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
  set_fat(prev_block,-2);  // End of the file
  

  return id;  // Return the file ID
}




void read_file(int id, uint8_t* data,int size){

  int block=id;
  uint16_t addr = block*block_size;

  uint16_t read_len = (readNumber(addr)<< 8);  // Read high byte
  read_len |= readNumber(addr+1);    // Read low byte

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
void init_fat(){
  uint16_t read_len = (readNumber(0)<< 8);  // Read high byte
  read_len |= readNumber(1);    // Read low byte

  uint16_t len = min(n_bloks, read_len);  //either the read_len or the n_bloks cause one block is a byte
  readSequential(2, fat, len);

  uint8_t read_stored_files = readNumber(boot_byte_size);
  stored_files = read_stored_files;
  
  uint16_t addr = boot_byte_size +1;

  for (int i = 0; i < read_stored_files; i++){
    //addr= addr+(file_name_size+1)*i;
    readSequential(addr, files[i].name, file_name_size);
    addr+= file_name_size;
    files[i].id = readNumber(addr);
    addr+=1;
  }
}

void set_fat(int adr,int8_t num){
  if (adr < 0 || adr >= n_bloks){
    Serial.println("array out of bounds");
    return;
  }
  fat[adr]=num;

  writeNumber(adr+2, num);
}

uint8_t write_file_by_name(char name[], uint8_t data[], uint16_t data_len){
  for (int i = 0; i < stored_files; i++) {
    if (strcmp(name, files[i].name)==0) {
      Serial.println("File whit that name already exists");
      return;
    }
  }
  
  uint8_t name_len = strlen(name)+1;

  if(name_len > file_name_size){
    Serial.println("File Name to long");
    return;
  }

  if (stored_files >= max_files) {
    Serial.println("Maximum file count reached.");
    return;
  }


  uint8_t file_id = write_file(data, data_len);

  strcpy(files[stored_files].name, name);
  files[stored_files].id=file_id;

  uint16_t addr = boot_byte_size + 1 + ((file_name_size + 1) * stored_files);

  

  write_page(addr, files[stored_files].name, file_name_size);
  writeNumber(addr+file_name_size, files[stored_files].id);

  stored_files++;
  writeNumber(boot_byte_size, stored_files);

  return file_id;
}

void read_file_by_name(char name[], uint8_t data[], uint16_t size){
  int8_t found = -1;

  for (int i = 0; i < stored_files; i++) {
    if (strcmp(name, files[i].name)==0) {
      found=i;
      break;
    }
  }
  if(found==-1){
    Serial.println("File not found");
    return;
  }

  uint8_t file_id = files[found].id;


  read_file(file_id, data, size);
}



