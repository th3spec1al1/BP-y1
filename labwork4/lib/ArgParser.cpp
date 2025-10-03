#include "ArgParser.h"

using namespace ArgumentParser;


// private

ArgParser::IntArgument::IntArgument(const std::string &param)
:name(param){}


ArgParser::IntArgument::IntArgument(char short_param, const std::string &param)
:short_name(short_param), name(param){}


ArgParser::IntArgument &ArgParser::IntArgument::Default(int value){
    key = value;
    default_flag = true;
    default_value = value;
    return *this;
}


ArgParser::IntArgument &ArgParser::IntArgument::StoreValue(int &value){
    key=value;
    store_value=&value;
    return *this;
}


ArgParser::IntArgument &ArgParser::IntArgument::StoreValues(std::vector<int> &value){
    store_values=&value;
    return *this;
}


ArgParser::IntArgument &ArgParser::IntArgument::MultiValue(size_t min_count){
    min_count_arguments = min_count;
    multi_flag = true;
    return *this;  
}


ArgParser::IntArgument &ArgParser::IntArgument::Positional(){
    positional = true;
    return *this;
}


ArgParser::StringArgument::StringArgument(const std::string &param)
:name(param){}


ArgParser::StringArgument::StringArgument(char short_param, const std::string &param)
:short_name(short_param), name(param){}


ArgParser::StringArgument &ArgParser::StringArgument::Default(std::string value){
    key = value;
    default_flag = true;
    default_value = value;
    return *this;
}


ArgParser::StringArgument &ArgParser::StringArgument::StoreValue(std::string &value){
    key=value;
    store_value=&value;
    return *this;
}


ArgParser::StringArgument &ArgParser::StringArgument::MultiValue(size_t min_count){
    min_count_arguments = min_count;
    multi_flag = true;
    return *this;  
}


ArgParser::FlagArgument::FlagArgument(const std::string &param)
:name(param){}


ArgParser::FlagArgument::FlagArgument(char short_param, const std::string &param)
:short_name(short_param), name(param){}


ArgParser::FlagArgument& ArgParser::FlagArgument::Default(bool value){
    key = value;
    default_flag = true;
    return *this;
}


ArgParser::FlagArgument& ArgParser::FlagArgument::StoreValue(bool &value){
    key=value;
    store_value=&value;
    return *this;
}



// public

ArgParser::ArgParser(const std::string &str) 
    :parser_name(str){}


