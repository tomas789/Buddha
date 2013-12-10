#include <complex>
#include <cstdint>
#include <sstream>
#include <utility>
#include <vector>

#include "Buddha.h"

/**
 *  TODO : Refactoring - move flushing of local_data to separate method
 */
void Buddha::worker(uint64_t from, uint64_t to) {
    uint64_t filled = 0;
    std::vector<uint64_t> local_data(thread_vector_size_);

    floating_type radius_sqr = radius_ * radius_;
    floating_type subpixel_width  = 2 * radius_ / x_size_;
    floating_type subpixel_height = 2 * radius_ / y_size_;

    for (uint64_t sub_x = 0; sub_x < subpixel_resolution_; ++sub_x) {
        for (uint64_t sub_y = 0; sub_y < subpixel_resolution_; ++sub_y) {
            for (uint64_t i = from; i < to; ++i) {

                progress_++;

                if (filled + max_iterations_ >= thread_vector_size_) {
                    // Vector is full, flush the data.
                    std::unique_lock<std::mutex> _(data_lock_);

                    for (uint64_t j = 0; j < filled; ++j) {
                        data_[local_data[j]]++;
                    }

                    filled = 0;
                }

                complex_type c = lin2complex(i);
                c.real(c.real() + sub_x * subpixel_width);
                c.imag(c.imag() + sub_y * subpixel_height);
                complex_type z = c;

                uint64_t pos = 0;
                // TODO: To use or not to use, that is the question.
                // Deal with the merge conflict that arised here.
                if (mandelbrot_hint(c))
                    continue;

                while (z.real() * z.real() + z.imag() * z.imag() < radius_sqr
                    && pos < max_iterations_) {
                    // TODO: Possible optimization when computing abs(z)^2.

                    uint64_t zpos = complex2lin(z);

                    if (zpos < data_.size()) {
                        local_data[filled + pos] = zpos;
                    }

                    z *= z;
                    z += c;
                    ++pos;
                }

                if (pos >= min_iterations_ && pos < max_iterations_) {
                    filled += pos;
                }
            }
        }
    }

    {
        std::unique_lock<std::mutex> _(data_lock_);

        for (uint64_t i = 0; i < filled; ++i) {
            data_[local_data[i]]++;
        }
    }
}
