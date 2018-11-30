#ifndef INPUT_H
#define INPUT_H

struct input_func;

void input_init(void);

int input_register_func(char name[], int (*func)(int argc, char *argv[]));

int input_get_func(char name[], struct input_func **rtn);

int input_parse(int argc, char *argv[], struct input_func **rtn);

struct input_func* input_allocate_struct(void);

#endif