bool ArgParser::Parse(const std::vector<std::string>& input_data){
    if (input_data.size() == 0) {
        return true;
    }

    if (input_data.size() == 1) {
        if (string_arguments.empty() && int_arguments.empty() && flag_arguments.empty()) {
            return true;
        }

        for (int i=0; i<string_arguments.size(); ++i){
            if (string_arguments[i]->default_flag == false){
                return false;
            }
        }

        for (int i=0; i<int_arguments.size(); ++i){
            if (int_arguments[i]->default_flag == false){
                return false;
            }
        }

        return true;
    }

    for (int i=1; i<input_data.size(); ++i){

        if (input_data[i][0] == '-'){
            std::string value;

            if (input_data[i][1] == '-'){

                size_t index = input_data[i].find('=');
                std::string current_argument;

                if (index == std::string::npos){
                    
                    current_argument = input_data[i].substr(2);

                    if (have_help == true){
                        if (current_argument == name_help){
                            use_help = true;
                            std::cout << HelpDescription();         
                            return true;
                        }
                    }

                    bool find_flag = false;
                    for (int j=0; j<flag_arguments.size(); ++j){
                        if (flag_arguments[j]->name == current_argument){

                            flag_arguments[j]->key = true;
                            if (flag_arguments[j]->store_value != nullptr){
                                *flag_arguments[j]->store_value = flag_arguments[j]->key;
                            }
                            find_flag = true;
                            break;
                        }
                    }

                    if (find_flag){
                        continue;
                    }
                    else{
                        past_arg = current_argument;
                        past_arg_free = true;
                        continue;          
                    }
                }

                else{
                    current_argument = input_data[i].substr(2, index - 2);
                    if (index == input_data[i].length()-1) return false;
                    value = input_data[i].substr(index+1);
                }

                for (int j = 0; j < string_arguments.size(); ++j){
                    if (string_arguments[j]->name == current_argument){
                        if (string_arguments[j]->multi_flag == false){
                            string_arguments[j]->key = value;
                            if (string_arguments[j]->store_value != nullptr){
                                *string_arguments[j]->store_value = string_arguments[j]->key;
                            }
                        }
                        else{
                            string_arguments[j]->keys.push_back(value);
                        }
                        break;
                    }
                }

                for (int j = 0; j < int_arguments.size(); ++j){
                    if (int_arguments[j]->name == current_argument){
                        if (int_arguments[j]->multi_flag == false){
                            int_arguments[j]->key = std::stoi(value);
                            if (int_arguments[j]->store_value != nullptr){
                                *int_arguments[j]->store_value = int_arguments[j]->key;
                            }
                        }
                        else{
                            int_arguments[j]->keys.push_back(std::stoi(value));
                            if (int_arguments[j]->store_values != nullptr){
                                *int_arguments[j]->store_values = int_arguments[j]->keys;
                            }
                        }
                        break;
                    }
                }

            }

            else{
                char current_argument = input_data[i][1];

                if (have_help == true){
                    if (current_argument == short_name_help){
                        use_help = true;
                        std::cout << HelpDescription();          
                        return true;
                    }
                }

                bool find_flag = false;
                for (int j=0; j<flag_arguments.size(); ++j){
                    if (flag_arguments[j]->short_name == current_argument){

                        flag_arguments[j]->key = true;
                        if (flag_arguments[j]->store_value != nullptr){
                            *flag_arguments[j]->store_value = flag_arguments[j]->key;
                        }
                        find_flag = true;
                        break;
                    }    
                }    

                if (find_flag == true){
                    for (int l = 2; l < input_data[i].length(); ++l) {
                        bool new_find_flag = false;
                        for (int j = 0; j < flag_arguments.size(); ++j) {
                            if (flag_arguments[j]->short_name == input_data[i][l]) {

                                flag_arguments[j]->key = true;
                                if (flag_arguments[j]->store_value != nullptr){
                                    *flag_arguments[j]->store_value = flag_arguments[j]->key;
                                }
                                new_find_flag = true;
                                break;
                            }
                        }
                        if (!new_find_flag){
                            return false;
                        }
                    }
                    continue;
                }

                if (input_data[i].find('=') != std::string::npos){

                    value = input_data[i].substr(3);

                    for (int j = 0; j < string_arguments.size(); ++j){
                        if (string_arguments[j]->short_name == current_argument){
                            if (string_arguments[j]->multi_flag == false){
                                string_arguments[j]->key = value;
                                if (string_arguments[j]->store_value != nullptr){
                                    *string_arguments[j]->store_value = string_arguments[j]->key;
                                }
                            }
                            else{
                                string_arguments[j]->keys.push_back(value);
                            }
                            break;
                        }
                    }

                    for (int j = 0; j < int_arguments.size(); ++j){
                        if (int_arguments[j]->short_name == current_argument){
                            if (int_arguments[j]->multi_flag == false){
                                int_arguments[j]->key = std::stoi(value);
                                if (int_arguments[j]->store_value != nullptr){
                                    *int_arguments[j]->store_value = int_arguments[j]->key;
                                }
                            }
                            else{
                                int_arguments[j]->keys.push_back(std::stoi(value));
                                if (int_arguments[j]->store_values != nullptr){
                                    *int_arguments[j]->store_values = int_arguments[j]->keys;
                                }
                            }
                            break;
                        }
                    }
                }

                else{
                    short_past_arg = current_argument;
                    past_arg_free = true;
                    continue;
                }
            }
        }
        else{

            bool add = false;

            if (past_arg_free == true){

                if (short_past_arg == ' '){
                    
                    for (int j = 0; j < int_arguments.size(); ++j){
                        if (int_arguments[j]->name == past_arg){
                            if (int_arguments[j]->multi_flag == false){
                                int_arguments[j]->key = std::stoi(input_data[i]);
                                if (int_arguments[j]->store_value != nullptr){
                                    *int_arguments[j]->store_value = int_arguments[j]->key;
                                }
                            }
                            else{
                                int_arguments[j]->keys.push_back(std::stoi(input_data[i]));
                                if (int_arguments[j]->store_values != nullptr){
                                    *int_arguments[j]->store_values = int_arguments[j]->keys;
                                }
                            }
                            add = true;
                            break;
                        }
                    }

                    for (int j = 0; j < string_arguments.size(); ++j){
                        if (string_arguments[j]->name == past_arg){
                            if (string_arguments[j]->multi_flag == false){
                                string_arguments[j]->key = input_data[i];
                                if (string_arguments[j]->store_value != nullptr){
                                    *string_arguments[j]->store_value = string_arguments[j]->key;
                                }
                            }
                            else{
                                string_arguments[j]->keys.push_back(input_data[i]);
                            }
                            add = true;
                            break;
                        }
                    }  
                }

                else{

                    for (int j = 0; j < int_arguments.size(); ++j){
                        if (int_arguments[j]->short_name == short_past_arg){
                            if (int_arguments[j]->multi_flag == false){
                                int_arguments[j]->key = std::stoi(input_data[i]);
                                if (int_arguments[j]->store_value != nullptr){
                                    *int_arguments[j]->store_value = int_arguments[j]->key;
                                }
                            }
                            else{
                                int_arguments[j]->keys.push_back(std::stoi(input_data[i]));
                                if (int_arguments[j]->store_values != nullptr){
                                    *int_arguments[j]->store_values = int_arguments[j]->keys;
                                }
                            }
                            add = true;
                            break;
                        }
                    }

                    for (int j = 0; j < string_arguments.size(); ++j){
                        if (string_arguments[j]->short_name == short_past_arg){
                            if (string_arguments[j]->multi_flag == false){
                                string_arguments[j]->key = input_data[i];
                                if (string_arguments[j]->store_value != nullptr){
                                    *string_arguments[j]->store_value = string_arguments[j]->key;
                                }
                            }
                            else{
                                string_arguments[j]->keys.push_back(input_data[i]);
                            }
                            add = true;
                            break;
                        }
                    }
                }

                short_past_arg = ' ';
                past_arg.clear();
                past_arg_free = false;

                if (add == true){
                    continue;
                }
                else{
                    return false;
                }
            }

            else{

                for (int j = 0; j < int_arguments.size(); ++j){
                    if (int_arguments[j]->positional == true){
                        if (int_arguments[j]->multi_flag == false){
                            int_arguments[j]->key = std::stoi(input_data[i]);
                            if (int_arguments[j]->store_value != nullptr){
                                *int_arguments[j]->store_value = int_arguments[j]->key;
                            }
                        }
                            
                        else{
                            int_arguments[j]->keys.push_back(std::stoi(input_data[i]));
                            if (int_arguments[j]->store_values != nullptr){
                                *int_arguments[j]->store_values = int_arguments[j]->keys;
                            }
                        }
                        add = true;
                        break;
                        
                    }
                }

                if (add == true){
                    continue;
                }
                else{
                    return false;
                }
            }
        }
    }

    for(int i=0; i<string_arguments.size(); ++i){
        if (string_arguments[i]->multi_flag == true){
            if (string_arguments[i]->keys.size() < string_arguments[i]->min_count_arguments){
                return false;
            }
        }
    }

    for(int i=0; i<int_arguments.size(); ++i){
        if (int_arguments[i]->multi_flag == true){
            if (int_arguments[i]->keys.size() < int_arguments[i]->min_count_arguments){
                return false;
            }
        }
    }

    return true;
}


