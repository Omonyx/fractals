#include "Fractal.hpp"
#ifndef MANDELBROTSET_HPP
#define MANDELBROTSET_HPP

class MandelbrotSet : public Fractal {
public:
    MandelbrotSet(int w, int h, int max_iter, int num_threads, int colorization, double rgb_coef[3]);
    void compute_fractal() override;
    void compute_each_thread(int y_start, int y_end) override;
};

#endif
