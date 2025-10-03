#include <iostream>
#include <fstream>
#include "lib/parser.hpp"
#include "lib/tsvparser.hpp"
#include "lib/sandpile.hpp"
#include "lib/createBMP.hpp"
#include "lib/fillBMP.hpp"

int main(int argc, char** argv){
    arguments_after_parsing::ConsoleArguments UserArgs;
    arguments_after_parsing::Parse(argc, argv, UserArgs);

    uint32_t grains_4 = 0;
    uint32_t iteration = 0;
    
    int min_x = 10000000, min_y = 10000000, max_x = -10000000, max_y = -10000000;

    tsv_after_parsing::ReadTsv1(UserArgs.inputfile, grains_4, min_x, min_y, max_x, max_y);

    uint64_t **matrix = new uint64_t*[max_y - min_y + 1];
    for (int i = 0; i < max_y - min_y + 1; i++){
        matrix[i] = new uint64_t[max_x - min_x + 1]{0};
    }

    tsv_after_parsing::ReadTsv2(UserArgs.inputfile, matrix, min_x, min_y, max_x, max_y);   

    while (grains_4 != 0){
        if (UserArgs.max_i == true && UserArgs.max_iterations == iteration){
            break;
        }

        iteration++;
        sandpile::Iteration(grains_4, matrix, min_x, min_y, max_x, max_y);  
        

        const int width = max_x - min_x + 1;
        const int height = max_y - min_y + 1;

        if (UserArgs.frequency > 0){
            if (iteration % UserArgs.frequency == 0){
                Fill(width, height, UserArgs.outputfile, matrix);
            }    
        }
    }
    
    const int width = max_x - min_x + 1;
    const int height = max_y - min_y + 1;

    Fill(width, height, UserArgs.outputfile, matrix);

    for (int i = 0; i < max_y - min_y + 1; i++){
        delete [] matrix[i];
    }
    delete [] matrix;

    return 0;
}