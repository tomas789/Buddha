#include <complex>
#include <cstdint>
#include <sstream>
#include <utility>
#include <vector>

#include "Buddha.h"

void Buddha::flush_data(std::vector<uint64_t> & data, uint64_t & filled) {
    std::unique_lock<std::mutex> _(data_lock_);

    for (uint64_t i = 0; i < filled; i++) {
        data_[data[i]]++;
    }

    filled = 0;
}

void Buddha::worker(uint64_t from, uint64_t to) {
    uint64_t filled = 0;
    std::vector<uint64_t> local_data(thread_vector_size_);
    std::size_t progress_local = 0;

    floating_type radius_sqr = radius_ * radius_;
    floating_type subpixel_width  = 2 * radius_ / x_size_;
    floating_type subpixel_height = 2 * radius_ / y_size_;

    for (uint64_t sub_x = 0; sub_x < subpixel_resolution_; ++sub_x) {
        for (uint64_t sub_y = 0; sub_y < subpixel_resolution_; ++sub_y) {
            for (uint64_t i = from; i < to; ++i) {

                ++progress_local;

                if (filled + max_iterations_ >= thread_vector_size_)
                    flush_data(local_data, filled);                

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

            if (progress_local > 10000) {
                progress_ += progress_local;
                progress_local = 0;
            }
        }
    }

    flush_data(local_data, filled);
}
