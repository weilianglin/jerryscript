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

#ifndef LIT_LITERAL_STORAGE_H
#define LIT_LITERAL_STORAGE_H

#include "ecma/base/ecma-globals.h"
#include "rcs/rcs-recordset.h"

class lit_literal_storage_t;
extern lit_literal_storage_t lit_storage;

/**
 * layout:
 * ------- header -----------------------
 * type (4 bits)
 * alignment (2 bits)
 * contains_long_characters_flag (1 bit)
 * unused (9 bits)
 * pointer to prev (16 bits)
 * ------- characters -------------------
 * ...
 * chars
 * ....
 * ------- alignment bytes --------------
 * unused bytes (their count is specified
 * by 'alignment' field in header)
 * --------------------------------------
 */
class lit_charset_record : public rcs_record_t
{
 public:
  static size_t size (size_t size)
  {
    return JERRY_ALIGNUP (size + header_size (), RCS_DYN_STORAGE_LENGTH_UNIT);
  }

  static size_t header_size ()
  {
    return _header_size;
  }

  size_t get_alignment_bytes_count () const
  {
    return get_field (_alignment_field_pos, _alignment_field_width);
  }

  void set_alignment_bytes_count (size_t count)
  {
    JERRY_ASSERT (count <= RCS_DYN_STORAGE_ALIGNMENT);
    set_field (_alignment_field_pos, _alignment_field_width, count);
  }

  bool contains_long_characters () const
  {
    return get_field (_has_special_characters_field_pos, _has_special_characters_field_width) != 0;
  }

  void set_contains_long_characters (bool has_long_characters)
  {
    set_field (_has_special_characters_field_pos, _has_special_characters_field_width, (size_t) has_long_characters);
  }

  size_t get_length () const
  {
    if (contains_long_characters ())
    {
      JERRY_UNIMPLEMENTED ("Support for unicode is not implemented yet.");
    }
    else
    {
      return (get_size () - header_size () - get_alignment_bytes_count ()) / sizeof (ecma_char_t);
    }
  }

  size_t get_size () const
  {
    return get_field (_length_field_pos, _length_field_width) * RCS_DYN_STORAGE_LENGTH_UNIT;
  }

  void set_size (size_t size)
  {
    JERRY_ASSERT (JERRY_ALIGNUP (size, RCS_DYN_STORAGE_ALIGNMENT) == size);

    set_field (_length_field_pos, _length_field_width, size >> RCS_DYN_STORAGE_ALIGNMENT_LOG);
  }

  rcs_record_t* get_prev () const;
  void set_prev (rcs_record_t* prev_rec_p);
  void set_charset (const ecma_char_t *s, size_t size);

  int compare_zt (const ecma_char_t *, size_t);
  bool equal (lit_charset_record *);
  bool equal_zt (const ecma_char_t *);
  bool equal_non_zt (const ecma_char_t *, ecma_length_t);

 private:
  static const uint32_t _alignment_field_pos = _fields_offset_begin;
  static const uint32_t _alignment_field_width = 2u;

  static const uint32_t _has_special_characters_field_pos = _alignment_field_pos + _alignment_field_width;
  static const uint32_t _has_special_characters_field_width = 1u;

  static const uint32_t _length_field_pos = (_has_special_characters_field_pos +
                                             _has_special_characters_field_width + 9u);
  static const uint32_t _length_field_width = 16u;

  static const uint32_t _prev_field_pos = _length_field_pos + _length_field_width;
  static const uint32_t _prev_field_width = rcs_cpointer_t::bit_field_width;

  static const size_t _header_size = RCS_DYN_STORAGE_LENGTH_UNIT + RCS_DYN_STORAGE_LENGTH_UNIT / 2;
};

/**
 * type (4 bits) | magic string id  (12 bits) | pointer to prev (half)
 *             2 | (ecma_magic_string_id_t)   |
 */
class lit_magic_record : public rcs_record_t
{
 public:
  static size_t size ()
  {
    return _size;
  }

  static size_t header_size ()
  {
    return _size;
  }

  size_t get_size () const
  {
    return _size;
  }

  void set_size (size_t size)
  {
    JERRY_ASSERT (size == get_size ());
  }

