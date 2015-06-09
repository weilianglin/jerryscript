/* Copyright 2014-2015 Samsung Electronics Co., Ltd.
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

#include "ecma-alloc.h"
#include "ecma-conversion.h"
#include "ecma-gc.h"
#include "ecma-function-object.h"
#include "ecma-lex-env.h"
#include "serializer.h"
#include "parser.h"

#define ECMA_BUILTINS_INTERNAL
#include "ecma-builtins-internal.h"

#define BUILTIN_INC_HEADER_NAME "ecma-builtin-function.inc.h"
#define BUILTIN_UNDERSCORED_ID function
#include "ecma-builtin-internal-routines-template.inc.h"

/** \addtogroup ecma ECMA
 * @{
 *
 * \addtogroup ecmabuiltins
 * @{
 *
 * \addtogroup function ECMA Function object built-in
 * @{
 */

/**
 * Handle calling [[Call]] of built-in Function object
 *
 * @return completion-value
 */
ecma_completion_value_t
ecma_builtin_function_dispatch_call (const ecma_value_t *arguments_list_p, /**< arguments list */
                                     ecma_length_t arguments_list_len) /**< number of arguments */
{
  JERRY_ASSERT (arguments_list_len == 0 || arguments_list_p != NULL);

  return ecma_builtin_function_dispatch_construct (arguments_list_p, arguments_list_len);
} /* ecma_builtin_function_dispatch_call */

/**
 * Handle calling [[Construct]] of built-in Function object
 *
 * @return completion-value
 */
ecma_completion_value_t
ecma_builtin_function_dispatch_construct (const ecma_value_t *arguments_list_p, /**< arguments list */
                                          ecma_length_t arguments_list_len) /**< number of arguments */
{
  JERRY_ASSERT (arguments_list_len == 0 || arguments_list_p != NULL);

  ecma_completion_value_t completion = ECMA_COMPLETION_TYPE_NORMAL;

  MEM_DEFINE_LOCAL_ARRAY (string_params_p,
                          arguments_list_len == 0 ? 1 : arguments_list_len,
                          ecma_string_t*);
  uint32_t params_count;

  size_t zt_strings_buffer_size;

  if (arguments_list_len == 0)
  {
    string_params_p[0] = ecma_new_ecma_string_from_magic_string_id (ECMA_MAGIC_STRING__EMPTY);
    zt_strings_buffer_size = sizeof (ecma_char_t);
    params_count = 1;
  }
  else
  {
    zt_strings_buffer_size = 0;

    for (params_count = 0;
         params_count < arguments_list_len;
         params_count++)
    {
      completion = ecma_op_to_string (arguments_list_p[params_count]);

      if (ecma_is_completion_value_throw (completion))
      {
        break;
      }
      else
      {
        JERRY_ASSERT (ecma_is_completion_value_normal (completion));

        string_params_p[params_count] = ecma_get_string_from_completion_value (completion);
        zt_strings_buffer_size += ((size_t) ecma_string_get_length (string_params_p[params_count]) +
                                   sizeof (ecma_char_t));
      }
    }
  }

  if (arguments_list_len == 0
      || !ecma_is_completion_value_throw (completion))
  {
    completion = ecma_make_simple_completion_value (ECMA_SIMPLE_VALUE_UNDEFINED);

    /*
     * If there were no exceptions during arguments conversion,
     * representing ecma-strings in zero-terminated form.
     */

    MEM_DEFINE_LOCAL_ARRAY (zt_string_params_p,
                            params_count,
                            ecma_char_t*);
    MEM_DEFINE_LOCAL_ARRAY (zt_string_buffer_p,
                            zt_strings_buffer_size,
                            ecma_char_t);

    ssize_t zt_string_buffer_pos = 0;
    for (uint32_t i = 0; i < params_count; i++)
    {
      ssize_t sz = ecma_string_to_zt_string (string_params_p[i],
                                             &zt_string_buffer_p[zt_string_buffer_pos],
                                             (ssize_t) zt_strings_buffer_size - zt_string_buffer_pos);
      JERRY_ASSERT (sz > 0);

      zt_string_params_p[i] = zt_string_buffer_p + zt_string_buffer_pos;

      zt_string_buffer_pos += sz;
    }

    parser_init ();
    parser_parse_new_function ((const char **) zt_string_params_p, params_count);
    const opcode_t* opcodes = (const opcode_t*) serializer_get_bytecode ();
    serializer_print_opcodes ();
    parser_free ();

    bool is_strict = false;
    bool do_instantiate_arguments_object = true;

    opcode_scope_code_flags_t scope_flags = vm_get_scope_flags (opcodes,
                                                                0);

    if (scope_flags & OPCODE_SCOPE_CODE_FLAGS_STRICT)
    {
      is_strict = true;
    }
    if ((scope_flags & OPCODE_SCOPE_CODE_FLAGS_NOT_REF_ARGUMENTS_IDENTIFIER)
        && (scope_flags & OPCODE_SCOPE_CODE_FLAGS_NOT_REF_EVAL_IDENTIFIER))
    {
      /* the code doesn't use 'arguments' identifier
       * and doesn't perform direct call to eval,
       * so Arguments object can't be referenced */
      do_instantiate_arguments_object = false;
    }

    ecma_object_t *glob_lex_env_p = ecma_get_global_environment ();

    ecma_object_t *func_obj_p = ecma_op_create_function_object (params_count > 1u ? string_params_p : NULL,
                                                                (ecma_length_t) (params_count - 1u),
                                                                glob_lex_env_p,
                                                                is_strict,
                                                                do_instantiate_arguments_object,
                                                                opcodes,
                                                                1);

    ecma_deref_object (glob_lex_env_p);

    completion = ecma_make_normal_completion_value (ecma_make_object_value (func_obj_p));

    MEM_FINALIZE_LOCAL_ARRAY (zt_string_buffer_p);
    MEM_FINALIZE_LOCAL_ARRAY (zt_string_params_p);
  }

  for (uint32_t i = 0; i < params_count; i++)
  {
    ecma_deref_ecma_string (string_params_p[i]);
  }

  MEM_FINALIZE_LOCAL_ARRAY (string_params_p);

  return completion;
} /* ecma_builtin_function_dispatch_construct */

/**
 * @}
 * @}
 * @}
 */
