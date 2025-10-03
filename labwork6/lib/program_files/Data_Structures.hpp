#ifndef DTSTRC_HPP
#define DTSTRC_HPP

#include <iostream>
#include <stdint.h>
#include <cstring>
#include <filesystem>
#include <ctime>
#include <string>

#include <nlohmann/json.hpp>
#include <cpr/cpr.h>

struct ConsoleArguments {
    std::string departure_date;
    bool route = false;
    std::string start = "c2";
    std::string end = "c25";
};

struct DataValues {
    const std::string source = "https://api.rasp.yandex.net/v3.0/search/";
    const std::string key = "30d63684-2e1d-401c-9f35-9082ba566e9e";
};

#endif