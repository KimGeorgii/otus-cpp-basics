// 212-Ким-guess_the_number
// Реализации функций

#include "func.h"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <map>
#include <string>

const std::string high_scores_filename = "high_scores.txt";

// 212-Ким-Функция инициализации генератора случайных чисел
void init_random() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

// 212-Ким-Функция генерации случайного числа в диапазоне [0, max_value - 1]
int generate_random_value(int max_value) {
    return std::rand() % max_value;
}

// 212-Ким-Функция приветствия и ввода имени пользователя
std::string ask_user_name() {
    std::cout << "Hi! Enter your name, please:" << std::endl;
    std::string user_name;
    std::cin >> user_name;
    return user_name;
}

// 212-Ким-Функция игровой цикл: возвращает количество попыток
int play_game(int random_value) {
    int attempts_count = 0;
    int current_value = 0;

    std::cout << "Enter your guess:" << std::endl;

    while (true) {
        std::cin >> current_value;

        if (std::cin.fail()) {
            std::cout << "Bad value! Try again." << std::endl;
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        attempts_count++;

        if (current_value < random_value) {
            std::cout << "The hidden number is greater than " << current_value << std::endl;
        }
        else if (current_value > random_value) {
            std::cout << "The hidden number is less than " << current_value << std::endl;
        }
        else {
            std::cout << "you win!" << std::endl;
            std::cout << "attempts = " << attempts_count << std::endl;
            break;
        }
    }

    return attempts_count;
}

// 212-Ким-Функция сохранения результата в таблицу рекордов
void save_high_score(const std::string& user_name, int attempts_count) {
    std::ofstream out_file{high_scores_filename, std::ios_base::app};
    if (!out_file.is_open()) {
        std::cout << "Failed to open file for write: " << high_scores_filename << "!" << std::endl;
        return;
    }

    out_file << user_name << ' ';
    out_file << attempts_count;
    out_file << std::endl;
}

// 212-Ким-Функция вывода таблицы рекордов на экран
// (с определением минимального значения для каждого игрока)
void print_high_scores() {
    std::ifstream in_file{high_scores_filename};
    if (!in_file.is_open()) {
        std::cout << "Failed to open file for read: " << high_scores_filename << "!" << std::endl;
        return;
    }

    std::cout << "High scores table:" << std::endl;

    std::string username;
    int high_score = 0;

    // Для каждого игрока храним минимальное число попыток
    std::map<std::string, int> best_scores;

    while (true) {
        in_file >> username;
        in_file >> high_score;
        in_file.ignore();

        if (in_file.fail()) {
            break;
        }

        // Если игрок уже есть в таблице — оставляем минимум
        auto it = best_scores.find(username);
        if (it == best_scores.end() || high_score < it->second) {
            best_scores[username] = high_score;
        }
    }

    // Выводим итоговую таблицу (по алфавиту, как в std::map)
    for (const auto& [name, attempts] : best_scores) {
        std::cout << name << '\t' << attempts << std::endl;
    }
}