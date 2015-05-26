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

#include "ecma/base/ecma-helpers.h"
#include "lit-literal-storage.h"
#include "rcs/rcs-recordset.h"

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

  return lit_storage.create_charset_record (s, len);
}

literal_t
lit_create_literal_from_s (const char *s, ecma_length_t len)
{
  return lit_create_literal_from_zt ((const ecma_char_t *) s, len);
}

literal_t lit_create_literal_from_num (ecma_number_t num)
{
  return lit_storage.create_number_record (num);
}

bool lit_find_literal_by_zt (const ecma_char_t *s, ecma_length_t len)
{

  for (literal_t lit = lit_storage.get_first (); lit != nullptr; lit = lit_storage.get_next (lit))
  {
    rcs_record_t::type_t type = lit->get_type ();
    if (type == lit_literal_storage_t::LIT_STR)
    {
      if (static_cast<lit_charset_record *>(lit)->get_length () != len)
      {
        continue;
      }

      if (!static_cast<lit_charset_record *>(lit)->compare_zt ((const ecma_char_t *) s, len))
      {
        return true;
      }
    }
    else if (type == lit_literal_storage_t::LIT_MAGIC_STR)
    {
      const char *str = (const char *) ecma_get_magic_string_zt (
                                         static_cast<lit_magic_record *>(lit)->get_magic_str_id ());
      if (strlen (str) != len)
      {
        continue;
      }
      if (!strncmp (str, (const char *)s, strlen (str)))
      {
        return true;
      }
    }
  }
  return false;
}

bool lit_find_literal_by_s (const char *s, ecma_length_t len)
{
  return lit_find_literal_by_zt ((const ecma_char_t *) s, len);
}

static bool
lit_literal_equal_charset_rec (literal_t lit, lit_charset_record *rec)
{
  JERRY_ASSERT (lit->get_type () == lit_literal_storage_t::LIT_STR);

  switch (lit->get_type ())
  {
    case lit_literal_storage_t::LIT_STR:
    {
      return static_cast<lit_charset_record *>(lit)->equal (rec);
    }
    case lit_literal_storage_t::LIT_MAGIC_STR:
    {
      return rec->equal_zt (ecma_get_magic_string_zt (static_cast<lit_magic_record *>(lit)->get_magic_str_id ()));
    }
    case lit_literal_storage_t::LIT_NUMBER:
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
  JERRY_ASSERT (lit->get_type () == lit_literal_storage_t::LIT_STR);

  switch (lit->get_type ())
  {
    case lit_literal_storage_t::LIT_STR:
    {
      return static_cast<lit_charset_record *>(lit)->equal_zt (s);
    }
    case lit_literal_storage_t::LIT_MAGIC_STR:
    {
      return ecma_compare_zt_strings (s,
               ecma_get_magic_string_zt (static_cast<lit_magic_record *>(lit)->get_magic_str_id ()));
    }
    case lit_literal_storage_t::LIT_NUMBER:
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

bool lit_literal_equal_non_zt (literal_t lit, const ecma_char_t *s, ecma_length_t len)
{
  JERRY_ASSERT (lit->get_type () == lit_literal_storage_t::LIT_STR);

  switch (lit->get_type ())
  {
    case lit_literal_storage_t::LIT_STR:
    {
      return static_cast<lit_charset_record *>(lit)->equal_non_zt (s, len);
    }
    case lit_literal_storage_t::LIT_MAGIC_STR:
    {
      return ecma_compare_non_zt_to_zt_string (s, len,
               ecma_get_magic_string_zt (static_cast<lit_magic_record *>(lit)->get_magic_str_id ()));
    }
    case lit_literal_storage_t::LIT_NUMBER:
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

bool lit_literal_equal (literal_t lit1, literal_t lit2)
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
  return lit_literal_equal_non_zt (lit, s, ecma_zt_string_length (s));
}

bool lit_literal_equal_type_non_zt (literal_t lit, const ecma_char_t *s, ecma_length_t len)
{
  if (lit->get_type () != lit_literal_storage_t::LIT_STR &&
      lit->get_type () != lit_literal_storage_t::LIT_MAGIC_STR)
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
