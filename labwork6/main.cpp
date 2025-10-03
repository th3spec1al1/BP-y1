#include "lib/MyLib.hpp"


int main(int argc, char** argv){         // .\main --date yyyy-mm-dd --rout back/there
    ConsoleArguments Args; 
    DataValues Val;
    Parser::Parse(argc, argv, Args);

    nlohmann::json data;

    if (WorkWithJson::IsCacheExist(Args) && WorkWithJson::IsTopical(Args)){
        data = WorkWithJson::LoadCache(Args);

        std::cout << "LOAD" << std::endl;
    }

    else{
        std::string request = MyRequest::CreateRequest(Args, Val);
        cpr::Response response = cpr::Get(cpr::Url{request});

        data = nlohmann::json::parse(response.text);

        WorkWithJson::AddCache(data, Args);

        std::cout << "ADD" << std::endl;
    }

    WorkWithJson::Output(data, Args);
}