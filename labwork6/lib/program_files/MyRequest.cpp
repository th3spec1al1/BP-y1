#include "MyRequest.hpp"

namespace MyRequest{

    std::string CreateRequest(const ConsoleArguments& Args, const DataValues& Val){
        std::string result;

        result += Val.source;
        result += "?apikey=";
        result += Val.key;
        result += "&format=json&from=";

        result += Args.start;
        result += "&to=";
        result += Args.end;


        result += "&lang=ru_RU&page=1&date=";
        result += Args.departure_date;

        result += "&transfers=true";

        return result;
    }

}