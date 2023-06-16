#include <stdlib.h>
#include <math.h>

#include "engine.h"

void tnh_derivative(Value* val1, Value* out);

void add_derivative(Value* val1, Value* val2, Value* out);
void min_derivative(Value* val1, Value* val2, Value* out);
void mul_derivative(Value* val1, Value* val2, Value* out);
void div_derivative(Value* val1, Value* val2, Value* out);

void (*unary_derivative[UNARY_LAST])(Value*, Value*) = {
	[OP_TANH] = tnh_derivative
};
void (*binary_derivative[OP_LAST])(Value*, Value*, Value*) = {
	[OP_ADD] = add_derivative,
	[OP_MIN] = min_derivative,
	[OP_MUL] = mul_derivative,
	[OP_DIV] = div_derivative
};

bool unary(Op op) {
	return op < UNARY_LAST;
}


Value* value(double val) {
	Value* v = (Value*)calloc(1, sizeof(Value));
	v->data = val;
	v->leaf = true;
	return v;
}

void set_unary_op(Value* val1, Value* out, Op op) {
	unary_derivative[op](val1, out);

	out->prev[0] = val1;

	out->op = op;
	out->leaf = false;
}

void set_binary_op(Value* val1, Value* val2, Value* out, Op op) {
	binary_derivative[op](val1, val2, out);

	out->prev[0] = val1;
	out->prev[1] = val2;

	out->op = op;
	out->leaf = false;
}

Value* add(Value* val1, Value* val2) {
	Value* out = value(val1->data + val2->data);
	set_binary_op(val1, val2, out, OP_ADD);

	return out;
}

void add_derivative(Value* val1, Value* val2, Value* out) {
	val1->local_derivative = 1;
	val2->local_derivative = 1;
}

Value* min(Value* val1, Value* val2) {
	Value* out = value(val1->data - val2->data);
	set_binary_op(val1, val2, out, OP_MIN);

	return out;
}

void min_derivative(Value* val1, Value* val2, Value* out) {
	val1->local_derivative = 1;
	val2->local_derivative = 1;
}

Value* mul(Value* val1, Value* val2) {
	Value* out = value(val1->data * val2->data);
	set_binary_op(val1, val2, out, OP_MUL);

	return out;
}

void mul_derivative(Value* val1, Value* val2, Value* out) {
	val1->local_derivative = val2->data;
	val2->local_derivative = val1->data;
}

Value* dv(Value* val1, Value* val2) {
	Value* out = value(val1->data / val2->data);
	set_binary_op(val1, val2, out, OP_DIV);

	return out;
}

void div_derivative(Value* val1, Value* val2, Value* out) {
	// not implemented
}

Value* tnh(Value* val1) {
	Value* out = value(tanh(val1->data));
	set_unary_op(val1, out, OP_TANH);

	return out;
}

void tnh_derivative(Value* val1, Value* out) {
	val1->local_derivative = (1 - pow(out->data,2));
}

void _backward(Value* val, double out_grad) {
	val->grad += val->local_derivative * out_grad;

	if(val->prev[0]) {
		if(unary(val->op)) {
			_backward(val->prev[0], val->grad);
		}
		else {
			_backward(val->prev[0], val->grad);
			_backward(val->prev[1], val->grad);
		}
	}
}
void backward(Value* val) {
	val->grad = 1;
	if(unary(val->op)) {
		_backward(val->prev[0], 1);
	}
	else {
		_backward(val->prev[0], 1);
		_backward(val->prev[1], 1);
	}
}
