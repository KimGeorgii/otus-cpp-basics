//212-Ким-задача конусы с волной, с МПК и k-means

#include "func.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <random>
#include <queue>
#include <cstdlib>
#include <cstdio>
#include <limits>
#include <chrono>

//212-Ким-реализация матрицы расстояний
DistanceMatrix::DistanceMatrix() : n(0) {}

//212-Ким-построение матрицы D
void DistanceMatrix::build(const std::vector<std::pair<float, float>>& points) {
    n = static_cast<int>(points.size());
    dist.resize(n * n, 0.0f);
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            float dx = points[i].first - points[j].first;
            float dy = points[i].second - points[j].second;
            float d = std::sqrt(dx * dx + dy * dy);
            dist[i * n + j] = d;
            dist[j * n + i] = d;
        }
    }
}

//212-Ким-классы модели
point::point() : x(0.0f), y(0.0f) {}
point::point(float x_, float y_) : x(x_), y(y_) {}
float point::getX() const { return x; }
float point::getY() const { return y; }
void point::setX(float newX) { x = newX; }
void point::setY(float newY) { y = newY; }
void point::print() const { std::cout << "Point(" << x << ", " << y << ")"; }

circle::circle() : point(), r(0.0f) {}
circle::circle(float x_, float y_, float r_) : point(x_, y_), r(r_) {}
float circle::getR() const { return r; }
void  circle::setR(float newR) { r = newR; }
float circle::area() const { return 3.14159265358979323846f * r * r; }
void circle::print() const { std::cout << "Circle(" << getX() << ", " << getY() << ", r=" << r << ")"; }

pyramid::pyramid() : point(), circle(), h(0.0f) {}
pyramid::pyramid(float x_, float y_, float r_, float h_) : point(x_, y_), circle(x_, y_, r_), h(h_) {}
float pyramid::getH() const { return h; }
void  pyramid::setH(float newH) { h = newH; }
float pyramid::volume() const { return (1.0f / 3.0f) * area() * h; }
float pyramid::meanValue() const { return (area() + volume()) / 2.0f; }
void pyramid::print() const { std::cout << "Pyramid(" << getX() << ", " << getY() << ", r=" << getR() << ", h=" << h << ")"; }

//212-Ким-чтение данных из файла
std::vector<pyramid> readPyramidsFromFile(const std::string& filename) {
    std::vector<pyramid> pyramids;
    std::ifstream file(filename);
    if (!file.is_open()) return pyramids;
    float x, y, r, h;
    while (file >> x >> y >> r >> h) pyramids.emplace_back(x, y, r, h);
    return pyramids;
}

//212-Ким-сортировка по среднему значению
void sortPyramidsByMeanValue(std::vector<pyramid>& pyramids) {
    std::sort(pyramids.begin(), pyramids.end(),
        [](const pyramid& a, const pyramid& b) { return a.meanValue() < b.meanValue(); });
}

//212-Ким-генерация гауссова облака
std::vector<std::pair<float, float>> generateGaussianCloud(
    float cx, float cy, float sigmaX, float sigmaY, float rho, int count)
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

//212-Ким-алгоритмы МПК (полный перебор)
std::vector<edge> mstBruteForce(const DistanceMatrix& matrix, const std::vector<std::pair<float, float>>& points) {
    std::vector<edge> result;
    int n = matrix.size();
    if (n < 2) return result;
    const float INF = std::numeric_limits<float>::max();
    std::vector<bool> inMST(n, false);
    std::vector<float> key(n, INF);
    std::vector<int> parent(n, -1);
    key[0] = 0.0f;

    for (int iter = 0; iter < n; ++iter) {
        int u = -1; float best = INF;
        for (int i = 0; i < n; ++i) if (!inMST[i] && key[i] < best) { best = key[i]; u = i; }
        if (u == -1) break;
        inMST[u] = true;
        if (parent[u] != -1) result.push_back({parent[u], u, key[u]});
        for (int v = 0; v < n; ++v) {
            if (inMST[v]) continue;
            //212-Ким-получение расстояния из матрицы
            float d = matrix.get(u, v);
            if (d < key[v]) { key[v] = d; parent[v] = u; }
        }
    }
    return result;
}

