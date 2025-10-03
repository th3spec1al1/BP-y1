#include "Parser.hpp"

void Parse(std::string& cmd, std::vector<Ship>& my_ships, uint64_t& my_kills, uint64_t& opp_kills,
 uint64_t& ordered_x, uint64_t& ordered_y, uint64_t& x, uint64_t& y, std::string& strategy, bool& stop,
 bool& win, bool& end, bool& turn_of_move, PlayingField& field, bool& half_cells){

    srand(time(NULL));

    if(cmd == "ping") {
        std::cout << "pong" << std::endl;
    }


    else if (cmd == "create slave"){
        turn_of_move = true;
        std::cout << "ok" << std::endl;
    }

 
    else if (cmd == "create master"){
        LoadFile(my_ships, field, master_files[(rand()%3)]);
        std::cout << "ok" << std::endl;
    }


    else if (cmd == "start"){
        stop = false;
        if (my_ships.size() == 0){
            CreateField(field, my_ships);
        }
        std::cout << "ok" << std::endl;
    }


    else if (cmd == "stop"){
        stop = true;
        std::cout << "ok" << std::endl;
    }


    else if (cmd.find("set width") != std::string::npos){
        field.width = stoull(cmd.substr(10));
        std::cout << "ok" << std::endl;
    }


    else if (cmd == "get width"){
        std::cout << field.width << std::endl;
    }


    else if (cmd.find("set height") != std::string::npos){
        field.height = stoull(cmd.substr(11));
        std::cout << "ok" << std::endl;
    }


    else if (cmd == "get height"){
        std::cout << field.height << std::endl;
    }


    else if (cmd.find("set count") != std::string::npos){
        if (cmd[10] == '1'){
            field.count_of_onedeck_ships = stoull(cmd.substr(12));
            std::cout << "ok" << std::endl;
        }
        else if (cmd[10] == '2'){
            field.count_of_twodeck_ships = stoull(cmd.substr(12));
            std::cout << "ok" << std::endl;
        }
        else if (cmd[10] == '3'){
            field.count_of_threedeck_ships = stoull(cmd.substr(12));
            std::cout << "ok" << std::endl;
        }
        else if (cmd[10] == '4'){
            field.count_of_fourdeck_ships = stoull(cmd.substr(12));
            std::cout << "ok" << std::endl;
        }
    }


    else if (cmd.find("get count") != std::string::npos){
        if (cmd[10] == '1'){
            std::cout << field.count_of_onedeck_ships << std::endl;
        }
        else if (cmd[10] == '2'){
            std::cout << field.count_of_twodeck_ships << std::endl;
        }
        else if (cmd[10] == '3'){
            std::cout << field.count_of_threedeck_ships << std::endl;
        }
        else if (cmd[10] == '4'){
            std::cout << field.count_of_fourdeck_ships << std::endl;
        }
    }


    else if (cmd.find("set strategy") != std::string::npos){
        strategy = cmd.substr(13);
        std::cout << "ok" << std::endl;
    }


    else if (cmd == "shot"){

        if (turn_of_move == true){
            ShotInOpponentDirection(field, strategy, ordered_x, ordered_y, half_cells);
        }
        
        else{
            std::cout << "It's not my turn" << std::endl;
        }
    }


    else if (cmd.find("shot") != std::string::npos){

        if (turn_of_move == false){
            std::sscanf(cmd.c_str(), "shot %llu %llu", &x, &y);
            ShotInMyDirection(my_ships, field, x, y, opp_kills, turn_of_move);
        }
        
        else{
            std::cout << "It's not your turn" << std::endl;
        }
    }


    else if (cmd.find("set result") != std::string::npos){

        if (cmd.substr(11) == "miss"){
            turn_of_move = false;
        }

        else if (cmd.substr(11) == "kill" || cmd.substr(11) == "hit"){
            my_kills++;
        }

        std::cout << "ok" << std::endl;
    }


    else if (cmd == "finished"){
        if (end == true){
            std::cout << "yes" << std::endl;
        }
        else{
            std::cout << "no" << std::endl;
        }
    }


    else if (cmd == "win"){
        if (end == true){
            if (win == true){
                std::cout << "yes" << std::endl;
            }
            else{
                std::cout << "no" << std::endl;
            }
        }
        else{
            std::cout << "game isn't over" << std::endl;
        }
    }


    else if (cmd == "lose"){
        if (end == true){
            if (win == true){
                std::cout << "no" << std::endl;
            }
            else{
                std::cout << "yes" << std::endl;
            }
        }
        else{
            std::cout << "game isn't over" << std::endl;
        }
    }


    else if (cmd.find("dump") != std::string::npos){
        WriteInFile(my_ships, field, cmd.substr(5));
        std::cout << "ok" << std::endl;
    }


    else if (cmd.find("load") != std::string::npos){
        LoadFile(my_ships, field, cmd.substr(5));
        std::cout << "ok" << std::endl;
    }


    if (stop == false && end == false){
        if (field.count_of_onedeck_ships + field.count_of_twodeck_ships*2 +
            field.count_of_threedeck_ships*3 + field.count_of_fourdeck_ships*4 == opp_kills){
            end = true;
            win = false;
        }

        else if (field.count_of_onedeck_ships + field.count_of_twodeck_ships*2 +
            field.count_of_threedeck_ships*3 + field.count_of_fourdeck_ships*4 == my_kills){
            end = true;
            win = true;
        }
    }

    return;
}
