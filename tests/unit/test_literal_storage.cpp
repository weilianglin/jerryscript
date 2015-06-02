/* Copyright 2015 Samsung Electronics Co., Ltd.
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

#include <assert.h>

#include "ecma/base/ecma-helpers.h"
#include "jrt/jrt.h"
#include "mem-allocator.h"
#include "lit/lit-literal.h"

extern "C"
{
  extern void srand (unsigned int __seed);
  extern int rand (void);
  extern long int time (long int *__timer);
  extern int printf (__const char *__restrict __format, ...);
  extern void *memset (void *__s, int __c, size_t __n);
}

// Heap size is 32K
#define test_heap_size (32 * 1024)

int
main (int __attr_unused___ argc,
      char __attr_unused___ **argv)
{
  mem_init ();

  const char *strings[] =
  {
    "String1.",
    "String1.",
    "String3.",
    "MAX_VALUE",
    "throw",
    "abcdefghijklmnopqrstuvwxyz abcdefghijklmnopqrstuvwxyz abcdefghijklmnopqrstuvwxyz abcdefghijklmnopqrstuvwxyz"
    "abcdefghijklmnopqrstuvwxyz abcdefghijklmnopqrstuvwxyz abcdefghijklmnopqrstuvwxyz abcdefghijklmnopqrstuvwxyz"
    "abcdefghijklmnopqrstuvwxyz abcdefghijklmnopqrstuvwxyz abcdefghijklmnopqrstuvwxyz abcdefghijklmnopqrstuvwxyz"
    "abcdefghijklmnopqrstuvwxyz abcdefghijklmnopqrstuvwxyz abcdefghijklmnopqrstuvwxyz abcdefghijklmnopqrstuvwxyz"
    "abcdefghijklmnopqrstuvwxyz abcdefghijklmnopqrstuvwxyz abcdefghijklmnopqrstuvwxyz abcdefghijklmnopqrstuvwxyz",
    "setUTCSeconds",
  };

  lit_init ();

  lit_create_literal_from_num (3.15);
  for (size_t i = 0; i < sizeof (strings) / sizeof (const char *); ++i)
  {
    lit_create_literal_from_s (strings[i], (ecma_length_t) strlen (strings[i]));
  }
  lit_create_literal_from_num (32342.3444);

  for (ecma_magic_string_id_t msi = (ecma_magic_string_id_t) 0;
       msi < ECMA_MAGIC_STRING__COUNT;
       msi = (ecma_magic_string_id_t) (msi + 1))
  {
    lit_create_literal_from_zt (ecma_get_magic_string_zt (msi),
                                (ecma_length_t) ecma_zt_string_length (ecma_get_magic_string_zt (msi)));
  }

  for (size_t i = 0; i < sizeof (strings) / sizeof (const char *); ++i)
  {
    assert (lit_find_literal_by_s (strings[i], (ecma_length_t) strlen (strings[i])));
  }

  for (ecma_magic_string_id_t msi = (ecma_magic_string_id_t) 0;
       msi < ECMA_MAGIC_STRING__COUNT;
       msi = (ecma_magic_string_id_t) (msi + 1))
  {
    assert (lit_find_literal_by_zt (ecma_get_magic_string_zt (msi),
                                    (ecma_length_t) ecma_zt_string_length (ecma_get_magic_string_zt (msi))));
  }

  // Test compare
  const char *s = "abcde";
  literal_t l1 = lit_create_literal_from_s (s, (ecma_length_t) strlen (s));
  assert (lit_literal_equal_s (l1, s));

  lit_finalize ();
  mem_finalize (true);
  return 0;
}
