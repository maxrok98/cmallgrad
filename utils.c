#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "engine.h"

void draw(Value* val, int depth, bool first_parent, bool pipes[]) {
	for(int i = 0; i < depth; i++) {
		pipes[i]
			? printf("│        ")
			: printf("         ");
	}
	//if(depth > 1) printf("  ");
	char* sp = first_parent ? "├─" : "└─";
	char* pref = depth > 0 ? sp : "  ";
	char* op = val->leaf ? "" : op_to_str(val->op);
	char* name = val->name ? val->name : "";

	val->data < 0 ? printf("%s%.3f %s %s\n", pref, val->data, op, val->name)
		: printf("%s%.4f %s %s\n", pref, val->data, op, val->name);

	bool* new_pipes = (bool*)malloc(sizeof(bool)*(depth+2));
	memcpy(new_pipes, pipes, depth+1);

	if(!val->leaf) {
		if(unary(val->op)) {
			new_pipes[depth+1] = false;
			draw(val->prev[0], depth+1, false, new_pipes);
		}
		else {
			// add pipe
			new_pipes[depth+1] = true;
			draw(val->prev[0], depth+1, true, new_pipes);

			// remove pipe
			new_pipes[depth+1] = false;
			draw(val->prev[1], depth+1, false, new_pipes);
		}
	}
	free(new_pipes);
}