//212-Ким-задача о пирамидах с алгоритмом "волна"

#pragma once

#include <vector>
#include <string>
#include <utility>

//212-Ким-кластер: центр и количество точек
struct cluster {
    float cx;
    float cy;
    int   size;
};

//212-Ким-ребро графа (i, j — индексы точек; length — длина)
struct edge {
    int   i;
    int   j;
    float length;
};

//212-Ким-результат кластеризации.
//pointClusterId == -1 означает "мусор" (точка не попала в кластер).
//edges — рёбра графа, на котором ищутся компоненты связности
//(для визуализации: если i-j соединены в алгоритме, они соединяются и в gnuplot).
struct clusteringResult {
    std::vector<cluster>              clusters;
    std::vector<int>                  pointClusterId;
    std::vector<std::pair<int, int>>  edges;
};

//212-Ким-класс точки на плоскости.
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

    float volume() const;
    float meanValue() const;

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

//212-Ким-минимальное покрывающее дерево (MST) — БЕЗ оптимизации.
//Для каждой точки перебираются все остальные точки.
std::vector<edge> mstBruteForce(
    const std::vector<std::pair<float, float>>& points);

//212-Ким-минимальное покрывающее дерево (MST) — С квадратично-гнездовой
//оптимизацией: плоскость разбивается на квадраты, при обновлении ключей
//проверяются только 9 ячеек (своя + 8 соседних).
std::vector<edge> mstQuadraticNested(
    const std::vector<std::pair<float, float>>& points);

//212-Ким-волновой кластер-анализ:
//точки, расстояние между которыми <= threshold, соединяются ребром;
//кластеры = компоненты связности. Рёбра возвращаются для визуализации.
clusteringResult graphWaveClustering(
    const std::vector<std::pair<float, float>>& points,
    float threshold,
    int   minClusterSize = 100);

//212-Ким-визуализация через Gnuplot
void visualizePyramids(
    const std::vector<pyramid>& pyramids,
    const std::vector<std::vector<std::pair<float, float>>>& allClouds,
    const std::vector<clusteringResult>& allResults);

//212-Ким-гистограмма длин рёбер MST (два варианта на одном графике)
void visualizeEdgeHistogram(
    const std::vector<edge>& edgesBrute,
    const std::vector<edge>& edgesNested,
    const std::string&       outputName = "mst_histogram.png");