bool ArgParser::Parse(int argc, char** argv){
    std::vector<std::string> input_data;
    for (int i = 0; i < argc; ++i) {
        input_data.push_back(argv[i]);
    }
    return Parse(input_data);
}


ArgParser::IntArgument &ArgParser::AddIntArgument(const std::string &full_name, const std::string &description){
    IntArgument* argument = new IntArgument(full_name);
    argument->help_information = description;
    int_arguments.push_back(argument);
    return *argument;
}


ArgParser::IntArgument &ArgParser::AddIntArgument(char letter_name, const std::string &full_name, const std::string &description){
    IntArgument* argument = new IntArgument(letter_name, full_name);
    argument->help_information = description;
    int_arguments.push_back(argument);
    return *argument;
}


int ArgParser::GetIntValue(const std::string &param, size_t index){
    for (int i=0; i<int_arguments.size(); ++i){
        if (int_arguments[i]->name == param){
            if (int_arguments[i]->multi_flag == false){
                return int_arguments[i]->key;
            }
            else{
                return int_arguments[i]->keys[index];
            }
        }
    }
    return 0;
}


ArgParser::StringArgument &ArgParser::AddStringArgument(const std::string &full_name, const std::string &description){
    StringArgument* argument = new StringArgument(full_name);
    argument->help_information = description;
    string_arguments.push_back(argument);
    return *argument;
}


