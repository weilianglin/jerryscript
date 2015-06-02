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

#ifndef LIT_LITERAL_H
#define LIT_LITERAL_H

#include "ecma-globals.h"
#include "lit-literal-storage.h"

#define LITERAL_TO_REWRITE (INVALID_VALUE - 1)

void lit_init ();
void lit_finalize ();
void lit_dump_literals ();

literal_t lit_create_literal_from_zt (const ecma_char_t *, ecma_length_t);
literal_t lit_create_literal_from_s (const char *s, ecma_length_t len);
literal_t lit_find_or_create_literal_from_s (const char *s, ecma_length_t len);
literal_t lit_create_literal_from_num (ecma_number_t);
literal_t lit_find_or_create_literal_from_num (ecma_number_t);

bool lit_literal_exists (literal_t lit);
literal_t lit_find_literal_by_zt (const ecma_char_t *s, ecma_length_t len);
literal_t lit_find_literal_by_s (const char *s, ecma_length_t len);
literal_t lit_find_literal_by_num (ecma_number_t);

bool lit_literal_equal (literal_t, literal_t);
bool lit_literal_equal_s (literal_t, const char *);
bool lit_literal_equal_zt (literal_t, const ecma_char_t *);
bool lit_literal_equal_non_zt (literal_t, const ecma_char_t *, ecma_length_t);
bool lit_literal_equal_num (literal_t, ecma_number_t);

bool lit_literal_equal_type (literal_t, literal_t);
bool lit_literal_equal_type_s (literal_t, const char *);
bool lit_literal_equal_type_zt (literal_t, const ecma_char_t *);
bool lit_literal_equal_type_non_zt (literal_t, const ecma_char_t *, ecma_length_t);
bool lit_literal_equal_type_num (literal_t, ecma_number_t);

const ecma_char_t *lit_literal_to_zt (literal_t lit, ecma_char_t *buff, size_t size);
const char *lit_literal_to_s_internal_buff (literal_t lit);

literal_t lit_get_literal_by_cp (lit_cpointer_t);

#endif /* LIT_LITERAL_H */
