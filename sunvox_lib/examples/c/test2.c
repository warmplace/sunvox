//
// * Creating a new module
// * Loading the module from disk
// * Connecting the module to the main Output
// * Sending some events to the module
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

	/*
	//Read curve 0 from MultiSynth:
	sv_lock_slot( 0 );
	int multisynth = sv_new_module( 0, "MultiSynth", "MultiSynth", 0, 0, 0 );
	sv_unlock_slot( 0 );
	float curve_data[ 1024 ];
	int len = sv_module_curve( 0, multisynth, 1, curve_data, 0, 0 );
	printf( "Curve length: %d\n", len );
	for( int i = 0; i < len; i++ ) printf( "%d: %f\n", i, curve_data[ i ] );
	*/

	//Create Generator module:
	sv_lock_slot( 0 );
	int mod_num = sv_new_module( 0, "Generator", "Generator", 0, 0, 0 );
	sv_unlock_slot( 0 );
	if( mod_num >= 0 )
	{
	    printf( "New module created: %d\n", mod_num );
	    //Connect the new module to the Main Output:
	    sv_lock_slot( 0 );
	    sv_connect_module( 0, mod_num, 0 );
	    sv_unlock_slot( 0 );
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

	//Load module and play it:
	int mod_num2 = -1;
	if( 1 )
	{
	    //Load from disk:
	    mod_num2 = sv_load_module( 0, "organ.sunsynth", 0, 0, 0 );
	}
	else
	{
	    //Or load from the memory buffer:
	    size_t size = 0;
	    void* data = load_file( "organ.sunsynth", &size );
	    if( data )
	    {
		mod_num2 = sv_load_module_from_memory( 0, data, (unsigned int)size, 0, 0, 0 );
		free( data );
	    }
	}
	if( mod_num2 >= 0 )
	{
	    printf( "Module loaded: %d\n", mod_num2 );
	    //Connect the new module to the Main Output:
	    sv_lock_slot( 0 );
	    sv_connect_module( 0, mod_num2, 0 );
	    sv_unlock_slot( 0 );
	    //Send Note ON:
	    printf( "Note ON\n" );
	    sv_send_event( 0, 0, 64, 128, mod_num2+1, 0, 0 );
	    sleep( 1 );
	    //Send Note OFF:
	    printf( "Note OFF\n" );
	    sv_send_event( 0, 0, NOTECMD_NOTE_OFF, 0, 0, 0, 0 );
	}
	else
	{
	    printf( "Can't load the module\n" );
	}

	sleep( 1 );

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
