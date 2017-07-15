#include <libguile.h>
#include <thread>
#include <SFML/Window.hpp>

#include "SchemeInterface.hpp"
#include "Engine.hpp"
#include "Singleton.hpp"
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
inline UID UIDCast(SCM uidAsScm) {
    return scm_to_ssize_t(uidAsScm);
}
    
auto& engine = Singleton<Engine>::Instance();

static void SignalMissingEntityError(const char* context) {
    scm_error(scm_misc_error_key, context,
              "Entity id lookup failed", SCM_EOL, SCM_EOL);
}

static void SignalMissingGraphicsComponentError(const char* context) {
    scm_error(scm_misc_error_key, context,
              "Entity has no graphics component", SCM_EOL, SCM_EOL);
}

static void SignalComponentTypeMismatchError(const char* context,
                                            const char* expected) {
    scm_error(scm_misc_error_key, context,
              ("Component type does not match the expected " +
               std::string(expected)).c_str(), SCM_EOL, SCM_EOL);
}

SCM_DEFINE (EntityCreate, "entity-create", 0, 0, 0,
            (), "Create an entity.") {
    return scm_from_ssize_t(engine.CreateEntity());
}

SCM_DEFINE (EntityRemove, "entity-remove", 1, 0, 0,
            (SCM entity), "Remove an entity.") {
    engine.RemoveEntity(UIDCast(entity));
    return SCM_EOL;
}

SCM_DEFINE (EntitySetPosition, "entity-set-position", 3, 0, 0,
            (SCM entity, SCM x, SCM y), "Set an entity\'s position.") {
    try {
        engine.SetEntityPosition(UIDCast(entity), {
            static_cast<float>(scm_to_double(x)),
            static_cast<float>(scm_to_double(y))
        });
    } catch (BadHandle& bad) {
        SignalMissingEntityError("entity-set-position");
    }
    return entity;
}

SCM_DEFINE (EntityGetPosition, "entity-get-position", 1, 0, 0,
            (SCM entity), "Get an entity\'s position.") {
    try {
        const auto& pos = engine.GetEntityPosition(UIDCast(entity));
        return scm_cons(scm_from_double(pos.x), scm_from_double(pos.y));
    } catch (BadHandle& bad) {
        SignalMissingEntityError("entity-get-position");
    }
    return SCM_EOL;
}

SCM_DEFINE (EntitySetAnimation, "entity-set-animation", 2, 0, 0,
            (SCM entity, SCM animation),
            "Set a keyframe sequence representation for an entity.") {
    try {
        engine.SetEntityAnimation(UIDCast(entity),
                                  scm_to_ssize_t(animation));
    } catch (BadHandle& bad) {
        // TODO...
    }
    return entity;
}

SCM_DEFINE (EntitySetKeyframe, "entity-set-keyframe", 2, 0, 0,
            (SCM entity, SCM keyframe),
            "Set the keyframe for an entity\'s animation.") {
    try {
        engine.SetEntityKeyframe(UIDCast(entity),
                                 scm_to_ssize_t(keyframe));
    } catch (BadHandle& bad) {
        SignalMissingEntityError("entity-set-keyframe");
    }
    return entity;
}

SCM_DEFINE (EntitySetScale, "entity-set-scale", 3, 0, 0,
            (SCM entity, SCM xScale, SCM yScale),
            "Set the scale factor for an entity\'s graphics component") {
    try {
        engine.SetEntityScale(UIDCast(entity), {
                static_cast<float>(scm_to_double(xScale)),
                static_cast<float>(scm_to_double(yScale))
            });
    } catch (...) {
        // TODO...
    }
    return entity;
}

SCM_DEFINE (EntitySetZOrder, "entity-set-zorder", 2, 0, 0,
            (SCM entity, SCM zOrder),
            "Set an entity\'s zorder index.") {
    try {
        engine.SetEntityZOrder(UIDCast(entity), scm_to_int(zOrder));
    } catch (...) {
        // TODO...
    }
    return entity;
}
    
enum {
    BlendAdd,
    BlendNone,
    BlendAlpha,
    BlendMultiply
};
    
