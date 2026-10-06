// 212-Ким-визуализация конусов, 2D-Гаусса и сетки квадратов

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

    // Генерация 2D-Гаусса под каждым конусом
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

    // Вычисление границ и создание сетки квадратов
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

    // Сортировка точек по квадратам
    std::vector<point> sorted_points = sortPointsBySquares(all_points, g);
    std::cout << "Sorted " << sorted_points.size() << " points by squares" << std::endl;

    // Запись результатов в CSV
    writePointsCSV("cones-points-sorted.csv", sorted_points, g);
    writePointsCSV("cones-points-unsorted.csv", all_points, g);

    // Генерация данных поверхности конусов и линий сетки для Gnuplot
    writeGnuplotSurface("cone-surface.txt", cones);
    writeGridLines("grid-lines.txt", g);

    // Сохранение PNG-файлов
    createGnuplotScript("plot_cones_static.gp", true);
    runGnuplot("plot_cones_static.gp");

    // Интерактивное окно (можно вращать мышью)
    createGnuplotScript("plot_cones_interactive.gp", false);
    runGnuplot("plot_cones_interactive.gp");

    return 0;
}