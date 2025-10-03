#ifndef SANDPILE_HPP
#define SANDPILE_HPP

#include "tsvparser.hpp"

namespace sandpile{
    void Iteration(uint32_t& grains_4, uint64_t**& matrix,
    int& min_x, int& min_y, int& max_x, int& max_y);
    
}

#endif