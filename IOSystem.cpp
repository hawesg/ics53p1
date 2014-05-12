//============================================================================
// Name        : IOSystem.cpp
// Author      : Garrett Hawes (ghawes@uci.edu), George Mokbel (gmokbel@uci.edu) & Dennis Long Tran (dennisnt@uci.edu)
// Version     : 0.1
// Copyright   : Your copyright notice
// Description : First Project Lab
//============================================================================

#include "IOSystem.h"
#include "string.h"

using namespace std;

const int IOSystem::MAX_BLOCK_LENGTH = 64;
const int IOSystem::NUMBER_OF_LOGICAL_BLOCKS = 64;
const std::string IOSystem::SAVE_FILE_NAME = "IOSystem.bin";

IOSystem::IOSystem() {
  ldisk = new char*[NUMBER_OF_LOGICAL_BLOCKS];
  for (int i = 0; i < NUMBER_OF_LOGICAL_BLOCKS; ++i) {
    ldisk[i] = new char[MAX_BLOCK_LENGTH];
  }
}

IOSystem::~IOSystem() {
  for (int i = 0; i < NUMBER_OF_LOGICAL_BLOCKS; ++i) {
    delete[] ldisk[i];
  }
  delete[] ldisk;
  ldisk = 0;
}

void IOSystem::read_block(int i, char *p) {
  char* blockToRead = &ldisk[i][0];
  memcpy(p, blockToRead, MAX_BLOCK_LENGTH * sizeof(char));
}

void IOSystem::write_block(int i, char *p) {
  char* blockToWrite = &ldisk[i][0];
  memcpy(blockToWrite, p, MAX_BLOCK_LENGTH * sizeof(char));
}

void IOSystem::save() {
  ofstream saveFile;
  saveFile.open(SAVE_FILE_NAME.c_str(), ios::binary);
  if (saveFile.is_open()) {
    for (int i = 0; i < NUMBER_OF_LOGICAL_BLOCKS; ++i) {
      saveFile.write(ldisk[i], MAX_BLOCK_LENGTH);
    }
  }
  saveFile.close();
}

// return 0 for exists, 1 for initialized since it didn't exist.
int IOSystem::restore() {
  int exists = 1;
  ifstream saveFile;
  saveFile.open(SAVE_FILE_NAME.c_str(), ios::binary);
  if (saveFile.is_open()) {
    for (int i = 0; i < NUMBER_OF_LOGICAL_BLOCKS; ++i) {
      saveFile.read(ldisk[i], MAX_BLOCK_LENGTH);
    }
    exists = 0;
  } else {
    save(); // create the file if it doesn't already exist
  }
  saveFile.close();
  return exists;
}

