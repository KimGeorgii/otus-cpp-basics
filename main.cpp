// 212-Ким-statistics

#include "func.h"

int main() {
    const size_t statistics_count = 6;
    IStatistics* statistics[statistics_count];

    statistics[0] = new Min{};
    statistics[1] = new Max{};
    statistics[2] = new Mean{};
    statistics[3] = new Std{};
    statistics[4] = new Pct90{};
    statistics[5] = new Pct95{};

    double val = 0.0;
    while (std::cin >> val) {
        for (size_t i = 0; i < statistics_count; ++i) {
            statistics[i]->update(val);
        }
    }

    // 212-Ким-Проверка корректности ввода: должен быть именно EOF
    if (!std::cin.eof() && !std::cin.good()) {
        std::cerr << "Invalid input data" << std::endl;
        for (size_t i = 0; i < statistics_count; ++i) {
            delete statistics[i];
        }
        return 1;
    }

    // 212-Ким-Печать результатов
    for (size_t i = 0; i < statistics_count; ++i) {
        std::cout << statistics[i]->name()
                  << " = "
                  << statistics[i]->eval()
                  << std::endl;
    }

    // 212-Ким-Освобождение динамической памяти
    for (size_t i = 0; i < statistics_count; ++i) {
        delete statistics[i];
    }

    return 0;
}