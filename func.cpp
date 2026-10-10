//212-Ким-задача о пирамидах с алгоритмом "волна"

#include "func.h"

#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <random>
#include <queue>
#include <cstdlib>
#include <cstdio>

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

//212-Ким-сортировка по среднему значению
void sortPyramidsByMeanValue(std::vector<pyramid>& pyramids) {
    std::sort(pyramids.begin(), pyramids.end(),
        [](const pyramid& a, const pyramid& b) {
            return a.meanValue() < b.meanValue();
        });
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

//212-Ким-матрично-волновой алгоритм
clusteringResult matrixWaveClustering(
    const std::vector<std::pair<float, float>>& points,
    float cellSize,
    int minClusterSize)
{
    clusteringResult result;

    if (points.empty()) return result;

    result.pointClusterId.assign(points.size(), -1);

    //границы bounding box
    float minX = points[0].first;
    float maxX = points[0].first;
    float minY = points[0].second;
    float maxY = points[0].second;

    for (const auto& p : points) {
        minX = std::min(minX, p.first);
        maxX = std::max(maxX, p.first);
        minY = std::min(minY, p.second);
        maxY = std::max(maxY, p.second);
    }

    //построение сетки
    int gridW = static_cast<int>((maxX - minX) / cellSize) + 1;
    int gridH = static_cast<int>((maxY - minY) / cellSize) + 1;

    const int MAX_GRID = 2000;
    gridW = std::min(gridW, MAX_GRID);
    gridH = std::min(gridH, MAX_GRID);

    std::vector<std::vector<bool>> occupied(gridH, std::vector<bool>(gridW, false));
    std::vector<std::vector<bool>> visited (gridH, std::vector<bool>(gridW, false));
    std::vector<std::vector<int>>  clusterId(gridH, std::vector<int>(gridW, -1));

    //раскладываем точки по ячейкам
    for (const auto& p : points) {
        int gx = static_cast<int>((p.first  - minX) / cellSize);
        int gy = static_cast<int>((p.second - minY) / cellSize);
        if (gx < 0) gx = 0;
        if (gy < 0) gy = 0;
        if (gx >= gridW) gx = gridW - 1;
        if (gy >= gridH) gy = gridH - 1;
        occupied[gy][gx] = true;
    }

    //BFS-волна по 8 направлениям
    const int dx[8] = {  1, -1,  0,  0,  1,  1, -1, -1 };
    const int dy[8] = {  0,  0,  1, -1,  1, -1,  1, -1 };

    int rawClusterId = 0;
    std::vector<std::vector<int>> rawClusterPoints;

    for (int gy = 0; gy < gridH; ++gy) {
        for (int gx = 0; gx < gridW; ++gx) {
            if (!occupied[gy][gx] || visited[gy][gx]) continue;

            std::queue<std::pair<int, int>> q;
            q.push({gx, gy});
            visited[gy][gx]   = true;
            clusterId[gy][gx] = rawClusterId;

            while (!q.empty()) {
                auto [cx, cy] = q.front();
                q.pop();

                for (int d = 0; d < 8; ++d) {
                    int nx = cx + dx[d];
                    int ny = cy + dy[d];
                    if (nx < 0 || ny < 0 || nx >= gridW || ny >= gridH) continue;
                    if (!occupied[ny][nx]) continue;
                    if (visited[ny][nx])   continue;

                    visited[ny][nx]   = true;
                    clusterId[ny][nx] = rawClusterId;
                    q.push({nx, ny});
                }
            }

            ++rawClusterId;
        }
    }

    //сопоставляем точки "сырым" кластерам
    rawClusterPoints.resize(rawClusterId);
    for (size_t i = 0; i < points.size(); ++i) {
        const auto& p = points[i];
        int gx = static_cast<int>((p.first  - minX) / cellSize);
        int gy = static_cast<int>((p.second - minY) / cellSize);
        if (gx < 0) gx = 0;
        if (gy < 0) gy = 0;
        if (gx >= gridW) gx = gridW - 1;
        if (gy >= gridH) gy = gridH - 1;

        int rid = clusterId[gy][gx];
        if (rid >= 0) {
            rawClusterPoints[rid].push_back(static_cast<int>(i));
        }
    }

    //фильтрация по размеру и подсчёт центров
    for (int rid = 0; rid < rawClusterId; ++rid) {
        const auto& idxs = rawClusterPoints[rid];
        if (static_cast<int>(idxs.size()) < minClusterSize) continue;

        float sumX = 0.0f, sumY = 0.0f;
        for (int idx : idxs) {
            sumX += points[idx].first;
            sumY += points[idx].second;
        }

        cluster cl;
        cl.cx   = sumX / idxs.size();
        cl.cy   = sumY / idxs.size();
        cl.size = static_cast<int>(idxs.size());

        int newId = static_cast<int>(result.clusters.size());
        result.clusters.push_back(cl);

        for (int idx : idxs) {
            result.pointClusterId[idx] = newId;
        }
    }

    return result;
}

//212-Ким-генерация различимого цвета по глобальному индексу кластера
static std::string colorByIndex(int idx) {
    const float golden = 0.618033988749895f;
    float hue = std::fmod(idx * golden, 1.0f);           // [0,1)

    float sat = 0.55f + 0.30f * ((idx % 3) / 2.0f);      // 0.55..0.85
    float val = 0.75f + 0.20f * ((idx % 2) ? 1.0f : 0.0f);

    int   i = static_cast<int>(hue * 6.0f);
    float f = hue * 6.0f - i;
    float p = val * (1.0f - sat);
    float q = val * (1.0f - f * sat);
    float t = val * (1.0f - (1.0f - f) * sat);

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
    std::snprintf(buf, sizeof(buf), "#%02X%02X%02X",
                  static_cast<int>(r * 255.0f),
                  static_cast<int>(g * 255.0f),
                  static_cast<int>(b * 255.0f));
    return std::string(buf);
}

//212-Ким-визуализация через Gnuplot (2 картинки)
void visualizePyramids(
    const std::vector<pyramid>& pyramids,
    const std::vector<std::vector<std::pair<float, float>>>& allClouds,
    const std::vector<clusteringResult>& allResults)
{
    std::ofstream script("pyramids.gnuplot");
    if (!script.is_open()) {
        std::cerr << "Cannot create gnuplot script\n";
        return;
    }

    //предрасчёт уникального цвета для каждого кластера
    std::vector<std::vector<std::string>> clusterColors(allClouds.size());
    int globalClusterId = 0;
    for (size_t i = 0; i < allClouds.size(); ++i) {
        clusterColors[i].resize(allResults[i].clusters.size());
        for (size_t j = 0; j < allResults[i].clusters.size(); ++j) {
            clusterColors[i][j] = colorByIndex(globalClusterId++);
        }
    }

    //общие настройки терминала и вида
    script << "set terminal png size 1400,1000\n";
    script << "set grid\n";
    script << "set xlabel 'X'\n";
    script << "set ylabel 'Y'\n";
    script << "set zlabel 'Z'\n";
    script << "set hidden3d\n";
    script << "set view 60, 30, 1, 1\n";
    script << "set xyplane at 0\n";

    //границы сцены
    if (!pyramids.empty()) {
        float minX = pyramids[0].getX() - pyramids[0].getR();
        float maxX = pyramids[0].getX() + pyramids[0].getR();
        float minY = pyramids[0].getY() - pyramids[0].getR();
        float maxY = pyramids[0].getY() + pyramids[0].getR();
        float maxZ = pyramids[0].getH();

        for (const auto& p : pyramids) {
            float sigma = p.getR() * 0.5f;
            minX = std::min(minX, p.getX() - 4.0f * sigma);
            maxX = std::max(maxX, p.getX() + 4.0f * sigma);
            minY = std::min(minY, p.getY() - 4.0f * sigma);
            maxY = std::max(maxY, p.getY() + 4.0f * sigma);
            maxZ = std::max(maxZ, p.getH());
        }

        float marginX = (maxX - minX) * 0.2f + 1.0f;
        float marginY = (maxY - minY) * 0.2f + 1.0f;

        script << "set xrange [" << (minX - marginX) << ":" << (maxX + marginX) << "]\n";
        script << "set yrange [" << (minY - marginY) << ":" << (maxY + marginY) << "]\n";
        script << "set zrange [0:" << (maxZ * 1.2f) << "]\n";
    }

    const int   N  = 40;
    const float PI = 3.14159265358979323846f;

    //данные пирамид (треугольные грани + основание)
    for (size_t i = 0; i < pyramids.size(); ++i) {
        const auto& p = pyramids[i];
        float cx = p.getX();
        float cy = p.getY();
        float r  = p.getR();
        float h  = p.getH();

        script << "$pyr" << i << " << EOD\n";

        for (int k = 0; k < N; ++k) {
            float a1 = 2.0f * PI * k / N;
            float a2 = 2.0f * PI * (k + 1) / N;

            float x1 = cx + r * std::cos(a1);
            float y1 = cy + r * std::sin(a1);
            float x2 = cx + r * std::cos(a2);
            float y2 = cy + r * std::sin(a2);

            script << x1 << " " << y1 << " 0\n";
            script << x2 << " " << y2 << " 0\n";
            script << cx << " " << cy << " " << h << "\n";
            script << "\n";
        }

        for (int k = 0; k < N; ++k) {
            float a = 2.0f * PI * k / N;
            float x = cx + r * std::cos(a);
            float y = cy + r * std::sin(a);
            script << x << " " << y << " 0\n";
        }
        script << "\n";
        script << "EOD\n";
    }

    //датасеты кластеров и "мусора"
    for (size_t i = 0; i < allClouds.size(); ++i) {
        const auto& cloud  = allClouds[i];
        const auto& result = allResults[i];

        for (size_t j = 0; j < result.clusters.size(); ++j) {
            script << "$cl_" << i << "_" << j << " << EOD\n";
            for (size_t k = 0; k < cloud.size(); ++k) {
                if (result.pointClusterId[k] == static_cast<int>(j)) {
                    script << cloud[k].first << " " << cloud[k].second << " 0\n";
                }
            }
            script << "EOD\n";
        }

        script << "$trash_" << i << " << EOD\n";
        for (size_t k = 0; k < cloud.size(); ++k) {
            if (result.pointClusterId[k] == -1) {
                script << cloud[k].first << " " << cloud[k].second << " 0\n";
            }
        }
        script << "EOD\n";
    }

    //центры кластеров — каждый в своём датасете
    for (size_t i = 0; i < allResults.size(); ++i) {
        for (size_t j = 0; j < allResults[i].clusters.size(); ++j) {
            const auto& cl = allResults[i].clusters[j];
            script << "$cent_" << i << "_" << j << " << EOD\n";
            script << cl.cx << " " << cl.cy << " 0\n";
            script << "EOD\n";
        }
    }

    //КАРТИНКА: конусы + точки под ними
    script << "set output 'pyramids_with_points.png'\n";
    script << "set title '3D Pyramids + 2D Gaussian clouds + clusters'\n";
    script << "splot ";

    for (size_t i = 0; i < pyramids.size(); ++i) {
        if (i > 0) script << ", ";
        script << "$pyr" << i
               << " using 1:2:3 with polygons "
               << "fc rgb 'steelblue' "
               << "title 'Pyramid " << (i + 1) << "'";
    }

    for (size_t i = 0; i < allClouds.size(); ++i) {
        for (size_t j = 0; j < allResults[i].clusters.size(); ++j) {
            script << ", $cl_" << i << "_" << j
                   << " using 1:2:(0.0) with dots "
                   << "lc rgb '" << clusterColors[i][j] << "' "
                   << "title 'Cluster " << i + 1 << "." << j + 1 << "'";
        }
    }

    for (size_t i = 0; i < allClouds.size(); ++i) {
        script << ", $trash_" << i
               << " using 1:2:(0.0) with dots "
               << "lc rgb 'black' "
               << "title 'Trash " << (i + 1) << "'";
    }

    for (size_t i = 0; i < allResults.size(); ++i) {
        for (size_t j = 0; j < allResults[i].clusters.size(); ++j) {
            script << ", $cent_" << i << "_" << j
                   << " using 1:2:(0.0) with points "
                   << "pt 7 ps 2 lc rgb '" << clusterColors[i][j] << "' "
                   << "title 'Center " << i + 1 << "." << j + 1 << "'";
        }
    }
    script << "\n";

    //КАРТИНКА: только точки (вид сверху)
    script << "set output 'pyramids_points_only.png'\n";
    script << "set title '2D Gaussian clouds + clusters (top view)'\n";
    script << "plot ";

    bool first = true;
    for (size_t i = 0; i < allClouds.size(); ++i) {
        for (size_t j = 0; j < allResults[i].clusters.size(); ++j) {
            if (!first) script << ", ";
            first = false;
            script << "$cl_" << i << "_" << j
                   << " using 1:2 with dots "
                   << "lc rgb '" << clusterColors[i][j] << "' "
                   << "title 'Cluster " << i + 1 << "." << j + 1 << "'";
        }
    }

    for (size_t i = 0; i < allClouds.size(); ++i) {
        if (!first) script << ", ";
        first = false;
        script << "$trash_" << i
               << " using 1:2 with dots "
               << "lc rgb 'black' "
               << "title 'Trash " << (i + 1) << "'";
    }

    for (size_t i = 0; i < allResults.size(); ++i) {
        for (size_t j = 0; j < allResults[i].clusters.size(); ++j) {
            if (!first) script << ", ";
            first = false;
            script << "$cent_" << i << "_" << j
                   << " using 1:2 with points "
                   << "pt 7 ps 2 lc rgb '" << clusterColors[i][j] << "' "
                   << "title 'Center " << i + 1 << "." << j + 1 << "'";
        }
    }
    script << "\n";

    script.close();

    //запуск Gnuplot
    int result = std::system("\"C:\\Program Files\\gnuplot\\bin\\gnuplot.exe\" pyramids.gnuplot");
    if (result == 0) {
        std::cout << "Visualizations saved: "
                  << "pyramids_with_points.png and pyramids_points_only.png\n";
    } else {
        std::cerr << "Error running gnuplot\n";
    }
}
