#include "util.h"

#include <cstdarg>
#include <cstdio>

void debug(char const *format, ...) {
#ifdef DEBUG_dizzYM
    fprintf(stderr, "[dizzYM] ");
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
    fprintf(stderr, "\n");
#endif
}
