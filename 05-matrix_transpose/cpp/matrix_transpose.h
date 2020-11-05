/*
 *  This file is #include'd inside the definition of a matrix class
 *  like this:
 *
 *  	class ClassName {
 *          // Number of rows and columns of the matrix
 *          unsigned N;
 *
 *          // Swap elements (i1,j1) and (i2,j2)
 *          void swap(unsigned i1, unsigned j1, unsigned i2, unsigned j2);
 *
 *          // Your code
 *          #include "matrix_transpose.h"
 *      }
 */

void transpose()
{
    trans(0,0, N);
}

void trans(unsigned i, unsigned j, unsigned width) {
    if (width > 1) {
        const unsigned n_width = width - width / 2;
        trans(i + n_width, j + n_width, width - n_width);
        cis(i + n_width, j, width - n_width, n_width, i, j + n_width);
        return trans(i, j, n_width);
    }
}

void cis(unsigned i1, unsigned j1, unsigned width, unsigned height , unsigned i2, unsigned j2) {
    if (width < height) {
        cis(i1, j1 + width, width, height - width, i2 + width, j2);
    } else if (width > height) {
        cis(i1 + height, j1, width - height, height, i2, j2 + height);
        width = height;
    }

    trans(i2, j2, width);
    swap_sq(i1, j1, width, i2, j2);
    return trans(i2, j2, width);
}

void swap_sq(unsigned i1, unsigned j1, unsigned width, unsigned i2, unsigned j2) {
        for (unsigned i = 0; i < width; ++i)
            for (unsigned j = 0; j < width; ++j)
                swap(i1 + i, j1 + j, i2 + i, j2 + j);
}