//212-Ким-алгоритмы МПК (квадратично-гнездовой)
std::vector<edge> mstQuadraticNested(const DistanceMatrix& matrix, const std::vector<std::pair<float, float>>& points) {
    std::vector<edge> result;
    int n = matrix.size();
    if (n < 2) return result;
    const float INF = std::numeric_limits<float>::max();

    float minX = points[0].first, maxX = points[0].first;
    float minY = points[0].second, maxY = points[0].second;
    for (const auto& p : points) {
        minX = std::min(minX, p.first); maxX = std::max(maxX, p.first);
        minY = std::min(minY, p.second); maxY = std::max(maxY, p.second);
    }
    float W = std::max(maxX - minX, 1e-6f);
    float H = std::max(maxY - minY, 1e-6f);
    float avgSpacing = std::sqrt(W * H / static_cast<float>(n));
    float cellSize = avgSpacing * 3.0f;

    int gridW = static_cast<int>(W / cellSize) + 1;
    int gridH = static_cast<int>(H / cellSize) + 1;
    gridW = std::min(gridW, 2000); gridH = std::min(gridH, 2000);

    auto cellIdx = [&](int gx, int gy) { return gy * gridW + gx; };
    std::vector<std::vector<int>> cellPoints(gridW * gridH);
    std::vector<int> gxi(n), gyi(n);

    for (int i = 0; i < n; ++i) {
        int gx = static_cast<int>((points[i].first - minX) / cellSize);
        int gy = static_cast<int>((points[i].second - minY) / cellSize);
        gx = std::max(0, std::min(gridW - 1, gx));
        gy = std::max(0, std::min(gridH - 1, gy));
        gxi[i] = gx; gyi[i] = gy;
        cellPoints[cellIdx(gx, gy)].push_back(i);
    }

    std::vector<bool> inMST(n, false);
    std::vector<float> key(n, INF);
    std::vector<int> parent(n, -1);
    key[0] = 0.0f;

    for (int iter = 0; iter < n; ++iter) {
        int u = -1; float best = INF;
        for (int i = 0; i < n; ++i) if (!inMST[i] && key[i] < best) { best = key[i]; u = i; }
        if (u == -1 || best == INF) break;
        inMST[u] = true;
        if (parent[u] != -1) result.push_back({parent[u], u, key[u]});

        int ux = gxi[u], uy = gyi[u];
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                int nx = ux + dx, ny = uy + dy;
                if (nx < 0 || ny < 0 || nx >= gridW || ny >= gridH) continue;
                for (int v : cellPoints[cellIdx(nx, ny)]) {
                    if (inMST[v]) continue;
                    //212-Ким-получение расстояния из матрицы
                    float d = matrix.get(u, v);
                    if (d < key[v]) { key[v] = d; parent[v] = u; }
                }
            }
        }
    }
    //212-Ким-резервный поиск для изолированных точек
    for (int i = 0; i < n; ++i) {
        if (inMST[i]) continue;
        int bestU = -1; float best = INF;
        for (int j = 0; j < n; ++j) {
            if (!inMST[j]) continue;
            //212-Ким-получение расстояния из матрицы
            float d = matrix.get(i, j);
            if (d < best) { best = d; bestU = j; }
        }
        if (bestU != -1) { result.push_back({bestU, i, best}); inMST[i] = true; }
    }
    return result;
}

