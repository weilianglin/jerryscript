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

#include "lit-literal-storage.h"

#include "ecma/base/ecma-helpers.h"

lit_literal_storage_t lit_storage;

rcs_record_t* lit_charset_record::get_prev () const
{
  rcs_record_iterator_t it ((rcs_recordset_t *)&lit_storage, (rcs_record_t *)this);
  it.skip (RCS_DYN_STORAGE_LENGTH_UNIT);

  cpointer_t cpointer;
  cpointer.packed_value = it.read<uint16_t> ();
  return cpointer_t::decompress (cpointer);
}

void lit_charset_record::set_prev (rcs_record_t *prev_rec_p)
{
  rcs_record_iterator_t it ((rcs_recordset_t *)&lit_storage, (rcs_record_t *)this);
  it.skip (RCS_DYN_STORAGE_LENGTH_UNIT);

  it.write<uint16_t> (cpointer_t::compress (prev_rec_p).packed_value);
}

void lit_charset_record::set_charset (const ecma_char_t *s, size_t size)
{
  JERRY_ASSERT (header_size () + size == get_size () - get_alignment_bytes_count ());

  rcs_record_iterator_t it ((rcs_recordset_t *)&lit_storage, (rcs_record_t *)this);
  it.skip (header_size ());
  // FIXME: fix further string processing when unicode will be supported
  ecma_length_t len = (ecma_length_t) (size / sizeof (ecma_char_t));
  for (size_t i = 0; i < len; ++i)
  {
    it.write<ecma_char_t> (s[i]);
    it.skip<ecma_char_t> ();
  }
}

int lit_charset_record::compare_zt (const ecma_char_t *s2, size_t n)
{
  size_t i;
  if (get_length () == 0)
  {
    if (s2 != nullptr)
    {
      return -1;
    }
    else
    {
      return 0;
    }
  }
  if (s2 == nullptr)
  {
    return 1;
  }
  rcs_record_iterator_t it (&lit_storage, this);
  it.skip (header_size ());
  for (i = 0; i < get_length () && i < n; i++)
  {
    ecma_char_t c = it.read<ecma_char_t> ();
    if (c > s2[i])
    {
      return 1;
    }
    else if (c < s2[i])
    {
      return -1;
    }
    it.skip<ecma_char_t> ();
  }

  if (i < n)
  {
    return -1;
  }

  return 0;
}

bool lit_charset_record::equal (lit_charset_record *rec)
{
  if (get_length () != rec->get_length ())
  {
    return false;
  }

  rcs_record_iterator_t it1 (&lit_storage, this);
  it1.skip (header_size ());
  rcs_record_iterator_t it2 (&lit_storage, rec);
  it2.skip (rec->header_size ());
  for (ecma_length_t i = 0; i < get_length (); i++)
  {
    if (it1.read<ecma_char_t> () != it2.read <ecma_char_t> ())
    {
      return false;
    }
    it1.skip<ecma_char_t> ();
    it2.skip<ecma_char_t> ();
  }
  return true;
}

bool lit_charset_record::equal_zt (const ecma_char_t *s)
{
  return equal_non_zt (s, ecma_zt_string_length (s));
}

bool lit_charset_record::equal_non_zt (const ecma_char_t *s, ecma_length_t len)
{
  rcs_record_iterator_t it1 (&lit_storage, this);
  it1.skip (header_size ());
  for (ecma_length_t i = 0; i < get_length () && i < len; i++)
  {
    if (it1.read<ecma_char_t> () != s[i])
    {
      return false;
    }
    it1.skip<ecma_char_t> ();
  }

  if (get_length () == len)
  {
    return true;
  }

  return false;
}

