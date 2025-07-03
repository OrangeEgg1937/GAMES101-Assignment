# Assignment 5: Ray Tracing

## Task

### Renderer.cpp - Render()

### Triangle.hpp - rayTriangleIntersect()

The basic idea is to use the Möller–Trumbore algorithm to find the intersection of a ray and a triangle. The algorithm uses the cross product to find the intersection point and checks if it lies within the triangle.

#### Process

- Baricentric Interpolation
  - For a **point** `p` inside a triangle △ABC, can be expressed as:
    - `p = αA + βB + γC`, `α + β + γ = 1`
    - where `α`, `β`, and `γ` are the barycentric coordinates of the point with respect to the triangle vertices A, B, and C.
  - We got: `α = (1 - β - γ)`

- Ray Equation: `r(t) = o + td`
  - `r(t)` is the **point** (ray) at time `t`
  - `o` is the origin of the ray
  - `d` is the direction of the ray

- If intersection exists, we can express the intersection point as:
  - `r(t) = o + td = (1 - β - γ)A + βB + γC`
    - `A`, `B`, and `C` are the vertices of the triangle
    - `o` is the origin of the ray
    - `d` is the direction of the ray
    - Unknowns are `t`, `β`, and `γ`. ***<- we need to solve for these.***

- Rearranging gives us:  
  - `o + td = (1 - β - γ)A + βB + γC`
  - `o + td = A - βA - γA + βB + γC`
  - `o + td = A + β(B - A) + γ(C - A)`
  - `o - A = β(B - A) + γ(C - A) - td`
  - We got the following system of equations in x, y, z:
  - `T = βE0 + γE1 - td`
    - where `E0 = B - A`, `E1 = C - A`, and `T = o - A`

- We can express this as a matrix equation:

```shell
  | E0.x  E1.x  -d.x |   | β |   | T.x |
  | E0.y  E1.y  -d.y | * | γ | = | T.y |
  | E0.z  E1.z  -d.z |   | t |   | T.z |
```

- By applying Cramer's Rule, we can solve for `β`, `γ`, and `t`:

![Cramer's Rule](https://media.geeksforgeeks.org/wp-content/uploads/20230212131806/Cramer's-Rule.png)

```shell
          | E0.x  E1.x  -d.x |
  D = Det | E0.y  E1.y  -d.y |
          | E0.z  E1.z  -d.z |

  Det(β) = | T.x  E1.x  -d.x |
           | T.y  E1.y  -d.y |
           | T.z  E1.z  -d.z |

  Det(γ) = | E0.x  T.x  -d.x |
           | E0.y  T.y  -d.y |
           | E0.z  T.z  -d.z |

  Det(t) = | E0.x  E1.x  T.x |
           | E0.y  E1.y  T.y |
           | E0.z  E1.z  T.z |

  β = Det(β) / D
  γ = Det(γ) / D
  t = Det(t) / D  
```

## Result



## Reference

https://zhuanlan.zhihu.com/p/451582864

https://zhuanlan.zhihu.com/p/448575965

https://www.geeksforgeeks.org/maths/cramers-rule/
