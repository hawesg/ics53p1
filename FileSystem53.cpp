//============================================================================
// Name        : FileSystem53.cpp
// Author      : Garrett Hawes (ghawes@uci.edu), George Mokbel (gmokbel@uci.edu) & Dennis Long Tran (dennisnt@uci.edu)
// Version     : 0.1
// Copyright   : Your copyright notice
// Description : First Project Lab
//============================================================================

#include "FileSystem53.h"
#include <string.h>
#include <algorithm>

#define unsigchar(x) (x<0)?x+256:x

FileSystem53::FileSystem53(int l, int b, string storage) {
  desc_table = new char*[MAX_BLOCK_NO_FOR_D_TABLE];
  for (int i = 0; i < MAX_BLOCK_NO_FOR_D_TABLE; i++) {
    desc_table[i] = new char[BLOCK_SIZE];
  }
  format();
  OpenFileTable();
}

FileSystem53::FileSystem53() {
  desc_table = new char*[MAX_BLOCK_NO_FOR_D_TABLE];
  for (int i = 0; i < MAX_BLOCK_NO_FOR_D_TABLE; i++) {
    desc_table[i] = new char[BLOCK_SIZE];
  }
  format();
  OpenFileTable();
}

FileSystem53::~FileSystem53() {
  for (int i = 0; i < MAX_BLOCK_NO_FOR_D_TABLE; i++)
    delete[] desc_table[i];
  delete[] desc_table;
  delete[] OFT;
}

void FileSystem53::OpenFileTable() {
  OFT = new char[264]; //198 3 entries + directory file at 0
  memset(OFT, 0, 264 * sizeof(char));
  // OFT 0 is pre set for directory file, at this point the buffer will be all 0s, pos = 0 and desc_id = 0, all consistent with the directory file
}

// Return -1 for too many files open
int FileSystem53::find_oft() {
  for (int i = 1; i <= MAX_OPEN_FILE; i++) { // Start at 1 and
    char* oftEntry = &OFT[i * 66]; // i* 66 gives you base, add 65 for descriptor
    if (oftEntry[65] == 0) // if he's pointed 0, and isnt OFT 0 then this one is empty
      return i;
  }
  return -1;
}

void FileSystem53::deallocate_oft(int index) {
  char *OFTEntry;
  OFTEntry = &OFT[index * 66];
  memset(OFTEntry, 0, 66 * sizeof(char));
}

void FileSystem53::format() {
  for (int i = 0; i < MAX_BLOCK_NO_FOR_D_TABLE; i++) {
    memset(desc_table[i], 0, BLOCK_SIZE * sizeof(char));
  }
  for (int i = 0; i < RESERVED_BLOCKS; i++) {
    desc_table[0][i] = 1;
  }
  // next 3 lines are to reserve the first block for the directory file
  int firstBlockForDirectory = find_empty_block();
  desc_table[1][1] = firstBlockForDirectory;
  desc_table[0][firstBlockForDirectory] = 1;

  sync_desc_table_to_io();
}

char* FileSystem53::read_descriptor(int no) {
  char *descriptor;
  descriptor = &desc_table[1][no * 4]; // no*4 gives us the base index
  return descriptor;
}

/*
  Could also be if we want to return a copy
  char* descriptor = new char[4];
  memcpy(descriptor,&desc_table[1][no*4],4);
  return descriptor;
*/

void FileSystem53::clear_descriptor(int no) {
  char *descriptor = read_descriptor(no);
  descriptor[0]=0; // zero out size
  for(int i=1;i<4;i++) {
    int blockNo = descriptor[i]; // for each block in use this will be the current block no, unsined char because the compiler will complain otherwise
    if(blockNo != 0){
	  desc_table[0][blockNo]=0; // Set bitmap
	  descriptor[i]=0; // set entry to zero
	}
  }
  sync_desc_table_to_io(); // Copy bitmap and descriptors over to IO
}

void FileSystem53::write_descriptor(int no, char* desc) { // See above for comments
  char *descriptor = read_descriptor(no);
  // memcpy(descriptor, desc, 4);
  for (int i = 1; i < 4; i++) {
    int blockNo = descriptor[i];
    if (blockNo != 0)
      desc_table[0][blockNo] = 1;
  }
  sync_desc_table_to_io();
}

