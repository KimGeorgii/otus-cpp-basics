//212-Ким-генерация 2D гауссовых облаков под пирамидами

#include "func.h"

#include <iostream>

int main() {
    //читаем пирамиды из data.txt (x y r h)
    std::vector<pyramid> pyramids = readPyramidsFromFile("data.txt");

    if (pyramids.empty()) {
        std::cerr << "File data.txt is empty or not found.\n";
        std::cerr << "Put it into the working directory of the program.\n";
        return 1;
    }

    const int GAUSS_COUNT = 10000;

    std::vector<std::vector<std::pair<float, float>>> allClouds;

    for (size_t i = 0; i < pyramids.size(); ++i) {
        const auto& p = pyramids[i];

        //под каждой пирамидой — гауссово облако с sigma = r/2
        float sigma = p.getR() * 0.5f;

        auto cloud = generateGaussianCloud(
            p.getX(), p.getY(),
            sigma, sigma,
            0.0f, GAUSS_COUNT);

        std::cout << "Pyramid " << (i + 1) << ": ";
        p.print();
        std::cout << "  -> cloud of " << cloud.size()
                  << " points, sigma=" << sigma << "\n";

        allClouds.push_back(std::move(cloud));
    }

    //визуализация — clouds.png
    visualizeClouds(allClouds, "clouds.png");

    return 0;
}
