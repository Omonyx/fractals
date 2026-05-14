#ifndef BURNINGSHIP_HPP
# define BURNINGSHIP_HPP
# include "Fractal.hpp"
# include <thread>
# include <cmath>

class BurningShip : public Fractal {
public:
    BurningShip(int w, int h, int max_iter, int num_threads, int colorization, double rgb_coef[3]);
    virtual void compute_fractal() override;
    virtual void compute_each_thread(int y_start, int y_end) override;
    virtual ~BurningShip();
};

#endif
