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
                       scm_shell(argc, argv);
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

#include <iostream>

namespace {
auto& engine = Singleton<Engine>::Instance();
    
SCM EntityCreate() {    
    return scm_from_ssize_t(engine.CreateEntity());
}

SCM EntityRemove(SCM params) {
    engine.RemoveEntity(scm_to_ssize_t(params));
    return SCM_EOL;
}

SCM EntitySetPosition(SCM id, SCM x, SCM y) {
    auto entity = engine.GetEntityRaw(scm_to_ssize_t(id));
    if (entity) {
        const Vec2 pos {
            static_cast<float>(scm_to_double(x)),
            static_cast<float>(scm_to_double(y))
        };
        entity->SetPosition(pos);
    }
    return SCM_EOL;
}
    
SCM IsRunning() {
    return scm_from_bool(engine.IsRunning());
}

SCM TimerCreate() {
    return scm_from_ssize_t(engine.CreateTimer());
}

SCM TimerReset(SCM params) {
    return scm_from_ssize_t(engine.ResetTimer(scm_to_ssize_t(params)));
}

SCM LoadAnimation(SCM fname, SCM x, SCM y, SCM w, SCM h) {
    std::cout << scm_to_ssize_t(x) << " "
              << scm_to_ssize_t(y) << " "
              << scm_to_ssize_t(w) << " "
              << scm_to_ssize_t(h) << std::endl;
}
    
void DoEngineAPIWrap() {
    scm_c_define_gsubr("ENGINE-entity-create", 0, 0, 0, (void*)EntityCreate);
    scm_c_define_gsubr("ENGINE-entity-remove", 0, 0, 0, (void*)EntityRemove);
    scm_c_define_gsubr("ENGINE-entity-set-position", 3, 0, 0, (void*)EntitySetPosition);
    scm_c_define_gsubr("ENGINE-is-running", 0, 0, 0, (void*)IsRunning);
    scm_c_define_gsubr("ENGINE-timer-reset", 1, 0, 0, (void*)TimerReset);
    scm_c_define_gsubr("ENGINE-timer-create", 0, 0, 0, (void*)TimerCreate);
    scm_c_define_gsubr("ENGINE-load-animation", 5, 0, 0, (void*)LoadAnimation);
}
}
