#pragma once
// Neutralize GNU-style attributes on MSVC
#ifndef __attribute__
#define __attribute__(...)
#endif
// Provide ssize_t via BaseTsd.h
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
// Enable math constants like M_SQRT1_2
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <cmath>
