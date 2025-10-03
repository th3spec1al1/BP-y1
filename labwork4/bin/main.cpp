#include <functional>
#include <lib/ArgParser.h>

#include <iostream>
#include <numeric>

struct Options {
    bool sum = false;
    bool mult = false;
};

int main(int argc, char** argv) {
    Options opt;
    std::vector<int> values;

    ArgumentParser::ArgParser parser("Program");
    parser.AddIntArgument("N").MultiValue(1).Positional().StoreValues(values);
    parser.AddFlag("sum", "add args").StoreValue(opt.sum);
    parser.AddFlag("mult", "multiply args").StoreValue(opt.mult);
    parser.AddHelp('h', "help", "Program accumulate arguments");

    if(!parser.Parse(argc, argv)) {
        std::cout << "Wrong argument" << std::endl;
        std::cout << parser.HelpDescription() << std::endl;
        return 1;
    }

    if(parser.Help()) {
        std::cout << parser.HelpDescription() << std::endl;
        return 0;
    }

    bool flag = false;
    if(opt.sum) {
        std::cout << "Sum Result: " << std::accumulate(values.begin(), values.end(), 0) << std::endl;
        flag = true;
    } 
    if(opt.mult) {
        std::cout << "Mult Result: " << std::accumulate(values.begin(), values.end(), 1, std::multiplies<int>()) << std::endl;
        flag = true;
    } 
    
    if (!flag){
        std::cout << "No one options had chosen" << std::endl;
        std::cout << parser.HelpDescription();
        return 1;
    }

    return 0;

}
