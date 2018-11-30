#ifndef INPUT_H
#define INPUT_H

void input_init(void);

int input_register_func(char name[], int (*func)(int argc, char *argv[]));

int input_parse(int argc, char *argv[]);

#endif