SCM_DEFINE (EntitySetBlendMode, "entity-set-blend-mode", 2, 0, 0,
            (SCM entity, SCM blendMode),
            "Set an entitiy\'s blend mode.") {
    try {
        static const std::unordered_map<unsigned, sf::BlendMode> modes {
            {BlendAdd, sf::BlendAdd},
            {BlendNone, sf::BlendNone},
            {BlendAlpha, sf::BlendAlpha},
            {BlendMultiply, sf::BlendMultiply}, 
        };
        auto modeIter = modes.find(scm_to_int(blendMode));
        auto mode = sf::BlendNone;
        if (modeIter != modes.end()) {
            mode = modeIter->second;
        }
        engine.SetEntityBlendMode(UIDCast(entity), mode);
    } catch (...) {
        // TODO...
    }
    return entity;
}

SCM_DEFINE (EntitySetColor, "entity-set-rgba", 5, 0, 0,
            (SCM entity, SCM r, SCM g, SCM b, SCM a),
            "Set an entity\'s color.") {
    try {
        engine.SetEntityColor(UIDCast(entity), {
                scm_to_uint8(r),
                scm_to_uint8(g),
                scm_to_uint8(b),
                scm_to_uint8(a)
            });
    } catch (...) {
        // ... 
    }
    return entity;
}
    
SCM_DEFINE (EntityGetKeyframe, "entity-get-keyframe", 1, 0, 0,
            (SCM entity), "Get the keyframe for an entity\'s animation.") {
    try {
        const auto keyframe = engine.GetEntityKeyframe(UIDCast(entity));
        return scm_from_ssize_t(keyframe);
    } catch (BadHandle& bad) {
        SignalMissingEntityError("entity-get-keyframe");
    }
    return SCM_EOL;
}
    
SCM_DEFINE (IsRunning, "eng-is-running?", 0, 0, 0,
            (), "Check whether the engine is running.") {
    return scm_from_bool(engine.IsRunning());
}

SCM_DEFINE (GetWindowSize, "window-size", 0, 0, 0,
            (), "Get the size of the game window in pixels.") {
    auto size = engine.GetWindowSize();
    return scm_cons(scm_from_uint(size.x),
                    scm_from_uint(size.y));
}
    
SCM_DEFINE (TimerCreate, "timer-create", 0, 0, 0,
            (), "Create a timer.") {
    return scm_from_ssize_t(engine.CreateTimer());
}

SCM_DEFINE (TimerReset, "timer-reset", 1, 0, 0,
            (SCM timer), "Create a timer.") {
    return scm_from_ssize_t(engine.ResetTimer(scm_to_ssize_t(timer)));
}

SCM_DEFINE (TimerRemove, "timer-remove", 1, 0, 0,
            (SCM timer), "Remove a timer.") {
    engine.RemoveTimer(scm_to_ssize_t(timer));
    return SCM_EOL;
}

SCM_DEFINE (MicroSleep, "micro-sleep", 1, 0, 0,
            (SCM usecs), "Sleep for a number of microseconds. Integers please.") {
    std::this_thread::sleep_for(std::chrono::microseconds(scm_to_uint(usecs)));
    return SCM_EOL;
}
    
SCM_DEFINE (KeyPressed, "key-pressed?", 1, 0, 0,
            (SCM keycode), "Test whether a key is pressed.") {
    return scm_from_bool(
        sf::Keyboard::isKeyPressed(
            static_cast<sf::Keyboard::Key>(scm_to_uint(keycode))));
}
    
