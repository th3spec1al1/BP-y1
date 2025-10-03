#ifndef CRF_HPP
#define CRF_HPP

#include "StructDatabase.hpp"

bool IsOnField(uint64_t x, uint64_t y, PlayingField& field);

bool IsFreeCoordinate(uint64_t x, uint64_t y, const std::vector<Ship>& my_ships);

bool IsValid(uint64_t x, uint64_t y, uint8_t deck, char orientation, PlayingField& field, const std::vector<Ship>& my_ships);

bool PlaceShips(PlayingField& field, const std::vector<uint8_t>& shipDecks, std::vector<Ship>& my_ships, int shipIndex);

void CreateField(PlayingField& field, std::vector<Ship>& my_ships);

#endif