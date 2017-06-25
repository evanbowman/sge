#include <iostream>
#include <regex>

#include "Singleton.hpp"
#include "Engine.hpp"
#include "ResourcePath.hpp"

int main(int argc, char** argv) {
    switch (argc) {
    case 1:
        Singleton<Engine>::Instance().Run(Engine::RunMode::Normal);
        break;

    case 2:
        if (std::string(argv[1]) == "-i") {
            Singleton<Engine>::Instance().Run(Engine::RunMode::Interactive);
        } else {
            std::cout << "Invalid option " << argv[1] << std::endl;
            return EXIT_FAILURE;
        }
        break;

    default:
        return EXIT_FAILURE;
        break;
    }
    return EXIT_SUCCESS;    
}


