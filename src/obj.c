/* vim: set ts=2 expandtab: */
/**
 *       @file  obj.c
 *      @brief  Commotion object model
 *
 *     @author  Josh King (jheretic), jking@chambana.net
 *
 *   @internal
 *      Created  11/07/2013 02:00:15 PM
 *     Compiler  gcc/g++
 * Organization  The Open Technology Institute
 *    Copyright  Copyright (c) 2013, Josh King
 *
 * This file is part of Commotion, Copyright (c) 2013, Josh King 
 * 
 * Commotion is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published 
 * by the Free Software Foundation, either version 3 of the License, 
 * or (at your option) any later version.
 * 
 * Commotion is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Commotion.  If not, see <http://www.gnu.org/licenses/>.
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "debug.h"
#include "obj.h"
#include "extern/halloc.h"


/*-----------------------------------------------------------------------------
 *   Constructors of character-array-types
 *-----------------------------------------------------------------------------*/
#define _DEFINE_CHAR(T, L) int co_##T##L##_alloc(co_obj_t *output, \
    const size_t out_size, const char *input, const size_t in_size, \
    const uint8_t flags) \
    { \
      CHECK((out_size - sizeof(uint##L##_t) - sizeof(co_obj_t) < UINT##L##_MAX), \
      "Object too large for type ##T##L## to address."); \
      if((in_size > 0) && (input != NULL)) \
      { \
        CHECK((in_size <= (out_size - sizeof(uint##L##_t) - sizeof(co_obj_t))), \
        "Value too large for output buffer."); \
        memmove(((co_##T##L##_t *)output)->data, input, in_size); \
      } \
      output->_type = _##T##L; \
      output->_flags = flags; \
      ((co_##T##L##_t *)output)->_len = (uint##L##_t)(out_size - sizeof(uint##L##_t) - \
          sizeof(co_obj_t)); \
      return 1; \
    error: \
      return 0; \
    } \
    co_obj_t *co_##T##L##_create(const char *input, \
    const size_t input_size, const uint8_t flags) \
    { \
      CHECK((input_size < UINT##L##_MAX), "Value too large for type ##T##L##."); \
      int output_size = input_size + sizeof(uint##L##_t) + sizeof(co_obj_t); \
      co_obj_t *output = h_calloc(1, output_size); \
      CHECK_MEM(output); \
      CHECK(co_##T##L##_alloc(output, output_size, input, input_size, flags), \
          "Failed to allocate object."); \
      return output; \
    error: \
      return NULL; \
    }

_DEFINE_CHAR(bin, 8);
_DEFINE_CHAR(bin, 16);
_DEFINE_CHAR(bin, 32);
_DEFINE_CHAR(ext, 8);
_DEFINE_CHAR(ext, 16);
_DEFINE_CHAR(ext, 32);

#define _DEFINE_STR(L) int co_str##L##_alloc(co_obj_t *output, \
    const size_t out_size, const char *input, const size_t in_size, \
    const uint8_t flags ) \
    { \
      CHECK((out_size - sizeof(uint##L##_t) - sizeof(co_obj_t) < UINT##L##_MAX), \
      "Object too large for type str##L## to address."); \
      if((in_size > 0) && (input != NULL)) \
      { \
        CHECK((in_size <= (out_size - sizeof(uint##L##_t) - sizeof(co_obj_t))), \
        "Value too large for output buffer."); \
        memmove(((co_str##L##_t *)output)->data, input, in_size); \
      } \
      output->_type = _str##L; \
      output->_flags = flags; \
      ((co_str##L##_t *)output)->_len = (uint##L##_t)(out_size - sizeof(uint##L##_t) - \
          sizeof(co_obj_t)); \
      ((co_str##L##_t *)output)->data[out_size - 1] = '\0'; \
      return 1; \
    error: \
      return 0; \
    } \
    co_obj_t *co_str##L##_create(const char *input, \
    const size_t input_size, const uint8_t flags) \
    { \
      CHECK((input_size < UINT##L##_MAX), "Value too large for type str##L##."); \
      int output_size = input_size + sizeof(uint##L##_t) + sizeof(co_obj_t); \
      co_obj_t *output = h_calloc(1, output_size); \
      CHECK_MEM(output); \
      CHECK(co_str##L##_alloc(output, output_size, input, input_size, flags), \
          "Failed to allocate object."); \
      return output; \
    error: \
      return NULL; \
    }

_DEFINE_STR(8);
_DEFINE_STR(16);
_DEFINE_STR(32);

/*-----------------------------------------------------------------------------
 *   Constructors of integer types
 *-----------------------------------------------------------------------------*/
#define _DEFINE_INTEGER(T, L) int co_##T##L##_alloc(co_obj_t *output, \
    const T##L##_t input, const uint8_t flags) \
    { \
      output->_type = _##T##L; \
      output->_flags = flags; \
      ((co_##T##L##_t *)output)->data = input; \
      return 1; \
    } \
  co_obj_t *co_##T##L##_create(\
    const T##L##_t input, const uint8_t flags) \
    { \
      int output_size = sizeof(T##L##_t) + sizeof(co_obj_t); \
      co_obj_t *output = h_calloc(1, output_size); \
      CHECK_MEM(output); \
      CHECK(co_##T##L##_alloc(output, input, flags), \
          "Failed to allocate object."); \
      return output; \
    error: \
      return NULL; \
    }

_DEFINE_INTEGER(int, 8);
_DEFINE_INTEGER(int, 16);
_DEFINE_INTEGER(int, 32);
_DEFINE_INTEGER(int, 64);
_DEFINE_INTEGER(uint, 8);
_DEFINE_INTEGER(uint, 16);
_DEFINE_INTEGER(uint, 32);
_DEFINE_INTEGER(uint, 64);

/* Type "Nil" constructors */
int 
co_nil_alloc(co_obj_t *output, const uint8_t flags)
{
  output->_type = _nil;
  output->_flags = flags;
  return 1;
} 

co_obj_t * 
co_nil_create(const uint8_t flags)
{
  co_obj_t *output = h_calloc(1, sizeof(co_nil_t));
  co_nil_alloc(output, flags);
  return output;
} 

/* Type "Bool" constructors */
int 
co_bool_alloc(co_obj_t *output, const bool input, const uint8_t flags)
{
  output->_type = _false;
  output->_flags = flags;
  if(input) output->_type = _true;
  return 1;
} 

co_obj_t * 
co_bool_create(const bool input, const uint8_t flags)
{
  co_obj_t *output = h_calloc(1, sizeof(co_bool_t));
  co_bool_alloc(output, input, flags);
  return output;
} 


/* Type "fixint" constructors */
int 
co_fixint_alloc(co_obj_t *output, const int input, const uint8_t flags)
{
  CHECK(input < 128, "Value too large for a fixint.");
  output->_type = (uint8_t)input;
  output->_flags = flags;
  return 1;
error:
  return 0;
} 

co_obj_t * 
co_fixint_create(const int input, const uint8_t flags)
{
  co_obj_t *output = h_calloc(1, sizeof(co_fixint_t));
  CHECK(co_fixint_alloc(output, input, flags), "Failed to allocate object.");
  return output;
error:
  return NULL;
} 

/* Type "float32" constructors */
int 
co_float32_alloc(co_obj_t *output, const float input, const uint8_t flags)
{
  output->_type = _float32;
  output->_flags = flags;
  ((co_float32_t *)output)->data = input;
  return 1;
} 

co_obj_t * 
co_float32_create(const double input, const uint8_t flags)
{
  co_obj_t *output = h_calloc(1, sizeof(co_float32_t));
  co_float32_alloc(output, input, flags);
  return output;
} 

/* Type "float64" constructors */
int 
co_float64_alloc(co_obj_t *output, const double input, const uint8_t flags)
{
  output->_type = _float64;
  output->_flags = flags;
  ((co_float64_t *)output)->data = input;
  return 1;
} 

co_obj_t * 
co_float64_create(const double input, const uint8_t flags)
{
  co_obj_t *output = h_calloc(1, sizeof(co_float64_t));
  co_float64_alloc(output, input, flags);
  return output;
} 

/*-----------------------------------------------------------------------------
 *   Deconstructors
 *-----------------------------------------------------------------------------*/
void
co_obj_free(co_obj_t *object)
{
  h_free(object);
  return;
}

/*-----------------------------------------------------------------------------
 *   Accessors
 *-----------------------------------------------------------------------------*/
size_t
co_obj_raw(void *data, const co_obj_t *object)
{
  switch(CO_TYPE(object))
  {
    case _float32:
      data = (void *)((&(((co_float32_t *)object)->data)) - 1);
      return sizeof(float) + 1;
    case _float64:
      data = (void *)((&(((co_float64_t *)object)->data)) - 1);
      return sizeof(double) + 1;
    case _uint8:
      data = (void *)((&(((co_uint8_t *)object)->data)) - 1);
      return sizeof(uint8_t) + 1;
    case _uint16:
      data = (void *)((&(((co_uint16_t *)object)->data)) - 1);
      return sizeof(uint16_t) + 1;
    case _uint32:
      data = (void *)((&(((co_uint32_t *)object)->data)) - 1);
      return sizeof(uint32_t) + 1;
    case _uint64:
      data = (void *)((&(((co_uint64_t *)object)->data)) - 1);
      return sizeof(uint64_t) + 1;
    case _int8:
      data = (void *)((&(((co_int8_t *)object)->data)) - 1);
      return sizeof(int8_t) + 1;
    case _int16:
      data = (void *)((&(((co_int16_t *)object)->data)) - 1);
      return sizeof(int16_t) + 1;
    case _int32:
      data = (void *)((&(((co_int32_t *)object)->data)) - 1);
      return sizeof(int32_t) + 1;
    case _int64:
      data = (void *)((&(((co_int64_t *)object)->data)) - 1);
      return sizeof(int64_t) + 1;
    case _str8:
      data = (char *)(((co_str8_t *)object)->data - 1);
      return ((co_str8_t *)object)->_len + 1;
      break;
    case _str16:
      data = (char *)(((co_str16_t *)object)->data - 1);
      return ((co_str16_t *)object)->_len + 1;
      break;
    case _str32:
      data = (char *)(((co_str32_t *)object)->data - 1);
      return ((co_str32_t *)object)->_len + 1;
      break;
    case _bin8:
      data = (char *)(((co_bin8_t *)object)->data - 1);
      return ((co_bin8_t *)object)->_len + 1;
      break;
    case _bin16:
      data = (char *)(((co_bin16_t *)object)->data - 1);
      return ((co_bin16_t *)object)->_len + 1;
      break;
    case _bin32:
      data = (char *)(((co_bin32_t *)object)->data - 1);
      return ((co_bin32_t *)object)->_len + 1;
      break;
    case _ext8:
      data = (char *)(((co_ext8_t *)object)->data - 1);
      return ((co_ext8_t *)object)->_len + 1;
      break;
    case _ext16:
      data = (char *)(((co_ext16_t *)object)->data - 1);
      return ((co_ext16_t *)object)->_len + 1;
      break;
    case _ext32:
      data = (char *)(((co_ext32_t *)object)->data - 1);
      return ((co_ext32_t *)object)->_len + 1;
      break;
    default:
      WARN("Not a valid object.");
      return -1;
  }
}

size_t
co_obj_data(void *data, const co_obj_t *object)
{
  switch(CO_TYPE(object))
  {
    case _float32:
      data = (void *)&(((co_float32_t *)object)->data);
      return sizeof(float);
    case _float64:
      data = (void *)&(((co_float64_t *)object)->data);
      return sizeof(double);
    case _uint8:
      data = (void *)&(((co_uint8_t *)object)->data);
      return sizeof(uint8_t);
    case _uint16:
      data = (void *)&(((co_uint16_t *)object)->data);
      return sizeof(uint16_t);
    case _uint32:
      data = (void *)&(((co_uint32_t *)object)->data);
      return sizeof(uint32_t);
    case _uint64:
      data = (void *)&(((co_uint64_t *)object)->data);
      return sizeof(uint64_t);
    case _int8:
      data = (void *)&(((co_int8_t *)object)->data);
      return sizeof(int8_t);
    case _int16:
      data = (void *)&(((co_int16_t *)object)->data);
      return sizeof(int16_t);
    case _int32:
      data = (void *)&(((co_int32_t *)object)->data);
      return sizeof(int32_t);
    case _int64:
      data = (void *)&(((co_int64_t *)object)->data);
      return sizeof(int64_t);
    case _str8:
      data = (void *)(((co_str8_t *)object)->data);
      return ((co_str8_t *)object)->_len;
      break;
    case _str16:
      data = (void *)(((co_str16_t *)object)->data);
      return ((co_str16_t *)object)->_len;
      break;
    case _str32:
      data = (void *)(((co_str32_t *)object)->data);
      return ((co_str32_t *)object)->_len;
      break;
    case _bin8:
      data = (void *)(((co_bin8_t *)object)->data);
      return ((co_bin8_t *)object)->_len;
      break;
    case _bin16:
      data = (void *)(((co_bin16_t *)object)->data);
      return ((co_bin16_t *)object)->_len;
      break;
    case _bin32:
      data = (void *)(((co_bin32_t *)object)->data);
      return ((co_bin32_t *)object)->_len;
      break;
    case _ext8:
      data = (void *)(((co_ext8_t *)object)->data);
      return ((co_ext8_t *)object)->_len;
      break;
    case _ext16:
      data = (void *)(((co_ext16_t *)object)->data);
      return ((co_ext16_t *)object)->_len;
      break;
    case _ext32:
      data = (void *)(((co_ext32_t *)object)->data);
      return ((co_ext32_t *)object)->_len;
      break;
    default:
      WARN("Not a valid object.");
      return -1;
  }
}

int 
co_obj_import(co_obj_t *output, const size_t out_size, const char *input, \
    const size_t in_size, const uint8_t flags)
{
  CHECK(((in_size > 0) && (input != NULL)), "Nothing to import.");
  CHECK((in_size <= (out_size - sizeof(co_obj_t) + 1)), \
      "Value too large for output buffer.");
  memmove(&(output->_type), input, in_size);
  output->_flags = flags;
  return 1;
error:
  return 0;
}

int
co_obj_getflags(const co_obj_t *object)
{
  return object->_flags;
}

void
co_obj_setflags(co_obj_t *object, const int flags)
{
  object->_flags = (uint8_t)flags;
  return;
}

/*-----------------------------------------------------------------------------
 *   Strings
 *-----------------------------------------------------------------------------*/

int 
co_strcpy(co_obj_t *dst, const co_obj_t *src, const size_t size)
{
  char *src_data = NULL;
  size_t length = 0;
  CHECK(((length = co_obj_data(src_data, src)) >= 0), "Not a character object.");
  switch(CO_TYPE(src))
  {
    case _str8:
      CHECK(co_str8_alloc(dst, size, src_data, length, src->_flags), \
          "Failed to allocate str8.");
      break;
    case _str16:
      CHECK(co_str16_alloc(dst, size, src_data, length, src->_flags), \
          "Failed to allocate str16.");
      break;
    case _str32:
      CHECK(co_str32_alloc(dst, size, src_data, length, src->_flags), \
          "Failed to allocate str32.");
      break;
    default:
      ERROR("Not a string.");
      break;
  }
	return 1;
error:
  return 0;
}

int 
co_strcat(co_obj_t *dst, const co_obj_t *src, const size_t size)
{
  char *src_data = NULL;
  char *dst_data = NULL; 
  size_t used, length, copy;

  CHECK(((used = co_obj_data(dst_data, dst)) >= 0), "Not a character object.");
  CHECK(((length = co_obj_data(src_data, src)) >= 0), "Not a character object.");
  if (size > 0 && used < size) {
    copy = (length >= size - used) ? size - used : length;
    memmove(dst + used, src, copy);
    switch(CO_TYPE(dst))
    {
      case _str8:
        ((co_str8_t *)dst)->_len = used + length;
        break;
      case _str16:
        ((co_str16_t *)dst)->_len = used + length;
        break;
      case _str32:
        ((co_str32_t *)dst)->_len = used + length;
        break;
      default:
        ERROR("Not a string.");
        break;
    }
    
  }
  return used + length;
error:
  return -1;
}