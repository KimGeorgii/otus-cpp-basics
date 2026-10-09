// 212-Ким-генерация 2D-Гаусса и кластеризация алгоритмом Волна

#include "func.h"

#include <iostream>
#include <fstream>
#include <vector>

int main() {
    std::list<cone> cones = readCones("data.txt");

    if (cones.empty()) {
        std::cerr << "Error: no data was found" << std::endl;
        return 1;
    }

    // генерация 2D-Гаусса под каждым конусом
    const int POINTS_PER_CONE = 1000;
    std::vector<point> all_points;
    all_points.reserve(cones.size() * POINTS_PER_CONE);

    int cluster_id = 0;
    for (const cone& c : cones) {
        std::vector<point> pts = generateGaussianPoints(c, POINTS_PER_CONE);
        for (point& p : pts) {
            p.setCluster(cluster_id);
            all_points.push_back(p);
        }
        cluster_id++;
    }

    std::cout << "Generated " << all_points.size()
              << " points for " << cones.size() << " cones" << std::endl;

    // вычисление границ и создание сетки квадратов
    float x_min = all_points[0].getX();
    float x_max = x_min;
    float y_min = all_points[0].getY();
    float y_max = y_min;

    for (const point& p : all_points) {
        if (p.getX() < x_min) x_min = p.getX();
        if (p.getX() > x_max) x_max = p.getX();
        if (p.getY() < y_min) y_min = p.getY();
        if (p.getY() > y_max) y_max = p.getY();
    }

    const float CELL_SIZE = 1.0f;
    grid g(x_min, y_min, x_max, y_max, CELL_SIZE);

    std::cout << "Grid: " << g.getNx() << " x " << g.getNy()
              << " squares, cell_size = " << g.getCellSize() << std::endl;

    // сортировка точек по квадратам
    std::vector<point> sorted_points = sortPointsBySquares(all_points, g);
    std::cout << "Sorted " << sorted_points.size() << " points by squares" << std::endl;

    // сохранение результатов 03.10
    writePointsTXT("cones-points-sorted.txt", sorted_points, g);
    writePointsTXT("cones-points-unsorted.txt", all_points, g);
    writeGnuplotSurface("cone-surface.txt", cones);
    writeGridLines("grid-lines.txt", g);

    const float WAVE_THRESHOLD = 1.5f;

    std::vector<int> wave_cluster_ids;
    std::vector<cluster> clusters = runWaveClustering(
        all_points, WAVE_THRESHOLD, wave_cluster_ids);

    // печать координат кластеров
    printClusterStats(clusters);

    // сохранение результата
    writeClustersTXT("clusters-points.txt", all_points, wave_cluster_ids);

    // визуализация кластеров (2 PNG + интерактив)
    createClusterGnuplotScript("plot_clusters_static.gp", true);
    runGnuplot("plot_clusters_static.gp");

    createClusterGnuplotScript("plot_clusters_interactive.gp", false);
    runGnuplot("plot_clusters_interactive.gp");

    return 0;
}