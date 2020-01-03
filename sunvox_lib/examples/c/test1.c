//
// * Loading SunVox project (song) from file/memory
// * Displaying information about the project and the first pattern
// * Sending Note ON/OFF events to the module (synth)
// * Playing the project
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include <signal.h>
#include <math.h>

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

void show_pattern( int slot, int pat_num )
{
    static char* h2c = "0123456789ABCDEF";
    sunvox_note* nn = sv_get_pattern_data( slot, pat_num );
    if( !nn ) return;
    int x = sv_get_pattern_x( slot, pat_num ); //time (line number)
    int y = sv_get_pattern_y( slot, pat_num ); //vertical position on timeline
    int tracks = sv_get_pattern_tracks( slot, pat_num ); //number of tracks
    int lines = sv_get_pattern_lines( slot, pat_num ); //number of lines
    const char* pat_name = sv_get_pattern_name( slot, pat_num );
    if( !pat_name ) pat_name = "";
    printf( "Pattern %d \"%s\": %d %d %dx%d\n", pat_num, pat_name, x, y, tracks, lines );
    for( int t = 0; t < tracks; t++ )
    {
	printf( "NNVVMMCCEEXXYY | " );
    }
    printf( "\n" );
    for( int l = 0; l < lines; l++ )
    {
	for( int t = 0; t < tracks; t++ )
	{
	    char evt[ 15 ];
	    memset( evt, ' ', sizeof( evt ) );
	    evt[ 14 ] = 0;
	    //Note:
	    if( nn->note > 0 && nn->note < 128 )
	    {
		int note_num = nn->note - 1;
		evt[ 0 ] = "CcDdEFfGgAaB"[ note_num % 12 ];
		evt[ 1 ] = note_num / 12 + '0'; //octave
	    }
	    if( nn->note == NOTECMD_NOTE_OFF )
	    {
		evt[ 0 ] = '=';
		evt[ 1 ] = '=';
	    }
	    //Velocity:
	    if( nn->vel )
	    {
		int vel = nn->vel - 1;
		evt[ 2 ] = h2c[ ( vel >> 4 ) & 15 ];
		evt[ 3 ] = h2c[ vel & 15 ];
	    }
	    //Module:
	    if( nn->module )
	    {
		int mod = nn->module - 1;
		evt[ 4 ] = h2c[ ( mod >> 4 ) & 15 ];
		evt[ 5 ] = h2c[ mod & 15 ];
	    }
	    //Ctl:
	    if( nn->ctl )
	    {
		evt[ 6 ] = h2c[ ( nn->ctl >> 12 ) & 15 ];
		evt[ 7 ] = h2c[ ( nn->ctl >> 8 ) & 15 ];
		evt[ 8 ] = h2c[ ( nn->ctl >> 4 ) & 15 ];
		evt[ 9 ] = h2c[ nn->ctl & 15 ];
	    }
	    //Ctl val:
	    if( nn->ctl_val )
	    {
		evt[ 10 ] = h2c[ ( nn->ctl_val >> 12 ) & 15 ];
		evt[ 11 ] = h2c[ ( nn->ctl_val >> 8 ) & 15 ];
		evt[ 12 ] = h2c[ ( nn->ctl_val >> 4 ) & 15 ];
		evt[ 13 ] = h2c[ nn->ctl_val & 15 ];
	    }
	    //Show the event:
	    printf( "%s | ", evt );
	    nn++;
	}
	printf( "\n" );
    }
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
	printf( "Current sample rate: %d\n", sv_get_sample_rate() );

	sv_open_slot( 0 );

	printf( "Loading SunVox project file...\n" );
	int res = -1;
	if( 1 )
	{
	    //load from file:
	    res = sv_load( 0, "test.sunvox" );
	}
	else
	{
	    //... or load from memory:
	    size_t file_size = 0;
	    void* data = load_file( "test.sunvox", &file_size );
	    if( data )
	    {
		res = sv_load_from_memory( 0, data, file_size );
		free( data );
	    }
	}
	if( res == 0 )
	    printf( "Loaded.\n" );
	else
	    printf( "Load error %d.\n", res );

	//Set volume to 100%
	sv_volume( 0, 256 );

	printf( "Project name: %s\n", sv_get_song_name( 0 ) );
	int mm = sv_get_number_of_modules( 0 );
	printf( "Number of modules: %d\n", mm );
	for( int i = 0; i < mm; i++ )
	{
	    uint32_t flags = sv_get_module_flags( 0, i );
    	    if( ( flags & SV_MODULE_FLAG_EXISTS ) == 0 ) continue;
    	    uint32_t xy = sv_get_module_xy( 0, i );
    	    uint32_t ft = sv_get_module_finetune( 0, i );
    	    int x, y, finetune, relnote;
    	    SV_GET_MODULE_XY( xy, x, y );
    	    SV_GET_MODULE_FINETUNE( ft, finetune, relnote );
    	    printf( "module %d: %s; x=%d y=%d finetune=%d rel.note=%d\n", i, sv_get_module_name( 0, i ), x, y, finetune, relnote );
	}

	//Show information about the first pattern:
	show_pattern( 0, 0 );

	//Send two events (Note ON) to the module "Kicker":
	int m = sv_find_module( 0, "Kicker" );
	sv_set_event_t( 0, 1, 0 );
	sv_send_event( 0, 0, 64, 129, m+1, 0, 0 ); //track 0; note 64; velocity 129 (max);
	sleep( 1 );

	/*
	//Play the exact frequency in Hz:
	//(but the actual frequency will depend the module and its parameters)
	m = sv_find_module( 0, "Generator" );
	sv_send_event( 0, 0, NOTECMD_SET_PITCH, 129, m+1, 0, SV_FREQUENCY_TO_PITCH( 440 ) ); //440 Hz
	sleep( 1 );
	sv_send_event( 0, 0, NOTECMD_NOTE_OFF, 0, 0, 0, 0 );
	sleep( 1 );
	*/

	sv_play_from_beginning( 0 );

	while( keep_running )
	{
	    printf( "Line counter: %f Module 7 -> %s = %d\n", 
		(float)sv_get_current_line2( 0 ) / 32, 
		sv_get_module_ctl_name( 0, 7, 1 ), //Get controller name
		sv_get_module_ctl_value( 0, 7, 1, 0 ) //Get controller value
	    );
	    sleep( 1 );
	}

	sv_stop( 0 );
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
