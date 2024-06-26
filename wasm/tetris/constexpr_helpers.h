#pragma once

#include <cstdint>
#include <memory>
#include <type_traits>
#include <immintrin.h>

#ifdef _MSC_VER
#define NOINLINE __declspec(noinline)
#else
#define NOINLINE __attribute__((noinline))
#endif

template <class T>
constexpr T pext(T a, T mask) {
  static_assert(
      std::is_same<T, uint32_t>::value ||
      std::is_same<T, uint64_t>::value,
      "not implemented");
#ifdef __BMI2__
  if (std::is_constant_evaluated()) {
#endif
    T res = 0;
    for (T bb = 1; mask != 0; bb <<= 1) {
      if (a & mask & -mask) res |= bb;
      mask &= (mask - 1);
    }
    return res;
#ifdef __BMI2__
  } else if constexpr(std::is_same<T, uint64_t>::value) {
    return _pext_u64(a, mask);
  } else {
    return _pext_u32(a, mask);
  }
#endif
}

template <class T>
constexpr T pdep(T a, T mask) {
  static_assert(
      std::is_same<T, uint32_t>::value ||
      std::is_same<T, uint64_t>::value,
      "not implemented");
#ifdef __BMI2__
  if (std::is_constant_evaluated()) {
#endif
    T res = 0;
    for (T bb = 1; mask; bb <<= 1) {
      if (a & bb) res |= mask & -mask;
      mask &= mask - 1;
    }
    return res;
#ifdef __BMI2__
  } else if constexpr(std::is_same<T, uint64_t>::value) {
    return _pdep_u64(a, mask);
  } else {
    return _pdep_u32(a, mask);
  }
#endif
}

template <class T>
constexpr int popcount(T a) {
  static_assert(
      std::is_same<T, uint32_t>::value ||
      std::is_same<T, uint64_t>::value,
      "not implemented");
#ifdef _MSC_VER
  return std::popcount(a);
#else
  return __builtin_popcountll(a);
#endif
}

template <class T>
constexpr int ctz(T a) {
  static_assert(
      std::is_same<T, uint32_t>::value ||
      std::is_same<T, uint64_t>::value,
      "not implemented");
#ifdef _MSC_VER
  return std::countr_zero(a);
#else
  if constexpr(std::is_same<T, uint64_t>::value) {
    return __builtin_ctzll(a);
  } else {
    return __builtin_ctz(a);
  }
#endif
}

template <class T>
constexpr int clz(T a) {
  static_assert(
      std::is_same<T, uint32_t>::value ||
      std::is_same<T, uint64_t>::value,
      "not implemented");
#ifdef _MSC_VER
  return std::countl_zero(a);
#else
  if constexpr(std::is_same<T, uint64_t>::value) {
    return __builtin_clzll(a);
  } else {
    return __builtin_clz(a);
  }
#endif
}

[[noreturn]] inline void unreachable() {
#ifdef __GNUC__ // GCC, Clang, ICC
  __builtin_unreachable();
#else
#ifdef _MSC_VER // MSVC
  __assume(false);
#endif
#endif
}

#define DO_PIECE_CASE(piece) \
  switch (piece) { \
    ONE_CASE(0) ONE_CASE(1) ONE_CASE(2) ONE_CASE(3) ONE_CASE(4) ONE_CASE(5) ONE_CASE(6) \
  } \
  unreachable();
