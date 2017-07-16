#ifndef SGE_H
#define SGE_H

#include "Types.h"

#ifdef __cplusplus
extern "C" {
#endif

    SGE_Bool SGE_IsRunning();

    void SGE_SetRefreshColor(SGE_Color color);

    SGE_UIVec2 SGE_GetWindowSize();

    SGE_Bool SGE_CreateEntity(SGE_UUID* entity);
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

    SGE_Bool SGE_CreateTimer(SGE_UUID* timer);
    SGE_Bool SGE_ResetTimer(SGE_UUID timer, SGE_USec* elapsed);
    SGE_Bool SGE_RemoveTimer(SGE_UUID timer);

    SGE_Bool SGE_CreateAnimation(SGE_UUID* animation,
                                 const char* sourceFile,
                                 SGE_IVec2 start,
                                 SGE_IVec2 frameSize,
                                 SGE_Vec2 origin);

    const char* SGE_GetError();
    
#ifdef __cplusplus
}
#endif
    
#endif // SGE_H