//212-Ким-волновой алгоритм (поиск кластеров)
clusteringResult graphWaveClustering(
    const DistanceMatrix& matrix, 
    const std::vector<std::pair<float, float>>& points,
    float threshold, int minClusterSize)
{
    clusteringResult result;
    int n = matrix.size();
    if (n == 0) return result;
    result.pointClusterId.assign(n, -1);

    //212-Ким-построение матрицы инцидентности B в битах
    std::vector<bool> B(n * n, false); 
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            //212-Ким-получение расстояния из матрицы
            if (matrix.get(i, j) <= threshold) {
                B[i * n + j] = true;
                B[j * n + i] = true;
                result.edges.push_back({i, j});
            }
        }
    }

    std::vector<bool> visited(n, false);
    std::vector<std::vector<int>> components;

    //212-Ким-обход в ширину (BFS - Волна)
    for (int s = 0; s < n; ++s) {
        if (visited[s]) continue;
        std::queue<int> q;
        q.push(s);
        visited[s] = true;
        std::vector<int> comp;
        while (!q.empty()) {
            int v = q.front(); q.pop();
            comp.push_back(v);
            for (int u = 0; u < n; ++u) {
                if (B[v * n + u] && !visited[u]) { visited[u] = true; q.push(u); }
            }
        }
        components.push_back(std::move(comp));
    }

    //212-Ким-вычисление центров кластеров
    for (auto& comp : components) {
        if (static_cast<int>(comp.size()) < minClusterSize) continue;
        float sx = 0.0f, sy = 0.0f;
        for (int idx : comp) { sx += points[idx].first; sy += points[idx].second; }
        cluster cl;
        cl.cx = sx / comp.size(); cl.cy = sy / comp.size(); cl.size = static_cast<int>(comp.size());
        int newId = static_cast<int>(result.clusters.size());
        result.clusters.push_back(cl);
        for (int idx : comp) result.pointClusterId[idx] = newId;
    }
    return result;
}

//212-Ким-алгоритм k-means
clusteringResult kMeansClustering(
    const std::vector<std::pair<float, float>>& points, 
    int k, bool& useSqrt)
{
    clusteringResult result;
    int n = static_cast<int>(points.size());
    if (n == 0 || k <= 0) return result;
    if (k > n) k = n;

    result.pointClusterId.assign(n, -1);
    
    //212-Ким-инициализация центров случайными точками
    std::vector<std::pair<float, float>> centers(k);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distIdx(0, n - 1);
    for (int i = 0; i < k; ++i) centers[i] = points[distIdx(gen)];

    auto t1 = std::chrono::high_resolution_clock::now();
    long long countSqrt = 0, countNoSqrt = 0;
    std::vector<int> localClusterId(n, -1);
    int maxIters = 100;

    //212-Ким-Вариант 1: С вычислением обычных расстояний (с sqrt)
    for (int iter = 0; iter < maxIters; ++iter) {
        bool changed = false;
        for (int i = 0; i < n; ++i) {
            float minDist = std::numeric_limits<float>::max();
            int bestK = -1;
            for (int j = 0; j < k; ++j) {
                float dx = points[i].first - centers[j].first;
                float dy = points[i].second - centers[j].second;
                //212-Ким-вычисление расстояния с sqrt
                float d = std::sqrt(dx * dx + dy * dy);
                countSqrt++;
                if (d < minDist) { minDist = d; bestK = j; }
            }
            if (localClusterId[i] != bestK) { localClusterId[i] = bestK; changed = true; }
        }
        if (!changed) break;
        std::vector<float> sumX(k, 0), sumY(k, 0);
        std::vector<int> cnt(k, 0);
        for (int i = 0; i < n; ++i) {
            sumX[localClusterId[i]] += points[i].first; sumY[localClusterId[i]] += points[i].second; cnt[localClusterId[i]]++;
        }
        for (int j = 0; j < k; ++j) {
            if (cnt[j] > 0) { centers[j].first = sumX[j] / cnt[j]; centers[j].second = sumY[j] / cnt[j]; }
        }
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    //212-Ким-Вариант 2: С квадратами расстояний (без sqrt)
    for (int i = 0; i < k; ++i) centers[i] = points[distIdx(gen)];
    std::fill(localClusterId.begin(), localClusterId.end(), -1);

    for (int iter = 0; iter < maxIters; ++iter) {
        bool changed = false;
        for (int i = 0; i < n; ++i) {
            float minDist2 = std::numeric_limits<float>::max();
            int bestK = -1;
            for (int j = 0; j < k; ++j) {
                float dx = points[i].first - centers[j].first;
                float dy = points[i].second - centers[j].second;
                //212-Ким-вычисление квадрата расстояния (без sqrt)
                float d2 = dx * dx + dy * dy;
                countNoSqrt++;
                if (d2 < minDist2) { minDist2 = d2; bestK = j; }
            }
            if (localClusterId[i] != bestK) { localClusterId[i] = bestK; changed = true; }
        }
        if (!changed) break;
        std::vector<float> sumX(k, 0), sumY(k, 0);
        std::vector<int> cnt(k, 0);
        for (int i = 0; i < n; ++i) {
            sumX[localClusterId[i]] += points[i].first; sumY[localClusterId[i]] += points[i].second; cnt[localClusterId[i]]++;
        }
        for (int j = 0; j < k; ++j) {
            if (cnt[j] > 0) { centers[j].first = sumX[j] / cnt[j]; centers[j].second = sumY[j] / cnt[j]; }
        }
    }
    auto t3 = std::chrono::high_resolution_clock::now();
    auto msSqrt = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    auto msNoSqrt = std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2).count();
    std::cout << "  [k-means Benchmark] With sqrt: " << msSqrt << " ms, Without sqrt: " << msNoSqrt << " ms\n";

    //212-Ким-заполнение результата
    result.pointClusterId = localClusterId;
    for (int j = 0; j < k; ++j) {
        cluster cl;
        cl.cx = centers[j].first; cl.cy = centers[j].second; cl.size = 0;
        for (int i = 0; i < n; ++i) if (localClusterId[i] == j) cl.size++;
        if (cl.size > 0) result.clusters.push_back(cl);
    }
    useSqrt = (msSqrt < msNoSqrt);
    return result;
}

