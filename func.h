//212-Ким-генерация 2D гауссовых облаков под пирамидами

#pragma once

#include <vector>
#include <string>
#include <utility>

//212-Ким-класс точки на плоскости
class point {
private:
    float x;
    float y;

public:
    point();
    point(float x, float y);
    virtual ~point() = default;

    float getX() const;
    float getY() const;
    void  setX(float newX);
    void  setY(float newY);

    virtual void print() const;
};

//212-Ким-класс круга: точка + радиус
class circle : virtual public point {
private:
    float r;

public:
    circle();
    circle(float x, float y, float r);
    virtual ~circle() = default;

    float getR() const;
    void  setR(float newR);
    float area() const;

    void print() const override;
};

//212-Ким-класс пирамиды: круг + высота
class pyramid : public circle {
private:
    float h;

public:
    pyramid();
    pyramid(float x, float y, float r, float h);
    virtual ~pyramid() = default;

    float getH() const;
    void  setH(float newH);

    float volume() const;
    float meanValue() const;

    void print() const override;
};

//212-Ким-чтение пирамид из файла. Формат строки: x y r h
std::vector<pyramid> readPyramidsFromFile(const std::string& filename);

//212-Ким-генерация 2D-гауссова облака точек
std::vector<std::pair<float, float>> generateGaussianCloud(
    float cx, float cy,
    float sigmaX, float sigmaY,
    float rho, int count);

//212-Ким-визуализация облаков через Gnuplot
void visualizeClouds(
    const std::vector<std::vector<std::pair<float, float>>>& allClouds,
    const std::string& filename = "clouds.png");
