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

void transpose() { trans(0, N); }


template<unsigned width>
void trans_swap_static(unsigned i, unsigned j) {
    if constexpr (width == 1) {
        swap(j, i, i, j);
    } else {
        constexpr unsigned h_width = width / 2;
        trans_swap_static<h_width>(i, j); // top left
        trans_swap_static<h_width>(i + h_width, j); // bottom left
        trans_swap_static<h_width>(i + h_width, j + h_width); // bottom right
        trans_swap_static<h_width>(i, j + h_width); // top right
    }
}

template<unsigned width>
void trans_static(unsigned i) {
    if constexpr (width > 1) {
        constexpr unsigned h_width = width / 2;
        trans_static<h_width>(i); // top left
        trans_swap_static<h_width>(i, i + h_width); // bottom left
        trans_static<h_width>(i + h_width); // bottom right
    }
}

void pre_trans_static(unsigned i, unsigned char log) {
    switch (log) {
    case 19U:
        trans_static<1U << 19U>(i);
        break;
    case 18U:
        trans_static<1U << 18U>(i);
        break;
    case 17U:
        trans_static<1U << 17U>(i);
        break;
    case 16U:
        trans_static<1U << 16U>(i);
        break;
    case 15U:
        trans_static<1U << 15U>(i);
        break;
    case 14U:
        trans_static<1U << 14U>(i);
        break;
    case 13U:
        trans_static<1U << 13U>(i);
        break;
    case 12U:
        trans_static<1U << 12U>(i);
        break;
    case 11U:
        trans_static<1U << 11U>(i);
        break;
    case 10U:
        trans_static<1U << 10U>(i);
        break;
    case 9U:
        trans_static<1U << 9U>(i);
        break;
    case 8U:
        trans_static<1U << 8U>(i);
        break;
    case 7U:
        trans_static<1U << 7U>(i);
        break;
    case 6U:
        trans_static<1U << 6U>(i);
        break;
    case 5U:
        trans_static<1U << 5U>(i);
        break;
    case 4U:
        trans_static<1U << 4U>(i);
        break;
    case 3U:
        trans_static<1U << 3U>(i);
        break;
    case 2U:
        trans_static<1U << 2U>(i);
        break;
    case 1U:
        trans_static<1U << 1U>(i);
        break;
    case 0U:
        break;
    }
}

void pre_trans_swap_static(unsigned i, unsigned j, unsigned char log) {
    switch (log) {
    case 19U:
        trans_swap_static<1U << 19U>(i, j);
        break;
    case 18U:
        trans_swap_static<1U << 18U>(i, j);
        break;
    case 17U:
        trans_swap_static<1U << 17U>(i, j);
        break;
    case 16U:
        trans_swap_static<1U << 16U>(i, j);
        break;
    case 15U:
        trans_swap_static<1U << 15U>(i, j);
        break;
    case 14U:
        trans_swap_static<1U << 14U>(i, j);
        break;
    case 13U:
        trans_swap_static<1U << 13U>(i, j);
        break;
    case 12U:
        trans_swap_static<1U << 12U>(i, j);
        break;
    case 11U:
        trans_swap_static<1U << 11U>(i, j);
        break;
    case 10U:
        trans_swap_static<1U << 10U>(i, j);
        break;
    case 9U:
        trans_swap_static<1U << 9U>(i, j);
        break;
    case 8U:
        trans_swap_static<1U << 8U>(i, j);
        break;
    case 7U:
        trans_swap_static<1U << 7U>(i, j);
        break;
    case 6U:
        trans_swap_static<1U << 6U>(i, j);
        break;
    case 5U:
        trans_swap_static<1U << 5U>(i, j);
        break;
    case 4U:
        trans_swap_static<1U << 4U>(i, j);
        break;
    case 3U:
        trans_swap_static<1U << 3U>(i, j);
        break;
    case 2U:
        trans_swap_static<1U << 2U>(i, j);
        break;
    case 1U:
        trans_swap_static<1U << 1U>(i, j);
        break;
    case 0U:
        trans_swap_static<1U << 0U>(i, j);
        break;
    }
}

unsigned ulog2(unsigned fits) {
    return (8U * sizeof(fits)) - 1U - __builtin_clz(fits);
}

void trans(unsigned i, unsigned width) {
    if (width > 1) {
        const unsigned log = ulog2(width) - 1;
        const unsigned l_width = 1U << log; // also t_height
        const unsigned r_width = width - l_width; // also b_height

        if (l_width == r_width) {
            pre_trans_static(i, log + 1);
        } else {
            pre_trans_static(i, log); // top left
            trans_swap(i + l_width, i, r_width, l_width); // bottom left
            trans(i + l_width, r_width); // bottom right
        }
    }
}

// the trans_swapped rectangle has to have nonzero area
void trans_swap(unsigned i, unsigned j, unsigned height, unsigned width) {
    if (width + height == 2) { // both are 1
        swap(j, i, i, j);
    } else if (width >= height) {
        const unsigned log = ulog2(width) - 1;
        const unsigned l_width = 1U << log;
        const unsigned r_width = width - l_width;
        if (l_width == height) {
            pre_trans_swap_static(i, j, log);
        } else {
            trans_swap(j, i, l_width, height); // left half with swapped axes
        }
        trans_swap(j + l_width, i, r_width, height); // right half with swapped axes
    } else { // this branch should be rare
        trans_swap(j, i, width, height); // just swap axes so width > height
    }
}
