#include <fs/fs.h>

#ifndef OP_HANDLER_H
#define OP_HANDLER_H

typedef struct
{
	msg_t	**msg;
	void (*run)(msg_t msg);
} op_handler_t;

void leaky_free(char ***p);

void fs_run(msg_t *msg);

void op_main(op_handler_t *handler);

#endif
