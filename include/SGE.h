#ifndef SGE_H
#define SGE_H

#include <stdlib.h>
#include <stdint.h>

typedef size_t SGE_UUID;
typedef unsigned char SGE_u8;
typedef unsigned long SGE_USec;
typedef size_t SGE_Keyframe;

typedef enum {
    SGE_False, SGE_True
} SGE_Bool;

typedef struct {
    float x, y;
} SGE_Vec2;

typedef struct {
    int x, y;
} SGE_IVec2;

typedef struct {
    unsigned x, y;
} SGE_UIVec2;

typedef struct {
    SGE_u8 r, g, b, a;
} SGE_Color;

typedef enum {
    SGE_Attr_Hidden,
    SGE_Attr_PositionAbsolute,
    SGE_Attr_Count
} SGE_Attribute;

typedef enum {
    SGE_BM_None,
    SGE_BM_Add,
    SGE_BM_Alpha,
    SGE_BM_Multiply,
    SGE_BM_Count
} SGE_BlendMode;

typedef enum {
    SGE_EventCode_TextEntered,
    SGE_EventCode_KeyPressed,
    SGE_EventCode_KeyReleased,
} SGE_EventCode;

typedef struct {
    uint32_t unicode;
} SGE_TextEnteredEvent;

typedef enum {
    SGE_KeyA,
    SGE_KeyB,
    SGE_KeyC,
    SGE_KeyD,
    SGE_KeyE,
    SGE_KeyF,
    SGE_KeyG,
    SGE_KeyH,
    SGE_KeyI,
    SGE_KeyJ,
    SGE_KeyK,
    SGE_KeyL,
    SGE_KeyM,
    SGE_KeyN,
    SGE_KeyO,
    SGE_KeyP,
    SGE_KeyQ,
    SGE_KeyR,
    SGE_KeyS,
    SGE_KeyT,
    SGE_KeyU,
    SGE_KeyV,
    SGE_KeyW,
    SGE_KeyX,
    SGE_KeyY,
    SGE_KeyZ,
    SGE_KeyNum0,
    SGE_KeyNum1,
    SGE_KeyNum2,
    SGE_KeyNum3,
    SGE_KeyNum4,
    SGE_KeyNum5,
    SGE_KeyNum6,
    SGE_KeyNum7,
    SGE_KeyNum8,
    SGE_KeyNum9,
    SGE_KeyEscape,
    SGE_KeyLControl,
    SGE_KeyLShift,
    SGE_KeyLAlt,
    SGE_KeyLSystem,
    SGE_KeyRControl,
    SGE_KeyRShift,
    SGE_KeyRAlt,
    SGE_KeyRSystem,
    SGE_KeyMenu,
    SGE_KeyLBracket,
    SGE_KeyRBracket,
    SGE_KeySemiColon,
    SGE_KeyComma,
    SGE_KeyPeriod,
    SGE_KeyQuote,
    SGE_KeySlash,
    SGE_KeyBackSlash,
    SGE_KeyTilde,
    SGE_KeyEqual,
    SGE_KeyDash,
    SGE_KeySpace,
    SGE_KeyReturn,
    SGE_KeyBackSpace,
    SGE_KeyTab,
    SGE_KeyPageUp,
    SGE_KeyPageDown,
    SGE_KeyEnd,
    SGE_KeyHome,
    SGE_KeyInsert,
    SGE_KeyDelete,
    SGE_KeyAdd,
    SGE_KeySubtract,
    SGE_KeyMultiply,
    SGE_KeyDivide,
    SGE_KeyLeft,
    SGE_KeyRight,
    SGE_KeyUp,
    SGE_KeyDown,
    SGE_KeyNumpad0,
    SGE_KeyNumpad1,
    SGE_KeyNumpad2,
    SGE_KeyNumpad3,
    SGE_KeyNumpad4,
    SGE_KeyNumpad5,
    SGE_KeyNumpad6,
    SGE_KeyNumpad7,
    SGE_KeyNumpad8,
    SGE_KeyNumpad9,
    SGE_KeyF1,
    SGE_KeyF2,
    SGE_KeyF3,
    SGE_KeyF4,
    SGE_KeyF5,
    SGE_KeyF6,
    SGE_KeyF7,
    SGE_KeyF8,
    SGE_KeyF9,
    SGE_KeyF10,
    SGE_KeyF11,
    SGE_KeyF12,
    SGE_KeyF13,
    SGE_KeyF14,
    SGE_KeyF15,
    SGE_KeyPause,
    SGE_KeyCount,
} SGE_KeyCode;