int FileSystem53::find_empty_descriptor() {
  for (int i = 1; i < MAX_FILE_NO; i++) {
    if (desc_table[1][i * 4 + 1] == 0) // check first block to see if it's in use
      return i;
  }
  return -1;
}

int FileSystem53::find_empty_block() {
  for (int i = 0; i < MAX_BLOCK_NO; i++) { // First 7 will be ignored because they are reserved so always 1
    if (desc_table[0][i] == 0)
      return i;
  }
  return -1;
}

int FileSystem53::restore() {
  int exists = IO.restore();
  // restore desc_table
  IO.read_block(0, desc_table[0]);
  IO.read_block(1, desc_table[1]);
  // setup OFT by zeroing out the existing one then opening the direcory (load first block into buffer
  memset(OFT, 0, 264 * sizeof(char));
  int firstBlockFromDirecotryDescriptor = desc_table[1][1]; 
  IO.read_block(firstBlockFromDirecotryDescriptor, OFT); // we want the first 64 bytes in OFT to be filled ie: the buffer for OFT 0 (directory file)
  return exists;
}

void FileSystem53::save() {
  sync_desc_table_to_io(); // Make sure it's up to date
  IO.save();
}

void FileSystem53::diskdump(int start, int size) {
  for (int i = start; i < start + size; i++) {
    char *buffer = new char[64];
    IO.read_block(i, buffer);
    for (int j = 0; j < 64; j++) {
      cout << buffer[j];
    }
    delete buffer;
  }
}

void FileSystem53::sync_desc_table_to_io() {
  IO.write_block(0, desc_table[0]);
  IO.write_block(1, desc_table[1]);
}

int FileSystem53::fgetc(int index) {
  char* oftEntry = &OFT[index * 66]; // pointer to the base for the entry in question
  int position = unsigchar(oftEntry[64]);
  int positionInBuffer = position % FILEIO_BUFFER_SIZE; // the remainder of position/64 is the position in the buffer
  if (feof(index))
    return _EOF;
  int charToGet = oftEntry[positionInBuffer];
  oftEntry[64] = ++position;
  return charToGet;
}

int FileSystem53::fputc(int c, int index) {
  char* oftEntry = &OFT[index * 66]; // pointer to the base for the entry in question
  int position = unsigchar(oftEntry[64]);
  if (position == 192)
    return -2;
  int positionInBuffer = position % FILEIO_BUFFER_SIZE; // the remainder of position/64 is the position in the buffer
  oftEntry[positionInBuffer] = c;
  oftEntry[64] = ++position;
  return c;
}

bool FileSystem53::feof(int index) {
  char* oftEntry = &OFT[index * 66]; // pointer to the base for the entry in question
  char* descriptor = read_descriptor(oftEntry[65]); // Descriptor is found at 65
  return (oftEntry[64] == descriptor[0]); // if pos = size we are at end of file
}

int FileSystem53::read(int index, char* mem_area, int count) {
  char* oftEntry = &OFT[index * 66]; // pointer to the base for the entry in question
  if (index != 0 && oftEntry[65] == 0)
    return -1;
  int currentDescriptorNo = oftEntry[65];
  char* descriptor = read_descriptor(currentDescriptorNo); // Descriptor is found at 65
  int fileSize = unsigchar(descriptor[0]);
  char* blocksInDescriptor = &descriptor[1]; // array of blocks in descriptor
  int currentPosition = unsigchar(oftEntry[64]);
  if (currentPosition == fileSize)
    return -2;
  int currentBlock = currentPosition / FILEIO_BUFFER_SIZE;
  int bytesRead = 0;
  int lastchar = 0;
  for (int i = 0; i < count; i++) {
    if (currentPosition % 64 == 0 && currentPosition != 0) {
      IO.write_block(blocksInDescriptor[currentBlock++], oftEntry);
      IO.read_block(blocksInDescriptor[currentBlock], oftEntry);
    }
    lastchar = fgetc(index);
    if (lastchar == _EOF) {
      return bytesRead;
    }
    mem_area[bytesRead] = lastchar;
    bytesRead++;
    currentPosition = unsigchar(oftEntry[64]);
  }
  return bytesRead;
}