//212-Ким-вспомогательная функция для генерации цвета
static std::string colorByIndex(int idx) {
    const float golden = 0.618033988749895f;
    float hue = std::fmod(idx * golden, 1.0f);
    float sat = 0.55f + 0.30f * ((idx % 3) / 2.0f);
    float val = 0.75f + 0.20f * ((idx % 2) ? 1.0f : 0.0f);
    int i = static_cast<int>(hue * 6.0f);
    float f = hue * 6.0f - i;
    float p = val * (1.0f - sat); float q = val * (1.0f - f * sat); float t = val * (1.0f - (1.0f - f) * sat);
    float r, g, b;
    switch (i % 6) {
        case 0: r = val; g = t;   b = p;   break;
        case 1: r = q;   g = val; b = p;   break;
        case 2: r = p;   g = val; b = t;   break;
        case 3: r = p;   g = q;   b = val; break;
        case 4: r = t;   g = p;   b = val; break;
        default:r = val; g = p;   b = q;   break;
    }
    char buf[16];
    std::snprintf(buf, sizeof(buf), "#%02X%02X%02X", (int)(r*255), (int)(g*255), (int)(b*255));
    return std::string(buf);
}

//212-Ким-визуализация конусов и кластеров
void visualizePyramids(
    const std::vector<pyramid>& pyramids,
    const std::vector<std::pair<float, float>>& allPoints,
    const clusteringResult& result)
{
    if (pyramids.empty()) return;
    std::ofstream script("pyramids.gnuplot");
    if (!script.is_open()) return;

    std::vector<std::string> clusterColors(result.clusters.size());
    for (size_t j = 0; j < result.clusters.size(); ++j) {
        clusterColors[j] = colorByIndex(static_cast<int>(j)); 
    }

    script << "set terminal png size 1400,1000\n";
    script << "set style fill transparent solid 0.6 border lc rgb 'black'\n";
    script << "set grid\nset xlabel 'X'\nset ylabel 'Y'\nset zlabel 'Z'\n";
    script << "set hidden3d\nset view 60, 30, 1, 1\nset xyplane at 0\n";

    float minX = pyramids[0].getX() - pyramids[0].getR();
    float maxX = pyramids[0].getX() + pyramids[0].getR();
    float minY = pyramids[0].getY() - pyramids[0].getR();
    float maxY = pyramids[0].getY() + pyramids[0].getR();
    float maxZ = pyramids[0].getH();

    for (const auto& p : pyramids) {
        float sigma = p.getR() * 0.5f;
        minX = std::min(minX, p.getX() - 4.0f * sigma); maxX = std::max(maxX, p.getX() + 4.0f * sigma);
        minY = std::min(minY, p.getY() - 4.0f * sigma); maxY = std::max(maxY, p.getY() + 4.0f * sigma);
        maxZ = std::max(maxZ, p.getH());
    }
    float marginX = (maxX - minX) * 0.2f + 1.0f;
    float marginY = (maxY - minY) * 0.2f + 1.0f;
    script << "set xrange [" << (minX - marginX) << ":" << (maxX + marginX) << "]\n";
    script << "set yrange [" << (minY - marginY) << ":" << (maxY + marginY) << "]\n";
    script << "set zrange [0:" << (maxZ * 1.2f) << "]\n";

    const int N = 40;
    const float PI = 3.14159265358979323846f;

    //212-Ким-генерация данных для конусов
    for (size_t i = 0; i < pyramids.size(); ++i) {
        const auto& p = pyramids[i];
        float cx = p.getX(), cy = p.getY(), r = p.getR(), h = p.getH();
        script << "$pyr" << i << " << EOD\n";
        for (int k = 0; k < N; ++k) {
            float a1 = 2.0f * PI * k / N; float a2 = 2.0f * PI * (k + 1) / N;
            script << (cx + r * std::cos(a1)) << " " << (cy + r * std::sin(a1)) << " 0\n";
            script << (cx + r * std::cos(a2)) << " " << (cy + r * std::sin(a2)) << " 0\n";
            script << cx << " " << cy << " " << h << "\n\n";
        }
        for (int k = 0; k < N; ++k) {
            float a = 2.0f * PI * k / N;
            script << (cx + r * std::cos(a)) << " " << (cy + r * std::sin(a)) << " 0\n";
        }
        script << "\nEOD\n";
    }

    //212-Ким-генерация данных для облаков точек
    for (size_t j = 0; j < result.clusters.size(); ++j) {
        script << "$cl_" << j << " << EOD\n";
        for (size_t k = 0; k < allPoints.size(); ++k) {
            if (result.pointClusterId[k] == static_cast<int>(j)) {
                script << allPoints[k].first << " " << allPoints[k].second << " 0\n";
            }
        }
        script << "EOD\n";
    }

    //212-Ким-генерация данных для мусора (если есть)
    bool hasTrash = false;
    for (int id : result.pointClusterId) if (id == -1) { hasTrash = true; break; }
    if (hasTrash) {
        script << "$trash << EOD\n";
        for (size_t k = 0; k < allPoints.size(); ++k) {
            if (result.pointClusterId[k] == -1) {
                script << allPoints[k].first << " " << allPoints[k].second << " 0\n";
            }
        }
        script << "EOD\n";
    }

    //212-Ким-генерация данных для РЁБЕР ПО КЛАСТЕРАМ
    std::vector<bool> hasClusterEdges(result.clusters.size(), false);
    for (size_t j = 0; j < result.clusters.size(); ++j) {
        script << "$edges_" << j << " << EOD\n";
        int count = 0;
        for (const auto& e : result.edges) {
            if (result.pointClusterId[e.first] == static_cast<int>(j) && 
                result.pointClusterId[e.second] == static_cast<int>(j)) {
                script << allPoints[e.first].first << " " << allPoints[e.first].second << " 0\n";
                script << allPoints[e.second].first << " " << allPoints[e.second].second << " 0\n\n";
                count++;
            }
        }
        if (count > 0) hasClusterEdges[j] = true;
        script << "EOD\n";
    }

    //212-Ким-генерация данных для мусорных рёбер
    bool hasTrashEdges = false;
    script << "$edges_trash << EOD\n";
    int trashEdgeCount = 0;
    for (const auto& e : result.edges) {
        if (result.pointClusterId[e.first] == -1 || result.pointClusterId[e.second] == -1) {
            script << allPoints[e.first].first << " " << allPoints[e.first].second << " 0\n";
            script << allPoints[e.second].first << " " << allPoints[e.second].second << " 0\n\n";
            trashEdgeCount++;
        }
    }
    if (trashEdgeCount > 0) hasTrashEdges = true;
    script << "EOD\n";

    //212-Ким-генерация данных для центров
    for (size_t j = 0; j < result.clusters.size(); ++j) {
        script << "$cent_" << j << " << EOD\n" << result.clusters[j].cx << " " << result.clusters[j].cy << " 0\nEOD\n";
    }

    //212-Ким-3D визуализация
    script << "set output 'pyramids_with_points.png'\n";
    script << "set title '3D Pyramids + 2D clouds'\n";
    script << "splot ";
    for (size_t i = 0; i < pyramids.size(); ++i) {
        if (i > 0) script << ", ";
        script << "$pyr" << i << " using 1:2:3 with polygons fc rgb 'steelblue' title 'Pyramid " << (i + 1) << "'";
    }
    
    // Добавляем рёбра по кластерам в 3D
    for (size_t j = 0; j < result.clusters.size(); ++j) {
        if (hasClusterEdges[j]) {
            script << ", $edges_" << j << " using 1:2:3 with lines lc rgb '" << clusterColors[j] << "' lw 0.4 notitle";
        }
    }
    if (hasTrashEdges) {
        script << ", $edges_trash using 1:2:3 with lines lc rgb '#000000' lw 0.4 notitle";
    }

    for (size_t j = 0; j < result.clusters.size(); ++j) {
        script << ", $cl_" << j << " using 1:2:(0.0) with points pt 7 ps 0.4 lc rgb '" << clusterColors[j] << "' title 'Cluster " << j + 1 << "'";
    }
    if (hasTrash) {
        script << ", $trash using 1:2:(0.0) with points pt 7 ps 0.4 lc rgb 'black' title 'Trash'";
    }
    for (size_t j = 0; j < result.clusters.size(); ++j) {
        script << ", $cent_" << j << " using 1:2:(0.0) with points pt 7 ps 2 lc rgb '" << clusterColors[j] << "' title 'Center " << j + 1 << "'";
    }
    script << "\n";

    //212-Ким-2D визуализация
    if (!allPoints.empty()) {
        script << "set output 'pyramids_points_only.png'\n";
        script << "set title '2D clouds (top view)'\n";
        script << "plot ";
        bool first = true;
        
        // Добавляем рёбра по кластерам в 2D
        for (size_t j = 0; j < result.clusters.size(); ++j) {
            if (hasClusterEdges[j]) {
                if (!first) script << ", "; first = false;
                script << "$edges_" << j << " using 1:2 with lines lc rgb '" << clusterColors[j] << "' lw 0.4 notitle";
            }
        }
        if (hasTrashEdges) {
            if (!first) script << ", "; first = false;
            script << "$edges_trash using 1:2 with lines lc rgb '#000000' lw 0.4 notitle";
        }

        for (size_t j = 0; j < result.clusters.size(); ++j) {
            if (!first) script << ", "; first = false;
            script << "$cl_" << j << " using 1:2 with points pt 7 ps 0.4 lc rgb '" << clusterColors[j] << "' title 'Cluster " << j + 1 << "'";
        }
        if (hasTrash) {
            if (!first) script << ", "; first = false;
            script << "$trash using 1:2 with points pt 7 ps 0.4 lc rgb 'black' title 'Trash'";
        }
        for (size_t j = 0; j < result.clusters.size(); ++j) {
            if (!first) script << ", "; first = false;
            script << "$cent_" << j << " using 1:2 with points pt 7 ps 2 lc rgb '" << clusterColors[j] << "' title 'Center " << j + 1 << "'";
        }
        script << "\n";
    }

    script.close();
    //212-Ким-вызов Gnuplot
    std::system("\"C:\\Program Files\\gnuplot\\bin\\gnuplot.exe\" pyramids.gnuplot");
}

