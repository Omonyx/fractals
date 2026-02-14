#include <SFML/Graphics.hpp>
#include <complex>
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
    double SAT;
    std::pair<double, double> C;
    std::pair<int, int> HUE;
    const int MAX_ITER;

    double x_min = -2;
    double x_max = 2;
    double y_min = -1.5;
    double y_max = 1.5;

    Mandelbrot(int width, int height, int max_iter, int numThreads, double cx, double cy, int add_hue, int coef_hue, double sat, int type): WIDTH(width), HEIGHT(height), MAX_ITER(max_iter), NUM_THREADS(numThreads), C(cx, cy), HUE(add_hue, coef_hue), SAT(sat), TYPE(type) {
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
                std::complex<double> c = TYPE == 2 ? std::complex(C.first, C.second) : std::complex(x_min + (x_max - x_min) * x / WIDTH, y_min + (y_max - y_min) * y / HEIGHT);
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
    sf::Image showFractal(sf::Image background) {
        for (unsigned int x = 0; x < WIDTH; x++) {
            for (unsigned int y = 0; y < HEIGHT; y++) {
                double t = image[y * WIDTH + x].first;
                RGB color_convert = hsv_to_rgb(HUE.first + HUE.second * t, SAT, image[y * WIDTH + x].second == MAX_ITER ? 0 : 1);
                sf::Color new_color(color_convert.r, color_convert.g, color_convert.b);
                background.setPixel({x, y}, new_color);
            }
        }
        return background;
    }
};
int main() {
    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) {
        num_threads = 1;
    }
    sf::RenderWindow window(sf::VideoMode({1200u, 900u}), "Mandelbrot");
    sf::Image image({1200, 900}, sf::Color::Black);
    Mandelbrot fractal = Mandelbrot(1200, 900, 25, num_threads, 0, 0, 0, 120, 0.7, 1);
    fractal.calcul_fractal();
    sf::Texture texture(fractal.showFractal(image));
    sf::Sprite sprite(texture);

    while (window.isOpen())
    {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        window.clear();
        window.draw(sprite);
        window.display();
    }

    return 0;
}
