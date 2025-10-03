#ifndef SDATA_HPP
#define SDATA_HPP

#include <string>
#include <cstdint>
#include <vector>
#include <fstream>
#include <iostream>
#include <ctime>

struct PlayingField
{
    uint64_t width = 0;
    uint64_t height = 0;

    uint64_t count_of_onedeck_ships = 0;
    uint64_t count_of_twodeck_ships = 0;
    uint64_t count_of_threedeck_ships = 0;
    uint64_t count_of_fourdeck_ships = 0;

    uint64_t opp_count_of_onedeck_ships = 0;
    uint64_t opp_count_of_twodeck_ships = 0;
    uint64_t opp_count_of_threedeck_ships = 0;
    uint64_t opp_count_of_fourdeck_ships = 0;
};

struct Ship
{
    uint8_t deck;
    char orientation;
    uint64_t x;
    uint64_t y;
    bool deck_array[4]{false};
};

#endif