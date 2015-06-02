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

#include "lit-literal.h"

#include "ecma-helpers.h"

void
lit_init ()
{
  new (&lit_storage) lit_literal_storage_t ();
  lit_storage.init ();
}

void
lit_finalize ()
{
  lit_storage.cleanup ();
  lit_storage.finalize ();
}

void
lit_dump_literals ()
{
  lit_storage.dump ();
}

/**
 * Create literal in literal storage.
 * Doesn't check if the same literal already exists.
 */
literal_t
lit_create_literal_from_zt (const ecma_char_t *s, ecma_length_t len)
{
  for (ecma_magic_string_id_t msi = (ecma_magic_string_id_t) 0;
       msi < ECMA_MAGIC_STRING__COUNT;
       msi = (ecma_magic_string_id_t) (msi + 1))
  {
    if (ecma_zt_string_length (ecma_get_magic_string_zt (msi)) != len)
    {
      continue;
    }
    if (!strncmp ((const char *) s, (const char *) ecma_get_magic_string_zt (msi), len))
    {
      return lit_storage.create_magic_record (msi);
    }
  }

  return lit_storage.create_charset_record (s, len * sizeof (ecma_char_t));
}

literal_t
lit_create_literal_from_s (const char *s, ecma_length_t len)
{
  return lit_create_literal_from_zt ((const ecma_char_t *) s, len);
}

literal_t
lit_find_or_create_literal_from_s (const char *s, ecma_length_t len)
{
  literal_t lit;
  lit = lit_find_literal_by_s (s, len);
  if (lit == NULL)
  {
    lit = lit_create_literal_from_s (s, len);
  }
  return lit;
}

literal_t
lit_create_literal_from_num (ecma_number_t num)
{
  return lit_storage.create_number_record (num);
}

literal_t
lit_find_or_create_literal_from_num (ecma_number_t num)
{
  literal_t lit;
  lit = lit_find_literal_by_num (num);
  if (lit == NULL)
  {
    lit = lit_create_literal_from_num (num);
  }
  return lit;
}

bool
lit_literal_exists (literal_t lit)
{
  for (literal_t l = lit_storage.get_first (); l != nullptr; l = lit_storage.get_next (l))
  {
    if (l == lit)
    {
      return true;
    }
  }

  return false;
}

literal_t
lit_find_literal_by_zt (const ecma_char_t *s, ecma_length_t len)
{

  for (literal_t lit = lit_storage.get_first (); lit != nullptr; lit = lit_storage.get_next (lit))
  {
    rcs_record_t::type_t type = lit->get_type ();
    if (type == LIT_STR_T)
    {
      if (static_cast<lit_charset_record *>(lit)->get_length () != len)
      {
        continue;
      }

      if (!static_cast<lit_charset_record *>(lit)->compare_zt ((const ecma_char_t *) s, len))
      {
        return lit;
      }
    }
    else if (type == LIT_MAGIC_STR_T)
    {
      const char *str = (const char *) ecma_get_magic_string_zt (
                                         static_cast<lit_magic_record *>(lit)->get_magic_str_id ());
      if (strlen (str) != len)
      {
        continue;
      }
      if (!strncmp (str, (const char *)s, strlen (str)))
      {
        return lit;
      }
    }
  }
  return NULL;
}

literal_t
lit_find_literal_by_s (const char *s, ecma_length_t len)
{
  return lit_find_literal_by_zt ((const ecma_char_t *) s, len);
}

literal_t
lit_find_literal_by_num (ecma_number_t num)
{
  for (literal_t lit = lit_storage.get_first (); lit != nullptr; lit = lit_storage.get_next (lit))
  {
    rcs_record_t::type_t type = lit->get_type ();
    if (type != LIT_NUMBER_T)
    {
      continue;
    }

    ecma_number_t lit_num = static_cast<lit_number_record *>(lit)->get_number ();
    if (lit_num == num)
    {
      return lit;
    }
  }
  return NULL;
}

static bool
lit_literal_equal_charset_rec (literal_t lit, lit_charset_record *rec)
{
  switch (lit->get_type ())
  {
    case LIT_STR_T:
    {
      return static_cast<lit_charset_record *>(lit)->equal (rec);
    }
    case LIT_MAGIC_STR_T:
    {
      return rec->equal_zt (ecma_get_magic_string_zt (static_cast<lit_magic_record *>(lit)->get_magic_str_id ()));
    }
    case LIT_NUMBER_T:
    {
      ecma_char_t buff[ECMA_MAX_CHARS_IN_STRINGIFIED_NUMBER];
      ecma_number_to_zt_string (static_cast<lit_number_record *>(lit)->get_number (), buff,
                                ECMA_MAX_CHARS_IN_STRINGIFIED_NUMBER);
      return rec->equal_zt (buff);
    }
    default:
    {
      JERRY_UNREACHABLE ();
    }
  }
}

bool
lit_literal_equal_s (literal_t lit, const char *s)
{
  return lit_literal_equal_zt (lit, (const ecma_char_t *) s);
}

bool
lit_literal_equal_zt (literal_t lit, const ecma_char_t *s)
{
  switch (lit->get_type ())
  {
    case LIT_STR_T:
    {
      return static_cast<lit_charset_record *>(lit)->equal_zt (s);
    }
    case LIT_MAGIC_STR_T:
    {
      return ecma_compare_zt_strings (s,
               ecma_get_magic_string_zt (static_cast<lit_magic_record *>(lit)->get_magic_str_id ()));
    }
    case LIT_NUMBER_T:
    {
      ecma_char_t buff[ECMA_MAX_CHARS_IN_STRINGIFIED_NUMBER];
      ecma_number_to_zt_string (static_cast<lit_number_record *>(lit)->get_number (), buff,
                                ECMA_MAX_CHARS_IN_STRINGIFIED_NUMBER);
      return ecma_compare_zt_strings (s, buff);
    }
    default:
    {
      JERRY_UNREACHABLE ();
    }
  }
}

