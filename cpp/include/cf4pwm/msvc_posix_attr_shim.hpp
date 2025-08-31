#pragma once
#ifdef _MSC_VER
  // Provide ssize_t on MSVC (64-bit)
  #include <basetsd.h>
  #ifndef ssize_t
    typedef SSIZE_T ssize_t;
  #endif

  // Make GNU-style attributes no-ops on MSVC; guard to avoid redefinitions
  #ifndef __attribute__
    #define __attribute__(x)
  #endif
  #ifndef __packed
    #define __packed
  #endif
  #ifndef packed
    #define packed
  #endif
#endif
