CC=g++
CFLAGS=-c
OBJS = IOSystem.o FileSystem53.o Shell.o Source.o 

Shell : $(OBJS)
	$(CC) $(OBJS) -o Shell

Source.o :Shell.h Source.cpp FileSystem53.h IOSystem.h
	$(CC) $(CFLAGS) Source.cpp

Shell.o : Shell.h Shell.cpp FileSystem53.h IOSystem.h
	$(CC) $(CFLAGS) Shell.cpp

FileSystem53.o : FileSystem53.h FileSystem53.cpp IOSystem.h
	$(CC) $(CFLAGS) FileSystem53.cpp

IOSystem.o : IOSystem.h IOSystem.cpp
	$(CC) $(CFLAGS) IOSystem.cpp

clean:
	\rm *.o *~ Shell
