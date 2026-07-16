#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "edu_runtime.h"

Value edu_num(double n)
{
    Value v;
    v.type = EDU_NUM;
    v.as.num = n;
    return v;
}

Value edu_str(const char *s)
{
    Value v;
    v.type = EDU_STR;
    v.as.str = (char *)s;
    return v;
}

Value edu_nil(void)
{
    Value v;
    v.type = EDU_NIL;
    v.as.num = 0;
    return v;
}

int edu_truthy(Value v)
{
    if (v.type != EDU_NUM) {
        fprintf(stderr, "Condition must evaluate to a number\n");
        exit(EXIT_FAILURE);
    }
    return v.as.num != 0;
}


static const char *value_to_cstr(Value v, char *scratch, size_t scratch_size)
{
    switch (v.type) {
        case EDU_STR:
            return v.as.str;
        case EDU_NUM:
            snprintf(scratch, scratch_size, "%g", v.as.num);
            return scratch;
        default:
            return "";
    }
}

static void require_numeric(Value l, Value r, const char *op)
{
    if (l.type != EDU_NUM || r.type != EDU_NUM) {
        fprintf(stderr, "Binop operands must evaluate to numbers (operator `%s`)\n", op);
        exit(EXIT_FAILURE);
    }
}

Value edu_add(Value l, Value r)
{
    if (l.type == EDU_NUM && r.type == EDU_NUM) {
        return edu_num(l.as.num + r.as.num);
    }
    if (l.type == EDU_STR || r.type == EDU_STR) {
        char lbuf[64], rbuf[64];
        const char *ls = value_to_cstr(l, lbuf, sizeof(lbuf));
        const char *rs = value_to_cstr(r, rbuf, sizeof(rbuf));
        char *result = malloc(strlen(ls) + strlen(rs) + 1);
        strcpy(result, ls);
        strcat(result, rs);
        return edu_str(result);
    }
    fprintf(stderr, "`+` operands must be numbers or strings\n");
    exit(EXIT_FAILURE);
}
Value edu_sub(Value l, Value r) { require_numeric(l, r, "-");  return edu_num(l.as.num - r.as.num); }
Value edu_mul(Value l, Value r) { require_numeric(l, r, "*");  return edu_num(l.as.num * r.as.num); }
Value edu_div(Value l, Value r) { require_numeric(l, r, "/");  return edu_num(l.as.num / r.as.num); }
Value edu_lt(Value l, Value r)  { require_numeric(l, r, "<");  return edu_num(l.as.num <  r.as.num); }
Value edu_gt(Value l, Value r)  { require_numeric(l, r, ">");  return edu_num(l.as.num >  r.as.num); }
Value edu_lteq(Value l, Value r){ require_numeric(l, r, "<="); return edu_num(l.as.num <= r.as.num); }
Value edu_gteq(Value l, Value r){ require_numeric(l, r, ">="); return edu_num(l.as.num >= r.as.num); }
Value edu_eq(Value l, Value r)  { require_numeric(l, r, "=="); return edu_num(l.as.num == r.as.num); }
Value edu_neq(Value l, Value r) { require_numeric(l, r, "!="); return edu_num(l.as.num != r.as.num); }

Value edu_neg(Value v)
{
    if (v.type != EDU_NUM) {
        fprintf(stderr, "Unary `-` operand must evaluate to a number\n");
        exit(EXIT_FAILURE);
    }
    return edu_num(-v.as.num);
}

void edu_print(Value v)
{
    switch (v.type) {
        case EDU_STR:
            fprintf(stderr, "%s\n", v.as.str);
            break;
        case EDU_NUM:
            printf("%g\n", v.as.num);
            break;
        default:
            printf("<unprintable value>\n");
            break;
    }
}

Value edu_read(void)
{
    char buffer[4096];
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return edu_nil();
    }

    size_t len = strlen(buffer);
    while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) {
        buffer[--len] = '\0';
    }

    char *endptr;
    double val = strtod(buffer, &endptr);
    if (len > 0 && endptr != buffer && *endptr == '\0') {
        return edu_num(val);
    }

    char *copy = malloc(len + 1);
    memcpy(copy, buffer, len + 1);
    return edu_str(copy);
}

void edu_write(Value v)
{
    switch (v.type) {
        case EDU_STR:
            printf("%s\n", v.as.str);
            break;
        case EDU_NUM:
            printf("%g\n", v.as.num);
            break;
        default:
            printf("<unprintable value>\n");
            break;
    }
}
