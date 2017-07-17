#include "SGE.h"
#include <libguile.h>
#include <thread>
#include <SFML/Window.hpp>
#include <iostream>

#include "SchemeInterface.hpp"
#include "ResourcePath.hpp"

namespace {
void DoEngineAPIWrap();
}

void scheme::Start() {
    std::pair<int, char**> dummyArgs{};
    scm_boot_guile(dummyArgs.first, dummyArgs.second,
                   [](void* closure, int argc, char** argv) {
                       static const auto entryScriptPath =
                           ResourcePath() + "scripts/main.scm";
                       DoEngineAPIWrap();
                       scm_c_primitive_load(entryScriptPath.c_str());
                       SCM func = scm_variable_ref(scm_c_lookup("main"));
                       scm_call_0(func);
                   }, 0);
}

namespace {
inline SGE_UUID UUIDCast(SCM uidAsScm) {
    return scm_to_ssize_t(uidAsScm);
}
    
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

SCM_DEFINE (EntityCreate, "sge-entity-create", 0, 0, 0,
            (), "Create an entity.") {
    SGE_UUID entity;
    if (SGE_CreateEntity(&entity)) {
        return scm_from_ssize_t(entity);
    }
    return SCM_EOL;
}

SCM_DEFINE (EntityRemove, "sge-entity-remove", 1, 0, 0,
            (SCM entity), "Remove an entity.") {
    SGE_RemoveEntity(UUIDCast(entity));
    return SCM_EOL;
}

SCM_DEFINE (EntitySetPosition, "sge-entity-set-position", 3, 0, 0,
            (SCM entity, SCM x, SCM y), "Set an entity\'s position.") {
    SGE_SetEntityPosition(UUIDCast(entity), {
            static_cast<float>(scm_to_double(x)),
            static_cast<float>(scm_to_double(y))
        });
    return entity;
}

SCM_DEFINE (EntityAddAttribute, "sge-entity-add-attrib", 2, 0, 0,
            (SCM entity, SCM attrib), "Add an attribute to an Entity.") {
    if (!SGE_AddEntityAttribute(UUIDCast(entity),
                                (SGE_Attribute)scm_to_int(attrib))) {
        // ...
    }
    return entity;
}

SCM_DEFINE (EntityRemoveAttribute, "sge-entity-remove-attrib", 2, 0, 0,
            (SCM entity, SCM attrib), "Remove an attribute from an Entity.") {
    if (!SGE_RemoveEntityAttribute(UUIDCast(entity),
                                   (SGE_Attribute)scm_to_int(attrib))) {
        // ...
    }
    return entity;
}
    
SCM_DEFINE (EntityGetPosition, "sge-entity-get-position", 1, 0, 0,
            (SCM entity), "Get an entity\'s position.") {
    SGE_Vec2 pos;
    if (SGE_GetEntityPosition(UUIDCast(entity), &pos)) {
        return scm_cons(scm_from_double(pos.x), scm_from_double(pos.y));    
    }
    return SCM_EOL;
}

SCM_DEFINE (EntitySetAnimation, "sge-entity-set-animation", 2, 0, 0,
            (SCM entity, SCM animation),
            "Set a keyframe sequence representation for an entity.") {
    if (!SGE_SetEntityAnimation(UUIDCast(entity),
                                scm_to_ssize_t(animation))) {
        // TODO...
    }
    return entity;
}

SCM_DEFINE (EntitySetKeyframe, "sge-entity-set-keyframe", 2, 0, 0,
            (SCM entity, SCM keyframe),
            "Set the keyframe for an entity\'s animation.") {
    if (!SGE_SetEntityKeyframe(UUIDCast(entity),
                               scm_to_ssize_t(keyframe))) {
        // TODO...
    }
    return entity;
}

SCM_DEFINE (EntitySetScale, "sge-entity-set-scale", 3, 0, 0,
            (SCM entity, SCM xScale, SCM yScale),
            "Set the scale factor for an entity\'s graphics component") {
    if (!SGE_SetEntityScale(UUIDCast(entity), {
                static_cast<float>(scm_to_double(xScale)),
                static_cast<float>(scm_to_double(yScale))
            })) {
        // TODO...
    }
    return entity;
}

SCM_DEFINE (EntitySetZOrder, "sge-entity-set-zorder", 2, 0, 0,
            (SCM entity, SCM zOrder),
            "Set an entity\'s zorder index.") {
    SGE_SetEntityZOrder(UUIDCast(entity), scm_to_int(zOrder));
    return entity;
}
    
SCM_DEFINE (EntitySetBlendMode, "sge-entity-set-blend-mode", 2, 0, 0,
            (SCM entity, SCM blendMode),
            "Set an entitiy\'s blend mode.") {
    SGE_SetEntityBlendMode(UUIDCast(entity), (SGE_BlendMode)scm_to_int(blendMode));
    return entity;
}

SCM_DEFINE (EntitySetColor, "sge-entity-set-rgba", 5, 0, 0,
            (SCM entity, SCM r, SCM g, SCM b, SCM a),
            "Set an entity\'s color.") {
    SGE_SetEntityColor(UUIDCast(entity), {
        scm_to_uint8(r),
        scm_to_uint8(g),
        scm_to_uint8(b),
        scm_to_uint8(a)
    });
    return entity;
}
    
SCM_DEFINE (EntityGetKeyframe, "sge-entity-get-keyframe", 1, 0, 0,
            (SCM entity), "Get the keyframe for an entity\'s animation.") {
    SGE_Keyframe keyframe;
    if (SGE_GetEntityKeyframe(UUIDCast(entity), &keyframe)) {
        return scm_from_ssize_t(keyframe);        
    }
    return SCM_EOL;
}
    
SCM_DEFINE (IsRunning, "sge-is-running?", 0, 0, 0,
            (), "Check whether the engine is running.") {
    return scm_from_bool(SGE_IsRunning());
}

SCM_DEFINE (GetWindowSize, "sge-window-size", 0, 0, 0,
            (), "Get the size of the game window in pixels.") {
    auto size = SGE_GetWindowSize();
    return scm_cons(scm_from_uint(size.x),
                    scm_from_uint(size.y));
}

SCM_DEFINE (BackgroundSetColor, "sge-set-refresh-rgba", 4, 0, 0,
            (SCM r, SCM g, SCM b, SCM a),
            "Set the refresh color of the window.") {
    SGE_SetRefreshColor({
        scm_to_uint8(r),
        scm_to_uint8(g),
        scm_to_uint8(b),
        scm_to_uint8(a)
    });
    return SCM_EOL;
}
    
SCM_DEFINE (TimerCreate, "sge-timer-create", 0, 0, 0,
            (), "Create a timer.") {
    SGE_UUID timer;
    if (SGE_CreateTimer(&timer)) {
        return scm_from_ssize_t(timer);
    }
    return SCM_EOL;
}

SCM_DEFINE (TimerReset, "sge-timer-reset", 1, 0, 0,
            (SCM timer), "Create a timer.") {
    SGE_USec elapsed;
    if (SGE_ResetTimer(UUIDCast(timer), &elapsed)) {
        return scm_from_ssize_t(elapsed);
    }
    return 0;
}

SCM_DEFINE (TimerRemove, "sge-timer-remove", 1, 0, 0,
            (SCM timer), "Remove a timer.") {
    SGE_RemoveTimer(UUIDCast(timer));
    return SCM_EOL;
}

SCM_DEFINE (MicroSleep, "sge-micro-sleep", 1, 0, 0,
            (SCM usecs), "Sleep for a number of microseconds. Integers please.") {
    std::this_thread::sleep_for(std::chrono::microseconds(scm_to_uint(usecs)));
    return SCM_EOL;
}
    
SCM_DEFINE (KeyPressed, "sge-key-pressed?", 1, 0, 0,
            (SCM keycode), "Test whether a key is pressed.") {
    return scm_from_bool(
        sf::Keyboard::isKeyPressed(
            static_cast<sf::Keyboard::Key>(scm_to_uint(keycode))));
}
    
SCM_DEFINE (AnimationCreate, "sge-animation-create", 7, 0, 0,
            (SCM fname, SCM x, SCM y, SCM w, SCM h, SCM ox, SCM oy),
            "Create an animation, "
            "(fname x y keyframe-width keyframe-height).") {
    SGE_UUID animation;
    if (SGE_CreateAnimation(&animation,
                            scm_to_latin1_string(fname), {
                            scm_to_int(x), scm_to_int(y)
                        }, {
                            scm_to_int(w), scm_to_int(h)
                        },
                        {
                            static_cast<float>(scm_to_double(ox)),
                            static_cast<float>(scm_to_double(oy))
                        })) {
        return scm_from_ssize_t(animation);
    }
    return SCM_EOL;
}
   
SCM_DEFINE (CameraSetTarget, "sge-camera-set-target", 1, 0, 0,
            (SCM entity),
            "Set camera target to an entity.") {
    SGE_SetCameraTarget(UUIDCast(entity));
    return SCM_EOL;
}

SCM_DEFINE (CameraSetCenter, "sge-camera-set-center", 2, 0, 0,
            (SCM x, SCM y),
            "Manually set the camera\'s center. This procedure has occasional"
            " uses, but better to set the camera\'s center by making it track an"
            " entity with camera-set-target") {
    SGE_SetCameraCenter({ static_cast<float>(scm_to_double(x)),
                          static_cast<float>(scm_to_double(y)) });
    return SCM_EOL;
}
    
SCM_DEFINE (CameraSetSpringiness, "sge-camera-set-springiness", 1, 0, 0,
            (SCM springiness),
            "Set the camera\'s elasticity when following its target.") {
    SGE_SetCameraSpringiness(static_cast<float>(scm_to_double(springiness)));
    return SCM_EOL;
}

SCM_DEFINE (CameraSetZoom, "sge-camera-set-zoom", 1, 0, 0,
            (SCM zoom),
            "Zoom the camera to a fraction of the window size.") {
    SGE_SetCameraZoom(static_cast<float>(scm_to_double(zoom)));
    return SCM_EOL;
}

SCM_DEFINE (CameraGetViewSize, "sge-camera-get-view-size", 0, 0, 0,
            (SCM zoom),
            "Get the camera\'s view size.") {
    auto viewSize = SGE_GetCameraViewSize();
    return scm_cons(scm_from_double(viewSize.x), scm_from_double(viewSize.y));
}    

SCM_DEFINE (PollEvents, "sge-poll-events", 0, 0, 0,
            (), "Poll for window input events.") {
    SGE_EventHolder holder;
    if (SGE_PollEvents(&holder)) {
        switch (holder.code) {
        case SGE_EventCode_TextEntered:
            return
                scm_cons(scm_string_to_symbol(
                             scm_from_latin1_string("sge-event-text")),
                         scm_from_uint32(holder.event.textEntered.unicode));

        case SGE_EventCode_KeyPressed:
            return
                scm_cons(scm_string_to_symbol(
                             scm_from_latin1_string("sge-event-key-pressed")),
                         scm_from_uint(holder.event.keyPressed.key));
        }
    }
    return SCM_EOL;
}

SCM_DEFINE (RecordEvents, "sge-record-events", 1, 0, 0,
            (SCM enabled), "Enable or disable event recording.") {
    SGE_RecordEvents((SGE_Bool)scm_to_bool(enabled));
    return SCM_EOL;
}
    
void ProvideAttributes() {
    scm_c_define("sge-attrib-hidden", scm_from_int(SGE_Attr_Hidden));
    scm_c_define("sge-attrib-position-absolute",
                 scm_from_int(SGE_Attr_PositionAbsolute));
}
    
void ProvideBlendModes() {
    scm_c_define("sge-blend-add", scm_from_int(SGE_BM_Add));
    scm_c_define("sge-blend-none", scm_from_int(SGE_BM_None));
    scm_c_define("sge-blend-alpha", scm_from_int(SGE_BM_Alpha));
    scm_c_define("sge-blend-multiply", scm_from_int(SGE_BM_Multiply));
}
    
void ProvideKeymap() {
#define MAP_KEY(FROM, TO) \
    scm_c_define(TO, scm_from_uint(static_cast<int>(FROM)));
    MAP_KEY(sf::Keyboard::A, "sge-key-a");
    MAP_KEY(sf::Keyboard::B, "sge-key-b");
    MAP_KEY(sf::Keyboard::C, "sge-key-c");
    MAP_KEY(sf::Keyboard::D, "sge-key-d");
    MAP_KEY(sf::Keyboard::E, "sge-key-e");
    MAP_KEY(sf::Keyboard::F, "sge-key-f");
    MAP_KEY(sf::Keyboard::G, "sge-key-g");
    MAP_KEY(sf::Keyboard::H, "sge-key-h");
    MAP_KEY(sf::Keyboard::I, "sge-key-i");
    MAP_KEY(sf::Keyboard::J, "sge-key-j");
    MAP_KEY(sf::Keyboard::K, "sge-key-k");
    MAP_KEY(sf::Keyboard::L, "sge-key-l");
    MAP_KEY(sf::Keyboard::M, "sge-key-m");
    MAP_KEY(sf::Keyboard::N, "sge-key-n");
    MAP_KEY(sf::Keyboard::O, "sge-key-o");
    MAP_KEY(sf::Keyboard::P, "sge-key-p");
    MAP_KEY(sf::Keyboard::Q, "sge-key-q");
    MAP_KEY(sf::Keyboard::R, "sge-key-r");
    MAP_KEY(sf::Keyboard::S, "sge-key-s");
    MAP_KEY(sf::Keyboard::T, "sge-key-t");
    MAP_KEY(sf::Keyboard::U, "sge-key-u");
    MAP_KEY(sf::Keyboard::V, "sge-key-v");
    MAP_KEY(sf::Keyboard::W, "sge-key-w");
    MAP_KEY(sf::Keyboard::X, "sge-key-x");
    MAP_KEY(sf::Keyboard::Y, "sge-key-y");
    MAP_KEY(sf::Keyboard::Z, "sge-key-z");
    MAP_KEY(sf::Keyboard::Num0, "sge-key-0");
    MAP_KEY(sf::Keyboard::Num1, "sge-key-1");
    MAP_KEY(sf::Keyboard::Num2, "sge-key-2");
    MAP_KEY(sf::Keyboard::Num3, "sge-key-3");
    MAP_KEY(sf::Keyboard::Num4, "sge-key-4");
    MAP_KEY(sf::Keyboard::Num5, "sge-key-5");
    MAP_KEY(sf::Keyboard::Num6, "sge-key-6");
    MAP_KEY(sf::Keyboard::Num7, "sge-key-7");
    MAP_KEY(sf::Keyboard::Num8, "sge-key-8");
    MAP_KEY(sf::Keyboard::Num9, "sge-key-9");
    MAP_KEY(sf::Keyboard::Escape, "sge-key-esc");
    MAP_KEY(sf::Keyboard::LControl, "sge-key-lctrl");
    MAP_KEY(sf::Keyboard::LShift, "sge-key-lshift");
    MAP_KEY(sf::Keyboard::LAlt, "sge-key-lalt");
    MAP_KEY(sf::Keyboard::LSystem, "sge-key-lsystem");
    MAP_KEY(sf::Keyboard::RControl, "sge-key-lctrl");
    MAP_KEY(sf::Keyboard::RShift, "sge-key-rshift");
    MAP_KEY(sf::Keyboard::RAlt, "sge-key-ralt");
    MAP_KEY(sf::Keyboard::RSystem, "sge-key-rsystem");
    MAP_KEY(sf::Keyboard::Menu, "sge-key-menu");
    MAP_KEY(sf::Keyboard::LBracket, "sge-key-lbracket");
    MAP_KEY(sf::Keyboard::RBracket, "sge-key-rbracket");
    MAP_KEY(sf::Keyboard::SemiColon, "sge-key-semicolon");
    MAP_KEY(sf::Keyboard::Comma, "sge-key-comma");
    MAP_KEY(sf::Keyboard::Period, "sge-key-period");
    MAP_KEY(sf::Keyboard::Quote, "sge-key-quote");
    MAP_KEY(sf::Keyboard::Slash, "sge-key-slash");
    MAP_KEY(sf::Keyboard::BackSlash, "sge-key-backslash");
    MAP_KEY(sf::Keyboard::Tilde, "sge-key-tilde");
    MAP_KEY(sf::Keyboard::Equal, "sge-key-equal");
    MAP_KEY(sf::Keyboard::Dash, "sge-key-dash");
    MAP_KEY(sf::Keyboard::Space, "sge-key-space");
    MAP_KEY(sf::Keyboard::Return, "sge-key-return");
    MAP_KEY(sf::Keyboard::BackSpace, "sge-key-backspace");
    MAP_KEY(sf::Keyboard::Tab, "sge-key-tab");
    MAP_KEY(sf::Keyboard::PageUp, "sge-key-pageup");
    MAP_KEY(sf::Keyboard::PageDown, "sge-key-pagedown");
    MAP_KEY(sf::Keyboard::End, "sge-key-end");
    MAP_KEY(sf::Keyboard::Home, "sge-key-home");
    MAP_KEY(sf::Keyboard::Insert, "sge-key-insert");
    MAP_KEY(sf::Keyboard::Delete, "sge-key-delete");
    MAP_KEY(sf::Keyboard::Add, "sge-key-plus");
    MAP_KEY(sf::Keyboard::Subtract, "sge-key-minus");
    MAP_KEY(sf::Keyboard::Multiply, "sge-key-multiply");
    MAP_KEY(sf::Keyboard::Divide, "sge-key-divide");
    MAP_KEY(sf::Keyboard::Left, "sge-key-left");
    MAP_KEY(sf::Keyboard::Right, "sge-key-right");
    MAP_KEY(sf::Keyboard::Up, "sge-key-up");
    MAP_KEY(sf::Keyboard::Down, "sge-key-down");
    MAP_KEY(sf::Keyboard::Numpad0, "sge-key-num0");
    MAP_KEY(sf::Keyboard::Numpad1, "sge-key-num1");
    MAP_KEY(sf::Keyboard::Numpad2, "sge-key-num2");
    MAP_KEY(sf::Keyboard::Numpad3, "sge-key-num3");
    MAP_KEY(sf::Keyboard::Numpad4, "sge-key-num4");
    MAP_KEY(sf::Keyboard::Numpad5, "sge-key-num5");
    MAP_KEY(sf::Keyboard::Numpad6, "sge-key-num6");
    MAP_KEY(sf::Keyboard::Numpad7, "sge-key-num7");
    MAP_KEY(sf::Keyboard::Numpad8, "sge-key-num8");
    MAP_KEY(sf::Keyboard::Numpad9, "sge-key-num9");
    MAP_KEY(sf::Keyboard::F1, "sge-key-f1");
    MAP_KEY(sf::Keyboard::F2, "sge-key-f2");
    MAP_KEY(sf::Keyboard::F3, "sge-key-f3");
    MAP_KEY(sf::Keyboard::F4, "sge-key-f4");
    MAP_KEY(sf::Keyboard::F5, "sge-key-f5");
    MAP_KEY(sf::Keyboard::F6, "sge-key-f6");
    MAP_KEY(sf::Keyboard::F7, "sge-key-f7");
    MAP_KEY(sf::Keyboard::F8, "sge-key-f8");
    MAP_KEY(sf::Keyboard::F9, "sge-key-f9");
    MAP_KEY(sf::Keyboard::F10, "sge-key-f10");
    MAP_KEY(sf::Keyboard::F11, "sge-key-f11");
    MAP_KEY(sf::Keyboard::F12, "sge-key-f12");
    MAP_KEY(sf::Keyboard::F13, "sge-key-f13");
    MAP_KEY(sf::Keyboard::F14, "sge-key-f14");
    MAP_KEY(sf::Keyboard::F15, "sge-key-f15");
    MAP_KEY(sf::Keyboard::Pause, "sge-key-pause");
}
    
void DoEngineAPIWrap() {
    /* guile comes with a handy utility called guile-snarf
     * that makes defining API functions easier (see SCM_DEFINES 
     * above). When adding a new function, remember to run the 
     * snarf script in the src dir. */
#include "snarf.x"
    ProvideBlendModes();
    ProvideKeymap();
    ProvideAttributes();
}

}
