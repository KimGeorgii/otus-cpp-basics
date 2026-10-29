//212-Ким-задача конусы с волной, с МПК и k-means
#pragma once

#include <vector>
#include <string>
#include <utility>
#include <iostream>

//212-Ким-структуры данных
struct cluster {
    float cx;
    float cy;
    int   size;
};

struct edge {
    int   i;
    int   j;
    float length;
};

struct clusteringResult {
    std::vector<cluster>              clusters;
    std::vector<int>                  pointClusterId;
    std::vector<std::pair<int, int>>  edges;
};

//212-Ким-класс матрицы расстояний
class DistanceMatrix {
private:
    int n;
    std::vector<float> dist;
public:
    DistanceMatrix();
    void build(const std::vector<std::pair<float, float>>& points);
    float get(int i, int j) const { return dist[i * n + j]; }
    int size() const { return n; }
};

//212-Ким-классы модели
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

//212-Ким-функции утилиты
std::vector<pyramid> readPyramidsFromFile(const std::string& filename);
void sortPyramidsByMeanValue(std::vector<pyramid>& pyramids);
std::vector<std::pair<float, float>> generateGaussianCloud(
    float cx, float cy, float sigmaX, float sigmaY, float rho, int count);

//212-Ким-МПК
std::vector<edge> mstBruteForce(const DistanceMatrix& matrix, const std::vector<std::pair<float, float>>& points);
std::vector<edge> mstQuadraticNested(const DistanceMatrix& matrix, const std::vector<std::pair<float, float>>& points);

//212-Ким-волновой алгоритм
clusteringResult graphWaveClustering(
    const DistanceMatrix& matrix, 
    const std::vector<std::pair<float, float>>& points,
    float threshold, int minClusterSize);

//212-Ким-k-means
clusteringResult kMeansClustering(
    const std::vector<std::pair<float, float>>& points, 
    int k, bool& useSqrt);

//212-Ким-визуализация
void visualizePyramids(
    const std::vector<pyramid>& pyramids,
    const std::vector<std::pair<float, float>>& allPoints,
    const clusteringResult& result);

void visualizeEdgeHistogram(
    const std::vector<edge>& edgesBrute,
    const std::vector<edge>& edgesNested,
    const std::string& outputName = "mst_histogram.png");

//212-Ким-сервер
class Server {
private:
    std::vector<pyramid> pyramids;
    std::vector<std::vector<std::pair<float, float>>> allClouds;
    
    // Теперь храним ОДИН общий результат для всей сцены
    clusteringResult lastResult;
    
    DistanceMatrix fullDistanceMatrix;
    std::vector<edge> edgesBrute;
    std::vector<edge> edgesNested;

    bool dataLoaded = false;
    bool generated = false;
    bool clustered = false;
    bool mstBuilt = false;

    const int GAUSS_COUNT = 200;

public:
    void loadData(const std::string& filename);
    void generateData();
    void runClustering(float userThreshold = 0.0f);
    void runKMeans(int k);
    void buildMST();
    void visualize() const;

    bool isDataLoaded() const { return dataLoaded; }
    bool isGenerated() const { return generated; }
};

//212-Ким-управляющий класс Control
class Control {
private:
    Server server;
public:
    void executeCommand(const std::string& command, float value = 0.0f, int k = 0);
};