#ifndef _BUDDHA_H
#define _BUDDHA_H

#include <complex>
#include <cstdint>
#include <exception>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

#define cimg_display 0
#include "CImg.h"
using namespace cimg_library;

class MaxIterationsTooBigException : public virtual std::exception { };

class MinGreaterThanMaxException : public virtual std::exception { };

struct rgb {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

class ColoringSchema {
public:
    virtual rgb color(uint64_t count, uint64_t max) const = 0;
};

class ColorGrayscale : public ColoringSchema {
public:
    virtual rgb color(uint64_t count, uint64_t max) const;
};

class Buddha {
public:
    typedef double floating_type;
    typedef std::complex<floating_type> complex_type; 

    struct Params {
        uint64_t width;
        floating_type radius;
        uint64_t max_iterations;
        uint64_t min_iterations;
        int num_threads;
        ColoringSchema * schema;
    };

    Buddha(const Params & p, std::size_t thread_vector_size = 10 * 1024 * 1024);

    static Params get_empty_params();

    void run();
private:
    uint64_t x_size_;
    uint64_t y_size_;
    floating_type radius_;
    uint64_t max_iterations_;
    uint64_t min_iterations_;
    ColoringSchema * schema;

    std::vector<uint64_t> data_;
    std::mutex data_lock_;

    std::pair<uint64_t, uint64_t> lin2car(uint64_t pos) const;
    uint64_t car2lin(uint64_t x, uint64_t y) const;

    complex_type car2complex(uint64_t x, uint64_t y) const;
    std::pair<uint64_t, uint64_t> complex2car(complex_type c) const;

    complex_type lin2complex(uint64_t pos) const;
    uint64_t complex2lin(complex_type c) const;

    const std::size_t thread_vector_size_;
    std::size_t num_threads_;
    std::vector<std::thread> threads_;
    void worker(uint64_t from, uint64_t to);
    
    CImg<unsigned char> render();

    bool mandelbrot_hint(complex_type z) const;
};

#endif // _BUDDHA_H