typedef struct {
    SGE_KeyCode keyCode;
} SGE_KeyEvent;

typedef union {
    SGE_TextEnteredEvent text;
    SGE_KeyEvent key;
} SGE_Event;

typedef struct {
    SGE_Event event;
    SGE_EventCode code;
} SGE_EventHolder;

#ifdef __cplusplus
extern "C" {
#endif

    SGE_Bool SGE_IsRunning();

    void SGE_SetRefreshColor(SGE_Color color);

    SGE_UIVec2 SGE_GetWindowSize();

    SGE_Bool SGE_EntityCreate(SGE_UUID* entity);
    SGE_Bool SGE_EntityClone(SGE_UUID src, SGE_UUID* dest);
    SGE_Bool SGE_EntityAddAttribute(SGE_UUID entity, SGE_Attribute attrib);
    SGE_Bool SGE_EntitySetAnimation(SGE_UUID entity, SGE_UUID animation);
    SGE_Bool SGE_EntitySetKeyframe(SGE_UUID entity, SGE_Keyframe keyframe);
    SGE_Bool SGE_EntitySetPosition(SGE_UUID entity, SGE_Vec2 pos);
    SGE_Bool SGE_EntitySetScale(SGE_UUID entity, SGE_Vec2 scale);
    SGE_Bool SGE_EntitySetBlendMode(SGE_UUID entity, SGE_BlendMode mode);
    SGE_Bool SGE_EntitySetZOrder(SGE_UUID entity, int zOrder);
    SGE_Bool SGE_EntitySetColor(SGE_UUID entity, SGE_Color color);
    SGE_Bool SGE_EntityGetPosition(SGE_UUID entity, SGE_Vec2* position);
    SGE_Bool SGE_EntityGetKeyframe(SGE_UUID entity, SGE_Keyframe* keyframe);
    SGE_Bool SGE_EntityRemove(SGE_UUID entity);
    SGE_Bool SGE_EntityRemoveAttribute(SGE_UUID entity, SGE_Attribute attrib);

    SGE_Bool SGE_CameraSetTarget(SGE_UUID entity);
    void SGE_CameraSetCenter(SGE_Vec2 center);
    void SGE_CameraSetSpringiness(float springiness);
    void SGE_CameraSetZoom(float zoom);
    SGE_Vec2 SGE_CameraGetViewSize();
    
    SGE_Bool SGE_TimerCreate(SGE_UUID* timer);
    SGE_Bool SGE_TimerReset(SGE_UUID timer, SGE_USec* elapsed);
    SGE_Bool SGE_TimerRemove(SGE_UUID timer);

    SGE_Bool SGE_AnimationCreate(SGE_UUID* animation,
                                 const char* sourceFile,
                                 SGE_IVec2 start,
                                 SGE_IVec2 frameSize,
                                 SGE_Vec2 origin);

    SGE_Bool SGE_PollEvents(SGE_EventHolder* event);

    SGE_Bool SGE_Main(void(*entryFn)());

    void SGE_Microsleep(SGE_USec sleepDuration);
    
    const char* SGE_GetError();

    const char* SGE_ResourcePath();
    void SGE_ConfigureResourcePath(const char* path);
    
    void SGE_Exit();
    
#ifdef __cplusplus
}
#endif
    
#endif // SGE_H
