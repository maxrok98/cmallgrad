#include <stdlib.h>
#include <math.h>

#include "engine.h"

void tnh_derivative(Value* val1, Value* out);

void add_derivative(Value* val1, Value* val2, Value* out);
void sub_derivative(Value* val1, Value* val2, Value* out);
void mul_derivative(Value* val1, Value* val2, Value* out);
void div_derivative(Value* val1, Value* val2, Value* out);
void pw_derivative(Value* val1, Value* val2, Value* out);

void (*unary_derivative[UNARY_LAST])(Value*, Value*) = {
	[OP_TANH] = tnh_derivative
};
void (*binary_derivative[OP_LAST])(Value*, Value*, Value*) = {
	[OP_ADD] = add_derivative,
	[OP_MUL] = mul_derivative,
	[OP_POW] = pw_derivative
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
	out->prev[0] = val1;

	out->op = op;
	out->leaf = false;
}

void set_binary_op(Value* val1, Value* val2, Value* out, Op op) {
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
	val1->grad += 1 * out->grad;
	val2->grad += 1 * out->grad;
}

Value* mul(Value* val1, Value* val2) {
	Value* out = value(val1->data * val2->data);
	set_binary_op(val1, val2, out, OP_MUL);

	return out;
}

void mul_derivative(Value* val1, Value* val2, Value* out) {
	val1->grad += val2->data * out->grad;
	val2->grad += val1->data * out->grad;
}

// tanh
Value* tnh(Value* val1) {
	Value* out = value(tanh(val1->data));
	set_unary_op(val1, out, OP_TANH);

	return out;
}

void tnh_derivative(Value* val1, Value* out) {
	val1->grad += (1 - pow(out->data,2)) * out->grad;
}

// pow
Value* pw(Value* val1, Value* val2) {
	Value* out = value(pow(val1->data, val2->data));
	set_binary_op(val1, val2, out, OP_POW);

	return out;
}

void pw_derivative(Value* val1, Value* val2, Value* out) {
	val1->grad += val2->data * pow(val1->data, (val2->data - 1)) * out->grad;
}

// following operation described in terms of higher ones
// hence they do not need derivative func

Value* neg(Value* val1) {
	return mul(val1, value(-1));
}

Value* sub(Value* val1, Value* val2) {
	return add(val1, neg(val2));
}

Value* dv(Value* val1, Value* val2) {
	return mul(val1, pw(val2, value(-1)));
}


ValueList* init_list() {
	ValueList* list = (ValueList*)malloc(sizeof(ValueList));
	list->array_size = 8;
	list->quantity = 0;
	
	list->values = (Value**)malloc(sizeof(ValueList*) * list->array_size);
	return list;
}

void add_value(ValueList* list, Value* value) {
	if(list->quantity == list->array_size) {
		list->array_size += list->array_size / 2;
		list->values = (Value**) realloc(list->values, sizeof(Value*) * list->array_size);
	}
	value->topo_number = list->quantity; // TODO: remove
	list->values[list->quantity] = value;
	list->quantity++;
}

ValueList* build_topo(Value* val) {
	ValueList* list = init_list();

	void _build_topo(Value* val, ValueList* list) {
		if(!val->visited) {
			val->visited = true;

			if(val->prev[0]) {
				if(unary(val->op)) {
					_build_topo(val->prev[0], list);
				}
				else {
					_build_topo(val->prev[0], list);
					_build_topo(val->prev[1], list);
				}
			}

			add_value(list, val);
		}
		
	}

	_build_topo(val, list);
	return list;
}

//  backward should set grad for each of its childrend
void _backward(Value* val) {
	// local_derivative can not be struct field because it can be different for each object
	// 
	if(val->prev[0]) {
		if(unary(val->op)) {
			unary_derivative[val->op](val->prev[0], val);
		}
		else {
			binary_derivative[val->op](val->prev[0], val->prev[1], val);
		}
	}
	
}

void backward(Value* val) {
	ValueList* topo_list = build_topo(val);

	// update gradient
	val->grad = 1;
	for(int i = topo_list->quantity-1; i >= 0; i--) {
		//if(topo_list->values[i]->out->grad == 0.0)
		//_backward(topo_list->values[i]);
		//else
		_backward(topo_list->values[i]);
		topo_list->values[i]->visited = false;
	}
	
	// free topo array
	free(topo_list->values);
	free(topo_list);
}
