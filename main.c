#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

#include "engine.h"
#include "utils.h"


int main() {
	//Value* x1 = value(2); x1->name = "x1";
	//Value* x2 = value(0); x2->name = "x2";

	//Value* w1 = value(-3); w1->name = "w1";
	//Value* w2 = value(1); w2->name = "w2";

	//Value* b = value(6.881373); b->name = "b";

	//Value* x1w1 = mul(x1, w1); x1w1->name = "x1w1";
	//Value* x2w2 = mul(x2, w2); x2w2->name = "x2w2";

	//Value* x1w1x2w2 = add(x1w1, x2w2); x1w1x2w2->name = "x1w1x2w2";

	//Value* n = add(x1w1x2w2, b); n->name = "n";

	//bool pipes[1] = {false};
	//draw(n, 0, false, pipes);
	
	Value* a = value(2); a->name = "a";	
	Value* b = value(-3); b->name = "b";	
	Value* c = value(10); c->name = "c";	

	Value* e = mul(a, b); e->name = "e";
	Value* d = add(e, c); d->name = "d";

	Value* f = value(-2); f->name = "f";
	Value* L = mul(d, f); L->name = "L";

	bool pipes[1] = {false};
	draw(L, 0, false, pipes);
	

	return 0;
}