void lit_literal_storage_t::dump ()
{
  for (rcs_record_t *rec_p = lit_storage.get_first (); rec_p != nullptr; rec_p = lit_storage.get_next (rec_p))
  {
    printf ("[type=%d] ", rec_p->get_type ());
    printf ("[size=%d] ", get_record_size (rec_p));
    switch (rec_p->get_type ())
    {
      case LIT_STR:
      {
        lit_charset_record *lit_p = static_cast<lit_charset_record *> (rec_p);
        rcs_record_iterator_t it (this, rec_p);
        it.skip (lit_charset_record::header_size ());
        for (size_t i = 0; i < lit_p->get_length (); ++i)
        {
          printf ("%c", it.read<ecma_char_t> ());
          it.skip<ecma_char_t> ();
        }
        break;
      }
      case LIT_MAGIC_STR:
      {
        lit_magic_record *lit_p = static_cast<lit_magic_record *> (rec_p);
        printf ("[id=%d] ", lit_p->get_magic_str_id ());
        printf ("%s", ecma_get_magic_string_zt (lit_p->get_magic_str_id ()));
        break;
      }
      case LIT_NUMBER:
      {
        lit_number_record *lit_p = static_cast<lit_number_record *> (rec_p);
        ecma_char_t buff[ECMA_MAX_CHARS_IN_STRINGIFIED_NUMBER];
        ecma_number_to_zt_string (lit_p->get_number (), buff, ECMA_MAX_CHARS_IN_STRINGIFIED_NUMBER);
        printf ("%s", buff);
        break;
      }
      default:
      {
        /* empty */
      }
    }

    printf ("\n");
  }
}

rcs_record_t* lit_literal_storage_t::get_prev (rcs_record_t* rec_p)
{
  switch (rec_p->get_type ())
  {
    case LIT_STR:
    {
      return (static_cast<lit_charset_record *> (rec_p))->get_prev ();
    }
    case LIT_MAGIC_STR:
    {
      return (static_cast<lit_magic_record *> (rec_p))->get_prev ();
    }
    case LIT_NUMBER:
    {
      return (static_cast<lit_number_record *> (rec_p))->get_prev ();
    }
    default:
    {
      JERRY_ASSERT (rec_p->get_type () < _first_type_id);

      return rcs_recordset_t::get_prev (rec_p);
    }
  }
}

void lit_literal_storage_t::set_prev (rcs_record_t* rec_p,
                                      rcs_record_t *prev_rec_p)
{
  switch (rec_p->get_type ())
  {
    case LIT_STR:
    {
      return (static_cast<lit_charset_record *> (rec_p))->set_prev (prev_rec_p);
    }
    case LIT_MAGIC_STR:
    {
      return (static_cast<lit_magic_record *> (rec_p))->set_prev (prev_rec_p);
    }
    case LIT_NUMBER:
    {
      return (static_cast<lit_number_record *> (rec_p))->set_prev (prev_rec_p);
    }
    default:
    {
      JERRY_ASSERT (rec_p->get_type () < _first_type_id);

      return rcs_recordset_t::set_prev (rec_p, prev_rec_p);
    }
  }
}

size_t lit_literal_storage_t::get_record_size (rcs_record_t* rec_p)
{
  switch (rec_p->get_type ())
  {
    case LIT_STR:
    {
      return (static_cast<lit_charset_record*> (rec_p))->get_size ();
    }
    case LIT_MAGIC_STR:
    {
      return lit_magic_record::size ();
    }
    case LIT_NUMBER:
    {
      return lit_number_record::size ();
    }
    default:
    {
      JERRY_ASSERT (rec_p->get_type () < _first_type_id);

      return rcs_recordset_t::get_record_size (rec_p);
    }
  }
}

template void rcs_record_iterator_t::skip<ecma_char_t> ();
template void rcs_record_iterator_t::skip<uint16_t > ();
template void rcs_record_iterator_t::skip<uint32_t > ();

template void rcs_record_iterator_t::write<ecma_char_t> (ecma_char_t);
template ecma_char_t rcs_record_iterator_t::read<ecma_char_t> ();

template void rcs_record_iterator_t::write<ecma_number_t> (ecma_number_t);
template ecma_number_t rcs_record_iterator_t::read<ecma_number_t> ();

template void rcs_record_iterator_t::write<uint16_t> (uint16_t);
template uint16_t rcs_record_iterator_t::read<uint16_t> ();

template lit_charset_record *rcs_recordset_t::alloc_record<lit_charset_record> (rcs_record_t::type_t type,
                                                                                size_t size);
template lit_magic_record *rcs_recordset_t::alloc_record<lit_magic_record> (rcs_record_t::type_t type);
template lit_number_record *rcs_recordset_t::alloc_record<lit_number_record> (rcs_record_t::type_t type);
