#include <complex>
#include <fstream>
#include <iostream>
#include <vector>
#include <thread>
#include <utility>

struct RGB {
    unsigned char r, g, b;
};

RGB hsv_to_rgb(double h, double s, double v) {
    double c = v * s;
    double x = c * (1 - std::fabs(std::fmod(h / 60.0, 2) - 1));
    double m = v - c;

    double r1, g1, b1;

    if (h < 60) {
        r1 = c; g1 = x; b1 = 0;
    } else if (h < 120) {
        r1 = x; g1 = c; b1 = 0;
    } else if (h < 180) {
        r1 = 0; g1 = c; b1 = x;
    } else if (h < 240) {
        r1 = 0; g1 = x; b1 = c;
    } else if (h < 300) {
        r1 = x; g1 = 0; b1 = c;
    } else {
        r1 = c; g1 = 0; b1 = x;
    }

    return {
        (unsigned char)((r1 + m) * 255),
        (unsigned char)((g1 + m) * 255),
        (unsigned char)((b1 + m) * 255)
    };
}
struct Mandelbrot {
    std::vector<std::pair<double, double>> image;
    int WIDTH;
    int HEIGHT;
    int TYPE;
    int NUM_THREADS;
    double CX;
    double CY;
    const int MAX_ITER;

    double x_min = -2;
    double x_max = 2;
    double y_min = -1.5;
    double y_max = 1.5;

    Mandelbrot(int width, int height, int max_iter, int numThreads, double cx, double cy, int type): WIDTH(width), HEIGHT(height), MAX_ITER(max_iter), NUM_THREADS(numThreads), CX(cx), CY(cy), TYPE(type) {
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
                std::complex<double> c = TYPE == 2 ? std::complex(CX, CY) : std::complex(x_min + (x_max - x_min) * x / WIDTH, y_min + (y_max - y_min) * y / HEIGHT);
                std::complex<double> z = TYPE == 2 ? std::complex(x_min + (x_max - x_min) * x / WIDTH, y_min + (y_max - y_min) * y / HEIGHT) : 0;
                int iter = 0;

                while (std::abs(z) <= 4 && iter < MAX_ITER) {
                    z = z * z + c;
                    iter++;
                }
                image[y * WIDTH + x] = {(double)((iter + 1 - log(log(std::abs(z))) / log(2)) / (double)MAX_ITER), iter};
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
        ofs << "P6\n" << WIDTH << " " << HEIGHT << "\n255\n";

        for (int i = 0; i < WIDTH * HEIGHT; i++) {
            double t = image[i].first;
            RGB color_convert = hsv_to_rgb(220 + 120 * t, 0.7, image[i].second == MAX_ITER ? 0 : 1);
            unsigned char r = color_convert.r;
            unsigned char g = color_convert.g;
            unsigned char b = color_convert.b;
            unsigned char color[3] = { r, g, b };
            ofs.write(reinterpret_cast<char*>(color), 3);
        }
        ofs.close();
    }
};
int main() {
    int width = 4000;
    int height = 3000;
    int max_iter = 25;
    int fractal_type = 0;
    double cx = 0;
    double cy = 0;
    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) {
        num_threads = 1;
    }

    std::string fileName;
    std::cout << "Enter : WIDTH HEIGHT MAX_ITER\n";
    std::cin >> width >> height >> max_iter;
    std::cout << "[1] Mandelbrot Set\n[2] Julia Set\n";
    std::cin >> fractal_type;
    switch (fractal_type) {
        case 1:
            std::cout << "Calculing Mandelbrot Set...\n";
            break;
        case 2:
            std::cout << "Enter size : CX CY\n";
            std::cin >> cx >> cy;
            std::cout << "Calculing Julia set...\n";
            break;
        default:
            break;
    }

    Mandelbrot fractal = Mandelbrot(width, height, max_iter, num_threads, cx, cy, fractal_type);
    fractal.calcul_fractal();
    std::cout << "Fractal generated.\n" << "Enter image file's name : \n";
    std::cin >> fileName;
    fractal.saveFractal(fileName + ".ppm");
    std::cout << "Image saved !";

    return 0;
}
