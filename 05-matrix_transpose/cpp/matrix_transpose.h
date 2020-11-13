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

void transpose() { trans(0U, 0U, N); }

std::tuple<unsigned, unsigned char> max_pow2(unsigned fits) {
    unsigned power = 1U;
    unsigned char log = 0U;

    while ((fits /= 2U) >= 1U) {
        power *= 2U;
        ++log;
    }

    return {power, log};
}

void trans(unsigned i, unsigned j, unsigned width) {
    if (width > 1U) {
        auto &&[power, log] = max_pow2(width);
        const unsigned n_width = power / 2U;
        if (power != width) {
            trans(i + n_width, j + n_width, width - n_width);
            pre_cis(i + n_width, j, width - n_width, n_width, i, j + n_width);
            pre_trans_static(i, j, log - 1U);
        } else {
            pre_trans_static(i, j, log);
        }
    }
}

void pre_cis(unsigned i1, unsigned j1, unsigned width, unsigned height, unsigned i2, unsigned j2) {
    switch ((width <= height) + (width == height)) {
    case 0U: /* width > height */
        cis(i1, j1, width, height, i2, j2);
        break;
    case 1U: /* width < height */
        cis(i2, j2, height, width, i1, j1);
        break;
    default: /* width == height */
        trans(i1, j1, width);
        swap_sq(i1, j1, width, i2, j2);
        trans(i1, j1, width);
        break;
    }
}

void cis(unsigned i1, unsigned j1, unsigned width, unsigned height, unsigned i2, unsigned j2) {
    const unsigned n_width = width - width / 2U;
    pre_cis(i2, j2, height, n_width, i1, j1);
    pre_cis(i2, j2 + n_width, height, width - n_width, i1 + n_width, j1);
}

void swap_sq(unsigned i1, unsigned j1, unsigned width, unsigned i2, unsigned j2) {
    for (unsigned i = 0U; i < width; ++i)
        for (unsigned j = 0U; j < width; ++j)
            swap(i1 + i, j1 + j, i2 + i, j2 + j);
}

void pre_trans_static(unsigned i, unsigned j, unsigned char log) {
    switch (log) {
    case 19U:
        trans_static<1U << 19U>(i, j);
        break;
    case 18U:
        trans_static<1U << 18U>(i, j);
        break;
    case 17U:
        trans_static<1U << 17U>(i, j);
        break;
    case 16U:
        trans_static<1U << 16U>(i, j);
        break;
    case 15U:
        trans_static<1U << 15U>(i, j);
        break;
    case 14U:
        trans_static<1U << 14U>(i, j);
        break;
    case 13U:
        trans_static<1U << 13U>(i, j);
        break;
    case 12U:
        trans_static<1U << 12U>(i, j);
        break;
    case 11U:
        trans_static<1U << 11U>(i, j);
        break;
    case 10U:
        trans_static<1U << 10U>(i, j);
        break;
    case 9U:
        trans_static<1U << 9U>(i, j);
        break;
    case 8U:
        trans_static<1U << 8U>(i, j);
        break;
    case 7U:
        trans_static<1U << 7U>(i, j);
        break;
    case 6U:
        trans_static<1U << 6U>(i, j);
        break;
    case 5U:
        trans_static<1U << 5U>(i, j);
        break;
    case 4U:
        trans_static<1U << 4U>(i, j);
        break;
    case 3U:
        trans_static<1U << 3U>(i, j);
        break;
    case 2U:
        trans_static<1U << 2U>(i, j);
        break;
    case 1U:
        trans_static<1U << 1U>(i, j);
        break;
    case 0U:
        break;
    }
}

template<unsigned width>
void trans_static(unsigned i, unsigned j) {
    if constexpr (width > 1U) {
        constexpr unsigned n_width = width / 2U;

        trans_static<n_width>(i + n_width, j + n_width);

        trans_static<n_width>(i + n_width, j);
        swap_sq(i + n_width, j, n_width, i, j + n_width);
        trans_static<n_width>(i + n_width, j);

        trans_static<n_width>(i, j);
    }
}
