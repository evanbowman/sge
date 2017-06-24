#include "Singleton.hpp"
#include "Engine.hpp"

int main() {
    Singleton<Engine>::Instance().Run(Engine::RunMode::Normal);
}
