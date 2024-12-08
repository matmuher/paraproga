# Boundary value problem

Problem statement (as all good problem statements it's in Russian):

<img src="./problem.png" width=60% height=auto>

To solve system using reduction method,
system should have size: 2^p + 1

Firsly, I linearize system using Newton's linearization.

Secondly, I make this system descrete: project continious functions to descrete grid
using Numerov's approximation

Thirdly, I solve this system using reduction method:

    a. Compute reduction coeffictients
    b. Invert reduction and compute 'y' values

Initial statistics:

```
        [TIME] total: 4.934359 ms
fill coefs: 57.7%
reduction: 9.4%
inverse: 30.3%
update: 2.5%
N = 32769, h = 0.000031
```

Tried paralleling with std::thread inverse part.
With N = 2<<17 there is benefit, but it's very small as bottleneck is "fill coefs" part