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

## Derivative
[Derivative](https://en.wikipedia.org/wiki/Derivative) - change of function with respect to its variable.
```
h -> 0
y = f(x)
dy/dx == (f(x+h) - f(x))/h
```
Gradient is a derivative on a given point.
### Numerical derivation
In graph example we can compute gradient for each node by increasing variable by h and recomputing graph. After we compare how new result changed with respect to old one.
```c
float h = 0.00001;

Value* a = value(2); a->name = "a";
Value* b = value(-3); b->name = "b";
Value* c = value(10+h); c->name = "c";

Value* e = mul(a, b); e->name = "e";
Value* d = add(e, c); d->name = "d";

Value* f = value(-2); f->name = "f";
Value* L = mul(d, f); L->name = "L";

float L2 = L->data;

dL/dc == (L2-L1)/h; // where L1 original value
```
### Analytic derivation (Chain rule)
Derivative of multiplication by a scalar equals scalar. y=2x, y'=2.
```
L = d * f
(f(x+h) - f(x))/h
((d+h)*f - d*f)/h
(df + fh - df)/h
dL/dd == f == -2
d->grad = -2

dL/df == d == 4
f->grad = 4
```
Computing dd/dc:
```
d = c + e
(c + h + e - c - e)/h
dd/dc == h/h == 1
```
Going deeper into graph [chain rule](https://en.wikipedia.org/wiki/Chain_rule) is used in order to compute derivative.
```
y = f(t), t = g(x), y = f(g(x))
dy/dx = dy/dt * dt/dx
```
Computing dL/dc by chain rule.
```
dL/dc == dL/dd * dd/dc
dL/dd == -2
dd/dc == 1
dL/dc == -2
c->grad = -2
```
