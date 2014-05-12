#ifndef SHELL_H_
#define SHELL_H_
#include <cstdlib>
#include <string.h>
#include "FileSystem53.h"


class Shell
{
public:
	Shell();
	~Shell();

	void run();
    void run(const string&);

private:
	FileSystem53 fileSystem;

	bool running;
	string lastResult;
	ofstream resultFile;

	void readCommandFile(const string&);
	void writeResult(const string&);
	void writeLastResult();
	void parseCommand(const string&);

	void create(const string&);
	void deleteFile(const string&);
	void open(const string&);
	void close(int);
	void read(int, int);
	void write(int, char, int);
	void setPosition(int, int);
	void directory();
	void initializeDisk();
	void saveDisk();
	void loadDisk(const string&);

	

};

#endif

