//212-Ким-функции и классы

#include "func.h"

#include <cmath>
#include <iostream>
#include <cstdlib>
#include <random>
#include <algorithm>

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

// 212-Ким-класс генерируемой точки
point::point(float a, float b, int c) : x(a), y(b), cluster(c) {}

float point::getX() const { return x; }
float point::getY() const { return y; }
int point::getCluster() const { return cluster; }
void point::setCluster(int c) { cluster = c; }

// 212-Ким-конструктор сетки квадратов
grid::grid(float x0, float y0, float x1, float y1, float cs)
    : x_min(x0), y_min(y0), cell_size(cs) {
    nx = static_cast<int>(std::ceil((x1 - x0) / cs)) + 1;
    ny = static_cast<int>(std::ceil((y1 - y0) / cs)) + 1;
}

float grid::getCellSize() const { return cell_size; }
float grid::getXMin() const { return x_min; }
float grid::getYMin() const { return y_min; }
int grid::getNx() const { return nx; }
int grid::getNy() const { return ny; }

// 212-Ким-определение индекса квадрата для точки
square grid::getSquare(float x, float y) const {
    square s;
    s.ix = static_cast<int>((x - x_min) / cell_size);
    s.iy = static_cast<int>((y - y_min) / cell_size);
    return s;
}

// 212-Ким-конструктор кластера
cluster::cluster(int cluster_id)
    : id(cluster_id), cx(0), cy(0),
      x_min(0), x_max(0), y_min(0), y_max(0) {}

// 212-Ким-добавление точки в кластер
void cluster::addPoint(const point& p) {
    points.push_back(p);
}

// 212-Ким-вычисление центра масс и границ кластера
void cluster::computeStats() {
    if (points.empty()) return;

    float sx = 0.0f;
    float sy = 0.0f;
    x_min = x_max = points[0].getX();
    y_min = y_max = points[0].getY();

    for (const point& p : points) {
        sx += p.getX();
        sy += p.getY();
        if (p.getX() < x_min) x_min = p.getX();
        if (p.getX() > x_max) x_max = p.getX();
        if (p.getY() < y_min) y_min = p.getY();
        if (p.getY() > y_max) y_max = p.getY();
    }

    cx = sx / static_cast<float>(points.size());
    cy = sy / static_cast<float>(points.size());
}

int cluster::getId() const { return id; }
size_t cluster::size() const { return points.size(); }
float cluster::getCX() const { return cx; }
float cluster::getCY() const { return cy; }
float cluster::getXMin() const { return x_min; }
float cluster::getXMax() const { return x_max; }
float cluster::getYMin() const { return y_min; }
float cluster::getYMax() const { return y_max; }
const std::vector<point>& cluster::getPoints() const { return points; }

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
            out << "\n";
        }
        out << "\n";
    }

    std::cout << "Generated " << filename << std::endl;
}

// 212-Ким-запись линий сетки квадратов в файл (в плоскости z=0)
void writeGridLines(const std::string& filename, const grid& g) {
    std::ofstream out(filename);
    if (!out.is_open()) {
        std::cerr << "Error: cannot create " << filename << std::endl;
        return;
    }

    const float x0 = g.getXMin();
    const float y0 = g.getYMin();
    const float cs = g.getCellSize();
    const int nx = g.getNx();
    const int ny = g.getNy();
    const float x1 = x0 + nx * cs;
    const float y1 = y0 + ny * cs;

    for (int i = 0; i <= nx; ++i) {
        float x = x0 + i * cs;
        out << x << " " << y0 << " 0\n";
        out << x << " " << y1 << " 0\n";
        out << "\n";
    }

    for (int j = 0; j <= ny; ++j) {
        float y = y0 + j * cs;
        out << x0 << " " << y << " 0\n";
        out << x1 << " " << y << " 0\n";
        out << "\n";
    }

    std::cout << "Written grid lines to " << filename << std::endl;
}

