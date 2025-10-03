#include "WorkWithFiles.hpp"

void LoadFile(std::vector<Ship>& my_ships, PlayingField& field, const std::string& input_path){
    std::ifstream input_file;
    input_file.open(input_path);

    char str[1000];
    input_file.getline(str, '\n');
    sscanf(str, "%llu %llu", &field.width, &field.height);

    while (input_file.getline(str, '\n')){

        Ship ship;
        sscanf(str, "%hhu %c %llu %llu", &ship.deck, &ship.orientation, &ship.x, &ship.y);

        for (int i=0; i < ship.deck; ++i){
            ship.deck_array[i] = true;
        }

        if (ship.deck == 1){
            field.count_of_onedeck_ships++;
        }
        else if (ship.deck == 2){
            field.count_of_twodeck_ships++;
        }
        else if (ship.deck == 3){
            field.count_of_threedeck_ships++;
        }
        else if (ship.deck == 4){
            field.count_of_fourdeck_ships++;
        }

        my_ships.push_back(ship);
    }

    input_file.close();
}

void WriteInFile(std::vector<Ship>& my_ships, PlayingField& field, const std::string& output_path){
    std::ofstream output_file;
    output_file.open(output_path);

    output_file << field.width << " " << field.height << std::endl;
    for (int i = 0; i < my_ships.size(); ++i){
        output_file << std::to_string(my_ships[i].deck) << " " << my_ships[i].orientation << " " << 
            std::to_string(my_ships[i].x) << " " << std::to_string(my_ships[i].y) << std::endl;
    }

    output_file.close();
}