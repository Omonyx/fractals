#include "JuliaSet.hpp"

JuliaSet::JuliaSet(int w, int h, int max_iter, int num_threads, int colorization, double rgb_coef[3], std::pair<double, double> c) : Fractal(w, h, max_iter, num_threads, colorization, rgb_coef), C(c) {}
void JuliaSet::compute_fractal() {
    std::vector<std::thread> threads;
    int thread_division = HEIGHT / NUM_THREADS;
    for (int i = 0; i < NUM_THREADS; i++) {
        int start = i * thread_division;
        int end = (i == NUM_THREADS - 1) ? HEIGHT : (i + 1) * thread_division;
        threads.emplace_back(&JuliaSet::compute_each_thread, this, start, end);
    }
    for (auto& thread : threads)
        thread.join();
}
void JuliaSet::compute_each_thread(int y_start, int y_end) {
    for (int y = y_start; y < y_end; y++) {
        for (int x = 0; x < WIDTH; x++) {
            double cr = C.first;
            double ci = C.second;
            double zr = x_min + (x_max - x_min) * x / WIDTH;
            double zi = y_min + (y_max - y_min) * y / HEIGHT;
            int iter = 0;
            while (zr * zr + zi * zi <= 4 && iter < MAX_ITER) {
                double temp = zr * zr - zi * zi + cr;
                zi = 2 * zr * zi + ci;
                zr = temp;
                iter++;
            }
            int index = y * WIDTH + x;
            iterations[index] = iter;
            if (iter < MAX_ITER) {
                smoother[index] = (double)(iter + 1 - log(log(sqrt(zr * zr + zi * zi))) / log(2)) / MAX_ITER;
            } else {
                smoother[index] = 0;
            }
        }
    }
}
JuliaSet::~JuliaSet() {}
