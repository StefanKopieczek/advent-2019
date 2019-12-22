#ifndef DEBUG_H
#define DEBUG_H 1

#ifdef DEBUG_ENABLE
#define debug(...) printf(__VA_ARGS__)
#else
#define debug(...)
#endif

#endif
