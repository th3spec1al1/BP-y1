#ifndef TPARSER_HPP
#define TPARSER_HPP

namespace tsv_after_parsing{

    void ReadTsv1(const char* InputPath, uint32_t& grains_4,
    int& min_x, int& min_y, int& max_x, int& max_y);

    void ReadTsv2(const char* InputPath, uint64_t**& matrix,
    int& min_x, int& min_y, int& max_x, int& max_y);
}

#endif