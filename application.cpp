#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <thread>
#include <iostream>
#include <cmath>
#include <stack>

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

struct Mandelbrot {
    int WIDTH;
    int HEIGHT;
    double MAX_ITER;
    int NUM_THREADS;
    int TYPE;
    double SAT;
    std::stack<std::pair<std::pair<double, double>, std::pair<double, double>>> history;
    std::pair<double, double> HUE;
    std::pair<double, double> C;

    double x_min = -2;
    double x_max =  2;
    double y_min = -1.5;
    double y_max =  1.5;

    std::vector<double> smoother;
    std::vector<int> iterations;

    Mandelbrot(int w, int h, int maxIter, int num_threads, double cx, double cy, int add_hue, int coef_hue, double sat, int type) : WIDTH(w), HEIGHT(h), MAX_ITER(maxIter), NUM_THREADS(num_threads), C(cx, cy), HUE(add_hue, coef_hue), SAT(sat), TYPE(type) {
        smoother.resize(WIDTH * HEIGHT);
        iterations.resize(WIDTH * HEIGHT);
        history.push(std::make_pair(std::make_pair(-2.0, 2.0), std::make_pair(-1.5, 1.5)));
    }
    void compute_fractal() {
        std::vector<std::thread> threads;
        int thread_division = HEIGHT / NUM_THREADS;
        for (int i = 0; i < NUM_THREADS; i++) {
            int start = i * thread_division;
            int end = (i == NUM_THREADS - 1) ? HEIGHT : (i + 1) * thread_division;

            threads.emplace_back(&Mandelbrot::compute_each_thread, this, start, end);
        }
        for (auto& thread : threads)
            thread.join();
    }
    void compute_each_thread(int y_start, int y_end) {
        for (int y = y_start; y < y_end; y++) {
            for (int x = 0; x < WIDTH; x++) {
                double cr = TYPE == 2 ? C.first : x_min + (x_max - x_min) * x / WIDTH;
                double ci = TYPE == 2 ? C.second : y_min + (y_max - y_min) * y / HEIGHT;
                double zr = TYPE == 2 ? x_min + (x_max - x_min) * x / WIDTH : 0;
                double zi = TYPE == 2 ? y_min + (y_max - y_min) * y / HEIGHT : 0;
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
    void zoom(double new_x_min, double new_x_max, double new_y_min, double new_y_max) {
        x_min = new_x_min;
        x_max = new_x_max;
        y_min = new_y_min;
        y_max = new_y_max;
    }
    void resetZoom() {
        x_min = -2;
        x_max = 2;
        y_min = -1.5;
        y_max = 1.5;
    }
    void render_fractal(sf::Image& new_image) {
        for (unsigned int y = 0; y < HEIGHT; y++) {
            for (unsigned int x = 0; x < WIDTH; x++) {
                double t = smoother[y * WIDTH + x];
                RGB color_convert = hsv_to_rgb(HUE.first + HUE.second * t, SAT, iterations[y * WIDTH + x] == MAX_ITER ? 0 : 1);
                new_image.setPixel({x, y}, sf::Color(color_convert.r, color_convert.g, color_convert.b));
            }
        }
    }
};

int main() {
    int DEPTH = 200;
    int TYPE = 1;
    double SAT = 0.7;
    int ADD_HUE = 0;
    int COEF_HUE = 360;
    std::pair<double, double> C;
    std::cout << "Enter TYPE (1) : \n[1] Mandelbrot set\n[2] Julia set\n";
    std::cin >> TYPE;
    std::cout << "Enter DEEP compute (200) : \n";
    std::cin >> DEPTH;
    std::cout << "Enter ADD_HUE (0), COEF_HUE (360), SAT (0.7) : \n";
    std::cin >> ADD_HUE >> COEF_HUE >> SAT;
    if (TYPE == 2) {
        std::cout << "Enter CX (0.3), CY (0.7) : \n";
        std::cin >> C.first >> C.second;
    }
    sf::RenderWindow window(sf::VideoMode({1200, 900}), "Fractal preview");
    sf::Image image({1200, 900}, sf::Color::Black);
    sf::Texture texture(image);
    sf::Sprite sprite(texture);
    unsigned threads = std::thread::hardware_concurrency();
    if (threads == 0) threads = 1;

    Mandelbrot fractal(1200, 900, DEPTH, threads, C.first, C.second, ADD_HUE, COEF_HUE, SAT, TYPE);

    fractal.compute_fractal();
    fractal.render_fractal(image);
    texture.update(image);

    sf::Vector2i start;
    sf::Vector2i end;
    bool selecting = false;

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::R) {
                    fractal.resetZoom();
                    fractal.compute_fractal();
                    fractal.render_fractal(image);
                    texture.update(image);
                } else if (keyPressed->code == sf::Keyboard::Key::U) {
                    std::pair<std::pair<double, double>, std::pair<double, double>> old_view = fractal.history.top();
                    fractal.zoom(old_view.first.first, old_view.first.second, old_view.second.first, old_view.second.second);
                    fractal.compute_fractal();
                    fractal.render_fractal(image);
                    texture.update(image);
                    if (fractal.history.top() != std::make_pair(std::make_pair(-2.0, 2.0), std::make_pair(-1.5, 1.5))) {
                        fractal.history.pop();
                    }
                }
            }
            if (event->is<sf::Event::MouseButtonPressed>()) {
                start = sf::Mouse::getPosition(window);
                selecting = true;
            }
            if (event->is<sf::Event::MouseButtonReleased>()) {
                end = sf::Mouse::getPosition(window);
                selecting = false;

                if (start != end) {
                    double new_x_min = fractal.x_min + (fractal.x_max - fractal.x_min) * std::min(start.x, end.x) / 1200;
                    double new_x_max = fractal.x_min + (fractal.x_max - fractal.x_min) * std::max(start.x, end.x) / 1200;
                    double new_y_min = fractal.y_min + (fractal.y_max - fractal.y_min) * std::min(start.y, end.y) / 900;
                    double new_y_max = fractal.y_min + (fractal.y_max - fractal.y_min) * std::max(start.y, end.y) / 900;

                    fractal.zoom(new_x_min, new_x_max, new_y_min, new_y_max);
                    fractal.history.push(std::make_pair(std::make_pair(new_x_min, new_x_max), std::make_pair(new_y_min, new_y_max)));
                    fractal.compute_fractal();
                    fractal.render_fractal(image);
                    texture.update(image);
                }
            }
        }
        window.clear();
        window.draw(sprite);
        window.display();
    }
    return 0;
}
