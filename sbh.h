/*
 * sbh.h
 *   sbh.h is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published
 *   by the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 *   sbh.h is distrubted in the hope that it will be useful, but WITHOUT ANY
 *   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 *   You should have received a copy of the GNU General Public License along
 * with sbh.h. If not, see <https://www.gnu.org/licenses/>
 *
 */

#ifndef SBH_H_
#define SBH_H_

#include <stdarg.h>
#include <stdio.h>

#ifndef sbh_char_m
#  define sbh_char_m char
#endif

#ifndef sbh_int_m
#  include <stdint.h>
#  define sbh_int_m int32_t
#endif

#ifndef sbh_size_m
#  include <stddef.h>
#  define sbh_size_m size_t
#endif

#ifdef SBH_USE_STATIC
#  define sbh_opt_static static
#else
#  define sbh_opt_static
#endif

typedef struct _Strb strb_t;

sbh_opt_static strb_t *strb_init( const sbh_char_m *init_string );
sbh_opt_static sbh_int_m strb_sprintf( strb_t *builder,
                                       const sbh_char_m *format, ... );
sbh_opt_static sbh_int_m strb_append( strb_t *builder,
                                      const sbh_char_m *string );
sbh_opt_static sbh_int_m strb_strb_append( strb_t *builder,
                                           const strb_t *append_builder );
sbh_opt_static sbh_int_m strb_scanf( strb_t *builder, const sbh_char_m *format,
                                     ... );
sbh_opt_static strb_t *strb_strbdup( strb_t *builder );

sbh_opt_static void strb_reset( strb_t *builder );
sbh_opt_static void strb_rewind( strb_t *builder );
sbh_opt_static void strb_destroy( strb_t *builder );

sbh_opt_static const sbh_char_m *strb_to_arr( strb_t *builder );
sbh_opt_static sbh_size_m strb_strlen( strb_t *builder );

#ifdef SBH_IMPLEMENTATION

#  if defined( __GNUC__ ) || __STDC_VERSION__ >= 202312UL
#    define sbh_static_assert _Static_assert
#  else
#    define sbh_static_assert( a, b )
#  endif

#  ifndef SBH_INIT_CAPACITY
#    define SBH_INIT_CAPACITY 4096
#  endif

sbh_static_assert( ( SBH_INIT_CAPACITY > 0 ) &&
                       ( ( SBH_INIT_CAPACITY & ( SBH_INIT_CAPACITY - 1 ) ) ==
                         0 ),
                   "SBH_INIT_CAPACITY is not a power of two" );

#  ifndef SBH_GROW_FACTOR
#    define SBH_GROW_FACTOR 2
#  endif

#  ifndef sbh_null
#    include <stddef.h>
#    define sbh_null NULL
#  endif

#  ifndef sbh_alloc
#    include <stdlib.h>
#    define sbh_alloc( len ) malloc( len )
#  endif

#  ifndef sbh_free
#    include <stdlib.h>
#    define sbh_free( ptr ) free( ptr )
#  endif

#  ifndef sbh_realloc
#    include <stdlib.h>
#    define sbh_realloc( ptr, len ) realloc( ptr, len )
#  endif

#  ifndef sbh_strlen
#    include <string.h>
#    define sbh_strlen( str ) strlen( str )
#  endif

#  ifndef sbh_strcpy
#    include <string.h>
#    define sbh_strcpy( dest, src ) strcpy( dest, src )
#  endif

#  ifndef sbh_memset
#    include <string.h>
#    define sbh_memset( ptr, data, len ) memset( ptr, data, len )
#  endif

#  include <stdint.h>

struct _Strb {
  sbh_char_m *string;
  sbh_size_m string_len;

  sbh_size_m alloc_size;
  uint8_t pad[ 4 ];
};

static sbh_int_m strb_grow( strb_t *builder, sbh_size_m appended_len ) {
  sbh_size_m alloc_size = builder->alloc_size;
  while ( builder->string_len + appended_len >= alloc_size ) {
    alloc_size *= SBH_GROW_FACTOR;
  }

  sbh_char_m *tmp;
  tmp = sbh_realloc( builder->string, alloc_size );
  if ( tmp == sbh_null ) {
    return -1;
  }

  builder->string = tmp;
  builder->alloc_size = alloc_size;
  return 0;
}

sbh_opt_static strb_t *strb_init( const sbh_char_m *init_string ) {
  strb_t *builder = sbh_alloc( sizeof( strb_t ) );
  if ( builder == sbh_null ) {
    return sbh_null;
  }

  sbh_memset( builder, 0, sizeof( strb_t ) );
  builder->alloc_size = SBH_INIT_CAPACITY;
  if ( init_string != sbh_null ) {
    sbh_size_m len = sbh_strlen( init_string );
    strb_grow( builder, len );
    sbh_strcpy( builder->string, init_string );
    builder->string_len = len;

  } else {
    builder->string = sbh_alloc( SBH_INIT_CAPACITY );
    builder->alloc_size = SBH_INIT_CAPACITY;
    builder->string_len = 0;
  }

  return builder;
}

sbh_opt_static sbh_int_m strb_sprintf( strb_t *builder,
                                       const sbh_char_m *format, ... ) {
#  include <stddef.h>

  char *off = builder->string + builder->string_len;
  va_list va_arg;
  va_start( va_arg, format );

  sbh_size_m len = vsnprintf( NULL, 0, format, va_arg );

  va_end( va_arg );

  if ( strb_grow( builder, len + 1 ) == -1 ) {
    return -1;
  }

  va_start( va_arg, format );
  len = vsnprintf( off, len + 1, format, va_arg );

  builder->string_len += len;

  va_end( va_arg );

  return len;
}

sbh_opt_static sbh_int_m strb_append( strb_t *builder,
                                      const sbh_char_m *string ) {
  char *off = builder->string + builder->string_len;
  sbh_size_m len = sbh_strlen( string );
  if ( strb_grow( builder, len ) == -1 ) {
    return -1;
  }

  sbh_strcpy( off, string );

  builder->string_len += len;

  return len;
}

sbh_opt_static sbh_int_m strb_strb_append( strb_t *builder,
                                           const strb_t *append_builder ) {
  return strb_append( builder, append_builder->string );
}

sbh_opt_static sbh_int_m strb_scanf( strb_t *builder, const sbh_char_m *format,
                                     ... ) {
  va_list va_arg;

  va_start( va_arg, format );

  sbh_int_m result = vsscanf( builder->string, format, va_arg );

  va_end( va_arg );

  return result;
}

sbh_opt_static void strb_reset( strb_t *builder ) {
  builder->string_len = 0;
  sbh_memset( builder->string, 0, builder->alloc_size * sizeof( sbh_char_m ) );
}

sbh_opt_static void strb_rewind( strb_t *builder ) {
  sbh_free( builder->string );
  builder->alloc_size = SBH_INIT_CAPACITY;
  builder->string_len = 0;
  builder->string = sbh_alloc( SBH_INIT_CAPACITY );
}

sbh_opt_static void strb_destroy( strb_t *builder ) {
  strb_reset( builder );
  sbh_free( builder->string );
  sbh_free( builder );
}

sbh_opt_static strb_t *strb_strbdup( strb_t *builder ) {
  strb_t *new_builder = strb_init( builder->string );
  return new_builder;
}

sbh_opt_static const sbh_char_m *strb_to_arr( strb_t *builder ) {
  return builder->string;
}

sbh_opt_static sbh_size_m strb_strlen( strb_t *builder ) {
  return builder->string_len;
}

#endif

#endif
