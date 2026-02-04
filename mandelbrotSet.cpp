#include <complex>
#include <fstream>
#include <iostream>
#include <vector>

struct Mandelbrot {
    std::vector<int> image;

    double x_min = -2;
    double x_max = 2;
    double y_min = -1.5;
    double y_max = 1.5;

    int WIDTH;
    int HEIGHT;
    const int MAX_ITER;

    Mandelbrot(int width, int height, int max_iter): WIDTH(width), HEIGHT(height), MAX_ITER(max_iter) {
        image.resize(width * height);
    }
    void calcul_fractal() {
        for (int x = 0; x < WIDTH; x++) {
            for (int y = 0; y < HEIGHT; y++) {
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
    int width, height, max_iter;
    std::string fileName;
    std::cout << "Enter WIDTH HEIGHT MAX_ITER" << "\n";
    std::cin >> width >> height >> max_iter;
    std::cout << "Calculing Mandelbrot set..." << "\n";

    Mandelbrot fractal = Mandelbrot(width, height, max_iter);
    fractal.calcul_fractal();
    std::cout << "Mandelbrot set generated.\n" << "Enter image file's name : \n";
    std::cin >> fileName;
    fractal.saveFractal(fileName + ".ppm");
    std::cout << "Image saved !";

    return 0;
}
