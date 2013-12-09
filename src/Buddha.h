#ifndef _BUDDHA_H
#define _BUDDHA_H

#include <complex>
#include <cstdint>
#include <exception>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

class MaxIterationsTooBigException : public virtual std::exception { };

class Buddha {
public:
    typedef double floating_type;
    typedef std::complex<floating_type> complex_type; 

    struct Params {
        uint64_t width;
        floating_type radius;
        uint64_t max_iterations;
        int num_threads;
    };

    Buddha(const Params & p, std::size_t thread_vector_size = 10 * 1024 * 1024);

    static Params get_empty_params();

    void run();
private:
    uint64_t x_size_;
    uint64_t y_size_;
    floating_type radius_;
    uint64_t max_iterations_;

    std::vector<uint64_t> data_;
    std::mutex data_lock_;

    std::pair<uint64_t, uint64_t> lin2car(uint64_t pos) const;
    uint64_t car2lin(uint64_t x, uint64_t y) const;

    complex_type car2complex(uint64_t x, uint64_t y) const;
    std::pair<uint64_t, uint64_t> complex2car(complex_type c) const;

    uint64_t complex2lin(complex_type c) const;

    const std::size_t thread_vector_size_;
    std::size_t num_threads_;
    std::vector<std::thread> threads_;
    void worker(uint64_t from, uint64_t to);
};

#endif // _BUDDHA_H
