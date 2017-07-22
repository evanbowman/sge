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

typedef struct {
    int keyCode;
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
    void SGE_RecordEvents(SGE_Bool enabled);

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
