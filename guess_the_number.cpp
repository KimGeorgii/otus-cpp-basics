// 212-Ким-guess_the_number

#include "func.h"

int main() {
    const int max_value = 100;

    init_random();
    const int random_value = generate_random_value(max_value);

    std::string user_name = ask_user_name();
    int attempts_count = play_game(random_value);

    save_high_score(user_name, attempts_count);
    print_high_scores();

    return 0;
}