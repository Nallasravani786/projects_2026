# Cache-Optimized Numerical Computation Engine

A C++ project implementing and benchmarking naive vs. cache-optimized matrix
multiplication, plus a Gaussian elimination linear system solver.

## Files
- `matrix_engine.cpp` — full source (naive, loop-reordered, and blocked matrix
  multiplication; Gaussian elimination with partial pivoting)
- `Project1_Cache_Optimized_Numerical_Engine.pdf` — write-up with verified benchmark results

## Compile & Run
```bash
g++ -O2 -std=c++17 matrix_engine.cpp -o matrix_engine
./matrix_engine
```

## Verified Results (from an actual run)
| Matrix Size | Naive (ms) | Blocked (ms) | Speedup |
|---|---|---|---|
| 128×128 | 2.032 | 0.956 | 2.13x |
| 256×256 | 18.796 | 8.201 | 2.29x |
| 512×512 | 164.216 | 74.555 | 2.20x |

Correctness verified via checksum comparison between naive and blocked outputs.
Gaussian elimination solver verified against a known analytical solution.

## CV Bullet
Implemented naive, loop-reordered, and blocked (tiled) matrix multiplication
in C++, achieving a 2.1–2.3x speedup over the naive baseline by exploiting
CPU cache locality; verified correctness via checksum validation and built a
numerically stable Gaussian elimination solver with partial pivoting.
