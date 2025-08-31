// include/cf4pwm/msvc_posix_attr_shim.hpp
#pragma once

// ---- ssize_t ----
#ifndef _SSIZE_T_DEFINED
  using ssize_t = long long; // Windows/MSVC 沒有 ssize_t
  #define _SSIZE_T_DEFINED
#endif

// ---- GNU-style attributes -> no-ops on MSVC ----
#ifndef __attribute__
  #define __attribute__(x)
#endif

#ifndef __packed
  #define __packed
#endif

#ifndef __unused
  #define __unused
#endif

// 🔴 IMPORTANT: 不要在這裡動 #pragma pack！
// 一律讓預設 8-byte packing 保持不變（/Zp8）。
// 需要 1-byte 對齊的結構，請在各自的 header 以 push/pop 做局部處理。
