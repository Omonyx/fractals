#include <SFML/Graphics.hpp>
#include "Fractal.hpp"
#include "MandelbrotSet.hpp"
#include "JuliaSet.hpp"
#include <iostream>

std::string GREEN = "\e[1;32m";
std::string BLUE = "\e[1;34m";
std::string RESET = "\e[0m";

int main() {
    Fractal* fractal = nullptr;
    int DEPTH = 200;
    int TYPE = 1;
    int COLORIZATION = 1;
    double SAT = 0.7;
    int ADD_HUE = 0;
    int COEF_HUE = 360;
    double RGB_COEF[3] = {9, 15, 8.5};
    std::pair<double, double> C;
    unsigned threads = std::thread::hardware_concurrency();
    if (threads == 0) threads = 1;

    std::cout << GREEN << "Enter TYPE (1) : \n[1] Mandelbrot set\n[2] Julia set\n" << BLUE;
    std::cin >> TYPE;
    std::cout << GREEN << "Enter COLORIZATION mode (1) : \n[1] Standard\n[2] Rainbow\n" << BLUE;
    std::cin >> COLORIZATION;
    std::cout << GREEN << "Enter DEEP compute (300) : \n" << BLUE;
    std::cin >> DEPTH;
    if (COLORIZATION == 2) {
        std::cout << GREEN << "Enter ADD_HUE (145), COEF_HUE (260), SAT (1) : \n" << BLUE;
        std::cin >> ADD_HUE >> COEF_HUE >> SAT;
    } else if (COLORIZATION == 1) {
        std::cout << GREEN << "Enter Red coef (9), Green coef (15) and Blue coef (8.5) : \n" << BLUE;
        std::cin >> RGB_COEF[0] >> RGB_COEF[1] >> RGB_COEF[2];
    }
    if (TYPE == 2) {
        std::cout << GREEN << "Enter CX (-0.7), CY (0.3) : \n" << BLUE;
        std::cin >> C.first >> C.second;
        fractal = new JuliaSet(1200, 900, DEPTH, threads, COLORIZATION, RGB_COEF, C);
    } else if (TYPE == 1) {
        fractal = new MandelbrotSet(1200, 900, DEPTH, threads, COLORIZATION, RGB_COEF);
    }

    sf::RenderWindow window(sf::VideoMode({1200, 900}), "Fractal preview");
    sf::Image image({1200, 900}, sf::Color::Black);
    sf::Texture texture(image);
    sf::Sprite sprite(texture);

    fractal->compute_fractal();
    fractal->render_fractal(image);
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
                    fractal->reset_zoom();
                    fractal->compute_fractal();
                    fractal->render_fractal(image);
                    texture.update(image);
                } else if (keyPressed->code == sf::Keyboard::Key::U) {
                    std::pair<std::pair<double, double>, std::pair<double, double>> old_view = fractal->history.top();
                    fractal->zoom(old_view.first.first, old_view.first.second, old_view.second.first, old_view.second.second);
                    fractal->compute_fractal();
                    fractal->render_fractal(image);
                    texture.update(image);
                    if (fractal->history.top() != std::make_pair(std::make_pair(-2.0, 2.0), std::make_pair(-1.5, 1.5))) {
                        fractal->history.pop();
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
                    double new_x_min = fractal->x_min + (fractal->x_max - fractal->x_min) * std::min(start.x, end.x) / 1200;
                    double new_x_max = fractal->x_min + (fractal->x_max - fractal->x_min) * std::max(start.x, end.x) / 1200;
                    double new_y_min = fractal->y_min + (fractal->y_max - fractal->y_min) * std::min(start.y, end.y) / 900;
                    double new_y_max = fractal->y_min + (fractal->y_max - fractal->y_min) * std::max(start.y, end.y) / 900;

                    fractal->history.push(std::make_pair(std::make_pair(fractal->x_min, fractal->x_max), std::make_pair(fractal->y_min, fractal->y_max)));
                    fractal->zoom(new_x_min, new_x_max, new_y_min, new_y_max);
                    fractal->compute_fractal();
                    fractal->render_fractal(image);
                    texture.update(image);
                }
            }
        }
        window.clear();
        window.draw(sprite);
        window.display();
    }
    delete fractal;
    return 0;
}
