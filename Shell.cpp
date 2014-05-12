#include "Shell.h"
#include <iostream>
#include <string.h>
#include <sstream>
#include <climits>
#define unsigchar(x) (x<0)?x+256:x

template<typename T> // this is borrowed
T from_string(std::string const & s) {
  std::stringstream ss(s);
  T result;
  ss >> result; // TODO handle errors
  return result;
}

Shell::Shell() :
  running(true), fileSystem() {

}

Shell::~Shell() {
}

void Shell::run() {
  while (running) {
    string input;
    cout << ":";
    getline(cin, input);
    parseCommand(input);
  }
}
void Shell::run(const string& commandFile) {
  readCommandFile(commandFile);
}

void Shell::parseCommand(const string& input) {
  bool isValidCommand = true;
  // first two chars in input stream are command type
  string commandType = input.substr(0, 2);

  if (commandType == "cl") {
    int oftIndex = atoi(&input[3]);
    close(oftIndex);
  } else if (commandType == "cr") {
    string fileName = input.substr(3);
    create(fileName);
  } else if (commandType == "de") {
    string fileName = input.substr(3);
    deleteFile(fileName);
  } else if (commandType == "dr") {
    directory();
  } else if (commandType == "in") {
    initializeDisk();
  } else if (commandType == "op") {
    string fileName = input.substr(3);
    open(fileName);
  } else if (commandType == "rd") {
    int oftIndex = atoi(&input[3]);
    int numberOfBytesToRead = from_string<int> (input.substr(4));
    read(oftIndex, numberOfBytesToRead);
  } else if (commandType == "sk") {
    int oftIndex = atoi(&input[3]);
    int pos = from_string<int> (input.substr(4));
    setPosition(oftIndex, pos);
  } else if (commandType == "sv") {
    saveDisk();
  } else if (commandType == "wr") {
    int oftIndex = atoi(&input[3]);
    char writeInput = input[5];
    int numberOfTimesToWrite = from_string<int> (input.substr(6));
    write(oftIndex, writeInput, numberOfTimesToWrite);
  } else if (commandType == "aa") {
    string fileName = input.substr(3);
    readCommandFile(fileName);
  } else if (commandType[0] == 'q') {
    running = false;
  } else {
    cout << "Invalid command press enter to continue";
    cin.ignore(INT_MAX, '\n');
    isValidCommand = false;
  }
}

void Shell::create(const string& file) {
  int result = fileSystem.create(file);
  if (result != 0) {
    cout << "error " << endl;
  } else {
    cout << "file " << file << " created" << endl;
  }
}

void Shell::deleteFile(const string& file)
{
  int success = fileSystem.deleteFile(file);
  if(success==-1)
  cout<<"error"<<endl;
  else
  cout << "file " << file << " deleted"<<endl;
}

void Shell::open(const string& file) {
  int openIndex = fileSystem.open(file);
  if (openIndex == -1)
    cout << "error" << endl;
  else
    cout << "file " << file << " opened, index = " << openIndex << endl;
}

void Shell::close(int oftIndex) {
  fileSystem.close(oftIndex);
  cout << "file with index " << oftIndex << " closed" << endl;
}

void Shell::read(int oftIndex, int count) {
  char* read = new char[count];
  int bytesRead = fileSystem.read(oftIndex, read, count);
  std::string readData = std::string(read, bytesRead);
  cout << bytesRead << " bytes read: " << readData << endl;

}

void Shell::write(int oftIndex, char input, int count) {
  int bytesWritten = fileSystem.write(oftIndex, input, count);
  cout << bytesWritten << " bytes written" << endl;
}

void Shell::setPosition(int oftIndex, int position) {
  int lseek = fileSystem.lseek(oftIndex, position);
  if (lseek == -1)
    cout << "error" << endl;
  else
    cout << "current position is " << position << endl;
}

void Shell::directory() {
  fileSystem.directory();
}

void Shell::initializeDisk() {
  int result = fileSystem.restore();
  if (result == 0)
    cout << "disk restored" << endl;
  else
    cout << "disk initialized " << endl;
}

void Shell::saveDisk() {
  fileSystem.save();
  cout << "disk saved" << endl;
}

// This is depricated as we did not need it in the project
void Shell::writeResult(const string& result) {
  ofstream resultFile;
  resultFile.open("result_file.txt", ios::app);
  if (resultFile.is_open())
    resultFile << result << endl;
  resultFile.close();
}

void Shell::readCommandFile(const string& file) {
  ifstream commandFile;
  commandFile.open(file.c_str());
  if (commandFile.is_open()) {
    string currentCommand;
    while (getline(commandFile, currentCommand)) {
      parseCommand(currentCommand);
      if (!commandFile)
        break;
    }
  }
  commandFile.close();
}

