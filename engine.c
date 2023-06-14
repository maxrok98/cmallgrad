#include <stdlib.h>

#include "engine.h"

bool unary(Op op) {
	return op < UNARY_LAST;
}

char* OpStr[OP_LAST] = {
	[OP_NEG] = "-",
	[OP_ADD] = "+",
	[OP_MIN] = "-",
	[OP_MUL] = "*",
	[OP_DIV] = "/"
};

char* op_to_str(Op op) {
	return OpStr[op];
}


Value* value(float val) {
	Value* v = (Value*)calloc(1, sizeof(Value));
	v->data = val;
	v->leaf = true;
	return v;
}


Value* add(Value* val1, Value* val2) {
	Value* out = value(val1->data + val2->data);
	out->prev[0] = val1;
	out->prev[1] = val2;
	out->op = OP_ADD;
	out->leaf = false;

	return out;
}

Value* min(Value* val1, Value* val2) {
	Value* out = value(val1->data - val2->data);
	out->prev[0] = val1;
	out->prev[1] = val2;
	out->op = OP_MIN;
	out->leaf = false;

	return out;
}

Value* mul(Value* val1, Value* val2) {
	Value* out = value(val1->data * val2->data);
	out->prev[0] = val1;
	out->prev[1] = val2;
	out->op = OP_MUL;
	out->leaf = false;

	return out;
}

Value* dv(Value* val1, Value* val2) {
	Value* out = value(val1->data / val2->data);
	out->prev[0] = val1;
	out->prev[1] = val2;
	out->op = OP_DIV;
	out->leaf = false;

	return out;
}