ArgParser::StringArgument &ArgParser::AddStringArgument(char letter_name, const std::string &full_name, const std::string &description){
    StringArgument* argument = new StringArgument(letter_name, full_name);
    argument->help_information = description;
    string_arguments.push_back(argument);
    return *argument;
}


std::string ArgParser::GetStringValue(const std::string &param, size_t index){
    for (int i=0; i<string_arguments.size(); ++i){
        if (string_arguments[i]->name == param){
            if (string_arguments[i]->multi_flag == false){
                return string_arguments[i]->key;
            }
            else{
                return string_arguments[i]->keys[index];
            }
        }
    }
    return {};
}


ArgParser::FlagArgument &ArgParser::AddFlag(const std::string &full_name, const std::string &description){
    FlagArgument* argument = new FlagArgument(full_name);
    argument->help_information = description;
    flag_arguments.push_back(argument);
    return *argument;
}


ArgParser::FlagArgument &ArgParser::AddFlag(char letter_name, const std::string &full_name, const std::string &description){
    FlagArgument* argument = new FlagArgument(letter_name, full_name);
    argument->help_information = description;
    flag_arguments.push_back(argument);
    return *argument;
}


bool ArgParser::GetFlag(const std::string &param){
    for (int i=0; i<flag_arguments.size(); ++i){
        if (flag_arguments[i]->name == param){
            return flag_arguments[i]->key;
        }
    }
    return false;
}


void ArgParser::AddHelp(char letter_name, const std::string &full_name, const std::string &description){   
    have_help = true;
    short_name_help = letter_name;
    name_help = full_name;
    description_help = description;
    return;
}