// 212-Ким-создание скрипта Gnuplot
void createGnuplotScript(const std::string& scriptFile, bool staticMode) {
    std::ofstream out(scriptFile);
    if (!out.is_open()) {
        std::cerr << "Error: cannot create " << scriptFile << std::endl;
        return;
    }

    out << "reset\n";
    out << "set xlabel 'X'\n";
    out << "set ylabel 'Y'\n";
    out << "set zlabel 'Z'\n";
    out << "set grid\n";
    out << "set ticslevel 0\n";
    out << "set hidden3d\n";
    out << "set pm3d depthorder\n";
    out << "set view 60, 30, 1, 1\n";
    out << "set palette model RGB defined (0 'red', 1 'blue', 2 'green', 3 'orange', 4 'purple', 5 'brown')\n";

    if (staticMode) {
        // PNG 1: конусы
        out << "set title 'Poverhnosti konusov (3D)'\n";
        out << "set palette rgb 33,13,10\n";
        out << "set terminal pngcairo size 1200,900\n";
        out << "set output 'cones-3d.png'\n";
        out << "splot 'cone-surface.txt' using 1:2:3 with pm3d title 'Konusy'\n";
        out << "set output\n";
        out << "\n";

        // PNG 2: сетка
        out << "reset\n";
        out << "set title 'Setka kvadratov'\n";
        out << "set xlabel 'X'\n";
        out << "set ylabel 'Y'\n";
        out << "set size ratio -1\n";
        out << "set terminal pngcairo size 1200,900\n";
        out << "set output 'grid-only.png'\n";
        out << "plot 'grid-lines.txt' using 1:2 with lines lc rgb '#333333' lw 0.5 notitle\n";
        out << "set output\n";
        out << "\n";

        // PNG 3: конусы + точки + сетка
        out << "reset\n";
        out << "set title 'Konusy + tochki + setka'\n";
        out << "set xlabel 'X'\n";
        out << "set ylabel 'Y'\n";
        out << "set zlabel 'Z'\n";
        out << "set grid\n";
        out << "set ticslevel 0\n";
        out << "set hidden3d\n";
        out << "set pm3d depthorder\n";
        out << "set view 60, 30, 1, 1\n";
        out << "set palette model RGB defined (0 'red', 1 'blue', 2 'green', 3 'orange', 4 'purple', 5 'brown')\n";
        out << "set terminal pngcairo size 1200,900\n";
        out << "set output 'cones-combined.png'\n";
        out << "splot 'cone-surface.txt' using 1:2:3 with pm3d title 'Konusy', \\\n";
        out << "      'grid-lines.txt' using 1:2:3 with lines lc rgb '#aaaaaa' lw 0.3 notitle, \\\n";
        out << "      'cones-points-sorted.txt' using 1:2:(0):3 with points pt 7 ps 0.5 palette title 'Tochki'\n";
        out << "set output\n";
    } else {
        out << "set title 'Konusy + tochki + setka'\n";
        out << "splot 'cone-surface.txt' using 1:2:3 with pm3d title 'Konusy', \\\n";
        out << "      'grid-lines.txt' using 1:2:3 with lines lc rgb '#aaaaaa' lw 0.3 notitle, \\\n";
        out << "      'cones-points-sorted.txt' using 1:2:(0):3 with points pt 7 ps 0.5 palette title 'Tochki'\n";
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

// 212-Ким-генерация count точек 2D-Гаусса под конусом
// sigma = R/2, тогда 2*sigma = R — точек ~95% внутри круга основания
std::vector<point> generateGaussianPoints(const cone& c, int count) {
    std::vector<point> points;
    points.reserve(count);

    const float sigma = c.getR() / 2.0f;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> dist_x(c.getX(), sigma);
    std::normal_distribution<float> dist_y(c.getY(), sigma);

    int generated = 0;
    while (generated < count) {
        float px = dist_x(gen);
        float py = dist_y(gen);

        float dx = px - c.getX();
        float dy = py - c.getY();
        if (dx * dx + dy * dy <= c.getR() * c.getR()) {
            points.emplace_back(px, py, 0);
            generated++;
        }
    }

    return points;
}

// 212-Ким-сортировка точек по квадратам, затем по x и y
std::vector<point> sortPointsBySquares(const std::vector<point>& points,
                                       const grid& g) {
    std::vector<point> sorted = points;

    std::sort(sorted.begin(), sorted.end(),
        [&g](const point& a, const point& b) {
            square sa = g.getSquare(a.getX(), a.getY());
            square sb = g.getSquare(b.getX(), b.getY());
            if (sa.ix != sb.ix) return sa.ix < sb.ix;
            if (sa.iy != sb.iy) return sa.iy < sb.iy;
            if (a.getX() != b.getX()) return a.getX() < b.getX();
            return a.getY() < b.getY();
        });

    return sorted;
}

// 212-Ким-запись точек в TXT: x y cluster square_ix square_iy
void writePointsTXT(const std::string& filename,
                    const std::vector<point>& points,
                    const grid& g) {
    std::ofstream out(filename);
    if (!out.is_open()) {
        std::cerr << "Error: cannot create " << filename << std::endl;
        return;
    }

    for (const point& p : points) {
        square s = g.getSquare(p.getX(), p.getY());
        out << p.getX() << " " << p.getY() << " "
            << p.getCluster() << " "
            << s.ix << " " << s.iy << "\n";
    }

    std::cout << "Written " << points.size()
              << " points to " << filename << std::endl;
}

// 212-Ким-матричный алгоритм Волна: поиск связных компонент
std::vector<cluster> runWaveClustering(const std::vector<point>& points,
                                        float threshold,
                                        std::vector<int>& out_cluster_ids) {
    const size_t n = points.size();
    out_cluster_ids.assign(n, -1);

    std::vector<cluster> clusters;
    if (n == 0) return clusters;

    // 212-Ким-матрица инцидентности
    std::vector<char> B(n * n, 0);
    const float t2 = threshold * threshold;

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            float dx = points[i].getX() - points[j].getX();
            float dy = points[i].getY() - points[j].getY();
            if (dx * dx + dy * dy < t2) {
                B[i * n + j] = 1;
                B[j * n + i] = 1;
            }
        }
    }

    // 212-Ким-вектор состояний
    std::vector<int> a(n, 0);

    int cluster_counter = 0;
    std::vector<size_t> current;
    std::vector<size_t> next;

    for (size_t start = 0; start < n; ++start) {
        if (a[start] != 0) continue;

        a[start] = 1;
        out_cluster_ids[start] = cluster_counter;
        current.clear();
        current.push_back(start);

        int step = 1;
        while (!current.empty()) {
            next.clear();

            for (size_t i : current) {
                for (size_t j = 0; j < n; ++j) {
                    if (B[i * n + j] && a[j] == 0) {
                        a[j] = step + 1;
                        out_cluster_ids[j] = cluster_counter;
                        next.push_back(j);
                    }
                }
            }

            current.swap(next);
            step++;
        }

        cluster_counter++;
    }

    // 212-Ким-сборка кластеров
    clusters.reserve(cluster_counter);
    for (int i = 0; i < cluster_counter; ++i) {
        clusters.emplace_back(i);
    }
    for (size_t i = 0; i < n; ++i) {
        clusters[out_cluster_ids[i]].addPoint(points[i]);
    }
    for (auto& c : clusters) {
        c.computeStats();
    }

    std::cout << "Wave clustering: " << cluster_counter
              << " clusters found (threshold = " << threshold << ")" << std::endl;

    return clusters;
}

