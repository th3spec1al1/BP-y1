#include "CreateField.hpp"

bool IsOnField(uint64_t x, uint64_t y, PlayingField& field){
    return x >= 0 && x < field.width && y >= 0 && y < field.height;
}


bool IsFreeCoordinate(uint64_t x, uint64_t y, const std::vector<Ship>& my_ships){
    for (size_t shipIndex = 0; shipIndex < my_ships.size(); ++shipIndex){
        for (uint8_t i = 0; i < my_ships[shipIndex].deck; ++i){
            uint64_t shipX = my_ships[shipIndex].x;
            uint64_t shipY = my_ships[shipIndex].y;

            if (my_ships[shipIndex].orientation == 'h'){
                shipX += i;
            }

            else{
                shipY += i;
            }

            if (shipX == x && shipY == y){
                return false;
            }
        }
    }

    return true;
}


bool IsValid(uint64_t x, uint64_t y, uint8_t deck, char orientation, PlayingField& field, const std::vector<Ship>& my_ships){
    for (uint8_t i = 0; i < deck; ++i){
        uint64_t currentX = x;
        uint64_t currentY = y;

        if (orientation == 'h'){
            currentX += i;
        }
        else{
            currentY += i;
        }

        if (!IsOnField(currentX, currentY, field) || !IsFreeCoordinate(currentX, currentY, my_ships)){
            return false;
        }

        for (int dx = -1; dx <= 1; ++dx){
            for (int dy = -1; dy <= 1; ++dy){
                if (dx == 0 && dy == 0){
                    continue;
                }
                if (IsOnField(currentX + dx, currentY + dy, field) && !IsFreeCoordinate(currentX + dx, currentY + dy, my_ships)){
                    return false;
                }
            }
        }
    }

    return true;
}


bool PlaceShips(PlayingField& field, const std::vector<uint8_t>& shipDecks, std::vector<Ship>& my_ships, int shipIndex){
    if (shipIndex == shipDecks.size()){
        return true;
    }

    uint8_t deck = shipDecks[shipIndex];

    for (uint64_t y = 0; y < field.height; ++y){
        for (uint64_t x = 0; x < field.width; ++x){
            for (int orientation = 0; orientation < 2; ++orientation){ 
                char curOrientation;

                if (orientation == 0){
                    curOrientation = 'h'; 
                } 
                else{
                    curOrientation = 'v';
                }

                if (IsValid(x, y, deck, curOrientation, field, my_ships)){
                    Ship newShip {deck, curOrientation, x, y};
                    for (int i=0; i<deck; ++i){
                        newShip.deck_array[i] = true;
                    }

                    my_ships.push_back(newShip);

                    if (PlaceShips(field, shipDecks, my_ships, shipIndex + 1)){
                        return true; 
                    }

                    my_ships.pop_back();
                }
            }
        }
    }

    return false;
}

void CreateField(PlayingField& field, std::vector<Ship>& my_ships){
    std::vector<uint8_t> shipDecks;
    for (int i = 0; i < field.count_of_fourdeck_ships; ++i){
        shipDecks.push_back(4);
    }
    for (int i = 0; i < field.count_of_threedeck_ships; ++i){
        shipDecks.push_back(3);
    }
    for (int i = 0; i < field.count_of_twodeck_ships; ++i){
        shipDecks.push_back(2);
    }
    for (int i = 0; i < field.count_of_onedeck_ships; ++i){
        shipDecks.push_back(1);
    }

    PlaceShips(field, shipDecks, my_ships, 0);
}