int FileSystem53::search_dir(int index, string symbolic_file_name) {
  char* oftEntry = &OFT[index * 66]; // pointer to the base for the entry in question
  int lenghtOfSymbolicFileName = symbolic_file_name.length();
  lseek(0, 0);
  char* fileAtributes = new char[11];
  while (!feof(DIRECTORY_OFT_INDEX)) {
    int positionOfFile = unsigchar(oftEntry[64]);
    read(0, fileAtributes, MAX_FILE_NAME_LEN + 1);
    std::string currentFileName = std::string(fileAtributes,
        lenghtOfSymbolicFileName); // make a string out of read atributes that is the same length as the one your searching for
    if (symbolic_file_name.compare(currentFileName) == 0
        && (lenghtOfSymbolicFileName == 10
            || fileAtributes[lenghtOfSymbolicFileName] == 0)) { // check to see that they are the same, then check to make sure that the name we are searching is not a substring ie: foo.bar would match foo.barr otherwise
      delete fileAtributes;
      return positionOfFile;
    }
  }
  delete fileAtributes;
  return -1;
}

void FileSystem53::delete_dir(int index, int start_pos, int len) {
  lseek(0, start_pos);
  write(0, 0, len);
  // next two lines are to save the current block we are wrting into because we arn't suposed to close the directory file
  int currentBlockIndex = unsigchar(OFT[65])/64 +1; 
  IO.write_block(desc_table[1][currentBlockIndex], OFT);
}

int FileSystem53::create(string symbolic_file_name) {
  int descNo = find_empty_descriptor();
  int existingFileLocation = search_dir(0, symbolic_file_name);
  if (descNo == -1) // if no descriptor availible then we get -1
    return -1;
  if (existingFileLocation != -1) // if it's found then we will get a number no -1
    return -2;
  std::string emptyString = std::string("\0");
  int existingEmptySpot = search_dir(0, emptyString);
  int newFileLocation = (existingEmptySpot == -1) ? unsigchar(desc_table[1][0])
      : existingEmptySpot; // if there is an empty spot set it to that index otherwise set it size of directory file ie: append it
  lseek(0, newFileLocation);
  for (unsigned int i = 0; i < 10; i++) {
    if (i < symbolic_file_name.length())
      write(0, symbolic_file_name[i], 1);
    else
      write(0, '\0', 1); // if we have hit the total amount of chars in the filename then fill in the rest with 0s
  }
  write(0, descNo, 1);
  char* descriptor = read_descriptor(descNo);
  descriptor[1] = find_empty_block();
  write_descriptor(descNo, descriptor);
  int currentBlockIndex = unsigchar(OFT[65])/64 +1;
  IO.write_block(desc_table[1][currentBlockIndex], OFT); // write directory to IO
  return 0;
}

int FileSystem53::open_desc(int desc_no) {
  int oftIndex = find_oft();
  if (oftIndex == -1)
    return -1;
  char* oftEntry = &OFT[oftIndex * 66]; // pointer to the base for the entry in question
  char* descriptor = read_descriptor(desc_no);
  oftEntry[65]=desc_no;
  IO.read_block(descriptor[1],oftEntry);
  return oftIndex;
}

int FileSystem53::open(string symbolic_file_name) {
  int positionInFile = search_dir(0, symbolic_file_name); // get index of file or -1
  if (positionInFile == -1)
    return -1;
  char* fileAtributes = new char[11]; 
  lseek(0, positionInFile);
  read(0, fileAtributes, 11); // read in file name + descriptor no
  int fileDescriptor = fileAtributes[10];
  int oftIndex = open_desc(fileDescriptor); // use desc no to open it
  delete fileAtributes;
  return (oftIndex == -1) ? -2 : oftIndex;
}

int FileSystem53::write(int index, char value, int count) {
  char* oftEntry = &OFT[index * 66]; // pointer to the base for the entry in question
  if (index != 0 && oftEntry[65] == 0)
    return -1;
  int currentDescriptorNo = oftEntry[65];
  char* descriptor = read_descriptor(currentDescriptorNo); // Descriptor is found at 65
  char* blocksInDescriptor = &descriptor[1]; // array of blocks in descriptor
  int currentPosition = unsigchar(oftEntry[64]);
  int currentBlockPosition = currentPosition % FILEIO_BUFFER_SIZE;
  int currentBlock = currentPosition / FILEIO_BUFFER_SIZE;
  int bytesWriten = 0;
  for (int i = 0; i < count; i++) {
    if (currentPosition != 0 && currentBlockPosition == 0) { // check if we are at the first byte of a block if so (and it is the last byte of the file then alocate a new block
      IO.write_block(descriptor[currentBlock], oftEntry);
      if(blocksInDescriptor[currentBlock]==0)
        blocksInDescriptor[currentBlock] = find_empty_block();
      write_descriptor(currentDescriptorNo, descriptor);
      IO.read_block(blocksInDescriptor[currentBlock], oftEntry);
    }
    fputc(value, index);
    bytesWriten++;
    currentPosition = unsigchar(oftEntry[64]);
    if(currentPosition==192) // if we try to write too far then it returns bytes written and stops writing
      return bytesWriten;
    currentBlockPosition = currentPosition % FILEIO_BUFFER_SIZE;
    currentBlock = currentPosition / FILEIO_BUFFER_SIZE;
  }
  descriptor[0] = std::max(currentPosition, unsigchar(descriptor[0])); // descriptor[0] is file size, if we are at EOF then the possition will be the new size, otherwise we have used lsink and size doesnt change
  return bytesWriten;
}

