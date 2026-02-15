#include "Fractal.hpp"

struct RGB {
    unsigned char r, g, b;
};

RGB hsv_to_rgb(double h, double s, double v) {
    h = fmod(h, 360);
    if (h < 0) h += 360;

    double c = v * s;
    double x = c * (1 - std::fabs(fmod(h / 60.0, 2) - 1));
    double m = v - c;
    double r1 = 0, g1 = 0, b1 = 0;

    if (h < 60)       { r1 = c; g1 = x; }
    else if (h < 120) { r1 = x; g1 = c; }
    else if (h < 180) { g1 = c; b1 = x; }
    else if (h < 240) { g1 = x; b1 = c; }
    else if (h < 300) { r1 = x; b1 = c; }
    else              { r1 = c; b1 = x; }

    return {(unsigned char)((r1 + m) * 255), (unsigned char)((g1 + m) * 255), (unsigned char)((b1 + m) * 255)};
}

Fractal::Fractal(int w, int h, int max_iter, int num_threads, int colorization, double rgb_coef[3]) : RGB_COEF{rgb_coef[0], rgb_coef[1], rgb_coef[2]} {
    WIDTH = w;
    HEIGHT = h;
    MAX_ITER = max_iter;
    NUM_THREADS = num_threads;
    COLORIZATION = colorization;
    x_min = -2;
    x_max = 2;
    y_min = -1.5;
    y_max = 1.5;
    smoother.resize(WIDTH * HEIGHT);
    iterations.resize(WIDTH * HEIGHT);
    history.push(std::make_pair(std::make_pair(-2.0, 2.0), std::make_pair(-1.5, 1.5)));
}
void Fractal::zoom(double new_x_min, double new_x_max, double new_y_min, double new_y_max) {
    x_min = new_x_min;
    x_max = new_x_max;
    y_min = new_y_min;
    y_max = new_y_max;
}
void Fractal::reset_zoom() {
    x_min = -2;
    x_max = 2;
    y_min = -1.5;
    y_max = 1.5;
}
void Fractal::render_fractal(sf::Image& new_image) {
    for (unsigned int y = 0; y < HEIGHT; y++) {
        for (unsigned int x = 0; x < WIDTH; x++) {
            double t = smoother[y * WIDTH + x];
            if (COLORIZATION == 1) {
                double r = RGB_COEF[0] * (1 - t) * t * t * t * 255;
                double g = RGB_COEF[1] * (1 - t) * (1 - t) * t * t * 255;
                double b = RGB_COEF[2] * (1 - t) * (1 - t) * (1 - t) * t * 255;
                new_image.setPixel({x, y}, sf::Color(r, g, b));
            } else if (COLORIZATION == 2) {
                RGB color_convert = hsv_to_rgb(120 + 220 * t, 0.7, iterations[y * WIDTH + x] == MAX_ITER ? 0 : 1);
                new_image.setPixel({x, y}, sf::Color(color_convert.r, color_convert.g, color_convert.b));
            }
        }
    }
}
Fractal::~Fractal() {}