  rcs_record_t* get_prev () const
  {
    return get_pointer (prev_field_pos, prev_field_width);
  }

  void set_prev (rcs_record_t* prev_rec_p)
  {
    set_pointer (prev_field_pos, prev_field_width, prev_rec_p);
  }

  ecma_magic_string_id_t get_magic_str_id () const
  {
    uint32_t id = get_field (magic_field_pos, magic_field_width);
    JERRY_ASSERT (id < ECMA_MAGIC_STRING__COUNT);
    return (ecma_magic_string_id_t) id;
  }

  void set_magic_str_id (ecma_magic_string_id_t id)
  {
    set_field (magic_field_pos, magic_field_width, id);
  }

 private:
  static const uint32_t magic_field_pos = _fields_offset_begin;
  static const uint32_t magic_field_width = 12u;

  static const uint32_t prev_field_pos = magic_field_pos + magic_field_width;
  static const uint32_t prev_field_width = rcs_cpointer_t::bit_field_width;

  static const size_t _size = RCS_DYN_STORAGE_LENGTH_UNIT;
};

#define DEFINE_NUMBER_RECORD(type) \
  class lit_number_record : public rcs_record_t \
  { \
   public: \
    static size_t size () { return _size; } \
    static size_t header_size () { return _header_size; } \
    size_t get_size () const { return _size; } \
    void set_size (size_t size) { JERRY_ASSERT (size == get_size ()); } \
     \
    rcs_record_t* get_prev () const \
    { \
      return get_pointer (prev_field_pos, prev_field_width); \
    } \
    \
    void set_prev (rcs_record_t* prev_rec_p) \
    { \
      set_pointer (prev_field_pos, prev_field_width, prev_rec_p); \
    } \
    \
    type get_number () \
    { \
      rcs_record_iterator_t it ((rcs_recordset_t *)&lit_storage, \
                                (rcs_record_t *)this); \
      it.skip (header_size ()); \
      return it.read<type> (); \
    } \
   private: \
    static const uint32_t magic_field_pos = _fields_offset_begin; \
    static const uint32_t magic_field_width = 12u; \
    \
    static const uint32_t prev_field_pos = magic_field_pos + magic_field_width; \
    static const uint32_t prev_field_width = rcs_cpointer_t::bit_field_width; \
    \
    static const size_t _header_size = RCS_DYN_STORAGE_LENGTH_UNIT; \
    static const size_t _size = _header_size + sizeof (type); \
  }

DEFINE_NUMBER_RECORD (ecma_number_t);

class lit_literal_storage_t : public rcs_recordset_t
{
 public:
  enum
  {
    LIT_STR = _first_type_id,
    LIT_MAGIC_STR,
    LIT_NUMBER
  };


  lit_charset_record *
  create_charset_record (const ecma_char_t *s, size_t buf_size)
  {
    lit_charset_record *ret = alloc_record<lit_charset_record, size_t> (LIT_STR, buf_size);
    ret->set_alignment_bytes_count (lit_charset_record::size (buf_size) -
                                    (lit_charset_record::header_size () + buf_size));
    ret->set_contains_long_characters (false); // FIXME: fix when unicode will be supported
    ret->set_charset (s, buf_size);
    return ret;
  }

  lit_magic_record *
  create_magic_record (ecma_magic_string_id_t id)
  {
    lit_magic_record *ret = alloc_record<lit_magic_record> (LIT_MAGIC_STR);
    ret->set_magic_str_id (id);
    return ret;
  }

  lit_number_record *
  create_number_record (ecma_number_t num)
  {
    lit_number_record *ret = alloc_record<lit_number_record> (LIT_NUMBER);
    rcs_record_iterator_t it (this, ret);
    it.skip (ret->header_size ());
    it.write<ecma_number_t> (num);
    return ret;
  }

  void dump ();
 private:
  virtual rcs_record_t* get_prev (rcs_record_t* rec_p);
  virtual void set_prev (rcs_record_t* rec_p, rcs_record_t *prev_rec_p);
  virtual size_t get_record_size (rcs_record_t* rec_p);
};

#endif /* LIT_LITERAL_STORAGE_H */
