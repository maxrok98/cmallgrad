#include <stdlib.h>
#include <stdio.h>

#include "engine.h"
#include "nn.h"

double randfrom(double min, double max) {
    double range = (max - min); 
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

Neuron* neuron_init(int nin) {
	Neuron* neuron = (Neuron*)malloc(sizeof(Neuron));
	neuron->w = (Value**)malloc(sizeof(Value*)*(nin-1));
	neuron->nw = nin;

	for(int i = 0; i < nin; i++) {
		neuron->w[i] = value(randfrom(-0.5, 0.5));
		neuron->w[i]->weight = true;
	}
	neuron->b = value(0);
	neuron->b->weight = true;

	return neuron;
}

Layer* layer_init(int nin, int nout) {
	Layer* layer = (Layer*)malloc(sizeof(Layer));
	layer->neurons = (Neuron**)malloc(sizeof(Neuron*)*nout);
	layer->n = nout;

	for(int i = 0; i < nout; i++) {
		layer->neurons[i] = neuron_init(nin);
	}
	
	return layer;
}

MLP* mlp_init(int layers[], int n) {
	MLP* mlp = (MLP*)malloc(sizeof(MLP));
	mlp->layers = (Layer**)malloc(sizeof(Layer*)*(n-1));
	mlp->n = n;

	for(int i = 0; i < n - 1; i++) {
		mlp->layers[i] = layer_init(layers[i], layers[i+1]);
	}

	return mlp;
}

Value* call_neuron(Neuron* neuron, Value* xs[]) {
	Value* prev = mul(neuron->w[0], xs[0]);
	for(int i = 1; i < neuron->nw; i++) {
		prev = add(prev, mul(neuron->w[i], xs[i]));
	}

	return tnh(add(prev, neuron->b));
}

Value** call_layer(Layer* layer, Value* xs[]) {
	Value** layer_results = (Value**)malloc(sizeof(Value*)*layer->n);
	for(int i = 0; i < layer->n; i++) {
		layer_results[i] = call_neuron(layer->neurons[i], xs);
	}
	
	return layer_results;
}

Value** call_mlp(MLP* mlp, double* xs) {
	int first_layer_n = mlp->layers[0]->n;
	Value** value_xs = (Value**)malloc(sizeof(Value*)*first_layer_n);
	for(int i = 0; i < first_layer_n; i++) {
		value_xs[i] = value(xs[i]);
	}

	Value** layer_results;
	for(int i = 0; i < mlp->n-1; i++) {
		layer_results = call_layer(mlp->layers[i], value_xs);
		free(value_xs);
		value_xs = layer_results;
	}

	return layer_results;
}

void zero_grad(MLP* mlp) {
	for(int i = 0; i < mlp->n-1; i++) {
		Layer* layer = mlp->layers[i];
		for(int j = 0; j < layer->n; j++) {
			Neuron* neuron = layer->neurons[j];
			for(int k = 0; k < neuron->nw; k++) {
				neuron->w[k]->grad = 0;
			}
			neuron->b->grad = 0;
		}
	}
}

void step_optimize(MLP* mlp, double step) {
	for(int i = 0; i < mlp->n-1; i++) {
		Layer* layer = mlp->layers[i];
		for(int j = 0; j < layer->n; j++) {
			Neuron* neuron = layer->neurons[j];
			for(int k = 0; k < neuron->nw; k++) {
				//printf("%f grad:%f|", neuron->w[k]->data, neuron->w[k]->grad);
				neuron->w[k]->data += step * (-neuron->w[k]->grad);
			}
			//printf("%f grad:%f|\n", neuron->b->data, neuron->b->grad);
			neuron->b->data += step * (-neuron->b->grad);
		}
	}
}
