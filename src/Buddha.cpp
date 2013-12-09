#include <cstdint>
#include <utility>

#include "Buddha.h"

Buddha::Buddha(const Params & p, const std::size_t thread_vector_size)
  : thread_vector_size_(p.num_threads) {
    x_size_ = p.width;
    y_size_ = p.width;
    radius_ = p.radius;
    max_iterations_ = p.max_iterations;
}
Buddha::Params Buddha::get_empty_params() {
    Params p;
    p.num_threads = -1;
    return p;
}

void Buddha::run() {

}

std::pair<uint64_t, uint64_t> Buddha::lin2car(uint64_t pos) const {
    return std::make_pair(pos % x_size_, pos / x_size_);
}

uint64_t Buddha::car2lin(uint64_t x, uint64_t y) const {
    return y * x_size_ + x;
}

