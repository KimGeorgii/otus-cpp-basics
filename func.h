//212-Ким-задача о конусах (Клиент-Сервер)
#pragma once

#include <vector>
#include <string>
#include <utility>
#include <iostream>

//212-Ким-структура кластера (центр и размер)
struct cluster {
    float cx;
    float cy;
    int   size;
};

//212-Ким-структура ребра графа (индексы точек и длина)
struct edge {
    int   i;
    int   j;
    float length;
};

//212-Ким-структура результата кластеризации
struct clusteringResult {
    std::vector<cluster>              clusters;
    std::vector<int>                  pointClusterId;
    std::vector<std::pair<int, int>>  edges;
};

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

//212-Ким-класс круга (наследуется от point)
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

//212-Ким-класс пирамиды (наследуется от circle)
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

//212-Ким-чтение пирамид из файла
std::vector<pyramid> readPyramidsFromFile(const std::string& filename);

//212-Ким-сортировка пирамид по возрастанию meanValue
void sortPyramidsByMeanValue(std::vector<pyramid>& pyramids);

//212-Ким-генерация 2D-гауссова облака точек
std::vector<std::pair<float, float>> generateGaussianCloud(
    float cx, float cy, float sigmaX, float sigmaY, float rho, int count);

//212-Ким-построение МПК без оптимизации (полный перебор)
std::vector<edge> mstBruteForce(const std::vector<std::pair<float, float>>& points);

//212-Ким-построение МПК с квадратично-гнездовой оптимизацией
std::vector<edge> mstQuadraticNested(const std::vector<std::pair<float, float>>& points);

//212-Ким-волновой кластер-анализ (на графе)
clusteringResult graphWaveClustering(
    const std::vector<std::pair<float, float>>& points, float threshold, int minClusterSize);

//212-Ким-визуализация пирамид, точек и рёбер через Gnuplot
void visualizePyramids(
    const std::vector<pyramid>& pyramids,
    const std::vector<std::vector<std::pair<float, float>>>& allClouds,
    const std::vector<clusteringResult>& allResults);

//212-Ким-визуализация гистограммы длин рёбер МПК через Gnuplot
void visualizeEdgeHistogram(
    const std::vector<edge>& edgesBrute,
    const std::vector<edge>& edgesNested,
    const std::string&       outputName = "mst_histogram.png");

//212-Ким-класс сервера (хранит данные и выполняет алгоритмы)
class Server {
private:
    std::vector<pyramid> pyramids;
    std::vector<std::vector<std::pair<float, float>>> allClouds;
    std::vector<clusteringResult> allResults;
    std::vector<edge> edgesBrute;
    std::vector<edge> edgesNested;

    bool dataLoaded = false;
    bool generated = false;
    bool clustered = false;
    bool mstBuilt = false;

    const int GAUSS_COUNT = 1000;

public:
    void loadData(const std::string& filename);
    void generateData();
    void runClustering();
    void buildMST();
    void visualize() const;

    bool isDataLoaded() const { return dataLoaded; }
    bool isGenerated() const { return generated; }
};

//212-Ким-управляющий класс Control (посредник между клиентом и сервером)
class Control {
private:
    Server server;
public:
    void executeCommand(const std::string& command);
};