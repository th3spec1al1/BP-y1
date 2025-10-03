#include <iostream>
#include <fstream>
#include "tsvparser.hpp"

namespace tsv_after_parsing{

    void ReadTsv1(const char* InputPath, uint32_t& grains_4, int& min_x, int& min_y,
    int& max_x, int& max_y){
        std::ifstream file(InputPath);
        if (!file.is_open()){
            printf("Error: Cannot open file: %s\n", InputPath);
            exit(1);
        }

        while (file.good()){
            uint64_t grains;
            int x, y;
            file >> x;
            file >> y;
            file >> grains;

            if (x < min_x){
                min_x = x;
            }
            if (y < min_y){
                min_y = y;
            }

            if (x > max_x){
                max_x = x;
            }
            if (y > max_y){
                max_y = y;
            }

            if (grains >= 4){
                grains_4++;
            }
        }
        file.close();
    }   

    void ReadTsv2(const char* InputPath, uint64_t**& matrix,
    int& min_x, int& min_y, int& max_x, int& max_y){
        std::ifstream file(InputPath);
        if (!file.is_open()){
            printf("Error: Cannot open file: %s\n", InputPath);
            exit(1);
        }
        
        while (file.good()){
            uint64_t grains;
            int x, y;
            file >> x;
            file >> y;
            file >> grains;
            matrix[y - min_y][x - min_x] = grains; 
            
        }
        file.close();
    }
}
