#include <complex>
#include <fstream>
#include <iostream>
#include <mutex>
#include <vector>
#include <thread>
#include <SFML/Graphics.hpp>
#include "mandelbrotPlus.cpp"

class Application {
 public:
  Application(int width, int height, int max_iter, int num_threads) : m_fractal(Mandelbrot(width, height, max_iter, num_threads)), m_window(sf::VideoMode(width, height), "Mandelbrot set", sf::Style::Titlebar | sf::Style::Close) {
    m_image.create(m_fractal.WIDTH, m_fractal.HEIGHT);
    drawFractal();
    m_texture.loadFromImage(m_image);
    m_sprite.setTexture(m_texture);
    m_selectionRect.setFillColor(sf::Color(0, 0, 0, 0));
    m_selectionRect.setOutlineColor(sf::Color::Red);
    m_selectionRect.setOutlineThickness(3);

    if (!m_font.loadFromFile("TimesNewRoman.ttf")) {
      std::cout << "Please, provide a Font!\n";
    }
    createButton(width - 150, 5, m_changeColorButton, m_changeColorButtonText,
                 "Change color");
    createButton(width - 150, 50, m_resetZoomingButton,
                 m_resetZoomingButtonText, "Reset Zooming");
  }

  void run() {
    while (m_window.isOpen()) {
      sf::Event event;
      while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
          m_window.close();
          return;
        }
        handleEvent(event);
        break;
      }
      m_window.draw(m_sprite);
      if (m_dragging) {
        m_window.draw(m_selectionRect);
      }
      m_window.draw(m_changeColorButton);
      m_window.draw(m_changeColorButtonText);
      m_window.draw(m_resetZoomingButton);
      m_window.draw(m_resetZoomingButtonText);

