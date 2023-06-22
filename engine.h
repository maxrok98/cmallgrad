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
	OP_SUB,
	OP_MUL,
	OP_DIV,
	OP_POW,
	OP_LAST
} Op;

typedef struct Value {
	double data;
	double grad;

	Op op;

	struct Value* prev[2];

	bool weight;
	bool visited; // for topological order

	//debug info
	char* name;
	bool leaf;
	int topo_number;
} Value;

typedef struct ValueList {
	Value** values;
	int quantity;

	int array_size;
} ValueList;

bool unary(Op op);
Value* value(double val);
Value* add(Value* val1, Value* val2);
Value* sub(Value* val1, Value* val2);
Value* mul(Value* val1, Value* val2);
Value* dv(Value* val1, Value* val2);
Value* pw(Value* val1, Value* val2);
Value* tnh(Value* val1);
Value* neg(Value* val1);

void backward(Value* val);
void free_graph(Value* val);

#endif
