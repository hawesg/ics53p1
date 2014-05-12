//============================================================================
// Name        : FileSystem53.h
// Author      : Garrett Hawes (ghawes@uci.edu), George Mokbel (gmokbel@uci.edu) & Dennis Long Tran (dennisnt@uci.edu)
// Version     : 0.1
// Copyright   : Your copyright notice
// Description : First Project Lab
//============================================================================

#ifndef FILESYSTEM53_H_
#define FILESYSTEM53_H_
#include "IOSystem.h"
using namespace std;

class FileSystem53 {

 private:
  // Filesystem format parameters:
  const static int FILE_SIZE_FIELD = 1;
  const static int ARRAY_SIZE = 3;
  const static int DESCR_SIZE = FILE_SIZE_FIELD+ARRAY_SIZE;
  const static int MAX_FILE_NO = 14;
  const static int MAX_BLOCK_NO = 64;
  const static int MAX_FILE_NAME_LEN = 10;
  const static int MAX_OPEN_FILE = 3;
  const static int FILEIO_BUFFER_SIZE = 64;
  const static int _EOF= -1;
  const static int BLOCK_SIZE = 64;
  const static int RESERVED_BLOCKS = 7;
  const static int MAX_BLOCK_NO_FOR_D_TABLE=2;
  const static int DIRECTORY_OFT_INDEX=0;

  char** desc_table;
  char* OFT;
  IOSystem IO;

 public:
  FileSystem53(int l, int b, string storage);
  FileSystem53();
  virtual ~FileSystem53();
  void OpenFileTable();
  int find_oft();
  void deallocate_oft(int index);
  void format();
  char* read_descriptor(int no);
  void clear_descriptor(int no);
  void write_descriptor(int no, char* desc);
  int find_empty_descriptor();
  int find_empty_block();
  int fgetc(int index);
  int fputc(int c, int index);
  bool feof(int index);
  int search_dir(int index, string symbolic_file_name);
  void delete_dir(int index, int start_pos, int len);
  int create(string symbolic_file_name);
  int open_desc(int desc_no);
  int open(string symbolic_file_name);
  int read(int index, char* mem_area, int count);
  int write(int index, char value, int count);
  int lseek(int index, int pos);
  void close(int index);
  int deleteFile(string fileName);
  void directory();
  int restore();
  int restore(const string&);
  void save();
  void save(const string&);
  void diskdump(int start, int size);

 private:
  void sync_desc_table_to_io();
};

#endif /* FILESYSTEM53_H_ */
