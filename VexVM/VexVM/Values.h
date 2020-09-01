#pragma once
#include <string>

typedef enum {
	VAL_NIL,
	VAL_BOOL,
	VAL_INT,
	VAL_FLOAT,
	VAL_VECTOR,
	VAL_COLOR,
	VAL_STRING,
	VAL_OBJECT
} ValueType;

typedef enum {
	OBJ_SPRITE
} ObjectType;

typedef struct {
	std::string s;
} VStr;

typedef struct {
	ObjectType type;
} VObj;

typedef struct {
	ValueType type;
	union {
		bool boolean;
		int integer;
		float fp;
		float vector[2];
		float color[3];
		VStr* string;
		VObj* object;
	} as;
} Value;

#define NIL_VAL(value)       (Value{ VAL_NIL, { .number = 0 } })
#define BOOL_VAL(value)      (Value{ VAL_BOOL, { .boolean = value } })
#define INT_VAL(value)    (Value{ VAL_INT, { .integer = value } })
#define FLOAT_VAL(value)  (Value{ VAL_FLOAT, { .fp = value } })
#define VECTOR_VAL(val1,val2)    (Value{ VAL_VECTOR, { .vector = {val1,val2} }})
#define COLOR_VAL(v1,v2,v3)      (Value{ VAL_COLOR, { .color = {v1,v2,v3} }})
#define STRING_VAL(value) (Value{ VAL_STRING, { .string = value } })
#define OBJECT_VAL(value) (Value{ VAL_OBJECT, { .object = value } })

#define AS_BOOL(value)       ((value).as.boolean)
#define AS_INT(value)     ((value).as.integer)
#define AS_FLOAT(value)   ((value).as.fp)
#define AS_VECTOR(value)     ((value).as.vector)
#define AS_COLOR(value)      ((value).as.color)
#define AS_STRING(value)    ((value).as.string)
#define AS_OBJECT(value)    ((value).as.object)

#define IS_NIL(value)		 ((value).type == VAL_NIL)
#define IS_BOOL(value)       ((value).type == VAL_BOOL)
#define IS_INT(value)     ((value).type == VAL_INT)
#define IS_FLOAT(value)   ((value).type == VAL_FLOAT)
#define IS_VECTOR(value)     ((value).type == VAL_VECTOR)
#define IS_COLOR(value)      ((value).type == VAL_COLOR)
#define IS_STRING(value)    ((value).type == VAL_STRING)
#define IS_OBJECT(value)    ((value).type == VAL_OBJECT)

#define LIFT_VECTOR(value)   Vec((value).as.vector[0], (value).as.vector[1])
#define LIFT_POS(value)      Pos((value).as.vector[0], (value).as.vector[1])
#define LIFT_COLOR(value)    Color((value).as.color[0], (value).as.color[1], (value).as.color[2])
