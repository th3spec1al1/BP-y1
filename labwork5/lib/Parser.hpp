#ifndef PARSER_HPP
#define PARSER_HPP

#include "StructDatabase.hpp"
#include "WorkWithFiles.cpp"
#include "Shot.cpp"
#include "CreateField.cpp"

const std::string master_files[5] = {"lib/files/Master1.txt", "lib/files/Master2.txt", 
                                     "lib/files/Master3.txt", "lib/files/Master4.txt", 
                                     "lib/files/Master5.txt"};

void Parse(std::string& cmd, std::vector<Ship>& my_ships, uint64_t& my_kills, uint64_t& opp_kills,
 uint64_t& ordered_x, uint64_t& ordered_y, uint64_t& x, uint64_t& y, std::string& strategy, bool& stop,
 bool& win, bool& end, bool& turn_of_move, PlayingField& field);

#endif