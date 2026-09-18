//212-Ким-функции и классы

#include "func.h"

#include <cmath>
#include <iostream>
#include <cstdlib>

// 212-Ким-класс точки центра
centre::centre(float a, float b) : x(a), y(b) {}

float centre::getX() const { return x; }
float centre::getY() const { return y; }

void centre::setX(float newX) { x = newX; }
void centre::setY(float newY) { y = newY; }

// 212-Ким-класс окружности
circle::circle(float x, float y, float r) : centre(x, y), R(r) {}

float circle::getR() const { return R; }
float circle::getS() const { return R * R * 3.141593f; }
void circle::setR(float newR) { R = newR; }

// 212-Ким-класс конуса
cone::cone(float x, float y, float r, float h) : circle(x, y, r), H(h) {}

float cone::getH() const { return H; }
float cone::getV() const { return H * getS() / 3.0f; }
void cone::setH(float newH) { H = newH; }

// 212-Ким-чтение конусов из файла
std::list<cone> readCones(const std::string& filename) {
    std::list<cone> cones;
    float x, y, r, h;
    std::ifstream fin(filename);

    while (fin >> x >> y >> r >> h) {
        cones.emplace_back(x, y, r, h);
    }

    return cones;
}

// 212-Ким-функция проверки близости значения к нулю
bool isNearZero(float v, float eps) {
    return std::abs(v) < eps;
}

// 212-Ким-функция записи уравнения поверхности конуса в файл
void writeEquation(std::ofstream& out, const cone& c) {
    out << "(x - " << c.getX() << ")^2 + (y - "
        << c.getY() << ")^2 = ("
        << c.getR() / c.getH() << "*z - "
        << c.getR() << ")^2" << std::endl;
}

// 212-Ким-генерация данных поверхности для Gnuplot
void writeGnuplotSurface(const std::string& filename, const std::list<cone>& cones) {
    std::ofstream out(filename);
    if (!out.is_open()) {
        std::cerr << "Error: cannot create " << filename << std::endl;
        return;
    }

    const int NTHETA = 40;   // количество точек по окружности
    const int NHEIGHT = 25;  // количество точек по высоте

    for (const cone& c : cones) {
        for (int i = 0; i <= NTHETA; ++i) {
            double u = 2.0 * 3.141593 * i / NTHETA;
            for (int j = 0; j <= NHEIGHT; ++j) {
                double v = c.getH() * j / NHEIGHT;
                double r = (c.getH() - v) * c.getR() / c.getH();
                double x = c.getX() + r * std::cos(u);
                double y = c.getY() + r * std::sin(u);
                out << x << " " << y << " " << v << "\n";
            }
            out << "\n"; // пустая строка между "сканами"
        }
        out << "\n"; // пустая строка между конусами
    }

    std::cout << "Generated " << filename << std::endl;
}

// 212-Ким-создание скрипта Gnuplot
void createGnuplotScript(const std::string& scriptFile, bool staticMode) {
    std::ofstream out(scriptFile);
    if (!out.is_open()) {
        std::cerr << "Error: cannot create " << scriptFile << std::endl;
        return;
    }

    out << "reset\n";

    if (staticMode) {
        // Режим сохранения PNG
        out << "set terminal pngcairo size 1200,900\n";
        out << "set output 'cones-3d.png'\n";
    }

    out << "set title 'Poverhnosti konusov (3D)'\n";
    out << "set xlabel 'X'\n";
    out << "set ylabel 'Y'\n";
    out << "set zlabel 'Z'\n";
    out << "set grid\n";
    out << "set ticslevel 0\n";
    out << "set hidden3d\n";
    out << "set pm3d depthorder\n";
    out << "set view 60, 30, 1, 1\n";
    out << "set palette rgb 33,13,10\n";
    out << "splot 'cone-surface.txt' using 1:2:3 with pm3d title 'Konusy'\n";

    if (staticMode) {
        out << "unset output\n";
    } else {
        // Интерактивный режим — ждём нажатия Enter
        out << "pause -1 'Нажми Enter для выхода...'\n";
    }

    out.close();
    std::cout << "Created " << scriptFile << std::endl;
}

// 212-Ким-запуск Gnuplot
void runGnuplot(const std::string& scriptFile) {
    std::string command;

#ifdef _WIN32
    // Запускаем Gnuplot в новом окне, которое останется открытым
    command = "start \"Gnuplot\" cmd /K \"gnuplot " + scriptFile + "\"";
#else
    // Linux/macOS — запускаем в фоне
    command = "gnuplot " + scriptFile + " &";
#endif

    std::cout << "Running: " << command << std::endl;
    int result = std::system(command.c_str());

    if (result != 0) {
        std::cerr << "Error: Gnuplot failed." << std::endl;
    }
}