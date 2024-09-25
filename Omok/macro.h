#pragma once

#define DEBUG_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            std::cerr << "Assertion failed: (" #condition ") in " << __FILE__ \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            std::abort(); \
        } \
    } while (false)


#define IS_CRASH(message) \
    DEBUG_ASSERT(0,message); \

#define BOARDSIZE 19
#define CELLSIZE 30
#define MARGIN 20
#define R 10