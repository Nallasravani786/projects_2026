// Cache-Optimized Numerical Computation Engine
// Implements: Naive matrix multiplication, Blocked/Tiled matrix multiplication,
//             Naive Gaussian Elimination (LU-style solve) for linear systems.
// Course tie-ins: Computer Organization & Architecture (cache/memory hierarchy),
//                 Numerical Methods (Gaussian elimination), Applied Computational Methods
//
// Compile: g++ -O2 -std=c++17 matrix_engine.cpp -o matrix_engine
// Run:     ./matrix_engine

#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <iomanip>
#include <cmath>

using namespace std;
using namespace std::chrono;

typedef vector<vector<double>> Matrix;

// ---------- Utility ----------
Matrix randomMatrix(int n, int m, unsigned seed) {
    Matrix M(n, vector<double>(m));
    mt19937 gen(seed);
    uniform_real_distribution<double> dist(0.0, 10.0);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            M[i][j] = dist(gen);
    return M;
}

double checksum(const Matrix &M) {
    // Used to verify naive and blocked results match (correctness check)
    double s = 0;
    for (auto &row : M) for (double v : row) s += v;
    return s;
}

// ---------- Naive O(n^3) multiplication: i-k-j order is already decent,
// but classic "naive" as taught is i-j-k, which has poor cache locality
// because B is accessed column-wise (stride n) in the inner loop. ----------
Matrix naiveMultiply(const Matrix &A, const Matrix &B, int n) {
    Matrix C(n, vector<double>(n, 0.0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0.0;
            for (int k = 0; k < n; k++) {
                sum += A[i][k] * B[k][j];   // B[k][j]: column access -> cache-unfriendly
            }
            C[i][j] = sum;
        }
    }
    return C;
}

// ---------- Cache-optimized: Loop-reordered (i-k-j) multiplication ----------
// Improves spatial locality: inner loop walks B[k][*] and C[i][*] row-wise (stride 1).
Matrix ikjMultiply(const Matrix &A, const Matrix &B, int n) {
    Matrix C(n, vector<double>(n, 0.0));
    for (int i = 0; i < n; i++) {
        for (int k = 0; k < n; k++) {
            double a = A[i][k];
            for (int j = 0; j < n; j++) {
                C[i][j] += a * B[k][j];      // stride-1 access on both B and C
            }
        }
    }
    return C;
}

// ---------- Cache-optimized: Blocked/Tiled multiplication ----------
// Splits matrices into BLOCK x BLOCK tiles so that the working set of each
// tile fits inside L1/L2 cache, drastically cutting cache misses for large n.
Matrix blockedMultiply(const Matrix &A, const Matrix &B, int n, int BLOCK) {
    Matrix C(n, vector<double>(n, 0.0));
    for (int ii = 0; ii < n; ii += BLOCK) {
        for (int kk = 0; kk < n; kk += BLOCK) {
            for (int jj = 0; jj < n; jj += BLOCK) {
                int iMax = min(ii + BLOCK, n);
                int kMax = min(kk + BLOCK, n);
                int jMax = min(jj + BLOCK, n);
                for (int i = ii; i < iMax; i++) {
                    for (int k = kk; k < kMax; k++) {
                        double a = A[i][k];
                        for (int j = jj; j < jMax; j++) {
                            C[i][j] += a * B[k][j];
                        }
                    }
                }
            }
        }
    }
    return C;
}

// ---------- Gaussian Elimination with Partial Pivoting (Numerical Methods) ----------
// Solves Ax = b, returns x. Also demonstrates numerical stability via pivoting.
vector<double> gaussianSolve(Matrix A, vector<double> b) {
    int n = A.size();
    for (int col = 0; col < n; col++) {
        // Partial pivoting: find row with max abs value in this column
        int pivot = col;
        for (int row = col + 1; row < n; row++)
            if (fabs(A[row][col]) > fabs(A[pivot][col])) pivot = row;
        swap(A[col], A[pivot]);
        swap(b[col], b[pivot]);

        for (int row = col + 1; row < n; row++) {
            double factor = A[row][col] / A[col][col];
            for (int k = col; k < n; k++) A[row][k] -= factor * A[col][k];
            b[row] -= factor * b[col];
        }
    }
    // Back substitution
    vector<double> x(n);
    for (int i = n - 1; i >= 0; i--) {
        double sum = b[i];
        for (int j = i + 1; j < n; j++) sum -= A[i][j] * x[j];
        x[i] = sum / A[i][i];
    }
    return x;
}

template<typename Func>
double timeIt(Func f) {
    auto start = high_resolution_clock::now();
    f();
    auto end = high_resolution_clock::now();
    return duration<double, milli>(end - start).count();
}

int main() {
    cout << fixed << setprecision(3);
    cout << "=== Cache-Optimized Numerical Computation Engine ===\n\n";

    vector<int> sizes = {128, 256, 512};
    int BLOCK = 32;

    cout << left << setw(8) << "n"
         << setw(16) << "Naive(ms)"
         << setw(16) << "Reordered(ms)"
         << setw(16) << "Blocked(ms)"
         << setw(16) << "Speedup(naive/blk)" << "\n";
    cout << string(72, '-') << "\n";

    for (int n : sizes) {
        Matrix A = randomMatrix(n, n, 1);
        Matrix B = randomMatrix(n, n, 2);

        Matrix C1, C2, C3;
        double tNaive = timeIt([&]() { C1 = naiveMultiply(A, B, n); });
        double tIkj   = timeIt([&]() { C2 = ikjMultiply(A, B, n); });
        double tBlock = timeIt([&]() { C3 = blockedMultiply(A, B, n, BLOCK); });

        // Correctness check
        double diff = fabs(checksum(C1) - checksum(C3));
        if (diff > 1e-3) {
            cout << "WARNING: result mismatch, diff = " << diff << "\n";
        }

        cout << left << setw(8) << n
             << setw(16) << tNaive
             << setw(16) << tIkj
             << setw(16) << tBlock
             << setw(16) << (tNaive / tBlock) << "\n";
    }

    cout << "\n=== Gaussian Elimination Demo (Numerical Methods) ===\n";
    Matrix A = {{2, 1, -1}, {-3, -1, 2}, {-2, 1, 2}};
    vector<double> b = {8, -11, -3};
    vector<double> x = gaussianSolve(A, b);
    cout << "Solving Ax = b for a 3x3 system.\n";
    cout << "Solution: x = [" << x[0] << ", " << x[1] << ", " << x[2] << "]\n";
    cout << "(Expected approx: [2, 3, -1])\n";

    return 0;
}
