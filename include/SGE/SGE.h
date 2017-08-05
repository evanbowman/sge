#ifndef SGE_H
#define SGE_H

#include "SGE_Types.h"

#ifdef __cplusplus
extern "C" {
#endif
    SGE_Bool SGE_Main(void(*entryFn)());
    SGE_Bool SGE_IsRunning();
    void SGE_Exit();

    void SGE_SetRefreshColor(const SGE_Color* color);

    SGE_UIVec2 SGE_GetWindowSize();
    SGE_Bool SGE_ChangeWindow(SGE_UIVec2* size,
                              const char* title,
                              SGE_Bool fullscreen);
    
    SGE_Bool SGE_EntityCreate(SGE_UUID* entity);
    SGE_Bool SGE_EntityClone(SGE_UUID src, SGE_UUID* dest);
    SGE_Bool SGE_EntityAddAttribute(SGE_UUID entity, SGE_Attribute attrib);
    SGE_Bool SGE_EntitySetAnimation(SGE_UUID entity, SGE_UUID animation);
    SGE_Bool SGE_EntitySetKeyframe(SGE_UUID entity, SGE_Keyframe keyframe);
    SGE_Bool SGE_EntitySetPosition(SGE_UUID entity, const SGE_Vec2* pos);
    SGE_Bool SGE_EntitySetScale(SGE_UUID entity, const SGE_Vec2* scale);
    SGE_Bool SGE_EntitySetBlendMode(SGE_UUID entity, SGE_BlendMode mode);
    SGE_Bool SGE_EntitySetZOrder(SGE_UUID entity, int zOrder);
    SGE_Bool SGE_EntitySetColor(SGE_UUID entity, const SGE_Color* color);
    SGE_Bool SGE_EntityGetPosition(SGE_UUID entity, SGE_Vec2* position);
    SGE_Bool SGE_EntityGetKeyframe(SGE_UUID entity, SGE_Keyframe* keyframe);
    SGE_Bool SGE_EntityRemove(SGE_UUID entity);
    SGE_Bool SGE_EntityRemoveAttribute(SGE_UUID entity, SGE_Attribute attrib);
    SGE_Bool SGE_EntityEmitSound(SGE_UUID entity,
                                 SGE_UUID sound,
                                 float attenuation);

    SGE_Bool SGE_CameraSetTarget(SGE_UUID entity);
    void SGE_CameraSetCenter(const SGE_Vec2* center);
    void SGE_CameraSetSpringiness(float springiness);
    void SGE_CameraSetZoom(float zoom);
    SGE_Vec2 SGE_CameraGetViewSize();

    SGE_Bool SGE_TimerCreate(SGE_UUID* timer);
    SGE_Bool SGE_TimerReset(SGE_UUID timer, SGE_USec* elapsed);
    SGE_Bool SGE_TimerRemove(SGE_UUID timer);

    SGE_Bool SGE_SoundCreate(SGE_UUID* sound, const char* sourceFile);
    SGE_Bool SGE_SoundPlay(SGE_UUID sound,
                           SGE_Vec2* position,
                           float attenuation);
    
    SGE_Bool SGE_AnimationCreate(SGE_UUID* animation,
                                 const char* sourceFile,
                                 SGE_IVec2 start,
                                 SGE_IVec2 frameSize,
                                 SGE_Vec2 origin);

    SGE_Bool SGE_CollisionTest();
    
    SGE_Bool SGE_PollEvents(SGE_EventHolder* event);

    void SGE_Microsleep(SGE_USec sleepDuration);
    
    const char* SGE_GetError();

    const char* SGE_ResourcePath();
    void SGE_ConfigureResourcePath(const char* path);
    
#ifdef __cplusplus
}
#endif
    
#endif // SGE_H
