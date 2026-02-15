#ifndef JULIASET_HPP
# define JULIASET_HPP
# include "Fractal.hpp"
# include <thread>
# include <cmath>

class JuliaSet : public Fractal {
private:
    std::pair<double, double> C;
public:
    JuliaSet(int w, int h, int max_iter, int num_threads, int colorization, double rgb_coef[3], std::pair<double, double> c);
    virtual void compute_fractal() override;
    virtual void compute_each_thread(int y_start, int y_end) override;
    virtual ~JuliaSet();
};

#endif
