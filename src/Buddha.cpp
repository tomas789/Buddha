#include <cstdint>
#include <utility>

#include "Buddha.h"

Buddha::Buddha(const Params & p, const std::size_t thread_vector_size)
  : thread_vector_size_(thread_vector_size) {
    x_size_ = p.width;
    y_size_ = p.width;
    radius_ = p.radius;
    num_threads_ = p.num_threads;
    max_iterations_ = p.max_iterations;

    if (max_iterations_ > thread_vector_size)
        throw MaxIterationsTooBigException();
}
Buddha::Params Buddha::get_empty_params() {
    Params p;
    p.num_threads = -1;
    return p;
}

void Buddha::run() {
    uint64_t part_size = x_size_ * y_size_ / num_threads_;

    for (int i = 0; i < num_threads_; ++i)
        threads_.emplace_back(&Buddha::worker, this, i * part_size, (i+1) * part_size);

    for (auto & t : threads_)
        t.join();
}

std::pair<uint64_t, uint64_t> Buddha::lin2car(uint64_t pos) const {
    return std::make_pair(pos % x_size_, pos / x_size_);
}

uint64_t Buddha::car2lin(uint64_t x, uint64_t y) const {
    return y * x_size_ + x;
}

Buddha::complex_type Buddha::car2complex(uint64_t x, uint64_t y) const {
    floating_type re = radius_ * ( 2. / x_size_ * x - 1);
    floating_type im = radius_ * ( 2. / y_size_ * x - 1);
    return complex_type(re, im);
}
