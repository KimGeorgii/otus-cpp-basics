// 212-Ким-визуализация конусов

#include "func.h"

#include <iostream>
#include <fstream>

int main() {
    std::list<cone> cones = readCones("data.txt");

    std::ofstream output("cones-eq.txt");
    std::ofstream points("cone-points.txt");

    if (!output.is_open() || !points.is_open()) {
        std::cerr << "Error: cannot create output files" << std::endl;
        return 1;
    }

    if (cones.empty()) {
        std::cerr << "Error: no data was found" << std::endl;
        return 1;
    }

    for (const cone& p : cones) {
        writeEquation(output, p);

        float xMin = p.getX() - p.getR() - 0.1f;
        float xMax = p.getX() + p.getR() + 0.1f;
        float yMin = p.getY() - p.getR() - 0.1f;
        float yMax = p.getY() + p.getR() + 0.1f;

        for (float x = xMin; x < xMax; x += 0.03f) {
            for (float y = yMin; y < yMax; y += 0.03f) {
                for (float z = 0.0f; z < p.getH() + 0.1f; z += 0.03f) {
                    float dx = x - p.getX();
                    float dy = y - p.getY();
                    float rhs = z * p.getR() / p.getH() - p.getR();
                    if (isNearZero(dx * dx + dy * dy - rhs * rhs)) {
                        points << x << " " << y << " " << z << std::endl;
                    }
                }
            }
        }

        std::cout << "done" << std::endl;
    }

    output.close();
    points.close();

    // Генерация данных для Gnuplot
    writeGnuplotSurface("cone-surface.txt", cones);

    // Сохранение PNG
    createGnuplotScript("plot_cones_static.gp", true);
    runGnuplot("plot_cones_static.gp");

    // Интерактивное окно
    createGnuplotScript("plot_cones_interactive.gp", false);
    runGnuplot("plot_cones_interactive.gp");

    return 0;
}