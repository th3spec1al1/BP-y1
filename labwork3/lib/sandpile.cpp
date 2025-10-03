#include <iostream>
#include <fstream>
#include "sandpile.hpp"

namespace sandpile{
    void Iteration(uint32_t& grains_4, uint64_t**& matrix,
    int& min_x, int& min_y, int& max_x, int& max_y){
        int new_min_x = min_x, new_max_x = max_x, new_min_y = min_y, new_max_y = max_y;

        for (int32_t i = min_y; i < max_y + 1; i++){
            for (int32_t j = min_x; j < max_x + 1; j++){

                if (matrix[i-min_y][j-min_x] >= 4){
                    if (i == max_y){
                        new_max_y = max_y + 1;
                    }
                    if (i == min_y){
                        new_min_y = min_y - 1;
                    }
                    if (j == max_x){
                        new_max_x = max_x + 1;
                    }
                    if (j == min_x){
                        new_min_x = min_x - 1;
                    }
                }
            }
        }
        
        if (new_max_x == max_x && new_max_y == max_y && new_min_x == min_x && new_min_y == min_y){
            // ничего не делаю, так как матрица не расширяется
        }

        else{
            uint64_t** new_matrix = new uint64_t*[new_max_y - new_min_y + 1];
            for (int32_t i = 0; i < new_max_y - new_min_y + 1; i++){
                new_matrix [i] = new uint64_t[new_max_x - new_min_x + 1]{0};
            }
            
            for (int32_t i = min_y; i < max_y + 1; i++){
                for (int32_t j = min_x; j < max_x + 1; j++){
                    new_matrix[i-new_min_y][j-new_min_x] = matrix[i-min_y][j-min_x];
                }
            }

            for (int32_t i = 0; i < max_y - min_y + 1; i++){
                delete [] matrix[i];
            }
            delete [] matrix;

            matrix = new_matrix;
        }

        uint64_t** new_matrix = new uint64_t*[new_max_y - new_min_y + 1];
        for (int32_t i = 0; i < new_max_y - new_min_y + 1; i++){
            new_matrix [i] = new uint64_t[new_max_x - new_min_x + 1]{0};
        }
            
        for (int32_t i = new_min_y; i < new_max_y + 1; i++){
            for (int32_t j = new_min_x; j < new_max_x + 1; j++){
                new_matrix[i-new_min_y][j-new_min_x] = matrix[i-new_min_y][j-new_min_x];
            }
        }

        for (int32_t i = new_min_y; i < new_max_y + 1; i++){
            for (int32_t j = new_min_x; j < new_max_x + 1; j++){
                if (new_matrix[i - new_min_y][j - new_min_x] >= 4){
                    matrix[i - new_min_y][j - new_min_x] = matrix[i - new_min_y][j - new_min_x] - 4;
                    matrix[i - new_min_y - 1][j - new_min_x]++;
                    matrix[i - new_min_y + 1][j - new_min_x]++;
                    matrix[i - new_min_y][j - new_min_x - 1]++;
                    matrix[i - new_min_y][j - new_min_x + 1]++;
                }
            }
        }

        for (int32_t i = 0; i < new_max_y - new_min_y + 1; i++){
                delete [] new_matrix[i];
            }
        delete [] new_matrix;

        grains_4 = 0;
        for (int32_t i = new_min_y; i < new_max_y + 1; i++){
            for (int32_t j = new_min_x; j < new_max_x + 1; j++){
                if (matrix[i - new_min_y][j - new_min_x] >= 4){
                    grains_4++;
                }
            }
        }

        min_x = new_min_x;
        min_y = new_min_y;
        max_x = new_max_x;
        max_y = new_max_y;
    }
}