#define SBH_IMPLEMENTATION
#include "../sbh.h"

#include <assert.h>
#include <string.h>

int main() {
  strb_t *strb = strb_init( "My init string" );

  strb_sprintf( strb, ", Hello World." );
  printf( "%s\n", strb_to_arr( strb ) );
  assert( strcmp( strb_to_arr( strb ), "My init string, Hello World." ) == 0 );

  strb_rewind( strb );
  strb_sprintf( strb, "FOOBAR2000" );
  strb_append( strb, " & DeaDBeeF" );
  printf( "%s\n", strb_to_arr( strb ) );
  strb_destroy( strb );
}
