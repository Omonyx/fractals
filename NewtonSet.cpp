#include "NewtonSet.hpp"
#include <complex>
#include <iostream>

NewtonSet::NewtonSet(int w, int h, int max_iter, int num_threads, int colorization, double rgb_coef[3], std::vector<std::complex<double>> pol) : Fractal(w, h, max_iter, num_threads, colorization, rgb_coef), pol(pol) {}
void NewtonSet::compute_fractal() {
    std::vector<std::thread> threads;
    int thread_division = HEIGHT / NUM_THREADS;

    for (size_t i = 1; i < pol.size(); i++) {dpol.push_back(pol[i] * (double)i);};

    std::cout << pol[0] << " " << pol[1] << " " << pol[2] << " " << pol[3] << std::endl;
    std::cout << dpol[0] << " " << dpol[1] << " " << dpol[2] << std::endl;

    for (int i = 0; i < NUM_THREADS; i++) {
        int start = i * thread_division;
        int end = (i == NUM_THREADS - 1) ? HEIGHT : (i + 1) * thread_division;
        threads.emplace_back(&NewtonSet::compute_each_thread, this, start, end);
    };
    for (auto& thread : threads)
        thread.join();
};
std::complex<double> NewtonSet::definePol(const std::vector<std::complex<double>>& pol, std::complex<double> z) {
    std::complex<double> result = 0.0;

    for (int i = (int)pol.size() - 1; i >= 0; --i) {
        result = result * z + pol[i];
    };
    return result;
};
void NewtonSet::compute_each_thread(int y_start, int y_end) {
    for (int y = y_start; y < y_end; y++) {
        for (int x = 0; x < WIDTH; x++) {
            std::complex<double> z(x_min + (x_max - x_min) * x / WIDTH,y_max - (y_max - y_min) * y / HEIGHT);
            double iter = 0;

            for (iter; iter < MAX_ITER; iter++) {
                std::complex<double> p = definePol(pol, z);
                std::complex<double> dp = definePol(dpol, z);

                if (std::abs(dp) < 1e-12) {break;};

                std::complex<double> next = z - p / dp;

                if (std::abs(next - z) < 1e-8) {break;};
                z = next;
            };

            int index = y * WIDTH + x;
            zs[index] = (std::complex<double>)z;
            if (iter < MAX_ITER) {
                smoother[index] = (double)iter;
            } else {
                smoother[index] = 0;
            };
        };
    };
};

NewtonSet::~NewtonSet() {};