const std::string ArgParser::HelpDescription(){     // надо расширить в будущем (описание для int аргументов)
    std::string help_output = parser_name + '\n' + description_help + '\n' + '\n';

    
    for (int i = 0; i < string_arguments.size(); ++i){
        std::string help_string_arguments;
        if (string_arguments[i]->short_name != ' '){
            help_string_arguments = help_string_arguments + '-' + string_arguments[i]->short_name + ',';
        }
        else{
            help_string_arguments = help_string_arguments + "   ";
        }
        help_string_arguments = help_string_arguments + "  --" + string_arguments[i]->name + "=<string>";

        if (string_arguments[i]->help_information != ""){
            help_string_arguments = help_string_arguments + ",  " + string_arguments[i]->help_information;
        }

        if (string_arguments[i]->default_flag == true){
            help_string_arguments = help_string_arguments + " [default = " + string_arguments[i]->default_value;
        }

        if (string_arguments[i]->multi_flag == true){
            if (help_string_arguments.find("[") == std::string::npos){
                help_string_arguments = help_string_arguments + " [repeated";
            }
            else{
                help_string_arguments = help_string_arguments + ", repeated";
            }


            help_string_arguments = help_string_arguments + ", min args = " + std::to_string(string_arguments[i]->min_count_arguments);
        }

        if (string_arguments[i]->store_value != nullptr){
            if (help_string_arguments.find("[") == std::string::npos){
                help_string_arguments = help_string_arguments + " [storevalue";
            }
            else{
                help_string_arguments = help_string_arguments + ", storevalue";
            }
        }

        if (help_string_arguments.find("[") != std::string::npos){
            help_string_arguments = help_string_arguments + ']';
        }

        help_output = help_output + help_string_arguments + '\n';
    }

    for (int i = 0; i < flag_arguments.size(); ++i){
        if (flag_arguments[i]->short_name != ' '){
            help_output = help_output + '-' + flag_arguments[i]->short_name + ',';
        }
        else{
            help_output = help_output + "   ";
        }
        help_output = help_output + "  --" + flag_arguments[i]->name;

        if (flag_arguments[i]->help_information != ""){
            help_output = help_output + ",  " + flag_arguments[i]->help_information;
        }

        if (flag_arguments[i]->default_flag == true){
            help_output = help_output + " [default = true]";
        }
        help_output = help_output + '\n';
    }

    for (int i = 0; i < int_arguments.size(); ++i){
        std::string help_int_arguments;
        if (int_arguments[i]->short_name != ' '){
            help_int_arguments = help_int_arguments + '-' + int_arguments[i]->short_name + ',';
        }
        else{
            help_int_arguments = help_int_arguments + "   ";
        }
        help_int_arguments = help_int_arguments + "  --" + int_arguments[i]->name + "=<int>";

        if (int_arguments[i]->help_information != ""){
            help_int_arguments = help_int_arguments + ",  " + int_arguments[i]->help_information;
        }

        if (int_arguments[i]->default_flag == true){
            help_int_arguments = help_int_arguments + " [default = " + std::to_string(int_arguments[i]->default_value);
        }

        if (int_arguments[i]->multi_flag == true){
            if (help_int_arguments.find("[") == std::string::npos){
                help_int_arguments = help_int_arguments + " [repeated";
            }
            else{
                help_int_arguments = help_int_arguments + ", repeated";
            }

            help_int_arguments = help_int_arguments + ", min args = " + std::to_string(int_arguments[i]->min_count_arguments);
        }

        if (int_arguments[i]->store_value != nullptr){
            if (help_int_arguments.find("[") == std::string::npos){
                help_int_arguments = help_int_arguments + " [storevalue";
            }
            else{
                help_int_arguments = help_int_arguments + ", storevalue";
            }
        }

        if (int_arguments[i]->store_values != nullptr){
            if (help_int_arguments.find("[") == std::string::npos){
                help_int_arguments = help_int_arguments + " [storevalues";
            }
            else{
                help_int_arguments = help_int_arguments + ", storevalues";
            }
        }

        if (int_arguments[i]->positional == true){
            if (help_int_arguments.find("[") == std::string::npos){
                help_int_arguments = help_int_arguments + " [positional";
            }
            else{
                help_int_arguments = help_int_arguments + ", positional";
            }
        }

        if (help_int_arguments.find("[") != std::string::npos){
            help_int_arguments = help_int_arguments + ']';
        }

        help_output = help_output + help_int_arguments + '\n';
    }

    help_output = help_output + '\n' + '-' + short_name_help + ", --" + name_help + " Display this help and exit" + '\n';
    return help_output;
}


bool ArgParser::Help(){
    return use_help;
}
