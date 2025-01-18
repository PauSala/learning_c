
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "../include/logger.h"
#include "../include/response_t.h"

#define DEF_BUILD_RESULT(type, input_type, name)       \
    type result_##name(EResult ty, ...)                \
    {                                                  \
        type result;                                   \
        result.ty = ty;                                \
        va_list args;                                  \
        va_start(args, ty);                            \
                                                       \
        if (ty == Ok)                                  \
        {                                              \
            result.val.res = va_arg(args, input_type); \
        }                                              \
        else if (ty == Err)                            \
        {                                              \
            result.val.err = va_arg(args, char *);     \
        }                                              \
        va_end(args);                                  \
        return result;                                 \
    }

#define DEF_FREE_RESULT(free_func, input_type, name)   \
    void free_result_##name(input_type *result)        \
    {                                                  \
        if (!result)                                   \
            return;                                    \
                                                       \
        if (result->ty == Ok)                          \
        {                                              \
            logger("Freeing res", DEBUG);              \
            free_func(result->val.res);                \
        }                                              \
        else if (result->ty == Err && result->val.err) \
        {                                              \
            logger("Freeing err", DEBUG);              \
            free(result->val.err);                     \
        }                                              \
    }

// * char
DEF_BUILD_RESULT(Resultchar, char *, char)
DEF_FREE_RESULT(free, Resultchar, char)

// int
DEF_BUILD_RESULT(Resultint, int, int)
void free_int(int _) { (void)_; }
DEF_FREE_RESULT(free_int, Resultint, int)
