// 212-Ким-statistics
// объявление классов статистики

#pragma once

#include <iostream>
#include <string>
#include <vector>

// 212-Ким-Интерфейс статистической характеристики
class IStatistics {
public:
    virtual ~IStatistics() = default;

    virtual void update(double next) = 0;
    virtual double eval() const = 0;
    virtual const char* name() const = 0;
};

// 212-Ким-Минимальное значение последовательности
class Min : public IStatistics {
public:
    Min();
    void update(double next) override;
    double eval() const override;
    const char* name() const override;

private:
    double m_min;
    bool m_has_value;
};

// 212-Ким-Максимальное значение последовательности
class Max : public IStatistics {
public:
    Max();
    void update(double next) override;
    double eval() const override;
    const char* name() const override;

private:
    double m_max;
    bool m_has_value;
};

// 212-Ким-Арифметическое среднее
class Mean : public IStatistics {
public:
    Mean();
    void update(double next) override;
    double eval() const override;
    const char* name() const override;

private:
    double m_sum;
    size_t m_count;
};

// 212-Ким-Среднеквадратическое отклонение
class Std : public IStatistics {
public:
    Std();
    void update(double next) override;
    double eval() const override;
    const char* name() const override;

private:
    double m_sum;
    double m_sum_sq;
    size_t m_count;
};

// 212-Ким-Базовый класс для процентилей
class Percentile : public IStatistics {
public:
    explicit Percentile(double pct);

    void update(double next) override;
    double eval() const override;

protected:
    double m_pct;

private:
    std::vector<double> m_values;
};

// 212-Ким-90-й процентиль
class Pct90 : public Percentile {
public:
    Pct90();
    const char* name() const override;
};

// 212-Ким-95-й процентиль
class Pct95 : public Percentile {
public:
    Pct95();
    const char* name() const override;
};