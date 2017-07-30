#ifndef SGE_TYPES_H
#define SGE_TYPES_H

#include "SGE_KeyCode.h"

typedef unsigned long SGE_UUID;
typedef unsigned char SGE_u8;
typedef unsigned long SGE_USec;
typedef unsigned long SGE_Keyframe;

typedef enum __SGE_Bool {
    SGE_False, SGE_True
} SGE_Bool;

typedef struct __SGE_Vec2 {
    float x, y;
} SGE_Vec2;

typedef struct __SGE_IVec2 {
    int x, y;
} SGE_IVec2;

typedef struct __SGE_UIVec2 {
    unsigned x, y;
} SGE_UIVec2;

typedef struct __SGE_Color{
    SGE_u8 r, g, b, a;
} SGE_Color;

typedef enum __SGE_Attribute {
    SGE_Attr_Hidden,
    SGE_Attr_PositionAbsolute,
    SGE_Attr_Count
} SGE_Attribute;

typedef enum __SGE_BlendMode {
    SGE_BM_None,
    SGE_BM_Add,
    SGE_BM_Alpha,
    SGE_BM_Multiply,
    SGE_BM_Count
} SGE_BlendMode;

typedef enum __SGE_EventCode {
    SGE_EventCode_TextEntered,
    SGE_EventCode_KeyPressed,
    SGE_EventCode_KeyReleased,
    SGE_EventCode_Collision,
} SGE_EventCode;

typedef struct __SGE_TextEnteredEvent {
    unsigned unicode;
} SGE_TextEnteredEvent;

typedef struct __SGE_KeyEvent {
    SGE_KeyCode keyCode;
} SGE_KeyEvent;

typedef struct __SGE_CollisionEvent {
    SGE_UUID first, second;
} SGE_CollisionEvent;

typedef union __SGE_Event {
    SGE_TextEnteredEvent text;
    SGE_KeyEvent key;
    SGE_CollisionEvent collision;
} SGE_Event;

typedef struct __SGE_EventHolder {
    SGE_Event event;
    SGE_EventCode code;
} SGE_EventHolder;

typedef struct __SGE_AABB {
    SGE_Vec2 center, halfExtent;
} SGE_AABB;

#endif /* SGE_TYPES_H */
