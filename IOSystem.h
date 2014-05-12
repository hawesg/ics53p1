//============================================================================
// Name        : IOSystem.h
// Author      : Garrett Hawes (ghawes@uci.edu), George Mokbel (gmokbel@uci.edu) & Dennis Long Tran (dennisnt@uci.edu)
// Version     : 0.1
// Copyright   : Your copyright notice
// Description : First Project Lab
//============================================================================

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#ifndef IOSYSTEM_H_
#define IOSYSTEM_H_

class IOSystem{
  const static int NUMBER_OF_LOGICAL_BLOCKS;
  const static int MAX_BLOCK_LENGTH;
  const static std::string SAVE_FILE_NAME;
  char** ldisk;

 public:
  IOSystem();
  virtual ~IOSystem();
  void read_block(int i, char *p);
  void write_block(int i, char *p);
  void save();
  int restore();

  void save(const string&);
  int restore(const string&);
};

#endif /* IOSYSTEM_H_ */

