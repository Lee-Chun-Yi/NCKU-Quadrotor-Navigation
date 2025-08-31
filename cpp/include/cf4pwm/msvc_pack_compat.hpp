#pragma once
// Only for our own packed structs if needed; does NOT try to redefine upstream behavior.
#if defined(_MSC_VER)
  #define CF_PACK_PUSH_1  __pragma(pack(push, 1))
  #define CF_PACK_POP     __pragma(pack(pop))
  #define CF_PACKED       /* MSVC uses pragma pack for layout */
#else
  #define CF_PACK_PUSH_1  _Pragma("pack(push, 1)")
  #define CF_PACK_POP     _Pragma("pack(pop)")
  #define CF_PACKED       __attribute__((packed))
#endif
