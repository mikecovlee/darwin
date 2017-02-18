#pragma once
#if defined(__WIN32__) || defined(WIN32)
#include "./darwin_win32.hpp"
#else
#include "./darwin_unix.hpp"
#endif