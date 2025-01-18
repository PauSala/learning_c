#ifndef RESPONSE_T
#define RESPONSE_T

#include "errors.h"

// Enum to specify the type of response
typedef enum
{
    Ok,
    Err,
} EResult;

// Macro to create a Result with a specific type for res
#define DEF_RESULT(type, name) \
    typedef struct             \
    {                          \
        EResult ty;            \
        union                  \
        {                      \
            type res;          \
            ServerError err;   \
        } val;                 \
    } Result##name;

DEF_RESULT(char *, Char)
DEF_RESULT(int, Int)

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
            result.val.err = va_arg(args, int);        \
        }                                              \
        va_end(args);                                  \
        return result;                                 \
    }

#define DEF_FREE_RESULT(free_func, input_type, name) \
    void free_result_##name(input_type *result)      \
    {                                                \
        const char *env_var = "DEBUG_C_SERVER";      \
        char *value = getenv(env_var);               \
        if (!result)                                 \
            return;                                  \
                                                     \
        if (result->ty == Ok)                        \
        {                                            \
            if (value != NULL)                       \
            {                                        \
                logger("Freeing result", DEBUG);     \
            }                                        \
            free_func(result->val.res);              \
        }                                            \
    }

void free_result_char(ResultChar *result);
ResultChar result_char(EResult ty, ...);

void free_result_int(ResultInt *result);
ResultInt result_int(EResult ty, ...);

#endif
