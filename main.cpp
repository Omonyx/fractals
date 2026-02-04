#include <complex>
#include <fstream>
#include <iostream>
#include <mutex>
#include <vector>
#include <thread>
#include <SFML/Graphics.hpp>
#include "application.cpp"

int main() {
    int width = 600;
    int height = 450;
    int max_iter = 100;
    int num_threads = std::thread::hardware_concurrency();

    Application application(width, height, max_iter, num_threads);
    application.run();

    return 0;
}