//212-Ким-визуализация гистограммы длин рёбер
void visualizeEdgeHistogram(const std::vector<edge>& edgesBrute, const std::vector<edge>& edgesNested, const std::string& outputName) {
    if (edgesBrute.empty()) return;
    std::ofstream fb("edges_brute.txt");
    for (const auto& e : edgesBrute) fb << e.length << "\n";
    fb.close();
    std::ofstream fn("edges_nested.txt");
    for (const auto& e : edgesNested) fn << e.length << "\n";
    fn.close();

    std::vector<float> lens;
    for (const auto& e : edgesBrute) lens.push_back(e.length);
    std::sort(lens.begin(), lens.end());
    float threshold = 0.0f;
    if (lens.size() >= 2) {
        float maxGap = -1.0f; size_t gapIdx = 1;
        for (size_t i = 1; i < lens.size(); ++i) {
            float gap = lens[i] - lens[i - 1];
            if (gap > maxGap) { maxGap = gap; gapIdx = i; }
        }
        threshold = 0.5f * (lens[gapIdx - 1] + lens[gapIdx]);
    }
    float maxLen = lens.empty() ? 1.0f : lens.back();
    float binWidth = (maxLen > 0.0f) ? (maxLen / 40.0f) : 1.0f;

    std::ofstream script("histogram.gnuplot");
    script << "set terminal pngcairo size 1400,900\nset output '" << outputName << "'\nset grid\n";
    script << "set xlabel 'Edge length'\nset ylabel 'Frequency (log scale)'\n";
    script << "set title 'MST edge length histogram'\n";
    script << "set boxwidth " << (binWidth * 0.9f) << " absolute\nset style fill transparent solid 0.5 border\n";
    script << "set logscale y\nset yrange [0.8:*]\nset format y '10^{%L}'\nset mytics 10\nset grid mytics ytics\n";
    script << "binwidth = " << binWidth << "\nbin(x) = binwidth * floor(x / binwidth + 0.5)\n";
    script << "set arrow 1 from " << threshold << ", graph 0 to " << threshold << ", graph 1 nohead lc rgb 'black' lw 2 dt 2\n";
    script << "set label 1 sprintf('threshold = %.3f', " << threshold << ") at " << threshold << ", graph 0.95 offset 1,0 font ',11' tc rgb 'black'\n";
    script << "plot 'edges_brute.txt' using (bin($1)):(1.0) smooth freq with boxes fc rgb '#E05050' title 'Brute force', \\\n";
    script << "     'edges_nested.txt' using (bin($1)):(1.0) smooth freq with boxes fc rgb '#5070E0' title 'Quadratic nested'\n";
    script.close();
    //212-Ким-вызов Gnuplot
    std::system("\"C:\\Program Files\\gnuplot\\bin\\gnuplot.exe\" histogram.gnuplot");
}

