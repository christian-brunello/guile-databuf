/*
 * databuf.c
 * This file is part of guile-databuf
 *
 * Copyright (C) 2018 Christian Brunello
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Library General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this program; if not, see <http://www.gnu.org/licenses/>.
 */
#include <libguile.h>

#include <databuf.h>

static SCM databuf_type;

#define DATABUF(p) ((databuf_t*) (scm_assert_foreign_object_type(databuf_type, p), scm_foreign_object_ref(p, 0)))

static void finalize_databuf(SCM obj)
{
  databuf_t *ptr;

  ptr = scm_foreign_object_ref(obj, 0);

  databuf_delete(ptr);
}

static SCM make_databuf(SCM e)
{
  databuf_endianness_t endianness;
  databuf_t *r = NULL;

  endianness = (databuf_endianness_t) scm_to_int(e);

  if((r = databuf_new(endianness)) == NULL)
    scm_syserror("make-databuf");

  return scm_make_foreign_object_1(databuf_type, r);
}

SCM databuf_seek_x(SCM databuf, SCM off, SCM whence)
{
  databuf_t *p;
  off_t o;
  int w;
  int r;

  p = DATABUF(databuf);
  o = scm_to_int64(off);
  w = scm_to_int(whence);

  if((r = databuf_seek(p, o, w)) < 0)
    scm_syserror("databuf-seek!");

  return scm_from_int(r);
}

