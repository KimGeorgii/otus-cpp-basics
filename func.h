// 212-Ким-guess_the_number
// объявление функций

#pragma once

#include <string>

// 212-Ким-Функция инициализации генератора случайных чисел
void init_random();

// 212-Ким-Функция генерации случайного числа в диапазоне [0, max_value - 1]
int generate_random_value(int max_value);

// 212-Ким-Функция приветствия и ввода имени пользователя
std::string ask_user_name();

// 212-Ким-Функция игровой цикл: возвращает количество попыток
int play_game(int random_value);

// 212-Ким-Функция сохранения результата в таблицу рекордов
void save_high_score(const std::string& user_name, int attempts_count);

// 212-Ким-Функция вывода таблицы рекордов на экран
void print_high_scores();