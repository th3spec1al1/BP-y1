#ifndef APARSER_HPP
#define APARSER_HPP

namespace arguments_after_parsing{

    struct ConsoleArguments{
        const char* inputfile = nullptr;
        const char* outputfile = nullptr;
        uint64_t max_iterations;
        bool max_i = false;
        uint64_t frequency;
    };

    void Parse(int argc, char** argv, ConsoleArguments& UserArgs);
}

#endif