      m_window.display();
    }
  }

 private:
  void drawFractal() {
    m_fractal.calcul_fractal();
    for (int y = 0; y < m_fractal.HEIGHT; ++y) {
      for (int x = 0; x < m_fractal.WIDTH; ++x) {
        int iter = m_fractal.image[y * m_fractal.WIDTH + x];
        sf::Color color = getColor(iter);
        m_image.setPixel(x, y, color);
      }
    }
  }

  sf::Color getColor(int iter) {
    if (iter < m_fractal.MAX_ITER) {
      switch (m_COLOR) {
        case 1:
          return sf::Color(0, 255 * iter / m_fractal.MAX_ITER,
                           255 * iter / m_fractal.MAX_ITER);
        case 2:
          return sf::Color(255 * iter / m_fractal.MAX_ITER, 0,
                           255 * iter / m_fractal.MAX_ITER);
        case 3:
          return sf::Color(255 * iter / m_fractal.MAX_ITER,
                           255 * iter / m_fractal.MAX_ITER, 0);
      }
    }
    return sf::Color::Black;
  }

  void handleEvent(sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
      if (m_changeColorButton.getGlobalBounds().contains(event.mouseButton.x,
                                                         event.mouseButton.y)) {
        changeColorButtonHandler();

      } else if (m_resetZoomingButton.getGlobalBounds().contains(
                     event.mouseButton.x, event.mouseButton.y)) {
        resetZoomingButtonHandler();

      } else {
        m_dragging = true;
        m_start = sf::Mouse::getPosition(m_window);
      }
      return;
    }
    if (event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Left && m_dragging) {
      handleZoomingIn();
      return;
    }

    if (m_dragging) {
      handleDragging();
      return;
    }

    if (event.type == sf::Event::MouseWheelScrolled &&
        event.mouseWheelScroll.delta != 0) {
      ZoomByScrolling(event.mouseWheelScroll.delta, event.mouseWheelScroll.x,
                      event.mouseWheelScroll.y);
    }
  }

  void changeColorButtonHandler() {
    m_COLOR = (m_COLOR % 3) + 1;
    drawFractal();
    m_texture.loadFromImage(m_image);
    m_sprite.setTexture(m_texture);
  }

  void resetZoomingButtonHandler() {
    m_fractal.resetZoom();
    drawFractal();
    m_texture.loadFromImage(m_image);
    m_sprite.setTexture(m_texture);
  }

  void handleZoomingIn() {
    m_dragging = false;
    m_end = sf::Mouse::getPosition(m_window);
    if (std::abs(m_start.x - m_end.x) < EPS &&
        std::abs(m_start.y - m_end.y) < EPS) {
      return;
    }
    double widthRatio = 4.0 / 3.0;
    double newWidth = m_end.x - m_start.x;
    double newHeight = newWidth / widthRatio;

    if (newHeight < 0)
      newHeight = -newHeight;
    if (m_end.y < m_start.y) {
      m_end.y = m_start.y - newHeight;
    } else {
      m_end.y = m_start.y + newHeight;
    }
    double realMin =
        m_fractal.x_min + (m_fractal.x_max - m_fractal.x_min) *
                                 std::min(m_start.x, m_end.x) / m_fractal.WIDTH;
    double realMax =
        m_fractal.x_min + (m_fractal.x_max - m_fractal.x_min) *
                                 std::max(m_start.x, m_end.x) / m_fractal.WIDTH;
    double imagMin = m_fractal.y_min +
                     (m_fractal.y_max - m_fractal.y_min) *
                         std::min(m_start.y, m_end.y) / m_fractal.HEIGHT;
    double imagMax = m_fractal.y_min +
                     (m_fractal.y_max - m_fractal.y_min) *
                         std::max(m_start.y, m_end.y) / m_fractal.HEIGHT;
    m_selectionRect.setSize(sf::Vector2f(0, 0));

    m_fractal.zoomFractal(realMin, realMax, imagMin, imagMax);
    drawFractal();
    m_texture.loadFromImage(m_image);
    m_sprite.setTexture(m_texture);
  }

  void handleDragging() {
    m_end = sf::Mouse::getPosition(m_window);

    double newWidth = std::abs(m_end.x - m_start.x);
    double newHeight =
        newWidth * 3.0 / 4.0;

    if (m_end.x > m_start.x && m_end.y < m_start.y) {
      m_end.y = m_start.y - newHeight;
    } else if (m_end.x < m_start.x && m_end.y < m_start.y) {
      m_end.y = m_start.y - newHeight;
      m_end.x = m_start.x - newWidth;
    } else if (m_end.x < m_start.x && m_end.y > m_start.y) {
      m_end.x = m_start.x - newWidth;
    } else {
      m_end.y = m_start.y + newHeight;
    }

    m_selectionRect.setPosition(sf::Vector2f(std::min(m_start.x, m_end.x),
                                             std::min(m_start.y, m_end.y)));
    m_selectionRect.setSize(sf::Vector2f(newWidth, newHeight));
  }

  void ZoomByScrolling(float delta, int mouseX, int mouseY) {
    double zoomFactor = (delta > 0) ? 0.9 : 1.1;
    double mouseRe =
        m_fractal.x_min +
        (m_fractal.x_max - m_fractal.x_min) * mouseX / m_fractal.WIDTH;
    double mouseIm =
        m_fractal.y_min +
        (m_fractal.y_max - m_fractal.y_min) * mouseY / m_fractal.HEIGHT;
    double newWidth = (m_fractal.x_max - m_fractal.x_min) * zoomFactor;
    double newHeight = (m_fractal.y_max - m_fractal.y_min) * zoomFactor;
    double newRealMin = mouseRe - (mouseRe - m_fractal.x_min) * zoomFactor;
    double newRealMax = newRealMin + newWidth;
    double newImagMin = mouseIm - (mouseIm - m_fractal.y_min) * zoomFactor;
    double newImagMax = newImagMin + newHeight;

    m_fractal.zoomFractal(newRealMin, newRealMax, newImagMin, newImagMax);
    drawFractal();
    m_texture.loadFromImage(m_image);
    m_sprite.setTexture(m_texture);
  }

  void createButton(int x, int y, sf::RectangleShape& button, sf::Text& text,
                    const std::string& title) {
    button = sf::RectangleShape(sf::Vector2f(125, 30));
    button.setFillColor(sf::Color::Black);
    button.setPosition(x, y);
    button.setOutlineThickness(2);
    button.setOutlineColor(sf::Color::White);

    text.setFont(m_font);
    text.setString(title);
    text.setCharacterSize(15);
    text.setFillColor(sf::Color::White);
    text.setPosition(button.getPosition().x + 10, button.getPosition().y + 5);
  }

  Mandelbrot m_fractal;
  sf::RenderWindow m_window;

  sf::Image m_image;
  sf::Texture m_texture;
  sf::Sprite m_sprite;

  sf::Vector2i m_start;
  sf::Vector2i m_end;
  sf::RectangleShape m_selectionRect;

  sf::RectangleShape m_changeColorButton;
  sf::Text m_changeColorButtonText;
  sf::RectangleShape m_resetZoomingButton;
  sf::Text m_resetZoomingButtonText;
  sf::Font m_font;

  constexpr static const double EPS = 1e-10;
  int m_COLOR = 1;
  bool m_dragging = false;
};
