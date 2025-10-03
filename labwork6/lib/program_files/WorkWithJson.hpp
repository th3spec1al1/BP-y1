#ifndef WWF_HPP
#define WWF_HPP

#include "Data_Structures.hpp"

namespace WorkWithJson{
    
    void Separator();

    std::string CreateJsonFilename(const ConsoleArguments& Args);

    void Output(const nlohmann::json& data, const ConsoleArguments& Args);

    bool IsCacheExist(const ConsoleArguments& Args);

    nlohmann::json LoadCache(const ConsoleArguments& Args);

    void AddCache(nlohmann::json& data, const ConsoleArguments& Args);

    bool IsTopical(const ConsoleArguments& Args);

}

#endif