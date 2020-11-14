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

void trans_swap(unsigned i, unsigned j, unsigned height, unsigned width) {
    if (height == 1 || width == 1) {
        if (i != j) {
            swap(i, j, j, i);
            if (height == 2 || width == 2)
                swap(i + height - 1, j + width - 1, j + width - 1, i + height - 1);
        }
    } else {
        const unsigned b_height = height / 2;
        const unsigned t_height = height - b_height;
        const unsigned r_width = width / 2;
        const unsigned l_width = width - r_width;

        trans_swap(i, j, t_height, l_width); // top left
        trans_swap(i + t_height, j, b_height, l_width); // bottom left
        trans_swap(i + t_height, j + l_width, b_height, r_width); // bottom right
        trans_swap(i, j + l_width, t_height, r_width); // top right
    }
}
