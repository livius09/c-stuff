#include <Wire.h>
#include <string.h>

// EEPROM I2C address with A0 = 1 (A1 and A2 = 0, 0)
#define EEPROM_ADDRESS 0x51  // Base address is 0x50, A0=1 adds 1

// EEPROM memory address to write to (choose an address within the 64KB range)
#define EEPROM_MEM_ADDR 0x0000

const int8_t n_bloks = 40;
const uint8_t block_size = 5;//the size of each block
const uint8_t min_boot_size = 20; //how many bytes the boot sector should have 
const uint8_t boot_blocks = ceil((double)(min_boot_size)/(double)(block_size)); //blocks nedded for boot sector
const uint8_t max_files=10;
uint8_t boot_left= min_boot_size; //how many bites are remaining decreased by every file write
uint8_t stored_files=0;
int8_t fat[n_bloks];//lets you look up if a block is in use

struct fil{
  char name[10];
  uint8_t id;
};
  
fil files[max_files];


//meanings in the fat:
//-1 is free
//-2 end of file
//-3 is alocated but dosnt point to no file
// >=0: used and links to the next block


void setup() {
  Serial.begin(9600);
  for (int i = 0; i < boot_blocks; i++) {
        fat[i] = -3;  // Initialize FAT: -3 means the block is in use in this case the boot sector
  }

  for (int i = boot_blocks; i < n_bloks; i++) {
        fat[i] = -1;  // Initialize FAT: -1 means the block is free
  }

  pinMode(2, OUTPUT);
  Wire.begin();

  

  bool wp = 0;
  
  // Set write protect pin
  digitalWrite(2, wp ? HIGH : LOW);

  uint8_t tt[] = {1,2,3,4,5,6,7,8,9,10,11,12};
  uint8_t fid = write_file_by_name(tt, 12, "abc");
  uint8_t fid2 = write_file_by_name(tt, 6, "lig");
  Serial.println("read file size:");
  Serial.println(get_file_size(fid));

  uint8_t re[12];
  uint8_t re2[12];
  uint8_t lig[30];
  readSequential(0, lig, 30);
  read_file_by_name("abc", re);
  read_file_by_name("lig", re2);

  Serial.println("mem read:");
  for (int i = 0; i < 30; i++) {
    Serial.println(lig[i]);
  }

  Serial.println("file read:");
  for (int i = 0; i < 12; i++) {
    Serial.println(re[i]);
  }
  Serial.println("file 2 read:");
  for (int i = 0; i < 6; i++) {
    Serial.println(re2[i]);
  }
  Serial.println("fat:");
  for (int i = 0; i < 25; i++) {
    Serial.println(fat[i]);
  }
  Serial.println("Structs:");
  for (int i = 0; i < max_files; i++) {
    Serial.print(files[i].id);
    Serial.print(files[i].name);
    Serial.print("\n");
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




void read_file_by_id(int id, uint8_t* data, uint16_t size){

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
    int next_block = fat[block]; //get the following block before it get overwriten
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

void read_boot(){
  uint8_t tmp[min_boot_size];
  uint8_t *ptr = tmp;
  readSequential(0,tmp, min_boot_size);
  uint8_t ofs=0;

  while (true){
    if(ptr[1]!=":"){ //if its corupted stop
      break;
    }
    if(stored_files>=max_files){ //if its full stop
      break;
    }
    files[stored_files].id=ptr[0];
    ptr+=2;
    ofs+=2;
    uint8_t i = 0;
    char c;
    while(i<10){
      c=ptr[i];
      files[stored_files].name[i]=c;
      i++;
      if(c == '\n'){
        break;
      }
    }
    stored_files++;
    ptr+=i;
    ofs+=i;
  }
  boot_left=ofs;
}

uint8_t write_file_by_name(uint8_t data[], int data_len, char name[]){
  for (int i = 0; i < stored_files; i++) {
    if (strcmp(name, files[i].name)==0) {
      Serial.println("File whit that name already exists");
      return;
    }
  }
  
  uint8_t name_len = strlen(name);

  if(name_len>10){
    Serial.println("File Name to long");
    return;
  }

  uint8_t addr = min_boot_size - boot_left;

  uint8_t file_id = write_file(data, data_len);

  writeNumber(addr, file_id);
  writeNumber(addr + 1, ':');
  write_page(addr+2, name, name_len);
  writeNumber(addr + name_len +2, '\n');

  boot_left-=(3+name_len);

  files[stored_files].id=file_id;
  strcpy(files[stored_files].name, name);
  stored_files++;
  return file_id;
}

void read_file_by_name(char name[],uint8_t data[]){
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

  uint16_t file_size = get_file_size(file_id);

  read_file_by_id(file_id, data, file_size);
}
