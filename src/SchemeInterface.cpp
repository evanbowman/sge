#include <libguile.h>
#include <thread>

#include "SchemeInterface.hpp"
#include "Engine.hpp"
#include "Singleton.hpp"
#include "ResourcePath.hpp"

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
    
SCM_DEFINE (EntityCreate, "entity-create", 0, 0, 0,
            (), "Create an entity.") {
    return scm_from_ssize_t(engine.CreateEntity());
}

SCM_DEFINE (EntityRemove, "entity-remove", 1, 0, 0,
            (SCM id), "Remove an entity.") {
    engine.RemoveEntity(scm_to_ssize_t(id));
    return SCM_EOL;
}

SCM_DEFINE (EntitySetPosition, "entity-set-position", 3, 0, 0,
            (SCM id, SCM x, SCM y), "Set an entity\'s position.") {
    auto entity = engine.GetEntityRaw(scm_to_ssize_t(id));
    if (entity) {
        const Vec2 pos {
            static_cast<float>(scm_to_double(x)),
            static_cast<float>(scm_to_double(y))
        };
        entity->SetPosition(pos);
    } else {
        ThrowMissingEntityError("entity-set-position");
    }
    return id;
}

SCM_DEFINE (EntityGetPosition, "entity-get-position", 1, 0, 0,
            (SCM id), "Get an entity\'s position.") {
    auto entity = engine.GetEntityRaw(scm_to_ssize_t(id));
    if (entity) {
        return scm_cons(scm_from_double(entity->GetPosition().x),
                        scm_from_double(entity->GetPosition().y));
    }
    ThrowMissingEntityError("entity-get-position");
    return SCM_EOL;
}

SCM_DEFINE (EntitySetAnimation, "entity-set-animation", 2, 0, 0,
            (SCM entityId, SCM animId), "Set a visual representation for an entity.") {
    auto entity = engine.GetEntityRaw(scm_to_ssize_t(entityId));
    if (!entity) {
        ThrowMissingEntityError("entity-set-animation");
    }
    auto animation = engine.GetAnimationRaw(scm_to_ssize_t(animId));
    if (!animation) {
        scm_error(scm_misc_error_key, "entity-set-animation",
                  "Animation id lookup failed", SCM_EOL, SCM_EOL);
    }
    entity->SetAnimation(animation);
    return entityId;
}

SCM_DEFINE (EntitySetKeyframe, "entity-set-keyframe", 2, 0, 0,
            (SCM id, SCM keyframe), "Set the keyframe for an entity\'s animation.") {
    auto entity = engine.GetEntityRaw(scm_to_ssize_t(id));
    if (!entity) {
        ThrowMissingEntityError("entity-set-keyframe");
    }
    entity->SetKeyframe(scm_to_ssize_t(keyframe));
    return id;
}

SCM_DEFINE (EntityGetKeyframe, "entity-get-keyframe", 1, 0, 0,
            (SCM id), "Get the keyframe for an entity\'s animation.") {
    auto entity = engine.GetEntityRaw(scm_to_ssize_t(id));
    if (!entity) {
        ThrowMissingEntityError("entity-get-keyframe");
    }
    return scm_from_ssize_t(entity->GetKeyframe());
}
    
SCM_DEFINE (IsRunning, "eng-is-running?", 0, 0, 0,
            (), "Check whether the engine is running.") {
    return scm_from_bool(engine.IsRunning());
}

SCM_DEFINE (TimerCreate, "timer-create", 0, 0, 0,
            (), "Create a timer.") {
    return scm_from_ssize_t(engine.CreateTimer());
}

SCM_DEFINE (TimerReset, "timer-reset", 1, 0, 0,
            (SCM id), "Create a timer.") {
    return scm_from_ssize_t(engine.ResetTimer(scm_to_ssize_t(id)));
}

SCM_DEFINE (TimerRemove, "timer-remove", 1, 0, 0,
            (SCM id), "Remove a timer.") {
    engine.RemoveTimer(scm_to_ssize_t(id));
    return SCM_EOL;
}

SCM_DEFINE (RNGCreate, "rng-create", 0, 0, 0,
            (), "Create an rng.") {
    return scm_from_ssize_t(engine.CreateRNG());
}

SCM_DEFINE (RNGGet, "rng-next", 1, 0, 0,
            (SCM id), "Get the next state of the RNG.") {
    return scm_from_ssize_t(engine.GetRandom(scm_to_ssize_t(id)));
}

SCM_DEFINE (MicroSleep, "micro-sleep", 1, 0, 0,
            (SCM usecs), "Seep for a number of microseconds. Integers please.") {
    std::this_thread::sleep_for(std::chrono::microseconds(scm_to_uint(usecs)));
    return SCM_EOL;
}
    
SCM_DEFINE (AnimationCreate, "animation-create", 7, 0, 0,
            (SCM fname, SCM x, SCM y, SCM w, SCM h, SCM ox, SCM oy),
            "Create an animation, "
            "(fname x y keyframe-width keyframe-height)") {
    return scm_from_ssize_t(
                engine.CreateAnimation(scm_to_latin1_string(fname), {
                        scm_to_int(x), scm_to_int(y),
                        scm_to_int(w), scm_to_int(h)
                    },
                    {
                        static_cast<float>(scm_to_double(ox)),
                        static_cast<float>(scm_to_double(oy))
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
