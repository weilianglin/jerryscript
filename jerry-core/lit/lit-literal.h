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

#include "ecma/base/ecma-globals.h"
#include "lit/lit-literal-storage.h"

typedef rcs_record_t * literal_t;

literal_t lit_create_literal_from_zt (const ecma_char_t *, ecma_length_t);
literal_t lit_create_literal_from_s (const char *s, ecma_length_t len);
literal_t lit_create_literal_from_num (ecma_number_t);

bool lit_find_literal_by_zt (const ecma_char_t *s, ecma_length_t len);
bool lit_find_literal_by_s (const char *s, ecma_length_t len);

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

#endif /* LIT_LITERAL_H */
