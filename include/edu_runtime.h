#ifndef EDU_RUNTIME_H
#define EDU_RUNTIME_H

typedef enum {
    EDU_NIL,
    EDU_NUM,
    EDU_STR
} edu_type_t;

typedef struct {
    edu_type_t type;
    union {
        double num;
        char *str;
    } as;
} Value;

Value edu_num(double n);
Value edu_str(const char *s);
Value edu_nil(void);

int edu_truthy(Value v);

Value edu_add(Value l, Value r);
Value edu_sub(Value l, Value r);
Value edu_mul(Value l, Value r);
Value edu_div(Value l, Value r);
Value edu_lt(Value l, Value r);
Value edu_gt(Value l, Value r);
Value edu_lteq(Value l, Value r);
Value edu_gteq(Value l, Value r);
Value edu_eq(Value l, Value r);
Value edu_neq(Value l, Value r);
Value edu_neg(Value v);

void edu_print(Value v);
Value edu_read(void);
void edu_write(Value v);

#endif 