int FileSystem53::lseek(int index, int pos) {
  char* oftEntry = &OFT[index * 66]; // pointer to the base for the entry in question
  if ((index != 0) && (oftEntry[65] == 0))
    return -1;
  char* descriptor = read_descriptor(oftEntry[65]); // Descriptor is found at 65
  char* blocksInDescriptor = &descriptor[1]; // array of blocks in descriptor
  pos = std::min(pos, unsigchar(descriptor[0])); 
  int currentPosition = unsigchar(oftEntry[64]);
  int currentBlock = currentPosition / FILEIO_BUFFER_SIZE;
  int newBlock = pos / FILEIO_BUFFER_SIZE;
  if (currentBlock != newBlock) {
    IO.write_block(blocksInDescriptor[currentBlock], oftEntry);
    IO.read_block(blocksInDescriptor[newBlock], oftEntry);
  }
  oftEntry[64] = pos;
  return 0;
}

void FileSystem53::close(int index) {
  char* oftEntry = &OFT[index * 66]; // pointer to the base for the entry in question
  char* descriptor = read_descriptor(oftEntry[65]); // Descriptor is found at 65
  char* blocksInDescriptor = &descriptor[1]; // array of blocks in descriptor
  int currentPosition = unsigchar(oftEntry[64]);
  int currentBlock = currentPosition / FILEIO_BUFFER_SIZE;
  IO.write_block(blocksInDescriptor[currentBlock], oftEntry);
  int currentSize = unsigchar(descriptor[0]);
  descriptor[0] = std::max(currentPosition, currentSize); // descriptor[0] is file size, if we are at EOF then the possition will be the new size, otherwise we have used lsink and size doesnt change
  memset(oftEntry, 0, 66 * sizeof(char)); // clear the oft entry
  write_descriptor(oftEntry[65], descriptor);
}

int FileSystem53::deleteFile(string fileName) {
  int postionOfFile = search_dir(0, fileName);
  if (postionOfFile == -1)
    return -1;
  lseek(0, postionOfFile);
  char* fileAttributes = new char[11];
  read(0, fileAttributes, 11);
  int descriptor = fileAttributes[10];
  clear_descriptor(descriptor);
  delete_dir(0, postionOfFile, 11);
  return 0;
}

void FileSystem53::directory() {
  lseek(0, 0);
  while (!feof(DIRECTORY_OFT_INDEX)) {
    char* fileAtributes = new char[11];
    read(0, fileAtributes, MAX_FILE_NAME_LEN + 1);
    int fileDescriptorBase = fileAtributes[10] * 4;
    int size = unsigchar(desc_table[1][fileDescriptorBase]);
    if (fileAtributes[0] != 0) {// Skip over this entry if is an empty one.
      std::string delimiter = (feof(DIRECTORY_OFT_INDEX)) ? "" : ", "; // , if there are remaining ones, . otherwise
      std::string fileName = std::string(fileAtributes, 10);
      printf("%s %d bytes%s", fileName.c_str(), size, delimiter.c_str());
    }
    delete fileAtributes;
  }
  cout<<endl;
}
// This whole function can be replaced with:
// char* fileAtributes=new char[11];
// while(!feof(oftIndex)){
//   read(oftIndex,fileAtributes,MAX_FILE_NAME_LEN+1);
//   printf("%s %d %s%s", std::string(fileAtributes,MAX_FILE_NAME_LEN).c_str(), filename[10], (fileAtributes[0]==0)?"":"bytes", (fileAtributes[0]==0 && feof(oftIndex))?"":", ");
// }

