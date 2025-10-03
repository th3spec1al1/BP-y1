#include "lib/Parser.cpp"


int main() {

    PlayingField field;

    bool turn_of_move = false;
    bool end = false;
    bool win = false;
    bool stop = true;
    bool half_cells = false;

    std::string strategy = "custom";

    uint64_t x, y;
    uint64_t ordered_x = 0, ordered_y = 0;
    uint64_t my_kills = 0, opp_kills = 0;

    std::vector<Ship> my_ships;

    while(true) {

        std::string cmd;
        std::getline(std::cin, cmd);

        if (cmd == "exit"){
            std::cout << "ok" << std::endl;
            return 0;
        }

        Parse(cmd, my_ships, my_kills, opp_kills, ordered_x, ordered_y, x, y, strategy, stop, win, end, turn_of_move, field, half_cells);

    }

    return 0;
}