//212-Ким-методы сервера
void Server::loadData(const std::string& filename) {
    //212-Ким-вызов функции чтения
    pyramids = readPyramidsFromFile(filename);
    if (pyramids.empty()) { std::cerr << "[Server] Error: File empty or not found.\n"; return; }
    //212-Ким-вызов функции сортировки
    sortPyramidsByMeanValue(pyramids);
    dataLoaded = true;
    std::cout << "[Server] Loaded " << pyramids.size() << " pyramids and sorted them.\n";
}

void Server::generateData() {
    if (!dataLoaded) return;
    allClouds.clear(); 
    std::vector<std::pair<float, float>> allPoints;
    for (const auto& p : pyramids) {
        float sigma = p.getR() * 0.5f;
        //212-Ким-вызов функции генерации гауссова облака
        auto cloud = generateGaussianCloud(p.getX(), p.getY(), sigma, sigma, 0.0f, GAUSS_COUNT);
        allPoints.insert(allPoints.end(), cloud.begin(), cloud.end());
        allClouds.push_back(std::move(cloud));
    }
    
    lastResult.pointClusterId.assign(allPoints.size(), -1);
    lastResult.clusters.clear();
    lastResult.edges.clear();

    //212-Ким-вызов функции построения матрицы расстояний
    fullDistanceMatrix.build(allPoints);
    generated = true;
    clustered = false;
    mstBuilt = false;
    std::cout << "[Server] Generated " << allClouds.size() << " clouds (" << GAUSS_COUNT << " points each).\n";
}

