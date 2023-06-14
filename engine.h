#ifndef ENG_H_
#define ENG_H_

#include <stdbool.h>

typedef enum Op {
	// un op
	OP_NEG,
	UNARY_LAST,

  // bin op
	OP_ADD,
	OP_MIN,
	OP_MUL,
	OP_DIV,
	OP_LAST
} Op;

typedef struct Value {
	float data;
	float grad;
	struct Value* prev[2];
	Op op;

	//debug info
	char* name;
	bool leaf;
} Value;

Value* value(float val);
Value* add(Value* val1, Value* val2);
Value* min(Value* val1, Value* val2);
Value* mul(Value* val1, Value* val2);
Value* dv(Value* val1, Value* val2);
bool unary(Op op);
char* op_to_str(Op op);

#endif
