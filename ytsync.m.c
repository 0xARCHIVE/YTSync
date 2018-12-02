#include "input.h"
#include "ytdl.h"

int main(int argc, char *argv[]) {
	ytdl_init();

	// remove first arguments (program name) and pass to input parser
	int _argc = argc - 1;
	char **_argv = &argv[1];
	input_parse(_argc,_argv);

	return 0;
}
