
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "../include/logger.h"
#include "../include/response_t.h"

// * char
DEF_BUILD_RESULT(ResultChar, char *, char)
DEF_FREE_RESULT(free, ResultChar, char)

// int
DEF_BUILD_RESULT(ResultInt, int, int)
void free_int(int _) { (void)_; }
DEF_FREE_RESULT(free_int, ResultInt, int)
