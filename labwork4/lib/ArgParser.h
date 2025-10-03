#pragma once

#include <iostream>
#include <vector>
#include <cstdint>

namespace ArgumentParser {

    class ArgParser {

    private:

        class StringArgument{
            friend ArgParser;

        public:

            StringArgument(const std::string& param);
            StringArgument(char short_param, const std::string& param);

            ArgParser::StringArgument& Default(std::string value);
            ArgParser::StringArgument& StoreValue(std::string& value);
            ArgParser::StringArgument& MultiValue(size_t min_count=0);

        private:

            std::string key;
            std::string name;
            char short_name=' ';
            bool default_flag = false;
            std::string default_value;
            bool multi_flag = false;
            std::vector<std::string> keys;
            size_t min_count_arguments = 0;
            std::string help_information;
            std::string* store_value = nullptr;
            std::vector<std::string>* store_values = nullptr;

        };  // class StringArgument

        class IntArgument{
            friend ArgParser;

        public:

            IntArgument(const std::string& param);
            IntArgument(char short_param, const std::string& param);

            ArgParser::IntArgument& Default(int value);
            ArgParser::IntArgument& StoreValue(int& value);
            ArgParser::IntArgument& StoreValues(std::vector<int>& value);
            ArgParser::IntArgument& MultiValue(size_t min_count=0);
            ArgParser::IntArgument& Positional();

        private:

            int key;
            std::string name;
            char short_name=' ';
            bool default_flag = false;
            int default_value;
            bool positional = false;
            bool multi_flag = false;
            std::vector<int> keys;
            size_t min_count_arguments = 0;
            std::string help_information;
            int* store_value = nullptr;
            std::vector<int>* store_values = nullptr;

        };  // class IntArgument

        class FlagArgument{
            friend ArgParser;

        public:

            FlagArgument(const std::string& param);
            FlagArgument(char short_param, const std::string& param);

            ArgParser::FlagArgument& Default(bool value);
            ArgParser::FlagArgument& StoreValue(bool& value);

        private:

            std::string name;
            char short_name=' ';
            bool default_flag=false;
            bool key=false;
            std::string help_information;
            bool* store_value = nullptr;
            

        };  // class FlagArgument

        std::vector<StringArgument*> string_arguments;
        std::vector<IntArgument*> int_arguments;
        std::vector<FlagArgument*> flag_arguments;

        char short_name_help;
        std::string name_help;
        std::string description_help;
        bool have_help = false;
        bool use_help = false;
        
        std::string past_arg;
        char short_past_arg = ' ';
        bool past_arg_free = false;

    public:

        std::string parser_name;
        ArgParser(const std::string& str);

        bool Parse(int argc, char** argv);
        bool Parse(const std::vector<std::string>& input_data);

        ArgParser::IntArgument& AddIntArgument(const std::string& full_name, const std::string& description="");
        ArgParser::IntArgument& AddIntArgument(char letter_name, const std::string& full_name, const std::string& description="");

        int GetIntValue(const std::string& param, size_t index=0);

        ArgParser::StringArgument& AddStringArgument(const std::string& full_name, const std::string& description="");
        ArgParser::StringArgument& AddStringArgument(char letter_name, const std::string& full_name, const std::string& description="");

        std::string GetStringValue(const std::string& param, size_t index=0);

        ArgParser::FlagArgument& AddFlag(const std::string& full_name, const std::string& description="");
        ArgParser::FlagArgument& AddFlag(char letter_name, const std::string& full_name, const std::string& description="");

        bool GetFlag(const std::string& param);

        void AddHelp(char letter_name,const std::string& full_name,const std::string& description="");
        const std::string HelpDescription();
        bool Help();

        ~ArgParser(){
            for (int i=0;i<string_arguments.size();i++){
                delete string_arguments[i];
            }
            for (int i=0;i<int_arguments.size();i++){
                delete int_arguments[i];
            }
            for (int i=0;i<flag_arguments.size();i++){
                delete flag_arguments[i];
            }
        };

    };  // class ArgParser

} // namespace ArgumentParser