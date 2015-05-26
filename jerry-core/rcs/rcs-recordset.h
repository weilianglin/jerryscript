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

#ifndef RCS_RECORDSET_H
#define RCS_RECORDSET_H

#include <string.h>
#include "jrt.h"
#include "jrt-bit-fields.h"
#include "mem-allocator.h"
#include "rcs-chunked-list.h"

/** \addtogroup recordset Recordset
 * @{
 *
 * Non-contiguous container abstraction with iterator.
 *
 * @{
 */

/**
 * Logarithm of a dynamic storage unit alignment
 */
#define RCS_DYN_STORAGE_ALIGNMENT_LOG (2u)

/**
 * Dynamic storage unit alignment
 */
#define RCS_DYN_STORAGE_ALIGNMENT     (1ull << RCS_DYN_STORAGE_ALIGNMENT_LOG)

/**
 * Unit of length
 *
 * See also:
 *          rcs_dyn_storage_length_t
 */
#define RCS_DYN_STORAGE_LENGTH_UNIT   (4u)

/**
 * Dynamic storage
 *
 * Note:
 *      Static C++ constructors / desctructors are supposed to be not supported.
 *      So, initialization / destruction is implemented through init / finalize
 *      static functions.
 */
class rcs_recordset_t
{
 public:
  /* Constructor */
  void init (void)
  {
    _chunk_list.init ();

    JERRY_ASSERT (_chunk_list.get_data_space_size () % RCS_DYN_STORAGE_LENGTH_UNIT == 0);
  } /* init */

  /* Destructor */
  void finalize (void)
  {
    _chunk_list.free ();
  } /* finalize */

  /**
   * Record type
   */
  class record_t
  {
   public:
    typedef uint8_t type_t;

    type_t get_type (void) const;
    void set_type (type_t type);

    /**
     * Dynamic storage-specific extended compressed pointer
     *
     * Note:
     *      the pointer can represent addresses aligned by RCS_DYN_STORAGE_ALIGNMENT,
     *      while mem_cpointer_t can only represent addressed aligned by MEM_ALIGNMENT.
     */
    struct cpointer_t
    {
      static const uint32_t bit_field_width = MEM_CP_WIDTH + MEM_ALIGNMENT_LOG - RCS_DYN_STORAGE_ALIGNMENT_LOG;

      union
      {
        struct
        {
          mem_cpointer_t base_cp : MEM_CP_WIDTH; /**< pointer to base of addressed area */
#if MEM_ALIGNMENT_LOG > RCS_DYN_STORAGE_ALIGNMENT_LOG
          uint16_t ext     : (MEM_ALIGNMENT_LOG - RCS_DYN_STORAGE_ALIGNMENT_LOG); /**< extension of the basic
                                                                                   *   compressed pointer
                                                                                   *   used for more detailed
                                                                                   *   addressing */
#endif /* MEM_ALIGNMENT_LOG > RCS_DYN_STORAGE_ALIGNMENT_LOG */
        } value;
        uint16_t packed_value;
      };

      static cpointer_t compress (record_t* pointer_p);
      static record_t* decompress (cpointer_t pointer_cp);
    };

   private:
    /**
     * Offset of 'type' field, in bits
     */
    static const uint32_t _type_field_pos   = 0u;

    /**
     * Width of 'type' field, in bits
     */
    static const uint32_t _type_field_width = 4u;

   protected:
    void check_this (void) const;

    uint32_t get_field (uint32_t field_pos, uint32_t field_width) const;
    void set_field (uint32_t field_pos, uint32_t field_width, size_t value);

    record_t* get_pointer (uint32_t field_pos, uint32_t field_width) const;
    void set_pointer (uint32_t field_pos, uint32_t field_width, record_t* pointer_p);

    /**
     * Offset of a derived record's fields, in bits
     */
    static const uint32_t _fields_offset_begin = _type_field_pos + _type_field_width;
  };

 private:
  friend class rcs_record_iterator_t;

  /**
   * Type identifier for free record
   */
  static const record_t::type_t _free_record_type_id = 0;

  /**
   * Chunked list used for memory allocation
   */
  rcs_chunked_list_t _chunk_list;

