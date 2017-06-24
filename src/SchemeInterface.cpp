#include "SchemeInterface.hpp"
#include "Engine.hpp"
#include "Singleton.hpp"
#include <libguile.h>

namespace {
void DoEngineAPIWrap();
static const auto mainScript = "main.scm";
}

void scheme::REPLEntry() {
    std::pair<int, char**> dummyArgs{};
    scm_boot_guile(dummyArgs.first, dummyArgs.second,
                   [](void* closure, int argc, char** argv) {
                       DoEngineAPIWrap();
                       scm_c_primitive_load(mainScript);
                   }, 0);
}

void scheme::ScriptEntry() {
    std::pair<int, char**> dummyArgs{};
    scm_boot_guile(dummyArgs.first, dummyArgs.second,
                   [](void* closure, int argc, char** argv) {
                       DoEngineAPIWrap();
                       scm_c_primitive_load(mainScript);
                       SCM func = scm_variable_ref(scm_c_lookup("main"));
                       scm_call_0(func);
                   }, 0);
}

namespace {
SCM CreateEntity(SCM param) {
    auto& engine = Singleton<Engine>::Instance();
    return scm_from_int(engine.CreateEntity());
}

SCM IsRunning(SCM param) {
    auto& engine = Singleton<Engine>::Instance();
    return scm_from_bool(engine.IsRunning());
}

SCM CreateTimer(SCM param) {
    auto& engine = Singleton<Engine>::Instance();
    return scm_from_int(engine.CreateTimer());
}

SCM ResetTimer(SCM param) {
    auto& engine = Singleton<Engine>::Instance();
    return scm_from_int(engine.ResetTimer(scm_to_int(param)));
}
    
void DoEngineAPIWrap() {
    scm_c_define_gsubr("ENGINE-create-entity", 0, 0, 0, (void*)CreateEntity);
    scm_c_define_gsubr("ENGINE-is-running", 0, 0, 0, (void*)IsRunning);
    scm_c_define_gsubr("ENGINE-reset-timer", 1, 0, 0, (void*)ResetTimer);
    scm_c_define_gsubr("ENGINE-create-timer", 0, 0, 0, (void*)CreateTimer);
}
}
