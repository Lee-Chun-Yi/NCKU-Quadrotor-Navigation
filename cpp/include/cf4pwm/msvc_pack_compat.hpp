#pragma once
#ifdef _MSC_VER
  // Swallow GCC-style attributes on MSVC
  #ifndef __attribute__
  #define __attribute__(x)
  #endif
  // Match GCC packed layout during inclusion of upstream headers
  #pragma pack(push, 1)
#endif
