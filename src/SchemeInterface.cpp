#include "SchemeInterface.hpp"
#include "Engine.hpp"
#include "Singleton.hpp"
#include <libguile.h>
#include "ResourcePath.hpp"
#include "Exceptions.hpp"

namespace {
void DoEngineAPIWrap();
static const auto entryScriptPath = ResourcePath() + "scripts/main.scm";
}

void scheme::REPLEntry() {
    std::pair<int, char**> dummyArgs{};
    scm_boot_guile(dummyArgs.first, dummyArgs.second,
                   [](void* closure, int argc, char** argv) {
                       DoEngineAPIWrap();
                       scm_c_primitive_load(entryScriptPath.c_str());
                       scm_shell(argc, argv);
                   }, 0);
}

void scheme::ScriptEntry() {
    std::pair<int, char**> dummyArgs{};
    scm_boot_guile(dummyArgs.first, dummyArgs.second,
                   [](void* closure, int argc, char** argv) {
                       DoEngineAPIWrap();
                       scm_c_primitive_load(entryScriptPath.c_str());
                       SCM func = scm_variable_ref(scm_c_lookup("main"));
                       scm_call_0(func);
                   }, 0);
}

namespace {
auto& engine = Singleton<Engine>::Instance();

static void ThrowMissingEntityError(const char* context) {
    scm_error(scm_misc_error_key, context,
              "Entity id lookup failed", SCM_EOL, SCM_EOL);   
}
    
SCM_DEFINE (EntityCreate, "eng-entity-create", 0, 0, 0,
            (), "Create an entity.") {
    return scm_from_ssize_t(engine.CreateEntity());
}

SCM_DEFINE (EntityRemove, "eng-entity-remove", 1, 0, 0,
            (SCM id), "Remove an entity.") {
    engine.RemoveEntity(scm_to_ssize_t(id));
    return SCM_EOL;
}

SCM_DEFINE (EntitySetPosition, "eng-entity-set-position", 3, 0, 0,
            (SCM id, SCM x, SCM y), "Set an entity\'s position.") {
    auto entity = engine.GetEntityRaw(scm_to_ssize_t(id));
    if (entity) {
        const Vec2 pos {
            static_cast<float>(scm_to_double(x)),
            static_cast<float>(scm_to_double(y))
        };
        entity->SetPosition(pos);
    } else {
        ThrowMissingEntityError("eng-entity-set-position");
    }
    return SCM_EOL;
}

SCM_DEFINE (EntityGetX, "eng-entity-get-x-position", 1, 0, 0,
            (SCM id), "Get an entity\'s x position.") {
    auto entity = engine.GetEntityRaw(scm_to_ssize_t(id));
    if (entity) {
        return scm_from_double(entity->GetPosition().x);
    }
    ThrowMissingEntityError("eng-entity-get-x-position");
    return SCM_EOL;
}

SCM_DEFINE (EntityGetY, "eng-entity-get-y-position", 1, 0, 0,
            (SCM id), "Get an entity\'s y position.") {
    auto entity = engine.GetEntityRaw(scm_to_ssize_t(id));
    if (entity) {
        return scm_from_double(entity->GetPosition().y);
    }
    ThrowMissingEntityError("eng-entity-get-y-position");
    return SCM_EOL;
}

SCM_DEFINE (IsRunning, "eng-is-running?", 0, 0, 0,
            (), "Check whether the engine is running.") {
    return scm_from_bool(engine.IsRunning());
}

SCM_DEFINE (TimerCreate, "eng-timer-create", 0, 0, 0,
            (), "Create a timer.") {
    return scm_from_ssize_t(engine.CreateTimer());
}

SCM_DEFINE (TimerReset, "eng-timer-reset", 1, 0, 0,
            (SCM id), "Create a timer.") {
    return scm_from_ssize_t(engine.ResetTimer(scm_to_ssize_t(id)));
}

SCM_DEFINE (TimerRemove, "eng-timer-remove", 1, 0, 0,
            (SCM id), "Remove a timer.") {
    engine.RemoveTimer(scm_to_ssize_t(id));
    return SCM_EOL;
}
    
SCM_DEFINE (CreateAnimation, "eng-create-animation", 5, 0, 0,
            (SCM fname, SCM x, SCM y, SCM w, SCM h),
            "Create an animation, "
            "(fname x y keyframe-width keyframe-height)") {
    return scm_from_ssize_t(
                engine.CreateAnimation(scm_to_latin1_string(fname), {
                        scm_to_int(x), scm_to_int(y),
                        scm_to_int(w), scm_to_int(h)
                    }));
}

void DoEngineAPIWrap() {
    /* guile comes with a handy utility called guile-snarf
     * that makes defining API functions easier (see SCM_DEFINES 
     * above). When adding a new function, remember to run the 
     * snarf script in the src dir. */
#include "snarf.x"
}
}
