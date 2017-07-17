#ifndef TYPES_H
#define TYPES_H

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
    SGE_EventCode_KeyPressed
} SGE_EventCode;

typedef struct {
    uint32_t unicode;
} SGE_TextEntered;

typedef struct {
    int key;
} SGE_KeyPressed;

typedef union {
    SGE_TextEntered textEntered;
    SGE_KeyPressed keyPressed;
} SGE_Event;

typedef struct {
    SGE_Event event;
    SGE_EventCode code;
} SGE_EventHolder;

#endif /* TYPES_H */
