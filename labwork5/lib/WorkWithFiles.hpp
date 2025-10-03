#ifndef WWF_HPP
#define WWF_HPP

#include "StructDatabase.hpp"

void LoadFile(std::vector<Ship>& my_ships, PlayingField& field, const std::string& input_path);
void WriteInFile(std::vector<Ship>& my_ships, PlayingField& field, const std::string& output_path);

#endif