#ifndef SHOT_HPP
#define SHOT_HPP

#include "StructDatabase.hpp"

void ShotInMyDirection(std::vector<Ship>& ships, PlayingField& field, uint64_t& x, uint64_t& y, uint64_t& opp_kills, bool& turn);

void ShotInOpponentDirection(PlayingField& field, std::string& strategy, uint64_t& x, uint64_t& y);

#endif