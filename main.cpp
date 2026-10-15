//212-Ким-задача о пирамидах с алгоритмом "волна"

#include "func.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

int main() {
    // читаем пирамиды из файла data.txt
    std::vector<pyramid> pyramids = readPyramidsFromFile("data.txt");

    if (pyramids.empty()) {
        std::cerr << "File data.txt is empty or not found.\n";
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

    const int GAUSS_COUNT = 1000;

    std::vector<std::vector<std::pair<float, float>>> allClouds;
    std::vector<clusteringResult> allResults;

    for (size_t i = 0; i < pyramids.size(); ++i) {
        const auto& p = pyramids[i];
        float sigma = p.getR() * 0.5f;

        auto cloud = generateGaussianCloud(
            p.getX(), p.getY(),
            sigma, sigma,
            0.0f, GAUSS_COUNT);

        // волновой алгоритм теперь на графе:
        // соединяем точки, если расстояние <= threshold.
        float threshold = sigma * 0.35f;

        auto result = graphWaveClustering(cloud, threshold, 100);

        std::cout << "\nPyramid " << (i + 1)
                  << ": gaussian cloud of " << cloud.size() << " points"
                  << ", sigma=" << sigma
                  << ", threshold=" << threshold << "\n";

        std::cout << "Found " << result.clusters.size() << " clusters:\n";
        for (size_t j = 0; j < result.clusters.size(); ++j) {
            const auto& cl = result.clusters[j];
            std::cout << "  Cluster " << j + 1
                      << ": center=(" << cl.cx << ", " << cl.cy << ")"
                      << ", size=" << cl.size << " points\n";
        }

        int trash = 0;
        for (int id : result.pointClusterId) if (id == -1) ++trash;
        std::cout << "  Trash points: " << trash << "\n";
        std::cout << "  Edges in graph: " << result.edges.size() << "\n";

        allClouds.push_back(std::move(cloud));
        allResults.push_back(std::move(result));
    }

    //MST + замеры времени

    // Собираем все точки в один массив
    std::vector<std::pair<float, float>> allPoints;
    for (const auto& cloud : allClouds) {
        allPoints.insert(allPoints.end(), cloud.begin(), cloud.end());
    }

    std::cout << "\n=== MST benchmark on " << allPoints.size()
              << " points ===\n";

    auto t1 = std::chrono::high_resolution_clock::now();
    auto edgesBrute = mstBruteForce(allPoints);
    auto t2 = std::chrono::high_resolution_clock::now();
    auto edgesNested = mstQuadraticNested(allPoints);
    auto t3 = std::chrono::high_resolution_clock::now();

    auto msBrute  = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    auto msNested = std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2).count();

    std::cout << "Brute force         : " << msBrute
              << " ms, edges = " << edgesBrute.size() << "\n";
    std::cout << "Quadratic nested    : " << msNested
              << " ms, edges = " << edgesNested.size() << "\n";

    //визуализация

    visualizePyramids(pyramids, allClouds, allResults);
    visualizeEdgeHistogram(edgesBrute, edgesNested, "mst_histogram.png");

    return 0;
}