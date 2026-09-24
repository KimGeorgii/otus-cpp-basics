// 212-Ким-statistics
// реализации классов статистики

#include "func.h"

#include <algorithm>
#include <cmath>
#include <limits>

// 212-Ким-Конструктор Min: инициализируем максимально возможным значением
Min::Min()
    : m_min(std::numeric_limits<double>::max())
    , m_has_value(false) {}

// 212-Ким-Обновление минимума
void Min::update(double next) {
    if (!m_has_value || next < m_min) {
        m_min = next;
        m_has_value = true;
    }
}

// 212-Ким-Возврат минимума
double Min::eval() const {
    return m_min;
}

// 212-Ким-Имя статистики
const char* Min::name() const {
    return "min";
}

// 212-Ким-Конструктор Max: инициализируем минимально возможным значением
Max::Max()
    : m_max(std::numeric_limits<double>::lowest())
    , m_has_value(false) {}

// 212-Ким-Обновление максимума
void Max::update(double next) {
    if (!m_has_value || next > m_max) {
        m_max = next;
        m_has_value = true;
    }
}

// 212-Ким-Возврат максимума
double Max::eval() const {
    return m_max;
}

// 212-Ким-Имя статистики
const char* Max::name() const {
    return "max";
}

// 212-Ким-Конструктор Mean
Mean::Mean()
    : m_sum(0.0)
    , m_count(0) {}

// 212-Ким-Накопление суммы
void Mean::update(double next) {
    m_sum += next;
    m_count++;
}

// 212-Ким-Возврат среднего арифметического
double Mean::eval() const {
    if (m_count == 0) {
        return 0.0;
    }
    return m_sum / static_cast<double>(m_count);
}

// 212-Ким-Имя статистики
const char* Mean::name() const {
    return "mean";
}

// 212-Ким-Конструктор Std
Std::Std()
    : m_sum(0.0)
    , m_sum_sq(0.0)
    , m_count(0) {}

// 212-Ким-Накопление суммы и суммы квадратов
void Std::update(double next) {
    m_sum += next;
    m_sum_sq += next * next;
    m_count++;
}

// 212-Ким-Возврат среднеквадратического отклонения
// формула: sqrt(E[x^2] - (E[x])^2)
double Std::eval() const {
    if (m_count == 0) {
        return 0.0;
    }
    const double mean = m_sum / static_cast<double>(m_count);
    const double mean_sq = m_sum_sq / static_cast<double>(m_count);
    const double variance = mean_sq - mean * mean;
    return std::sqrt(variance > 0.0 ? variance : 0.0);
}

// 212-Ким-Имя статистики
const char* Std::name() const {
    return "std";
}

// 212-Ким-Конструктор процентиля
Percentile::Percentile(double pct)
    : m_pct(pct) {}

// 212-Ким-Накопление всех значений последовательности
void Percentile::update(double next) {
    m_values.push_back(next);
}

// 212-Ким-Возврат процентиля: сортируем и берём элемент по индексу
double Percentile::eval() const {
    if (m_values.empty()) {
        return 0.0;
    }
    std::vector<double> sorted = m_values;
    std::sort(sorted.begin(), sorted.end());

    // 212-Ким-Индекс ближайшего элемента, не превышающего pct% данных
    const double rank = m_pct / 100.0 * static_cast<double>(sorted.size() - 1);
    const size_t index = static_cast<size_t>(rank + 0.5);
    const size_t clamped = index < sorted.size() ? index : sorted.size() - 1;
    return sorted[clamped];
}

// 212-Ким-Конструктор 90-го процентиля
Pct90::Pct90()
    : Percentile(90.0) {}

// 212-Ким-Имя статистики
const char* Pct90::name() const {
    return "pct90";
}

// 212-Ким-Конструктор 95-го процентиля
Pct95::Pct95()
    : Percentile(95.0) {}

// 212-Ким-Имя статистики
const char* Pct95::name() const {
    return "pct95";
}