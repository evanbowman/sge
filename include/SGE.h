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

    SGE_Bool SGE_CreateEntity(SGE_UUID* entity);
    SGE_Bool SGE_CloneEntity(SGE_UUID src, SGE_UUID* dest);
    SGE_Bool SGE_AddEntityAttribute(SGE_UUID entity, SGE_Attribute attrib);
    SGE_Bool SGE_SetEntityAnimation(SGE_UUID entity, SGE_UUID animation);
    SGE_Bool SGE_SetEntityKeyframe(SGE_UUID entity, SGE_Keyframe keyframe);
    SGE_Bool SGE_SetEntityPosition(SGE_UUID entity, SGE_Vec2 pos);
    SGE_Bool SGE_SetEntityScale(SGE_UUID entity, SGE_Vec2 scale);
    SGE_Bool SGE_SetEntityBlendMode(SGE_UUID entity, SGE_BlendMode mode);
    SGE_Bool SGE_SetEntityZOrder(SGE_UUID entity, int zOrder);
    SGE_Bool SGE_SetEntityColor(SGE_UUID entity, SGE_Color color);
    SGE_Bool SGE_GetEntityPosition(SGE_UUID entity, SGE_Vec2* position);
    SGE_Bool SGE_GetEntityKeyframe(SGE_UUID entity, SGE_Keyframe* keyframe);
    SGE_Bool SGE_RemoveEntity(SGE_UUID entity);
    SGE_Bool SGE_RemoveEntityAttribute(SGE_UUID entity, SGE_Attribute attrib);

    SGE_Bool SGE_SetCameraTarget(SGE_UUID entity);
    void SGE_SetCameraCenter(SGE_Vec2 center);
    void SGE_SetCameraSpringiness(float springiness);
    void SGE_SetCameraZoom(float zoom);
    SGE_Vec2 SGE_GetCameraViewSize();
    
    SGE_Bool SGE_CreateTimer(SGE_UUID* timer);
    SGE_Bool SGE_ResetTimer(SGE_UUID timer, SGE_USec* elapsed);
    SGE_Bool SGE_RemoveTimer(SGE_UUID timer);

    SGE_Bool SGE_CreateAnimation(SGE_UUID* animation,
                                 const char* sourceFile,
                                 SGE_IVec2 start,
                                 SGE_IVec2 frameSize,
                                 SGE_Vec2 origin);

    SGE_Bool SGE_PollEvents(SGE_EventHolder* event);
    void SGE_RecordEvents(SGE_Bool enabled);

    SGE_Bool SGE_Main(void(*entryFn)());
    
    const char* SGE_GetError();
    
    void SGE_Exit();
    
#ifdef __cplusplus
}
#endif
    
#endif // SGE_H
