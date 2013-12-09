#include <complex>
#include <cstdint>
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

    for (uint64_t i = from; i < to; ++i) {
        if (filled + max_iterations_ >= thread_vector_size_) {
            // Vector is full, flush the data.
            std::unique_lock<std::mutex> _(data_lock_);

            for (uint64_t j = 0; j < filled; ++j) {
                data_[local_data[j]]++;
            }

            filled = 0;
        }

        std::pair<uint64_t, uint64_t> coords = lin2car(i);
        complex_type c = car2complex(coords.first, coords.second);
        complex_type z = c;

        uint64_t pos = 0;
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

        if (pos != max_iterations_) {
            filled += pos;
        }
    }

    {
        std::unique_lock<std::mutex> _(data_lock_);

        for (uint64_t i = 0; i < filled; ++i) {
            data_[local_data[i]]++;
        }
    }
}
