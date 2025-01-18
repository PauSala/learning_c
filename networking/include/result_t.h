#ifndef RESULT_T
#define RESULT_T

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
            char *err;         \
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

void free_result_char(ResultChar *result);
ResultChar result_char(EResult ty, ...);

void free_result_int(ResultInt *result);
ResultInt result_int(EResult ty, ...);

#endif
