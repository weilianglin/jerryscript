/* Copyright 2014 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef JERRY_GLOBALS_H
#define JERRY_GLOBALS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * Types
 */
typedef unsigned long mword_t;
typedef signed long ssize_t;

/**
 * Attributes
 */
#define __unused __attribute__((unused))
#define __packed __attribute__((packed))
#define __noreturn __attribute__((noreturn))
#define __noinline __attribute__((noinline))

/**
 * Constants
 */
#define JERRY_BITSINBYTE 8u

/**
 * Error codes
 */
typedef enum
{
  ERR_OK = 0,
  ERR_IO = -1,
  ERR_BUFFER_SIZE = -2,
  ERR_SEVERAL_FILES = -3,
  ERR_NO_FILES = -4,
  ERR_NON_CHAR = -5,
  ERR_UNCLOSED = -6,
  ERR_INT_LITERAL = -7,
  ERR_STRING = -8,
  ERR_PARSER = -9,
  ERR_MEMORY = -10,
  ERR_SYSCALL = -11,
  ERR_GENERAL = -255
} jerry_status_t;

/**
 * Asserts
 *
 * Warning:
 *         Don't use JERRY_STATIC_ASSERT in headers, because
 *         __LINE__ may be the same for asserts in a header
 *         and in an implementation file.
 */
#define JERRY_STATIC_ASSERT_GLUE_(a, b) a ## b
#define JERRY_STATIC_ASSERT_GLUE(a, b) JERRY_STATIC_ASSERT_GLUE_ (a, b)
#define JERRY_STATIC_ASSERT(x) \
  typedef char JERRY_STATIC_ASSERT_GLUE (static_assertion_failed_, __LINE__) \
  [ (x) ? 1 : -1 ] __unused

#define CALL_PRAGMA(x) _Pragma (#x)

#ifdef JERRY_PRINT_TODO
#define TODO(x) CALL_PRAGMA (message ("TODO - " #x))
#else /* !JERRY_PRINT_TODO */
#define TODO(X)
#endif /* !JERRY_PRINT_TODO */

#ifdef JERRY_PRINT_FIXME
#define FIXME(x) CALL_PRAGMA (message ("FIXME - " #x))
#else /* !JERRY_PRINT_FIXME */
#define FIXME(X)
#endif /* !JERRY_PRINT_FIXME */

/**
 * Variable that must not be referenced.
 *
 * May be used for static assertion checks.
 */
extern uint32_t jerry_unreferenced_expression;

extern void __noreturn jerry_assert_fail (const char *assertion, const char *file, const uint32_t line);

#ifndef JERRY_NDEBUG
#define JERRY_ASSERT(x) do { if (__builtin_expect (!(x), 0)) { jerry_assert_fail (#x, __FILE__, __LINE__); } } while (0)
#else /* !JERRY_NDEBUG */
#define JERRY_ASSERT(x) (void) (x)
#endif /* !JERRY_NDEBUG */

/**
 * Mark for unreachable points and unimplemented cases
 */
extern void jerry_ref_unused_variables (int unused_variables_follow, ...);
#define JERRY_UNREACHABLE() \
  do \
  { \
    JERRY_ASSERT (false); \
    jerry_exit (ERR_GENERAL); \
  } while (0)
#define JERRY_UNIMPLEMENTED() JERRY_UNREACHABLE ()
#define JERRY_UNIMPLEMENTED_REF_UNUSED_VARS(...) \
  do \
  { \
    JERRY_UNIMPLEMENTED (); \
    if (false) \
    { \
      jerry_ref_unused_variables (0, __VA_ARGS__); \
    } \
  } while (0)

/**
 * Conditions' likeliness, unlikeliness.
 */
#define likely(x) __builtin_expect (!!(x), 1)
#define unlikely(x) __builtin_expect (!!(x) , 0)

/**
 * Exit
 */
extern void __noreturn jerry_exit (jerry_status_t code);

/**
 * sizeof, offsetof, ...
 */
#define JERRY_SIZE_OF_STRUCT_MEMBER(struct_name, member_name) sizeof (((struct_name*)NULL)->member_name)

/**
 * Alignment
 */

/**
 * Aligns @value to @alignment.
 *
 * Returns maximum positive value, that divides @alignment and is less than or equal to @value
 */
#define JERRY_ALIGNDOWN(value, alignment) ((alignment) * ((value) / (alignment)))

/**
 * Aligns @value to @alignment.
 *
 * Returns minimum positive value, that divides @alignment and is more than or equal to @value
 */
#define JERRY_ALIGNUP(value, alignment) ((alignment) * (((value) + (alignment) - 1) / (alignment)))

/**
 * min, max
 */
#define JERRY_MIN(v1, v2) ((v1 < v2) ? v1 : v2)
#define JERRY_MAX(v1, v2) ((v1 < v2) ? v2 : v1)

/**
 * Bit-fields
 */
inline uint32_t jerry_extract_bit_field (uint32_t value, uint32_t lsb,
                                         uint32_t width);
inline uint32_t jerry_set_bit_field_value (uint32_t value, uint32_t bit_field_value,
                                           uint32_t lsb, uint32_t width);

/**
 * Extract a bit-field from the integer.
 *
 * @return bit-field's value
 */
inline uint32_t
jerry_extract_bit_field (uint32_t
                         container, /**< container to extract bit-field from */
                         uint32_t lsb, /**< least significant bit of the value
                                     *   to be extracted */
                         uint32_t width) /**< width of the bit-field to be extracted */
{
  JERRY_ASSERT (lsb < JERRY_BITSINBYTE * sizeof (uint32_t));
  JERRY_ASSERT ((lsb + width) <= JERRY_BITSINBYTE * sizeof (uint32_t));

  uint32_t shifted_value = container >> lsb;
  uint32_t bit_field_mask = (1u << width) - 1;

  return (shifted_value & bit_field_mask);
} /* jerry_extract_bit_field */

/**
 * Extract a bit-field from the integer.
 *
 * @return bit-field's value
 */
inline uint32_t
jerry_set_bit_field_value (uint32_t
                           container, /**< container to insert bit-field to */
                           uint32_t new_bit_field_value, /**< value of bit-field to insert */
                           uint32_t lsb, /**< least significant bit of the value
                                          *   to be extracted */
                           uint32_t width) /**< width of the bit-field to be extracted */
{
  JERRY_ASSERT (lsb < JERRY_BITSINBYTE * sizeof (uint32_t));
  JERRY_ASSERT ((lsb + width) <= JERRY_BITSINBYTE * sizeof (uint32_t));
  JERRY_ASSERT (new_bit_field_value <= (1u << width));

  uint32_t bit_field_mask = (1u << width) - 1;
  uint32_t shifted_bit_field_mask = bit_field_mask << lsb;
  uint32_t shifted_new_bit_field_value = new_bit_field_value << lsb;

  return (container & ~shifted_bit_field_mask) | shifted_new_bit_field_value;
} /* jerry_set_bit_field_value */

#endif /* !JERRY_GLOBALS_H */
