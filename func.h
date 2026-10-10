//212-Ким-задача о пирамидах с алгоритмом "волна"

#pragma once

#include <vector>
#include <string>
#include <utility>

//212-Ким-кластер: центр и количество точек
struct cluster {
    float cx;   // центр кластера по X
    float cy;   // центр кластера по Y
    int   size; // количество точек в кластере
};

//212-Ким-результат кластеризации.
//pointClusterId == -1 означает "мусор" (точка не попала в кластер).
struct clusteringResult {
    std::vector<cluster> clusters;       // найденные кластеры
    std::vector<int>     pointClusterId; // для каждой точки — индекс кластера или -1
};

//212-Ким-класс точки на плоскости. Поля приватные — доступ через get/set.
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

//212-Ким-класс круга: точка + радиус.
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

//212-Ким-класс пирамиды: круг + высота.
class pyramid : public circle {
private:
    float h;

public:
    pyramid();
    pyramid(float x, float y, float r, float h);
    virtual ~pyramid() = default;

    float getH() const;
    void  setH(float newH);

    float volume() const;    // (1/3) * S_основания * h
    float meanValue() const; // среднее (area + volume) / 2

    void print() const override;
};

//212-Ким-чтение пирамид из файла. Формат строки: x y r h
std::vector<pyramid> readPyramidsFromFile(const std::string& filename);

//212-Ким-сортировка по возрастанию meanValue()
void sortPyramidsByMeanValue(std::vector<pyramid>& pyramids);

//212-Ким-генерация 2D-гауссова облака точек
std::vector<std::pair<float, float>> generateGaussianCloud(
    float cx, float cy,
    float sigmaX, float sigmaY,
    float rho, int count);

//212-Ким-матрично-волновой алгоритм кластеризации
clusteringResult matrixWaveClustering(
    const std::vector<std::pair<float, float>>& points,
    float cellSize,
    int   minClusterSize = 100);

//212-Ким-визуализация через Gnuplot
void visualizePyramids(
    const std::vector<pyramid>& pyramids,
    const std::vector<std::vector<std::pair<float, float>>>& allClouds,
    const std::vector<clusteringResult>& allResults);
