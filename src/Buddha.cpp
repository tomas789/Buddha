#include <iostream>
#include <cstdint>
#include <utility>

#define cimg_display 0
#include "CImg.h"
using namespace cimg_library;

#include "Buddha.h"

Buddha::Buddha(const Params & p, const std::size_t thread_vector_size)
  : thread_vector_size_(thread_vector_size) {
    x_size_ = p.width;
    y_size_ = p.width;
    radius_ = p.radius;
    max_iterations_ = p.max_iterations;
    num_threads_ = p.num_threads > 0 
        ? p.num_threads
        : std::thread::hardware_concurrency();

    if (max_iterations_ > thread_vector_size)
        throw MaxIterationsTooBigException();

    data_ = std::vector<uint64_t>(x_size_ * y_size_);
}

Buddha::Params Buddha::get_empty_params() {
    Params p;
    p.num_threads = -1;
    return p;
}

void Buddha::run() {
    uint64_t part_size = x_size_ * y_size_ / num_threads_;

    for (std::size_t i = 0; i < num_threads_; ++i)
        threads_.emplace_back(&Buddha::worker, this, i * part_size, (i+1) * part_size);

    for (auto & t : threads_)
        t.join();

    auto img = render();
    img.save("buddhabrot.png");
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

std::pair<uint64_t, uint64_t> Buddha::complex2car(
    Buddha::complex_type c) const {
    uint64_t x = (uint64_t)(x_size_ * (c.real() / radius_ + 1) / 2.);
    uint64_t y = (uint64_t)(y_size_ * (c.imag() / radius_ + 1) / 2.);
    return std::make_pair(x, y);
}

Buddha::complex_type Buddha::lin2complex(uint64_t pos) const {
   auto pair = lin2car(pos);
   return car2complex(pair.first, pair.second);
}

uint64_t Buddha::complex2lin(Buddha::complex_type c) const {
    auto pair = complex2car(c);
    return car2lin(pair.first, pair.second);
}

CImg<unsigned char> Buddha::render() {
    std::lock_guard<std::mutex> _(data_lock_);
    CImg<unsigned char> img(x_size_, y_size_, 1, 1, 0);

    uint64_t max = 0;
    for (uint64_t i = 0; i < data_.size(); ++i) 
        if (max < data_[i])
            max = data_[i];

    for (uint64_t i = 0; i < data_.size(); ++i) {
        auto car = lin2car(i);
        unsigned char c = data_[i] / (floating_type)max * 255;
        img.draw_point(car.first, car.second, &c);
    }

    return img;
}
