#include <complex>
#include <fstream>
#include <iostream>
#include <vector>
#include <thread>

struct Mandelbrot {
    std::vector<int> image;
    int WIDTH;
    int HEIGHT;

    double x_min = -2;
    double x_max = 2;
    double y_min = -1.5;
    double y_max = 1.5;

    int NUM_THREADS;
    const int MAX_ITER;

    Mandelbrot(int width, int height, int max_iter, int numThreads): WIDTH(width), HEIGHT(height), MAX_ITER(max_iter), NUM_THREADS(numThreads) {
        image.resize(width * height);
    }
    void calcul_fractal() {
        std::vector<std::thread> threads;
        int threads_div = HEIGHT / NUM_THREADS;
        for (int i = 0; i < NUM_THREADS; i++) {
            int start = i * threads_div;
            int end = (i == NUM_THREADS - 1) ? HEIGHT : (i + 1) * threads_div;

            threads.push_back(std::thread(&Mandelbrot::calcul_each_threads, this, start, end));
        }
        for (auto& thread : threads) {
            thread.join();
        }
    }
    void calcul_each_threads(int start, int end) {
        for (int x = 0; x < WIDTH; x++) {
            for (int y = start; y < end; y++) {
                std::complex<double> c(x_min + (x_max - x_min) * x / WIDTH, y_min + (y_max - y_min) * y / HEIGHT);
                std::complex<double> z = 0;
                int iter = 0;

                while (std::abs(z) <= 2 && iter < MAX_ITER) {
                    z = z * z + c;
                    iter++;
                }
                image[y * WIDTH + x] = iter;
            }
        }
    }
    void zoomFractal(double x_min, double x_max, double y_min, double y_max) {
        x_min = x_min;
        x_max = x_max;
        y_min = y_min;
        y_max = y_max;
    }
    void resetZoom() {
        x_min = -2;
        x_max = 2;
        y_min = -1.5;
        y_max = 1.5;
    }
    void saveFractal(const std::string &filename) {
        std::ofstream ofs(filename, std::ios::binary);
        ofs << "P5\n" << WIDTH << " " << HEIGHT << "\n255\n";

        for (int i = 0; i < WIDTH * HEIGHT; i++) {
            int iter_num = image[i];
            unsigned char color = static_cast<unsigned char>(255 - 255 * iter_num / MAX_ITER);
            ofs << color;
        }
        ofs.close();
    }
};
int main() {
    int width;
    int height;
    int max_iter;
    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) {
        num_threads = 1;
    }

    std::string fileName;
    std::cout << "Enter WIDTH HEIGHT MAX_ITER" << "\n";
    std::cin >> width >> height >> max_iter;
    std::cout << "Calculing Mandelbrot set..." << "\n";

    Mandelbrot fractal = Mandelbrot(width, height, max_iter, num_threads);
    fractal.calcul_fractal();
    std::cout << "Mandelbrot set generated.\n" << "Enter image file's name : \n";
    std::cin >> fileName;
    fractal.saveFractal(fileName + ".ppm");
    std::cout << "Image saved !";

    return 0;
}