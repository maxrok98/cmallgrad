# cmallgrad
Trying to recreate [micrograd](https://github.com/karpathy/micrograd/) in C.

## Computational graph
```c
Value* a = value(2); a->name = "a";	
Value* b = value(-3); b->name = "b";	
Value* c = value(10); c->name = "c";	

Value* e = mul(a, b); e->name = "e";
Value* d = add(e, c); d->name = "d";

Value* f = value(-2); f->name = "f";
Value* L = mul(d, f); L->name = "L";
```
Translates to such graph.
```
  -8.000 * L
         ├─4.0000 + d
         │        ├─-6.000 * e
         │        │        ├─2.0000  a
         │        │        └─-3.000  b
         │        └─10.0000  c
         └─-2.000  f
 ```
