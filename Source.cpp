#include "Shell.h"

#define unsigchar(x) (x<0)?x+256:x

int main(int argc, char* argv[])
{
	Shell shell;	
	// argc counts the number of command line parameters, by default
	// argv[0] is the name of this specific file, just a heads up
	// incase you don't know what this means.
	if (argc > 1)
		shell.run(argv[1]);
	else
		shell.run();

	cin.get();
	return 0;
}
