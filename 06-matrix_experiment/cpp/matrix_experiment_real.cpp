#include <functional>
#include <string>
#include <vector>
#include <cstdio>
#include <cmath>
#include <iostream>

#include <chrono>

using namespace std;

// If the condition is not true, report an error and halt.
#define EXPECT(condition, message) do { if (!(condition)) expect_failed(message); } while (0)

void expect_failed(const string& message) {
    cerr << "Test error: " << message << endl;
    exit(1);
}

class Matrix {
    vector<unsigned> items;
    unsigned &item(unsigned i, unsigned j) { return items[i*N + j]; }
  public:
    unsigned N;
    Matrix(unsigned N) { this->N = N; items.resize(N*N, 0); }

    void swap(unsigned i1, unsigned j1, unsigned i2, unsigned j2)
    {
        // EXPECT(i1 < N && j1 < N && i2 < N && j2 < N, "Swap out of range: " + coord_string(i1, j1) + " with " + coord_string(i2, j2) + ".");
        std::swap(item(i1, j1), item(i2, j2));
    }

    void naive_transpose()
    {
        for (unsigned i=0; i<N; i++)
            for (unsigned j=0; j<i; j++)
                swap(i, j, j, i);
    }

#include "matrix_transpose_real.h"
};

void real_test(unsigned min, unsigned max, std::function<unsigned(unsigned)> transform, bool naive)
{
    for (unsigned e=min; e <= max; e++) {
        const unsigned N = transform(e);
        Matrix m(N);

        unsigned tries = 1;
        std::chrono::duration<double> difference;
        do {
            auto start = std::chrono::high_resolution_clock::now();
            for (unsigned t=0; t < tries; t++) {
                if (naive)
                    m.naive_transpose();
                else
                    m.transpose();
            }
            auto end = std::chrono::high_resolution_clock::now();

            if ((difference = (end - start)).count() > 0.3) break;
            tries *= 2;
        } while (true);

        double ns_per_item = difference.count() / (N*(N-1)) / tries * 1e9;
        printf("%d\t%.6f\n", N, ns_per_item);
    }
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s (smart|naive)[128]\n", argv[0]);
        return 1;
    }

    std::string mode = argv[1];

    if (mode == "smart")
        real_test(40, 120, [](unsigned e){ return (unsigned) pow(2, e/8.); }, false);
    else if (mode == "smart128")
        real_test(2, 256, [](unsigned e){ return e * 128U; }, false);
    else if (mode == "naive")
        real_test(40, 120, [](unsigned e){ return (unsigned) pow(2, e/8.); }, true);
    else if (mode == "naive128")
        real_test(2, 256, [](unsigned e){ return e * 128U; }, true);
    else {
        fprintf(stderr, "The argument must be either 'smart' or 'naive'\n");
        return 1;
    }

    return 0;
}
