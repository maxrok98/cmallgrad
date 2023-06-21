#ifndef NN_H_
#define NN_H_

#include "engine.h"

typedef struct Neuron {
	Value** w;
	Value* b;
	int nw;
} Neuron;

typedef struct Layer {
	Neuron** neurons;
	int n;
} Layer;

typedef struct MLP {
	Layer** layers;
	int n;
} MLP;

Neuron* neuron_init(int nin);
Layer* layer_init(int nin, int nout);
MLP* mlp_init(int layers[], int n);

// outputs array of values of last layer length
Value** call_mlp(MLP* mlp, double* xs);
void step_optimize(MLP* mlp, double step);
void zero_grad(MLP* mlp);

#endif