// 212-Ким-печать статистики по кластерам
void printClusterStats(const std::vector<cluster>& clusters) {
    std::cout << "\n=== Cluster statistics ===" << std::endl;
    for (const cluster& c : clusters) {
        std::cout << "Cluster " << c.getId()
                  << ": size = " << c.size() << "\n"
                  << "  center of mass = (" << c.getCX() << ", " << c.getCY() << ")\n"
                  << "  bounding box: x in [" << c.getXMin() << ", " << c.getXMax()
                  << "], y in [" << c.getYMin() << ", " << c.getYMax() << "]\n";
    }
    std::cout << "===========================\n" << std::endl;
}

// 212-Ким-запись точек с номерами кластеров в TXT (для Gnuplot)
void writeClustersTXT(const std::string& filename,
                       const std::vector<point>& points,
                       const std::vector<int>& cluster_ids) {
    std::ofstream out(filename);
    if (!out.is_open()) {
        std::cerr << "Error: cannot create " << filename << std::endl;
        return;
    }

    for (size_t i = 0; i < points.size(); ++i) {
        out << points[i].getX() << " "
            << points[i].getY() << " "
            << cluster_ids[i] << "\n";
    }

    std::cout << "Written " << points.size()
              << " points with cluster ids to " << filename << std::endl;
}