#define DEFINEWRITE(type, c_type, conv)\
static SCM databuf_write_ ## type ## _x (SCM databuf, SCM value)\
{\
  databuf_t *p;\
  c_type v;\
\
  p = DATABUF(databuf);\
  v = conv (value);\
\
  if(databuf_write(p, &v, sizeof(v)) != 0)\
    scm_syserror("databuf-write-" #type "!");\
\
  return SCM_UNDEFINED;\
}

#define DEFINEREAD(type, c_type, conv)\
static SCM databuf_read_ ## type (SCM databuf)\
{\
  databuf_t *p;\
  c_type v;\
\
  p = DATABUF(databuf);\
\
  if(databuf_read(p, &v, sizeof(v)) != 0)\
    scm_syserror("databuf-read-" #type);\
\
  return conv (v); \
}

DEFINEWRITE(u8, uint8_t, scm_to_uint8)
DEFINEWRITE(s8, int8_t, scm_to_int8)
DEFINEWRITE(u16, uint16_t, scm_to_uint16)
DEFINEWRITE(s16, int16_t, scm_to_int16)
DEFINEWRITE(u32, uint32_t, scm_to_uint32)
DEFINEWRITE(s32, int32_t, scm_to_int32)
DEFINEWRITE(u64, uint64_t, scm_to_uint64)
DEFINEWRITE(s64, int64_t, scm_to_int64)
DEFINEWRITE(double, double, scm_to_double)

static SCM databuf_write_string_x (SCM databuf, SCM value)
{
  databuf_t *p;
  char *v;
  uint16_t len;

  scm_dynwind_begin(0);

  p = DATABUF(databuf);
  v = scm_to_locale_string(value);
  scm_dynwind_free(v);

  len = strlen(v) + 1;

  if(databuf_write(p, &len, sizeof len) != 0
     || databuf_write_array(p, v, sizeof(char), len) != 0)
    scm_syserror("databuf-write-string!");

  scm_dynwind_end();

  return SCM_UNDEFINED;
}

DEFINEREAD(u8, uint8_t, scm_from_uint8)
DEFINEREAD(s8, int8_t, scm_from_int8)
DEFINEREAD(u16, uint16_t, scm_from_uint16)
DEFINEREAD(s16, int16_t, scm_from_int16)
DEFINEREAD(u32, uint32_t, scm_from_uint32)
DEFINEREAD(s32, int32_t, scm_from_int32)
DEFINEREAD(u64, uint64_t, scm_from_uint64)
DEFINEREAD(s64, int64_t, scm_from_int64)
DEFINEREAD(double, double, scm_from_double)

static SCM databuf_read_string(SCM databuf)
{
  databuf_t *p;
  uint16_t len;
  char *v;
  SCM r;

  scm_dynwind_begin(0);

  p = DATABUF(databuf);

  if(databuf_read(p, &len, sizeof(len)) != 0)
    scm_syserror("databuf-read-string");

  if((v = malloc(len * sizeof(char))) == NULL)
    scm_syserror("databuf-read-string");

  scm_dynwind_free(v);

  if(databuf_read_array(p, v, sizeof(char), len) != 0)
    scm_syserror("databuf-read-string");

  r = scm_from_locale_string(v);

  scm_dynwind_end();

  return r;
}

static SCM databuf_to_bytevector(SCM databuf)
{
  databuf_t *p;
  void *data;
  size_t size;
  SCM r;

  scm_dynwind_begin(0);

  p = DATABUF(databuf);

  if(databuf_get_data(p, &data, &size) != 0)
    scm_syserror("databuf->bytevector");

  scm_dynwind_free(data);

  r = scm_c_make_bytevector(size);
  memcpy(SCM_BYTEVECTOR_CONTENTS(r), data, size);

  scm_dynwind_end();

  return r;
}

static SCM databuf_from_bytevector(SCM bv, SCM endian)
{
  databuf_t *p;

  scm_dynwind_begin(0);

  if((p = databuf_new(scm_to_int(endian))) == NULL)
    scm_syserror("bytevector->databuf");

  scm_dynwind_unwind_handler((void(*)(void*)) databuf_delete, p, 0);

  if(databuf_set_data(p, SCM_BYTEVECTOR_CONTENTS(bv), SCM_BYTEVECTOR_LENGTH(bv)) != 0)
    scm_syserror("bytevector->databuf");

  scm_dynwind_end();

  return scm_make_foreign_object_1(databuf_type, p);
}

void init_guile_databuf(void)
{
  SCM name, slots;
  scm_t_struct_finalize finalizer;

  name = scm_from_utf8_symbol("databuf");
  slots = scm_list_1(scm_from_utf8_symbol("data"));
  finalizer = finalize_databuf;

  databuf_type = scm_make_foreign_object_type(name, slots, finalizer);

  scm_c_define("DATABUF_ENDIANNESS_BIG", scm_from_int(DATABUF_ENDIANNESS_BIG));
  scm_c_define("DATABUF_ENDIANNESS_LITTLE", scm_from_int(DATABUF_ENDIANNESS_LITTLE));

  scm_c_define_gsubr("make-databuf", 1, 0, 0, make_databuf);
  
  scm_c_define_gsubr("databuf-seek!", 3, 0, 0, databuf_seek_x);

  scm_c_define_gsubr("databuf-write-u8!", 2, 0, 0, databuf_write_u8_x);
  scm_c_define_gsubr("databuf-write-s8!", 2, 0, 0, databuf_write_s8_x);
  scm_c_define_gsubr("databuf-write-u16!", 2, 0, 0, databuf_write_u16_x);
  scm_c_define_gsubr("databuf-write-s16!", 2, 0, 0, databuf_write_s16_x);
  scm_c_define_gsubr("databuf-write-u32!", 2, 0, 0, databuf_write_u32_x);
  scm_c_define_gsubr("databuf-write-s32!", 2, 0, 0, databuf_write_s32_x);
  scm_c_define_gsubr("databuf-write-u64!", 2, 0, 0, databuf_write_u64_x);
  scm_c_define_gsubr("databuf-write-s64!", 2, 0, 0, databuf_write_s64_x);
  scm_c_define_gsubr("databuf-write-double!", 2, 0, 0, databuf_write_double_x);
  scm_c_define_gsubr("databuf-write-string!", 2, 0, 0, databuf_write_string_x);

  scm_c_define_gsubr("databuf-read-u8", 1, 0, 0, databuf_read_u8);
  scm_c_define_gsubr("databuf-read-s8", 1, 0, 0, databuf_read_s8);
  scm_c_define_gsubr("databuf-read-u16", 1, 0, 0, databuf_read_u16);
  scm_c_define_gsubr("databuf-read-s16", 1, 0, 0, databuf_read_s16);
  scm_c_define_gsubr("databuf-read-u32", 1, 0, 0, databuf_read_u32);
  scm_c_define_gsubr("databuf-read-s32", 1, 0, 0, databuf_read_s32);
  scm_c_define_gsubr("databuf-read-u64", 1, 0, 0, databuf_read_u64);
  scm_c_define_gsubr("databuf-read-s64", 1, 0, 0, databuf_read_s64);
  scm_c_define_gsubr("databuf-read-double", 1, 0, 0, databuf_read_double);
  scm_c_define_gsubr("databuf-read-string", 1, 0, 0, databuf_read_string);

  scm_c_define_gsubr("databuf->bytevector", 1, 0, 0, databuf_to_bytevector);
  scm_c_define_gsubr("bytevector->databuf", 2, 0, 0, databuf_from_bytevector);
}

