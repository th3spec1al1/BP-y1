#include "WorkWithJson.hpp"

namespace WorkWithJson{

    void Separator(){
        std::cout << "----------------------------------------------------------------------" << std::endl;
    }

    std::string CreateJsonFilename(const ConsoleArguments& Args){
        std::string filename = std::filesystem::current_path();
        
        filename += "/../lib/cache/";
        filename += "date=";
        filename += Args.departure_date;
        filename += "_route=";
        filename += Args.start;
        filename += "-";
        filename += Args.end;
        filename += ".json";

        return filename;
    }

    void Output(const nlohmann::json& data, const ConsoleArguments& Args){
        if (data.empty()){
            std::cerr << "Error: Invalid request (check your arguments and/or run the program again)" << std::endl;
        }

        int number_of_paths = 0;

        Separator();
        for (const auto& segment : data["segments"]){
            if (segment["has_transfers"] == false){
                std::cout << "From: " << (Args.start == "c2" ? "Санкт-Петербург" : "Псков") << std::endl;
                std::cout << "To: " << (Args.end == "c2" ? "Санкт-Петербург" : "Псков") << std::endl;
                std::cout << "Departure time: " << std::string(segment["departure"]) << std::endl;
                std::cout << "Arrival time: " << std::string(segment["arrival"]) << std::endl;
                std::cout << "Transport type: " << std::string(segment["from"]["transport_type"]) << std::endl;
                Separator();

                ++number_of_paths;
            }

            else if (segment["transfers"].size() > 1){
                continue;
            }

            else{
                std::cout << "From: " << (Args.start == "c2" ? "Санкт-Петербург" : "Псков") << std::endl;
                std::cout << "Transfer: " << std::string(segment["transfers"][0]["title"]) << std::endl;
                std::cout << "To: " << (Args.end == "c2" ? "Санкт-Петербург" : "Псков") << std::endl;
                std::cout << "Departure time: " << std::string(segment["details"][0]["departure"]) << std::endl;
                std::cout << "Arrival time at the transfer point: " << std::string(segment["details"][0]["arrival"]) << std::endl;
                std::cout << "Transport type: " << std::string(segment["details"][0]["from"]["type"]) << std::endl;
                std::cout << "Departure time from the transfer point: " << std::string(segment["details"][2]["departure"]) << std::endl;
                std::cout << "Arrival time: " << std::string(segment["details"][2]["arrival"]) << std::endl;
                std::cout << "Transport type: " << std::string(segment["details"][2]["from"]["type"]) << std::endl;
                Separator();

                ++number_of_paths;
            }
        }

        std::cout << "Number of paths: " << number_of_paths << std::endl;
    }

    bool IsCacheExist(const ConsoleArguments& Args){
        std::string filename = CreateJsonFilename(Args);

        if (std::filesystem::exists(filename)){
            return true;
        }

        return false;
    }

    nlohmann::json LoadCache(const ConsoleArguments& Args){
        std::string filename = CreateJsonFilename(Args);

        std::ifstream file(filename);
        nlohmann::json j;

        if (file.is_open()){
            file >> j;
            file.close();
            return j;
        }

        else{
            std::cerr << "Error: Cache load" << std::endl;
            exit(1);
        }
    }

    void AddCache(nlohmann::json& data, const ConsoleArguments& Args){
        std::string filename = CreateJsonFilename(Args);

        std::ofstream file(filename);

        if (file.is_open()){
            uint64_t time = uint64_t(std::time(nullptr));

            data["create_time"] = time;
            file << data.dump(4);
            file.close();
            return;
        }

        else{
            std::cerr << "Error: Cache add" << std::endl;
            exit(1);
        }
    }

    bool IsTopical(const ConsoleArguments& Args){
        uint64_t time = uint64_t(std::time(nullptr));

        std::string filename = CreateJsonFilename(Args);

        std::ifstream file(filename);
        nlohmann::json j;

        if (file.is_open()){
            file >> j;
            file.close();

            uint64_t create_time = j["create_time"];
            if (time - create_time < 3*60*60){
                return true;
            }

            return false;
        }

        else{
            std::cerr << "Error: Cache load" << std::endl;
            exit(1); 
        }
    }

}