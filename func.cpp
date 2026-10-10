//212-Ким-генерация 2D гауссовых облаков под пирамидами

#include "func.h"

#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <random>
#include <cstdlib>

//212-Ким-класс point
point::point() : x(0.0f), y(0.0f) {}

point::point(float x_, float y_) : x(x_), y(y_) {}

float point::getX() const { return x; }
float point::getY() const { return y; }

void point::setX(float newX) { x = newX; }
void point::setY(float newY) { y = newY; }

void point::print() const {
    std::cout << "Point(" << x << ", " << y << ")";
}

//212-Ким-класс circle
circle::circle() : point(), r(0.0f) {}

circle::circle(float x_, float y_, float r_) : point(x_, y_), r(r_) {}

float circle::getR() const { return r; }
void  circle::setR(float newR) { r = newR; }

float circle::area() const {
    return 3.14159265358979323846f * r * r;
}

void circle::print() const {
    std::cout << "Circle(" << getX() << ", " << getY()
              << ", r=" << r << ")";
}

//212-Ким-класс pyramid
pyramid::pyramid() : point(), circle(), h(0.0f) {}

pyramid::pyramid(float x_, float y_, float r_, float h_)
    : point(x_, y_), circle(x_, y_, r_), h(h_) {}

float pyramid::getH() const { return h; }
void  pyramid::setH(float newH) { h = newH; }

float pyramid::volume() const {
    return (1.0f / 3.0f) * area() * h;
}

float pyramid::meanValue() const {
    return (area() + volume()) / 2.0f;
}

void pyramid::print() const {
    std::cout << "Pyramid(" << getX() << ", " << getY()
              << ", r=" << getR() << ", h=" << h << ")";
}

//212-Ким-чтение пирамид из файла
std::vector<pyramid> readPyramidsFromFile(const std::string& filename) {
    std::vector<pyramid> pyramids;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << filename << "\n";
        return pyramids;
    }

    float x, y, r, h;
    while (file >> x >> y >> r >> h) {
        pyramids.emplace_back(x, y, r, h);
    }

    file.close();
    return pyramids;
}

//212-Ким-генерация 2D-гауссова облака
std::vector<std::pair<float, float>> generateGaussianCloud(
    float cx, float cy,
    float sigmaX, float sigmaY,
    float rho, int count)
{
    std::vector<std::pair<float, float>> points;
    points.reserve(count);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> distX(0.0f, 1.0f);
    std::normal_distribution<float> distY(0.0f, 1.0f);

    for (int i = 0; i < count; ++i) {
        float z1 = distX(gen);
        float z2 = distY(gen);
        float x = cx + sigmaX * z1;
        float y = cy + sigmaY * (rho * z1 + std::sqrt(1.0f - rho * rho) * z2);

        points.emplace_back(x, y);
    }

    return points;
}

//212-Ким-визуализация облаков
void visualizeClouds(
    const std::vector<std::vector<std::pair<float, float>>>& allClouds,
    const std::string& filename)
{
    std::ofstream script("clouds.gnuplot");
    if (!script.is_open()) {
        std::cerr << "Cannot create gnuplot script\n";
        return;
    }

    //границы сцены по всем точкам
    float minX = 0.0f, maxX = 0.0f, minY = 0.0f, maxY = 0.0f;
    bool first = true;
    for (const auto& cloud : allClouds) {
        for (const auto& p : cloud) {
            if (first) {
                minX = maxX = p.first;
                minY = maxY = p.second;
                first = false;
            } else {
                minX = std::min(minX, p.first);
                maxX = std::max(maxX, p.first);
                minY = std::min(minY, p.second);
                maxY = std::max(maxY, p.second);
            }
        }
    }
    float marginX = (maxX - minX) * 0.1f + 1.0f;
    float marginY = (maxY - minY) * 0.1f + 1.0f;

    //датасет на каждое облако
    for (size_t i = 0; i < allClouds.size(); ++i) {
        script << "$cloud_" << i << " << EOD\n";
        for (const auto& p : allClouds[i]) {
            script << p.first << " " << p.second << "\n";
        }
        script << "EOD\n";
    }

    //настройки терминала
    script << "set terminal png size 1400,1000\n";
    script << "set output '" << filename << "'\n";
    script << "set title '2D Gaussian clouds'\n";
    script << "set xlabel 'X'\n";
    script << "set ylabel 'Y'\n";
    script << "set grid\n";
    script << "set size ratio -1\n";
    script << "set xrange [" << (minX - marginX) << ":" << (maxX + marginX) << "]\n";
    script << "set yrange [" << (minY - marginY) << ":" << (maxY + marginY) << "]\n";
    script << "plot ";

    //все облака — одним цветом (steelblue)
    for (size_t i = 0; i < allClouds.size(); ++i) {
        if (i > 0) script << ", ";
        script << "$cloud_" << i
               << " using 1:2 with dots "
               << "lc rgb 'steelblue' "
               << "title 'Cloud " << (i + 1) << "'";
    }
    script << "\n";

    script.close();

    //запуск Gnuplot
    int result = std::system("\"C:\\Program Files\\gnuplot\\bin\\gnuplot.exe\" clouds.gnuplot");
    if (result == 0) {
        std::cout << "Visualization saved to " << filename << "\n";
    } else {
        std::cerr << "Error running gnuplot\n";
    }
}