void Server::runClustering(float userThreshold) {
    if (!generated) return;
    
    std::vector<std::pair<float, float>> allPoints;
    for (const auto& cloud : allClouds) allPoints.insert(allPoints.end(), cloud.begin(), cloud.end());

    float threshold = (userThreshold > 0.0f) ? userThreshold : 5.0f;
    //212-Ким-вызов волнового алгоритма
    lastResult = graphWaveClustering(fullDistanceMatrix, allPoints, threshold, 100);
    
    clustered = true;
    std::cout << "[Server] Wave clustering complete. Found " << lastResult.clusters.size() << " clusters total.\n";
}

void Server::runKMeans(int k) {
    if (!generated) return;
    
    std::vector<std::pair<float, float>> allPoints;
    for (const auto& cloud : allClouds) allPoints.insert(allPoints.end(), cloud.begin(), cloud.end());

    bool useSqrt = false;
    //212-Ким-вызов алгоритма k-means
    lastResult = kMeansClustering(allPoints, k, useSqrt);
    
    clustered = true;
    std::cout << "[Server] k-means complete. Found " << lastResult.clusters.size() << " clusters total.\n";
}

void Server::buildMST() {
    if (!generated) return;
    std::vector<std::pair<float, float>> allPoints;
    for (const auto& cloud : allClouds) allPoints.insert(allPoints.end(), cloud.begin(), cloud.end());

    std::cout << "[Server] Building MST on " << allPoints.size() << " points...\n";
    auto t1 = std::chrono::high_resolution_clock::now();
    //212-Ким-вызов алгоритма МПК (полный перебор)
    edgesBrute = mstBruteForce(fullDistanceMatrix, allPoints);
    auto t2 = std::chrono::high_resolution_clock::now();
    //212-Ким-вызов алгоритма МПК (квадратично-гнездовой)
    edgesNested = mstQuadraticNested(fullDistanceMatrix, allPoints);
    auto t3 = std::chrono::high_resolution_clock::now();

    auto msBrute = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    auto msNested = std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2).count();
    std::cout << "[Server] Brute force: " << msBrute << " ms, edges = " << edgesBrute.size() << "\n";
    std::cout << "[Server] Nested:      " << msNested << " ms, edges = " << edgesNested.size() << "\n";

    lastResult.edges.clear();
    for (const auto& e : edgesBrute) {
        lastResult.edges.push_back({e.i, e.j});
    }
    mstBuilt = true;
}

