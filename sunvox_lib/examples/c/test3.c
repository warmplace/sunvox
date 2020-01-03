//
// * Creating a new Sampler and loading XI-file (set of samples) into it
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <signal.h>

#define SUNVOX_MAIN
#include "../../headers/sunvox.h"

int keep_running = 1;
void int_handler( int param ) 
{
    keep_running = 0;
}

void* load_file( const char* name, size_t* file_size )
{
    void* rv = 0;
    FILE* f = fopen( name, "rb" );
    if( f )
    {
        fseek( f, 0, 2 );
        size_t size = ftell( f ); //get file size
        rewind( f );
        printf( "file %s size: %d bytes\n", name, (int)size );
        if( size > 0 )
        {
            rv = malloc( size );
            if( rv )
            {
                fread( rv, 1, size, f );
                if( file_size ) *file_size = size;
            }
        }
        fclose( f );
    }
    return rv;
}

int main()
{
    signal( SIGINT, int_handler );

    if( sv_load_dll() )
	return 1;

    int ver = sv_init( 0, 44100, 2, 0 );
    if( ver >= 0 )
    {
	int major = ( ver >> 16 ) & 255;
	int minor1 = ( ver >> 8 ) & 255;
	int minor2 = ( ver ) & 255;
	printf( "SunVox lib version: %d.%d.%d\n", major, minor1, minor2 );

	sv_open_slot( 0 );

	//Create Sampler module:
	sv_lock_slot( 0 );
	int mod_num = sv_new_module( 0, "Sampler", "Sampler", 0, 0, 0 );
	sv_unlock_slot( 0 );
	if( mod_num >= 0 )
	{
	    printf( "New module created: %d\n", mod_num );
	    //Connect the new module to the Main Output:
	    sv_lock_slot( 0 );
	    sv_connect_module( 0, mod_num, 0 );
	    sv_unlock_slot( 0 );
	    //Load a sample:
	    if( 1 )
	    {
		//from disk:
		sv_sampler_load( 0, mod_num, "flute.xi", -1 );
	    }
	    else
	    {
		//or from the memory buffer:
		size_t size = 0;
        	void* data = load_file( "flute.xi", &size );
        	if( data )
        	{
		    sv_sampler_load_from_memory( 0, mod_num, data, (unsigned int)size, -1 );
            	    free( data );
        	}
	    }
	    //Send Note ON:
	    printf( "Note ON\n" );
	    sv_send_event( 0, 0, 64, 128, mod_num+1, 0, 0 );
	    sleep( 1 );
	    //Send Note OFF:
	    printf( "Note OFF\n" );
	    sv_send_event( 0, 0, NOTECMD_NOTE_OFF, 0, 0, 0, 0 );
	    sleep( 1 );
	}
	else
	{
	    printf( "Can't create the new module\n" );
	}

	while( keep_running )
	{
	    sleep( 1 );
	}

	sv_close_slot( 0 );
	sv_deinit();
    }
    else
    {
	printf( "sv_init() error %d\n", ver );
    }

    sv_unload_dll();

    return 0;
}
