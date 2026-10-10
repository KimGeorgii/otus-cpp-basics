//212-Ким-задача о пирамидах с алгоритмом "волна"

#include "func.h"

#include <iostream>
#include <fstream>

int main() {
    //читаем пирамиды из файла pyramids.txt
    std::vector<pyramid> pyramids = readPyramidsFromFile("pyramids.txt");

    if (pyramids.empty()) {
        std::cerr << "File pyramids.txt is empty or not found.\n";
        std::cerr << "Put it into the working directory of the program.\n";
        return 1;
    }

    std::cout << "Before sorting:\n";
    for (size_t i = 0; i < pyramids.size(); ++i) {
        const auto& p = pyramids[i];
        std::cout << "Pyramid " << i + 1 << ": ";
        p.print();
        std::cout << "  area=" << p.area()
                  << "  volume=" << p.volume()
                  << "  mean=" << p.meanValue() << "\n";
    }

    //сортируем по среднему значению
    sortPyramidsByMeanValue(pyramids);

    std::cout << "\nAfter sorting by mean(area, volume):\n";
    for (size_t i = 0; i < pyramids.size(); ++i) {
        const auto& p = pyramids[i];
        std::cout << "Pyramid " << i + 1 << ": ";
        p.print();
        std::cout << "  area=" << p.area()
                  << "  volume=" << p.volume()
                  << "  mean=" << p.meanValue() << "\n";
    }

    const int GAUSS_COUNT = 10000;

    std::vector<std::vector<std::pair<float, float>>> allClouds;
    std::vector<clusteringResult> allResults;

    for (size_t i = 0; i < pyramids.size(); ++i) {
        const auto& p = pyramids[i];
        float sigma = p.getR() * 0.5f;

        auto cloud = generateGaussianCloud(
            p.getX(), p.getY(),
            sigma, sigma,
            0.0f, GAUSS_COUNT);

        float cellSize = sigma * 0.4f;

        auto result = matrixWaveClustering(cloud, cellSize, 100);

        std::cout << "\nPyramid " << (i + 1)
                  << ": gaussian cloud of " << cloud.size() << " points"
                  << ", sigma=" << sigma
                  << ", cellSize=" << cellSize << "\n";

        std::cout << "Found " << result.clusters.size() << " clusters:\n";
        for (size_t j = 0; j < result.clusters.size(); ++j) {
            const auto& cl = result.clusters[j];
            std::cout << "  Cluster " << j + 1
                      << ": center=(" << cl.cx << ", " << cl.cy << ")"
                      << ", size=" << cl.size << " points\n";
        }

        int trash = 0;
        for (int id : result.pointClusterId) {
            if (id == -1) ++trash;
        }
        std::cout << "  Trash points: " << trash << "\n";

        allClouds.push_back(std::move(cloud));
        allResults.push_back(std::move(result));
    }

    //визуализация
    visualizePyramids(pyramids, allClouds, allResults);
    return 0;
}
