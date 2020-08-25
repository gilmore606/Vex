#pragma once

typedef enum {
	VAL_NIL,
	VAL_BOOL,
	VAL_NUMBER
} ValueType;

typedef struct {
	ValueType type;
	union {
		bool boolean;
		double number;
	} as;
} Value;

#define NIL_VAL(value)       (Value{ VAL_NIL, { .number = 0 } })
#define BOOL_VAL(value)      (Value{ VAL_BOOL, { .boolean = value } })
#define NUMBER_VAL(value)    (Value{ VAL_NUMBER, { .number = value } })

#define AS_BOOL(value)       ((value).as.boolean)
#define AS_NUMBER(value)     ((value).as.number)

#define IS_BOOL(value)       ((value).type == VAL_BOOL)
#define IS_NUMBER(value)     ((value).type == VAL_NUMBER)
#define IS_NIL(value)		 ((value).type == VAL_NIL)