void Server::visualize() const {
    if (!dataLoaded) return;
    std::cout << "[Server] Updating visualizations...\n";
    
    std::vector<std::pair<float, float>> allPoints;
    for (const auto& cloud : allClouds) allPoints.insert(allPoints.end(), cloud.begin(), cloud.end());
    
    //212-Ким-вызов функции визуализации
    visualizePyramids(pyramids, allPoints, lastResult);
    if (mstBuilt) {
        //212-Ким-вызов функции визуализации гистограммы
        visualizeEdgeHistogram(edgesBrute, edgesNested, "mst_histogram.png");
    }
}

//212-Ким-обработка команд контроллера
void Control::executeCommand(const std::string& command, float value, int k) {
    if (command == "load") {
        //212-Ким-вызов метода загрузки данных
        server.loadData("data.txt");
        //212-Ким-вызов визуализации
        server.visualize();
    } 
    else if (command == "generate") {
        if (!server.isDataLoaded()) { std::cerr << "[Control] Error: Load data first.\n"; return; }
        //212-Ким-вызов метода генерации
        server.generateData();
        //212-Ким-вызов визуализации
        server.visualize();
    } 
    else if (command == "cluster") {
        if (!server.isGenerated()) { std::cerr << "[Control] Error: Generate data first.\n"; return; }
        //212-Ким-вызов метода кластеризации
        server.runClustering(value);
        //212-Ким-вызов визуализации
        server.visualize();
    } 
    else if (command == "kmeans") {
        if (!server.isGenerated()) { std::cerr << "[Control] Error: Generate data first.\n"; return; }
        //212-Ким-вызов метода k-means
        server.runKMeans(k);
        //212-Ким-вызов визуализации
        server.visualize();
    }
    else if (command == "mst") {
        if (!server.isGenerated()) { std::cerr << "[Control] Error: Generate data first.\n"; return; }
        //212-Ким-вызов метода построения МПК
        server.buildMST();
        //212-Ким-вызов визуализации
        server.visualize();
    } 
    else if (command == "exit" || command == "quit") {
        std::cout << "[Control] Shutting down...\n";
    }
    else {
        std::cerr << "[Control] Unknown command.\n";
    }
}