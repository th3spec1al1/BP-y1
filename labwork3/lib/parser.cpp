#include <iostream>
#include "parser.hpp"

namespace arguments_after_parsing{

    void Parse(int argc, char** argv, ConsoleArguments& UserArgs){

        if (argc < 7){
            printf("Error: Few arguments\n");
            exit(1);
        }
        
        int number_of_args = 0;
        for (int i=1; i != argc; ++i){

            if (strcmp(argv[i], "--input") == 0 || strcmp(argv[i], "-i") == 0){
                UserArgs.inputfile = argv[i+1];
                number_of_args++;
            }

            else if (strcmp(argv[i], "--output") == 0 || strcmp(argv[i], "-o") == 0){
                UserArgs.outputfile = argv[i+1];
                number_of_args++;
            }

            else if (strcmp(argv[i], "--max-iter") == 0 || strcmp(argv[i], "-m") == 0){
                UserArgs.max_iterations = atoi(argv[i+1]);
                UserArgs.max_i = true;
                number_of_args++;
            }

            else if (strcmp(argv[i], "--freq") == 0 || strcmp(argv[i], "-f") == 0){
                UserArgs.frequency = atoi(argv[i+1]);
                number_of_args++;
            }

        }

        if (number_of_args < 3){
            printf("Error: Unknown arguments\n");
            exit(1);
        }
    }
}
