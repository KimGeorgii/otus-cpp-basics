//212-Ким-функции и классы

#pragma once

#include <list>
#include <string>
#include <fstream>

// 212-Ким-класс точки центра
class centre {
private:
    float x;
    float y;

public:
    centre(float a, float b);

    float getX() const;
    float getY() const;
    void setX(float newX);
    void setY(float newY);
};

// 212-Ким-класс окружности
class circle : public centre {
private:
    float R;

public:
    circle(float x, float y, float r);

    float getR() const;
    float getS() const;
    void setR(float newR);
};

// 212-Ким-класс конуса
class cone : public circle {
private:
    float H;

public:
    cone(float x, float y, float r, float h);

    float getH() const;
    float getV() const;
    void setH(float newH);
};

// 212-Ким-чтение конусов из файла
std::list<cone> readCones(const std::string& filename);

// 212-Ким-функция проверки близости значения к нулю
bool isNearZero(float v, float eps = 0.002f);

// 212-Ким-функция записи уравнения поверхности конуса в файл
void writeEquation(std::ofstream& out, const cone& c);

// 212-Ким-генерация данных поверхности для Gnuplot
void writeGnuplotSurface(const std::string& filename, const std::list<cone>& cones);

// 212-Ким-создание скрипта Gnuplot
// staticMode = true  — сохраняет PNG
// staticMode = false — открывает интерактивное окно
void createGnuplotScript(const std::string& scriptFile, bool staticMode);

// 212-Ким-запуск Gnuplot
void runGnuplot(const std::string& scriptFile);