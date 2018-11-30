#ifndef INPUT_H
#define INPUT_H

int input_register_func(char name[], int (*func)(int argc, char *argv[]));

int input_deregister_func(char name[]);

int input_parse(int argc, char *argv[]);

#endif