  void alloc_record_in_place (record_t* place_p,
                              size_t free_size,
                              record_t* next_record_p);

  void init_free_record (record_t *free_rec_p, size_t size, record_t *prev_rec_p);
  bool is_record_free (record_t *record_p);
 protected:
  /**
   * First type identifier that can be used for storage-specific record types
   */
  static const record_t::type_t _first_type_id = _free_record_type_id + 1;

  /**
   * Allocate new record of specified type
   *
   * @return pointer to the new record
   */
  template<
    typename T, /**< type of record structure */
    typename ... SizeArgs> /**< type of arguments of T::size */
  T* alloc_record (record_t::type_t type, /**< record's type identifier */
                   SizeArgs ... size_args) /**< arguments of T::size */
  {
    JERRY_ASSERT (type >= _first_type_id);

    size_t size = T::size (size_args...);

    record_t *prev_rec_p;
    T* rec_p = static_cast<T*> (alloc_space_for_record (size, &prev_rec_p));

    rec_p->set_type (type);
    rec_p->set_size (size);
    rec_p->set_prev (prev_rec_p);

    assert_state_is_correct ();

    return rec_p;
  } /* alloc_record */

  record_t* alloc_space_for_record (size_t bytes, record_t** out_prev_rec_p);
  void free_record (record_t* record_p);

  record_t* get_first (void);

  virtual record_t* get_prev (record_t* rec_p);
  record_t* get_next (record_t* rec_p);
  virtual void set_prev (record_t* rec_p, record_t *prev_rec_p);

  virtual size_t get_record_size (record_t* rec_p);

  void assert_state_is_correct (void);
}; /* rcs_recordset_t */

/**
 * Record type
 */
typedef rcs_recordset_t::record_t rcs_record_t;

/**
 * Recordset-specific compressed pointer type
 */
typedef rcs_record_t::cpointer_t rcs_cpointer_t;

/**
 * Record iterator
 */
class rcs_record_iterator_t
{
 public:
  rcs_record_iterator_t (rcs_recordset_t *rcs_p, rcs_record_t* rec_p)
  {
    _record_start_p = rec_p;
    _current_pos_p = (uint8_t *)_record_start_p;

    _recordset_p = rcs_p;
  }

  rcs_record_iterator_t (rcs_recordset_t *rcs_p, rcs_cpointer_t rec_ext_cp)
  {
    _record_start_p = rcs_cpointer_t::decompress (rec_ext_cp);
    _current_pos_p = (uint8_t *)_record_start_p;

    _recordset_p = rcs_p;
  }

 protected:
  typedef enum
  {
    ACCESS_WR,
    ACCESS_RD,
    ACCESS_SKIP
  } access_t;