SCM_DEFINE (AnimationCreate, "animation-create", 7, 0, 0,
            (SCM fname, SCM x, SCM y, SCM w, SCM h, SCM ox, SCM oy),
            "Create an animation, "
            "(fname x y keyframe-width keyframe-height).") {
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
   
SCM_DEFINE (CameraSetTarget, "camera-set-target", 1, 0, 0,
            (SCM entity),
            "Set camera target to an entity.") {
    try {
        engine.SetCameraTarget(UIDCast(entity));
    } catch (BadHandle& bad) {
        SignalMissingEntityError("entity-set-position");
    }
    return SCM_EOL;
}

SCM_DEFINE (CameraSetCenter, "camera-set-center", 2, 0, 0,
            (SCM x, SCM y),
            "Manually set the camera\'s center. This procedure has occasional"
            " uses, but better to set the camera\'s center by making it track an"
            " entity with camera-set-target") {
    engine.SetCameraCenter({ static_cast<float>(scm_to_double(x)),
                             static_cast<float>(scm_to_double(y)) });
    return SCM_EOL;
}
    
SCM_DEFINE (CameraSetSpringiness, "camera-set-springiness", 1, 0, 0,
            (SCM springiness),
            "Set the camera\'s elasticity when following its target.") {
    engine.SetCameraSpringiness(static_cast<float>(scm_to_double(springiness)));
    return SCM_EOL;
}

SCM_DEFINE (CameraSetZoom, "camera-set-zoom", 1, 0, 0,
            (SCM zoom),
            "Zoom the camera to a fraction of the window size.") {
    engine.SetCameraZoom(static_cast<float>(scm_to_double(zoom)));
    return SCM_EOL;
}
   
void ProvideBlendModes() {
    scm_c_define("blend-add", scm_from_int(BlendAdd));
    scm_c_define("blend-none", scm_from_int(BlendNone));
    scm_c_define("blend-alpha", scm_from_int(BlendAlpha));
    scm_c_define("blend-multiply", scm_from_int(BlendMultiply));
}
    
void ProvideKeymap() {
#define MAP_KEY(FROM, TO) \
    scm_c_define(TO, scm_from_uint(static_cast<int>(FROM)));
    MAP_KEY(sf::Keyboard::A, "key-a");
    MAP_KEY(sf::Keyboard::B, "key-b");
    MAP_KEY(sf::Keyboard::C, "key-c");
    MAP_KEY(sf::Keyboard::D, "key-d");
    MAP_KEY(sf::Keyboard::E, "key-e");
    MAP_KEY(sf::Keyboard::F, "key-f");
    MAP_KEY(sf::Keyboard::G, "key-g");
    MAP_KEY(sf::Keyboard::H, "key-h");
    MAP_KEY(sf::Keyboard::I, "key-i");
    MAP_KEY(sf::Keyboard::J, "key-j");
    MAP_KEY(sf::Keyboard::K, "key-k");
    MAP_KEY(sf::Keyboard::L, "key-l");
    MAP_KEY(sf::Keyboard::M, "key-m");
    MAP_KEY(sf::Keyboard::N, "key-n");
    MAP_KEY(sf::Keyboard::O, "key-o");
    MAP_KEY(sf::Keyboard::P, "key-p");
    MAP_KEY(sf::Keyboard::Q, "key-q");
    MAP_KEY(sf::Keyboard::R, "key-r");
    MAP_KEY(sf::Keyboard::S, "key-s");
    MAP_KEY(sf::Keyboard::T, "key-t");
    MAP_KEY(sf::Keyboard::U, "key-u");
    MAP_KEY(sf::Keyboard::V, "key-v");
    MAP_KEY(sf::Keyboard::W, "key-w");
    MAP_KEY(sf::Keyboard::X, "key-x");
    MAP_KEY(sf::Keyboard::Y, "key-y");
    MAP_KEY(sf::Keyboard::Z, "key-z");
    MAP_KEY(sf::Keyboard::Num0, "key-0");
    MAP_KEY(sf::Keyboard::Num1, "key-1");
    MAP_KEY(sf::Keyboard::Num2, "key-2");
    MAP_KEY(sf::Keyboard::Num3, "key-3");
    MAP_KEY(sf::Keyboard::Num4, "key-4");
    MAP_KEY(sf::Keyboard::Num5, "key-5");
    MAP_KEY(sf::Keyboard::Num6, "key-6");
    MAP_KEY(sf::Keyboard::Num7, "key-7");
    MAP_KEY(sf::Keyboard::Num8, "key-8");
    MAP_KEY(sf::Keyboard::Num9, "key-9");
    MAP_KEY(sf::Keyboard::Escape, "key-esc");
    MAP_KEY(sf::Keyboard::LControl, "key-lctrl");
    MAP_KEY(sf::Keyboard::LShift, "key-lshift");
    MAP_KEY(sf::Keyboard::LAlt, "key-lalt");
    MAP_KEY(sf::Keyboard::LSystem, "key-lsystem");
    MAP_KEY(sf::Keyboard::RControl, "key-lctrl");
    MAP_KEY(sf::Keyboard::RShift, "key-rshift");
    MAP_KEY(sf::Keyboard::RAlt, "key-ralt");
    MAP_KEY(sf::Keyboard::RSystem, "key-rsystem");
    MAP_KEY(sf::Keyboard::Menu, "key-menu");
    MAP_KEY(sf::Keyboard::LBracket, "key-lbracket");
    MAP_KEY(sf::Keyboard::RBracket, "key-rbracket");
    MAP_KEY(sf::Keyboard::SemiColon, "key-semicolon");
    MAP_KEY(sf::Keyboard::Comma, "key-comma");
    MAP_KEY(sf::Keyboard::Period, "key-period");
    MAP_KEY(sf::Keyboard::Quote, "key-quote");
    MAP_KEY(sf::Keyboard::Slash, "key-slash");
    MAP_KEY(sf::Keyboard::BackSlash, "key-backslash");
    MAP_KEY(sf::Keyboard::Tilde, "key-tilde");
    MAP_KEY(sf::Keyboard::Equal, "key-equal");
    MAP_KEY(sf::Keyboard::Dash, "key-dash");
    MAP_KEY(sf::Keyboard::Space, "key-space");
    MAP_KEY(sf::Keyboard::Return, "key-return");
    MAP_KEY(sf::Keyboard::BackSpace, "key-backspace");
    MAP_KEY(sf::Keyboard::Tab, "key-tab");
    MAP_KEY(sf::Keyboard::PageUp, "key-pageup");
    MAP_KEY(sf::Keyboard::PageDown, "key-pagedown");
    MAP_KEY(sf::Keyboard::End, "key-end");
    MAP_KEY(sf::Keyboard::Home, "key-home");
    MAP_KEY(sf::Keyboard::Insert, "key-insert");
    MAP_KEY(sf::Keyboard::Delete, "key-delete");
    MAP_KEY(sf::Keyboard::Add, "key-plus");
    MAP_KEY(sf::Keyboard::Subtract, "key-minus");
    MAP_KEY(sf::Keyboard::Multiply, "key-multiply");
    MAP_KEY(sf::Keyboard::Divide, "key-divide");
    MAP_KEY(sf::Keyboard::Left, "key-left");
    MAP_KEY(sf::Keyboard::Right, "key-right");
    MAP_KEY(sf::Keyboard::Up, "key-up");
    MAP_KEY(sf::Keyboard::Down, "key-down");
    MAP_KEY(sf::Keyboard::Numpad0, "key-num0");
    MAP_KEY(sf::Keyboard::Numpad1, "key-num1");
    MAP_KEY(sf::Keyboard::Numpad2, "key-num2");
    MAP_KEY(sf::Keyboard::Numpad3, "key-num3");
    MAP_KEY(sf::Keyboard::Numpad4, "key-num4");
    MAP_KEY(sf::Keyboard::Numpad5, "key-num5");
    MAP_KEY(sf::Keyboard::Numpad6, "key-num6");
    MAP_KEY(sf::Keyboard::Numpad7, "key-num7");
    MAP_KEY(sf::Keyboard::Numpad8, "key-num8");
    MAP_KEY(sf::Keyboard::Numpad9, "key-num9");
    MAP_KEY(sf::Keyboard::F1, "key-f1");
    MAP_KEY(sf::Keyboard::F2, "key-f2");
    MAP_KEY(sf::Keyboard::F3, "key-f3");
    MAP_KEY(sf::Keyboard::F4, "key-f4");
    MAP_KEY(sf::Keyboard::F5, "key-f5");
    MAP_KEY(sf::Keyboard::F6, "key-f6");
    MAP_KEY(sf::Keyboard::F7, "key-f7");
    MAP_KEY(sf::Keyboard::F8, "key-f8");
    MAP_KEY(sf::Keyboard::F9, "key-f9");
    MAP_KEY(sf::Keyboard::F10, "key-f10");
    MAP_KEY(sf::Keyboard::F11, "key-f11");
    MAP_KEY(sf::Keyboard::F12, "key-f12");
    MAP_KEY(sf::Keyboard::F13, "key-f13");
    MAP_KEY(sf::Keyboard::F14, "key-f14");
    MAP_KEY(sf::Keyboard::F15, "key-f15");
    MAP_KEY(sf::Keyboard::Pause, "key-pause");
}

void DoEngineAPIWrap() {
    /* guile comes with a handy utility called guile-snarf
     * that makes defining API functions easier (see SCM_DEFINES 
     * above). When adding a new function, remember to run the 
     * snarf script in the src dir. */
#include "snarf.x"
    ProvideBlendModes();
    ProvideKeymap();
}

}
