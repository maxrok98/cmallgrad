#ifndef ENG_H_
#define ENG_H_

#include <stdbool.h>

typedef enum Op {
	// un op
	OP_NEG,
	OP_TANH,
	UNARY_LAST,

  // bin op
	OP_ADD,
	OP_MIN,
	OP_MUL,
	OP_DIV,
	OP_LAST
} Op;

typedef struct Value {
	double data;
	double grad;
	double local_derivative;
	struct Value* prev[2];
	Op op;

	//debug info
	char* name;
	bool leaf;
} Value;

bool unary(Op op);
Value* value(double val);
Value* add(Value* val1, Value* val2);
Value* min(Value* val1, Value* val2);
Value* mul(Value* val1, Value* val2);
Value* dv(Value* val1, Value* val2);
Value* tnh(Value* val1);

void backward(Value* val);

#endif