  template<typename T>
  void access (access_t access_type, T* data)
  {
    const size_t node_data_space_size = _recordset_p->_chunk_list.get_data_space_size ();
    const size_t record_size = _recordset_p->get_record_size (_record_start_p);

    JERRY_ASSERT (!finished ());

    rcs_chunked_list_t::node_t *current_node_p = _recordset_p->_chunk_list.get_node_from_pointer (_current_pos_p);
    uint8_t *current_node_data_space_p = _recordset_p->_chunk_list.get_data_space (current_node_p);

    /*
     * Part I.
     * Calculate current offset in the record.
     */
    rcs_chunked_list_t::node_t *node_p = _recordset_p->_chunk_list.get_node_from_pointer (_record_start_p);
    size_t current_offset = 0;
    size_t left_in_node = 0;

    if (node_p == current_node_p)
    {
      current_offset = (size_t) (_current_pos_p - (uint8_t *) _record_start_p);
    }
    else
    {
      /* skip already iterated nodes */
      left_in_node = (node_data_space_size - (size_t) ((uint8_t *) _record_start_p -
                                                       _recordset_p->_chunk_list.get_data_space (node_p)));

      current_offset += left_in_node;
      node_p = _recordset_p->_chunk_list.get_next (node_p);
      JERRY_ASSERT (node_p);

      while (node_p != current_node_p)
      {
        node_p = _recordset_p->_chunk_list.get_next (node_p);
        JERRY_ASSERT (node_p);
        current_offset += node_data_space_size;
      }

      current_offset += (size_t) (_current_pos_p - current_node_data_space_p);
    }

    left_in_node = node_data_space_size - (size_t)(_current_pos_p - current_node_data_space_p);

    JERRY_ASSERT (current_offset + sizeof (T) <= record_size);

    /*
     * Part II.
     * Read the data and increase the current position pointer.
     */
    if (left_in_node >= sizeof (T))
    {
      /* all data is placed inside single node */
      if (access_type == ACCESS_RD)
      {
        *data = *(T *)_current_pos_p;
      }
      else if (access_type == ACCESS_WR)
      {
        *(T *)_current_pos_p = *data;
      }
      else
      {
        if (left_in_node > sizeof (T))
        {
          _current_pos_p += sizeof (T);
        }
        else if (current_offset + sizeof (T) < record_size)
        {
          current_node_p = _recordset_p->_chunk_list.get_next (current_node_p);
          JERRY_ASSERT (current_node_p);
          _current_pos_p = _recordset_p->_chunk_list.get_data_space (current_node_p);
        }
      }
    }
    else
    {
      /* data is distributed between two nodes */
      size_t first_chunk_size = node_data_space_size - (size_t) (_current_pos_p - current_node_data_space_p);

      if (access_type == ACCESS_RD)
      {
        memcpy (data, _current_pos_p, first_chunk_size);
      }
      else if (access_type == ACCESS_WR)
      {
        memcpy (_current_pos_p, data, first_chunk_size);
      }

      node_p = _recordset_p->_chunk_list.get_next (node_p);
      JERRY_ASSERT (node_p != nullptr);
      current_node_data_space_p = _recordset_p->_chunk_list.get_data_space (node_p);

      if (access_type == ACCESS_RD)
      {
        memcpy ((uint8_t *)data + first_chunk_size, current_node_data_space_p, sizeof (T) - first_chunk_size);
      }
      else if (access_type == ACCESS_WR)
      {
        memcpy (current_node_data_space_p, (uint8_t *)data + first_chunk_size, sizeof (T) - first_chunk_size);
      }
      else
      {
        _current_pos_p = current_node_data_space_p + sizeof (T) - first_chunk_size;
      }
    }

    /* check if we reached the end */
    if (access_type == ACCESS_SKIP)
    {
      if (current_offset + sizeof (T) == record_size)
      {
        _current_pos_p = nullptr;
      }
    }
  }

 public:
  template<typename T> T read (void)
  {
    T data;
    access (ACCESS_RD, &data);
    return data;
  }

  template<typename T> void write (T value)
  {
    access (ACCESS_WR, &value);
  }

  template<typename T> void skip ()
  {
    access<T> (ACCESS_SKIP, nullptr);
  }

  bool finished ()
  {
    return _current_pos_p == nullptr;
  }

  void reset ()
  {
    _current_pos_p = (uint8_t *)_record_start_p;
  }

 private:
  rcs_record_t* _record_start_p; /**< start of current record */
  uint8_t* _current_pos_p; /**< pointer to current offset in current record */

  rcs_recordset_t *_recordset_p; /**< recordset containing the record */
}; /* rcs_record_iterator_t */

/**
 * Free record layout description
 */
class rcs_free_record_t : public rcs_record_t
{
 public:
  size_t get_size (void) const;
  void set_size (size_t size);

  rcs_record_t* get_prev (void) const;
  void set_prev (rcs_record_t* prev_rec_p);
 private:
  /**
   * Offset of 'length' field, in bits
   */
  static const uint32_t _length_field_pos = _fields_offset_begin;

  /**
   * Width of 'length' field, in bits
   */
  static const uint32_t _length_field_width = 12u;

  /**
   * Offset of 'previous record' field, in bits
   */
  static const uint32_t _prev_field_pos = _length_field_pos + _length_field_width;

  /**
   * Width of 'previous record' field, in bits
   */
  static const uint32_t _prev_field_width = rcs_cpointer_t::bit_field_width;
};

/**
 * @}
 */

#endif /* RCS_RECORDSET_H */
