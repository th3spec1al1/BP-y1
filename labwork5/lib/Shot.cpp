#include "Shot.hpp"

void ShotInMyDirection(std::vector<Ship>& ships, PlayingField& field, uint64_t& x, uint64_t& y, uint64_t& opp_kills, bool& turn){
    for (int i=0; i<ships.size(); ++i){

        if (ships[i].orientation == 'h'){
            for (int j=0; j<unsigned(ships[i].deck); ++j){

                if ((ships[i].x + j) == x and ships[i].y == y){

                    if (ships[i].deck_array[j] == false){
                        std::cout << "miss" << std::endl;
                        turn = true;
                        return;
                    }

                    ships[i].deck_array[j] = false;
                    opp_kills++;

                    if (ships[i].deck_array[0] + ships[i].deck_array[1] + ships[i].deck_array[2] + ships[i].deck_array[3] == false){
                        std::cout << "kill" << std::endl;
                        ships.erase(ships.begin() + i);
                        return;
                    }

                    else{
                        std::cout << "hit" << std::endl;
                        return;
                    }
                }
            }
        }

        else if (ships[i].orientation == 'v'){
            for (int j=0; j<unsigned(ships[i].deck); ++j){

                if ((ships[i].y + j) == y and ships[i].x == x){

                    if (ships[i].deck_array[j] == false){
                        std::cout << "miss" << std::endl;
                        turn = true;
                        return;
                    }

                    ships[i].deck_array[j] = false;
                    opp_kills++;

                    if (ships[i].deck_array[0] + ships[i].deck_array[1] + ships[i].deck_array[2] + ships[i].deck_array[3] == false){  
                        std::cout << "kill" << std::endl;
                        ships.erase(ships.begin() + i);
                        return;
                    }

                    else{
                        std::cout << "hit" << std::endl;
                        return;
                    }
                }
            }
        }
    }

    std::cout << "miss" << std::endl;
    turn = true;
    return;
}

void ShotInOpponentDirection(PlayingField& field, std::string& strategy, uint64_t& x, uint64_t& y, bool& half_cells){
    if (strategy == "ordered"){
        if (x < field.width){
            std::cout << x << " " << y << std::endl;
            x++;
            return;
        }
        y++;
        if (y < field.height){
            std::cout << 0 << " " << y << std::endl;
            x=1;
            return;
        }
        else{
            std::cout << 0 << " " << 0 << std::endl;
            x=1;
            y=0;
            return;
        }
    }

    else if (strategy == "custom"){
        if (y < field.height) {
            std::cout << x << " " << y << std::endl;
            x += 2;
            if(x >= field.width){
                y++;
                if(y < field.height)
                {
                    x = ((y % 2 == 0 && !half_cells) || (y % 2 == 1 && half_cells)) ? 0 : 1;
                }
            } 
            return;
        } 
        else {
            std::cout << 1 << " " << 0 << std::endl;
            half_cells = true;
            x=1;
            y=0;
            return;
        }
    }

    return;
}