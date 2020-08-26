#pragma once

typedef enum {
	VAL_NIL,
	VAL_BOOL,
	VAL_INT,
	VAL_FLOAT,
	VAL_VECTOR
} ValueType;

typedef struct {
	ValueType type;
	union {
		bool boolean;
		int integer;
		float fp;
		float v1, v2;
	} as;
} Value;

#define NIL_VAL(value)       (Value{ VAL_NIL, { .number = 0 } })
#define BOOL_VAL(value)      (Value{ VAL_BOOL, { .boolean = value } })
#define INT_VAL(value)    (Value{ VAL_INT, { .integer = value } })
#define FLOAT_VAL(value)  (Value{ VAL_FLOAT, { .fp = value } })
#define VECTOR_VAL(val1,val2)    (Value{ VAL_VECTOR, { .v1 = val1, .v2 = val2 }})

#define AS_BOOL(value)       ((value).as.boolean)
#define AS_INT(value)     ((value).as.integer)
#define AS_FLOAT(value)   ((value).as.fp)
#define AS_VECTOR(value)     (value).as.v1, (value).as.v2

#define IS_NIL(value)		 ((value).type == VAL_NIL)
#define IS_BOOL(value)       ((value).type == VAL_BOOL)
#define IS_INT(value)     ((value).type == VAL_INT)
#define IS_FLOAT(value)   ((value).type == VAL_FLOAT)
#define IS_VECTOR(value)     ((value).type == VAL_VECTOR)
