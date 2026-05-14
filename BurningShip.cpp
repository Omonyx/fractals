#include "BurningShip.hpp"
#include <complex>

BurningShip::BurningShip(int w, int h, int max_iter, int num_threads, int colorization, double rgb_coef[3]) : Fractal(w, h, max_iter, num_threads, colorization, rgb_coef) {}
void BurningShip::compute_fractal() {
    std::vector<std::thread> threads;
    int thread_division = HEIGHT / NUM_THREADS;
    for (int i = 0; i < NUM_THREADS; i++) {
        int start = i * thread_division;
        int end = (i == NUM_THREADS - 1) ? HEIGHT : (i + 1) * thread_division;
        threads.emplace_back(&BurningShip::compute_each_thread, this, start, end);
    }
    for (auto& thread : threads)
        thread.join();
}
void BurningShip::compute_each_thread(int y_start, int y_end) {
    for (int y = y_start; y < y_end; y++) {
        for (int x = 0; x < WIDTH; x++) {
            std::complex<double> c(x_min + (x_max - x_min) * x / WIDTH, y_min + (y_max - y_min) * y / HEIGHT);
            std::complex<double> z(0, 0);
            int iter = 0;
            while (std::abs(z) < 2 && iter < MAX_ITER) {
                double re = std::abs(z.real());
                double im = std::abs(z.imag());
                z = std::complex<double>(re, im);
                z = z * z + c;
                iter++;
            }
            int index = y * WIDTH + x;
            iterations[index] = iter;
            if (iter < MAX_ITER) {
                smoother[index] = (double)(iter + 1 - log(log(sqrt(z.real() * z.real() + z.imag() * z.imag()))) / log(2)) / MAX_ITER;
            } else {
                smoother[index] = 0;
            };
        };
    };
};

BurningShip::~BurningShip() {};
