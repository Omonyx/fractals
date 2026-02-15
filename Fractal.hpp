#include <SFML/Graphics.hpp>
#include <stack>
#include <vector>
#ifndef FRACTAL_HPP
#define FRACTAL_HPP

class Fractal {
public:
    int WIDTH;
    int HEIGHT;
    double MAX_ITER;
    int NUM_THREADS;
    double x_min;
    double x_max;
    double y_min;
    double y_max;
    int COLORIZATION;
    double RGB_COEF[3];
    std::vector<double> smoother;
    std::vector<int> iterations;
    std::stack<std::pair<std::pair<double, double>, std::pair<double, double>>> history;
    Fractal(int w, int h, int max_iter, int num_threads, int colorization, double rgb_coef[3]);
    virtual void compute_fractal();
    virtual void compute_each_thread(int y_start, int y_end);
    virtual void zoom(double new_x_min, double new_x_max, double new_y_min, double new_y_max);
    virtual void reset_zoom();
    virtual void render_fractal(sf::Image& new_image);
    virtual ~Fractal() = default;
};

#endif
