#ifndef RESPONSE_T
#define RESPONSE_T

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

DEF_RESULT(char *, char)
DEF_RESULT(int, int)

void free_result_char(Resultchar *result);
Resultchar result_char(EResult ty, ...);

void free_result_int(Resultint *result);
Resultint result_int(EResult ty, ...);

#endif
