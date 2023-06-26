#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

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

void network() {
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
	int epochs = 30;
	for(int l = 0; l < epochs; l++) {
		// mean square error
		Value** ys_pred = call_mlp(mlp, xs[0]);
		Value* loss = pw(sub(ys_pred[0], value(ys[0])), value(2));
		free(ys_pred);
		for(int i = 1; i < 4; i++) {
			ys_pred = call_mlp(mlp, xs[i]);
			loss = add(loss, pw(sub(ys_pred[0], value(ys[i])), value(2)));

			// only array of pointers to predicted values are freed
			free(ys_pred);
		}

		printf("loss%i: %f\n", l, loss->data);

		zero_grad(mlp);

		backward(loss);

		step_optimize(mlp, 0.05);
	}

	printf("y0: %f\n", call_mlp(mlp, xs[0])[0]->data);
	printf("y1: %f\n", call_mlp(mlp, xs[1])[0]->data);
	printf("y2: %f\n", call_mlp(mlp, xs[2])[0]->data);
	printf("y3: %f\n", call_mlp(mlp, xs[3])[0]->data);
}

typedef struct Images {
	int magic_number;
	int count;
	int rows;
	int cols;
	uint8_t* pixels;
} Images;

typedef struct Labels {
	int magic_number;
	int count;
	uint8_t* labels;
} Labels;

int reverse_int (int i)
{
    unsigned char c1, c2, c3, c4;

    c1 = i & 255;
    c2 = (i >> 8) & 255;
    c3 = (i >> 16) & 255;
    c4 = (i >> 24) & 255;

    return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
}

Images* read_images(char* path) {
	FILE* images_file = fopen(path, "rb");

	Images* images = (Images*)malloc(sizeof(Images));
	fread(images, sizeof(int), 4, images_file);
	// numbers are stored in high endian
	// for x86 it should be reverted to low endian
	images->magic_number = reverse_int(images->magic_number);
	images->count = reverse_int(images->count);
	images->rows = reverse_int(images->rows);
	images->cols = reverse_int(images->cols);

	int all_pixels = images->count * images->rows * images->cols;
	images->pixels = (uint8_t*)malloc(sizeof(uint8_t)*all_pixels);
	fread(images->pixels, sizeof(uint8_t), all_pixels, images_file);

	fclose(images_file);

	return images;
}

Labels* read_labels(char* path) {
	FILE* labels_file = fopen(path, "rb");

	Labels* labels = (Labels*)malloc(sizeof(Labels));
	fread(labels, sizeof(int), 2, labels_file);
	// numbers are stored in high endian
	// for x86 it should be reverted to low endian
	labels->magic_number = reverse_int(labels->magic_number);
	labels->count = reverse_int(labels->count);

	labels->labels = (uint8_t*)malloc(sizeof(uint8_t)*labels->count);
	fread(labels->labels, sizeof(uint8_t), labels->count, labels_file);

	fclose(labels_file);

	return labels;
}

Value* mse(Value** ys_pred, uint8_t label) {
	Value* minus_one = value(-1);
	Value* one = value(1);
	Value* two = value(2);

	Value* loss = pw(sub(ys_pred[0], label==0 ? one : minus_one), two);
	for(int i = 1; i < 10; i++) {
		loss = add(loss, pw(sub(ys_pred[0], label==i ? one : minus_one), two));
	}
	return loss;
}

double* normalize_pixels(uint8_t* pixels) {
	double* result = (double*)malloc(sizeof(double)*28*28);
	for(int i = 0; i < 28*28; i++) {
			result[i] = pixels[i]/255.0;
	}
	return result;
}

void mnist() {
	Images* train_images = read_images("mnist_images/train_images");
	Labels* train_labels = read_labels("mnist_images/train_labels");

	int l[] = {train_images->rows*train_images->cols, 1000, 1000, 10};
	MLP* mlp = mlp_init(l, 4);

	int epochs = 50;
	int batch_size = 10;

	for(int epoch = 0; epoch < epochs; epoch++) {
		for(int batch = 0; batch < train_images->count / batch_size; batch++) {
			double* normalized_pixels = normalize_pixels(train_images->pixels+batch*batch_size + 0);
			Value** ys_pred = call_mlp(mlp, normalized_pixels);
			Value* loss = mse(ys_pred, train_labels->labels[batch*batch_size + 0]);
			free(ys_pred);
			free(normalized_pixels);
			for(int i = 1; i < batch_size; i++) {
				normalized_pixels = normalize_pixels(train_images->pixels+batch*batch_size + i);
				ys_pred = call_mlp(mlp, normalized_pixels);
				loss = add(loss, mse(ys_pred, train_labels->labels[batch*batch_size + i]));
				free(ys_pred);
				free(normalized_pixels);
			}

			printf("loss%i: %f\n", batch, loss->data);
			zero_grad(mlp);
			backward(loss);
			step_optimize(mlp, 0.1);
		}
	}

}


int main() {
	srand(time(NULL));

	//neuron();
	//network();
	mnist();

	return 0;
}
