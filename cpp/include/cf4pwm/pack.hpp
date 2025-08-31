// include/cf4pwm/pack.hpp
#pragma once

// 目的：只對「個別」結構做 1-byte 對齊，而不影響全域 packing。
// 用法：
//   CF_PACKED_STRUCT(
//     struct MyPacket {
//       uint8_t a;
//       uint16_t b;
//     };
//   );
//
// 注意：不要把這個巨集套在含有 virtual 成員或需要自然對齊的型別上。

#if defined(_MSC_VER)

// MSVC：用 __pragma 做局部 push/pop
#define CF_PACKED_STRUCT(definition) \
  __pragma(pack(push, 1)) definition __pragma(pack(pop))

// 也可加入欄位層級的 packed 標註（留白即可）
#define CF_PACKED /* no-op for MSVC */

#elif defined(__GNUC__) || defined(__clang__)

// GCC/Clang：用 __attribute__((packed))
#define CF_PACKED_STRUCT(definition) \
  definition __attribute__((packed))

#define CF_PACKED __attribute__((packed))

#else

// 其他編譯器：退化為原樣（至少不破壞編譯）
#warning "Unknown compiler: CF_PACKED_STRUCT falls back without packing."
#define CF_PACKED_STRUCT(definition) definition
#define CF_PACKED /* no-op */

#endif
