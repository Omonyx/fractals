#ifndef NEWTONSET_HPP
# define NEWTONSET_HPP
# include "Fractal.hpp"
# include <thread>
# include <cmath>
# include <complex>

class NewtonSet : public Fractal {
private:
    std::vector<std::complex<double>> pol;
    std::vector<std::complex<double>> dpol;
public:
    NewtonSet(int w, int h, int max_iter, int num_threads, int colorization, double rgb_coef[3], std::vector<std::complex<double>> pol);
    virtual void compute_fractal() override;
    virtual void compute_each_thread(int y_start, int y_end) override;
    virtual std::complex<double> definePol(const std::vector<std::complex<double>>& pol, std::complex<double> z);
    virtual ~NewtonSet();
};

#endif
