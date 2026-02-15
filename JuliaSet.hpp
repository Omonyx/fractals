#include "Fractal.hpp"
#ifndef JULIASET_HPP
#define JULIASET_HPP

class JuliaSet : public Fractal {
private:
    std::pair<double, double> C;
public:
    JuliaSet(int w, int h, int max_iter, int num_threads, int colorization, double rgb_coef[3], std::pair<double, double> c);
    void compute_fractal() override;
    void compute_each_thread(int y_start, int y_end) override;
};

#endif
