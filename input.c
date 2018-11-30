#include "input.h"

#include <stdlib.h>
#include <string.h>

#define INPUT_MAX_NAME_LENGTH 256
#define INPUT_MAX_NUM_FUNCS 10

static struct input_func* registered_funcs[INPUT_MAX_NUM_FUNCS] = {NULL};
static int init = 0;
static void input_init(void);
static void input_exit(void);
static int is_init(void);
static int input_get_func(char name[], struct input_func **rtn);
static struct input_func* input_allocate_struct(void);

static struct input_func {
	char name[INPUT_MAX_NAME_LENGTH];
	int (*execute)(int argc, char *argv[]);
};

static void input_init(void) {
	if (is_init()) { return; }
	atexit(input_exit);
	init = 1;
}

static void input_exit(void) {
	for (int i=0; i < INPUT_MAX_NUM_FUNCS; i++) {
		struct input_func *f = registered_funcs[i];
		if (f == NULL) { continue; }
		free(f);
		registered_funcs[i] = NULL;
	}

	init = 0;
}

static int is_init(void) {
	return init;
}

static int input_get_func(char name[], struct input_func **rtn) {
	if (!is_init()) { input_init(); }

	for (int i=0; i < INPUT_MAX_NUM_FUNCS; i++) {
		struct input_func *f = registered_funcs[i];
		if (f == NULL || f->name == NULL) { continue; }
		if (strncmp(name,f->name,INPUT_MAX_NAME_LENGTH) == 0) { *rtn = f; return 1; }
	}

	return 0;
}

static struct input_func* input_allocate_struct(void) {
	struct input_func *new_struct = malloc(sizeof(struct input_func));
	strcpy(new_struct->name,"");
	new_struct->execute = NULL;
	return new_struct;
}

int input_register_func(char name[], int (*func)(int argc, char *argv[])) {
	if (!is_init()) { input_init(); }

	for (int i=0; i < INPUT_MAX_NUM_FUNCS; i++) {
		struct input_func *f = registered_funcs[i];
		if (f == NULL) { f = input_allocate_struct(); registered_funcs[i] = f; }
		if (f->name == NULL) { continue; }
		strncpy(f->name,name,INPUT_MAX_NAME_LENGTH);
		f->execute = func;
		return 1;
	}

	return 0;
}

int input_deregister_func(char name[]) {
	if (!is_init()) { return 0; }

	for (int i=0; i < INPUT_MAX_NUM_FUNCS; i++) {
		struct input_func *f = registered_funcs[i];
		if (f == NULL || f->name == NULL) { continue; }
		if (strncmp(name,f->name,INPUT_MAX_NAME_LENGTH) == 0) {
			free(f);
			registered_funcs[i] = NULL;
			return 1;
		}
	}

	return 0;
}

int input_parse(int argc, char *argv[]) {
	if (!is_init()) { input_init(); }
	if (argc == 0) { return 0; }

	struct input_func *f;
	if (input_get_func(argv[0],&f)) {
		if (f->execute != NULL) { return f->execute(argc - 1, &argv[1]); }
	}

	return 0;
}
