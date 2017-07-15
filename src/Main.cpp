#include <iostream>

#include "cxxopts/cxxopts.hpp"

#include "Engine.hpp"
#include "Singleton.hpp"
#include "ResourcePath.hpp"

int main(int argc, char** argv) {
    cxxopts::Options options("sge", "Simple 2d Game Engine");
    options.add_options()
        ("i,interactive", "Run in interactive mode")    
        ("p,package", "Run on a specific directory",
         cxxopts::value<std::string>())
        ("h,help", "display this message");
    try {
        options.parse(argc, argv);
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    auto runMode = [&options] {
                       if (options.count("i")) {
                           return Engine::RunMode::Interactive;
                       } else {
                           return Engine::RunMode::Normal;
                       }
                   }();
    if (options.count("p")) {
        ConfigureResourcePath(options["p"].as<std::string>());
    }
    Singleton<Engine>::Instance().Run(runMode);
    return EXIT_SUCCESS;
}