bool
lit_literal_equal_non_zt (literal_t lit, const ecma_char_t *s, ecma_length_t len)
{
  switch (lit->get_type ())
  {
    case LIT_STR_T:
    {
      return static_cast<lit_charset_record *>(lit)->equal_non_zt (s, len);
    }
    case LIT_MAGIC_STR_T:
    {
      return ecma_compare_non_zt_to_zt_string (s, len,
               ecma_get_magic_string_zt (static_cast<lit_magic_record *>(lit)->get_magic_str_id ()));
    }
    case LIT_NUMBER_T:
    {
      ecma_char_t buff[ECMA_MAX_CHARS_IN_STRINGIFIED_NUMBER];
      ecma_number_to_zt_string (static_cast<lit_number_record *>(lit)->get_number (), buff,
                                ECMA_MAX_CHARS_IN_STRINGIFIED_NUMBER);
      return ecma_compare_non_zt_to_zt_string (s, len, buff);
    }
    default:
    {
      JERRY_UNREACHABLE ();
    }
  }
}

bool
lit_literal_equal_num (literal_t lit, ecma_number_t num)
{
  ecma_char_t buff[ECMA_MAX_CHARS_IN_STRINGIFIED_NUMBER];
  ecma_number_to_zt_string (num, buff, ECMA_MAX_CHARS_IN_STRINGIFIED_NUMBER);
  return lit_literal_equal_zt (lit, buff);
}

bool
lit_literal_equal (literal_t lit1, literal_t lit2)
{
  switch (lit2->get_type ())
  {
    case lit_literal_storage_t::LIT_STR:
    {
      return lit_literal_equal_charset_rec (lit1, static_cast<lit_charset_record *>(lit2));
    }
    case lit_literal_storage_t::LIT_MAGIC_STR:
    {
      return lit_literal_equal_zt (lit1, ecma_get_magic_string_zt (
                                           static_cast<lit_magic_record *>(lit2)->get_magic_str_id ()));
    }
    case lit_literal_storage_t::LIT_NUMBER:
    {
      return lit_literal_equal_num (lit1, static_cast<lit_number_record *>(lit2)->get_number ());
    }
    default:
    {
      JERRY_UNREACHABLE ();
    }
  }
}

bool
lit_literal_equal_type (literal_t lit1, literal_t lit2)
{
  if (lit1->get_type () != lit2->get_type ())
  {
    return false;
  }
  return lit_literal_equal (lit1, lit2);
}

bool
lit_literal_equal_type_s (literal_t lit, const char *s)
{
  return lit_literal_equal_type_zt (lit, (const ecma_char_t *) s);
}

bool
lit_literal_equal_type_zt (literal_t lit, const ecma_char_t *s)
{
  if (lit->get_type () != LIT_STR_T &&
      lit->get_type () != LIT_MAGIC_STR_T)
  {
    return false;
  }
  return lit_literal_equal_zt (lit, s);
}

bool lit_literal_equal_type_non_zt (literal_t lit, const ecma_char_t *s, ecma_length_t len)
{
  if (lit->get_type () != LIT_STR_T &&
      lit->get_type () != LIT_MAGIC_STR_T)
  {
    return false;
  }
  return lit_literal_equal_non_zt (lit, s, len);
}

bool
lit_literal_equal_type_num (literal_t lit, ecma_number_t num)
{
  if (lit->get_type () != lit_literal_storage_t::LIT_NUMBER)
  {
    return false;
  }
  return lit_literal_equal_num (lit, num);
}

const ecma_char_t *
lit_literal_to_zt (literal_t lit, ecma_char_t *buff, size_t size)
{
  rcs_record_t::type_t type = lit->get_type ();
  JERRY_ASSERT (type == LIT_STR_T || type == LIT_MAGIC_STR_T);

  switch (type)
  {
    case LIT_STR_T:
    {
      static_cast<lit_charset_record *> (lit)->get_charset (buff, size);
      return buff;
    }
    case LIT_MAGIC_STR_T:
    {
      return ecma_get_magic_string_zt (static_cast<lit_magic_record *> (lit)->get_magic_str_id ());
    }
    case LIT_NUMBER_T:
    {
      ecma_number_to_zt_string (static_cast<lit_number_record *> (lit)->get_number (), buff, (ssize_t) size);
      return buff;
    }
    default: JERRY_UNREACHABLE ();
  };
}

const char *lit_literal_to_s_internal_buff (literal_t lit)
{
  static ecma_char_t buff[ECMA_MAX_CHARS_IN_STRINGIFIED_NUMBER];
  return (const char *) lit_literal_to_zt (lit, buff, ECMA_MAX_CHARS_IN_STRINGIFIED_NUMBER);
}


literal_t
lit_get_literal_by_cp (lit_cpointer_t lit_cp)
{
  JERRY_ASSERT (lit_cp.packed_value != MEM_CP_NULL);
  literal_t lit = lit_cpointer_t::decompress (lit_cp);
  JERRY_ASSERT (lit_literal_exists (lit));
  return lit;
}
