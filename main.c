#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "engine.h"
#include "nn.h"
#include "utils.h"

void neuron() {
	Value* x1 = value(2); x1->name = "x1";
	Value* x2 = value(0); x2->name = "x2";

	Value* y1 = value(1); y1->name = "y1";

	Value* w1 = value(-3); w1->name = "w1";
	Value* w2 = value(1); w2->name = "w2";

	Value* b = value(6.8813735870); b->name = "b";

	Value* x1w1 = mul(x1, w1); x1w1->name = "x1w1";
	Value* x2w2 = mul(x2, w2); x2w2->name = "x2w2";

	Value* x1w1x2w2 = add(x1w1, x2w2); x1w1x2w2->name = "x1w1x2w2";

	Value* n = add(x1w1x2w2, b); n->name = "n";

	Value* o = tnh(n);

	Value* loss = pw(sub(o, y1), value(2));

	bool pipes[1] = {false};
	draw(loss, 0, false, pipes);

	backward(loss);

	draw(loss, 0, false, pipes);
}

void equation() {
	Value* a = value(2); a->name = "a";	
	Value* b = value(-3); b->name = "b";	
	Value* c = value(10); c->name = "c";	

	Value* e = mul(a, b); e->name = "e";
	Value* d = add(e, c); d->name = "d";

	Value* f = value(-2); f->name = "f";
	Value* L = mul(d, f); L->name = "L";

	double L1 = L->data;

	bool pipes[1] = {false};
	draw(L, 0, false, pipes);

	{
		double h = 0.00001;

		Value* a = value(2); a->name = "a";	
		Value* b = value(-3); b->name = "b";	
		Value* c = value(10); c->name = "c";	

		Value* e = mul(a, b); e->name = "e";
		Value* d = add(e, c); d->name = "d";

		Value* f = value(-2+h); f->name = "f";
		Value* L = mul(d, f); L->name = "L";

		float L2 = L->data;

		printf("%f\n", (L2-L1)/h);
	}
}


int main() {
	srand(time(NULL));

	//neuron();

	double xs[4][3] = {
		{2, 3, -1},
		{3, -1, 0.5},
		{0.5, 1, 1},
		{1, 1, -1}
	};

	double ys[] = {1, -1, -1, 1};

	int l[] = {3, 4, 4, 1};
	MLP* mlp = mlp_init(l, 4);

	// loop
	int epochs = 100;
	for(int l = 0; l < epochs; l++) {
		zero_grad(mlp);

		// mean square error
		Value* loss = pw(sub(call_mlp(mlp, xs[0])[0], value(ys[0])), value(2));
		for(int i = 1; i < 4; i++) {
			loss = add(loss, pw(sub(call_mlp(mlp, xs[i])[0], value(ys[i])), value(2)));
		}

		printf("loss%i: %f\n", l, loss->data);

		//printf("y0: %f\n", call_mlp(mlp, xs[0])[0]->data);
		//printf("y1: %f\n", call_mlp(mlp, xs[1])[0]->data);
		//printf("y2: %f\n", call_mlp(mlp, xs[2])[0]->data);
		//printf("y3: %f\n", call_mlp(mlp, xs[3])[0]->data);

		backward(loss);

		step_optimize(mlp, 0.05);

		// TODO: free graph
	}
	printf("y0: %f\n", call_mlp(mlp, xs[0])[0]->data);
	printf("y1: %f\n", call_mlp(mlp, xs[1])[0]->data);
	printf("y2: %f\n", call_mlp(mlp, xs[2])[0]->data);
	printf("y3: %f\n", call_mlp(mlp, xs[3])[0]->data);

	return 0;
}
