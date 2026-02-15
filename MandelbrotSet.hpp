#ifndef MANDELBROTSET_HPP
# define MANDELBROTSET_HPP
# include "Fractal.hpp"
# include <thread>
# include <cmath>

class MandelbrotSet : public Fractal {
public:
    MandelbrotSet(int w, int h, int max_iter, int num_threads, int colorization, double rgb_coef[3]);
    virtual void compute_fractal() override;
    virtual void compute_each_thread(int y_start, int y_end) override;
    virtual ~MandelbrotSet();
};

#endif
