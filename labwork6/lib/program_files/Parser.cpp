#include "Parser.hpp"

namespace Parser{

    void Parse(int quantity_of_args, char** arguments, ConsoleArguments& Args){  
        if (quantity_of_args != 5){
            std::cerr << "Error: Lack/Excess of arguments" << std::endl;
            exit(1);
        }

        bool last_arg = false;
        for (uint8_t i = 1; i < quantity_of_args; ++i){
            if (last_arg){
                last_arg = false;
                continue;
            }

            if (strcmp(arguments[i], "--date") == 0){
                Args.departure_date = arguments[i+1];
                last_arg = true;
            }

            if (strcmp(arguments[i], "--rout") == 0){
                last_arg = true;

                if (strcmp(arguments[i + 1], "there") == 0){
                    Args.route = true;
                }
                
                else if (strcmp(arguments[i + 1], "back") == 0){
                    Args.route = false;
                    std::string tmp = Args.start;
                    Args.start = Args.end;
                    Args.end = tmp;
                }

                else{
                    std::cerr << "Error: Wrong arguments" << std::endl;
                    exit(1);
                }   
            }
        }
    }

}