// 212-Ким-скрипт Gnuplot для визуализации кластеров (2 PNG + интерактив)
void createClusterGnuplotScript(const std::string& scriptFile, bool staticMode) {
    std::ofstream out(scriptFile);
    if (!out.is_open()) {
        std::cerr << "Error: cannot create " << scriptFile << std::endl;
        return;
    }

    out << "reset\n";
    out << "set xlabel 'X'\n";
    out << "set ylabel 'Y'\n";
    out << "set zlabel 'Z'\n";
    out << "set grid\n";
    out << "set ticslevel 0\n";
    out << "set hidden3d\n";
    out << "set pm3d depthorder\n";
    out << "set view 60, 30, 1, 1\n";
    out << "set palette rgb 33,13,10\n";

    if (staticMode) {
        // PNG 1: 2D-вид — только кластеры
        out << "reset\n";
        out << "set title 'Wave clustering result (2D)'\n";
        out << "set xlabel 'X'\n";
        out << "set ylabel 'Y'\n";
        out << "set size ratio -1\n";
        out << "set grid\n";
        out << "set palette rgb 33,13,10\n";
        out << "set terminal pngcairo size 1200,900\n";
        out << "set output 'clusters.png'\n";
        out << "plot 'clusters-points.txt' using 1:2:3 with points pt 7 ps 0.5 palette title 'Clusters'\n";
        out << "set output\n";
        out << "\n";

        // PNG 2: 3D-вид — конусы + кластеры
        out << "reset\n";
        out << "set title 'Wave clustering + cones (3D)'\n";
        out << "set xlabel 'X'\n";
        out << "set ylabel 'Y'\n";
        out << "set zlabel 'Z'\n";
        out << "set grid\n";
        out << "set ticslevel 0\n";
        out << "set hidden3d\n";
        out << "set pm3d depthorder\n";
        out << "set view 60, 30, 1, 1\n";
        out << "set palette rgb 33,13,10\n";
        out << "set terminal pngcairo size 1200,900\n";
        out << "set output 'clusters-3d.png'\n";
        out << "splot 'cone-surface.txt' using 1:2:3 with pm3d title 'Konusy', \\\n";
        out << "      'clusters-points.txt' using 1:2:(0):3 with points pt 7 ps 0.5 palette title 'Clusters'\n";
        out << "set output\n";
    } else {
        out << "set title 'Wave clustering + cones - vrashchayte myshyu'\n";
        out << "splot 'cone-surface.txt' using 1:2:3 with pm3d title 'Konusy', \\\n";
        out << "      'clusters-points.txt' using 1:2:(0):3 with points pt 7 ps 0.5 palette title 'Clusters'\n";
        out << "pause -1 'Нажми Enter для выхода...'\n";
    }

    out.close();
    std::cout << "Created " << scriptFile << std::endl;
}