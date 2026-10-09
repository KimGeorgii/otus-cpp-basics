//212-Ким-функции и классы

#pragma once

#include <list>
#include <string>
#include <vector>
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

// 212-Ким-класс генерируемой точки
class point {
private:
    float x;
    float y;
    int cluster;

public:
    point(float a, float b, int c = -1);

    float getX() const;
    float getY() const;
    int getCluster() const;
    void setCluster(int c);
};

// 212-Ким-структура координат квадрата в сетке
struct square {
    int ix;
    int iy;
};

// 212-Ким-класс сетки квадратов для пространственной индексации
class grid {
private:
    float x_min;
    float y_min;
    float cell_size;
    int nx;
    int ny;

public:
    grid(float x0, float y0, float x1, float y1, float cs);

    float getCellSize() const;
    float getXMin() const;
    float getYMin() const;
    int getNx() const;
    int getNy() const;

    square getSquare(float x, float y) const;
};

// 212-Ким-класс кластера - результат работы алгоритма Волна
class cluster {
private:
    int id;
    std::vector<point> points;
    float cx;
    float cy;
    float x_min;
    float x_max;
    float y_min;
    float y_max;

public:
    cluster(int cluster_id);

    void addPoint(const point& p);
    void computeStats();

    int getId() const;
    size_t size() const;
    float getCX() const;
    float getCY() const;
    float getXMin() const;
    float getXMax() const;
    float getYMin() const;
    float getYMax() const;
    const std::vector<point>& getPoints() const;
};

// 212-Ким-чтение конусов из файла
std::list<cone> readCones(const std::string& filename);

// 212-Ким-функция проверки близости значения к нулю
bool isNearZero(float v, float eps = 0.002f);

// 212-Ким-функция записи уравнения поверхности конуса в файл
void writeEquation(std::ofstream& out, const cone& c);

// 212-Ким-генерация данных поверхности для Gnuplot
void writeGnuplotSurface(const std::string& filename, const std::list<cone>& cones);

// 212-Ким-запись линий сетки квадратов в файл
void writeGridLines(const std::string& filename, const grid& g);

// 212-Ким-создание скрипта Gnuplot (конусы + точки + сетка)
void createGnuplotScript(const std::string& scriptFile, bool staticMode);

// 212-Ким-запуск Gnuplot
void runGnuplot(const std::string& scriptFile);

// 212-Ким-генерация count точек 2D-Гаусса под конусом
std::vector<point> generateGaussianPoints(const cone& c, int count);

// 212-Ким-сортировка точек по квадратам, затем по x и y
std::vector<point> sortPointsBySquares(const std::vector<point>& points,
                                       const grid& g);

// 212-Ким-запись точек в TXT: x y cluster square_ix square_iy
void writePointsTXT(const std::string& filename,
                    const std::vector<point>& points,
                    const grid& g);

// 212-Ким-матричный алгоритм Волна: поиск связных компонент
std::vector<cluster> runWaveClustering(const std::vector<point>& points,
                                        float threshold,
                                        std::vector<int>& out_cluster_ids);

// 212-Ким-печать статистики по найденным кластерам
void printClusterStats(const std::vector<cluster>& clusters);

// 212-Ким-запись точек с номерами кластеров в TXT (для Gnuplot)
void writeClustersTXT(const std::string& filename,
                       const std::vector<point>& points,
                       const std::vector<int>& cluster_ids);

// 212-Ким-скрипт Gnuplot для визуализации кластеров (2 PNG + интерактив)
void createClusterGnuplotScript(const std::string& scriptFile, bool staticMode);