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

void transpose() { trans(0, 0, N); }

void trans(unsigned i, unsigned j, unsigned width) {
    if (width > 1) {
        const unsigned r_width = width / 2; // also b_height
        const unsigned l_width = width - r_width; // also t_height

        trans(i, j, l_width); // top left
        trans_swap(i + l_width, j, r_width, l_width); // bottom left
        trans(i + l_width, j + l_width, r_width); // bottom right
    }
}

// the trans_swapped rectangle has to have nonzero area
void trans_swap(unsigned i, unsigned j, unsigned height, unsigned width) {
    if (width + height == 2) { // both are 1
        swap(j, i, i, j);
    } else if (width >= height) {
        const unsigned r_width = width / 2;
        const unsigned l_width = width - r_width;

        trans_swap(j, i, l_width, height); // left half with swapped axes
        trans_swap(j + l_width, i, r_width, height); // right half with swapped axes
    } else { // this branch should be rare
        trans_swap(j, i, width, height); // just swap axes so width > height
    }
}
