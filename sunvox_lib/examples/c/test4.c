//
// * Loading SunVox project (song) from file
// * Exporting audio to the WAV file
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <signal.h>

#define SUNVOX_MAIN
#include "../../headers/sunvox.h"

int g_sv_sample_rate = 44100; //Hz
const int g_sv_channels_num = 2; //1 - mono; 2 - stereo; only stereo supported in the current version
int g_sv_buffer_size = 1024; //Audio buffer size (number of frames)
int g_sv_sample_type = 2; //2 - 16bit; 4 - 32bit float;

int keep_running = 1;
void int_handler( int param ) 
{
    keep_running = 0;
}

int main()
{
    signal( SIGINT, int_handler );

    if( sv_load_dll() )
	return 1;

    int flags = SV_INIT_FLAG_USER_AUDIO_CALLBACK | SV_INIT_FLAG_ONE_THREAD;
    if( g_sv_sample_type == 2 )
        flags |= SV_INIT_FLAG_AUDIO_INT16;
    else
        flags |= SV_INIT_FLAG_AUDIO_FLOAT32;
    int ver = sv_init( 0, g_sv_sample_rate, g_sv_channels_num, flags );
    if( ver >= 0 )
    {
	int major = ( ver >> 16 ) & 255;
	int minor1 = ( ver >> 8 ) & 255;
	int minor2 = ( ver ) & 255;
	printf( "SunVox lib version: %d.%d.%d\n", major, minor1, minor2 );

	sv_open_slot( 0 );

	printf( "Loading SunVox song from file...\n" );
	if( sv_load( 0, "test.sunvox" ) == 0 )
	    printf( "Loaded.\n" );
	else
	    printf( "Load error.\n" );
	sv_volume( 0, 256 );

	sv_play_from_beginning( 0 );

	//Saving the audio stream to the WAV file:
	//(audio format: 16/32-bit stereo interleaved (LRLRLRLR...))
	FILE* f = fopen( "audio_stream.wav", "wb" );
	if( f )
	{
	    int frame_size = g_sv_channels_num * g_sv_sample_type; //bytes per frame

	    void* buf = malloc( g_sv_buffer_size * frame_size ); //Audio buffer
	    int song_len_frames = sv_get_song_length_frames( 0 );
            int song_len_bytes = song_len_frames * frame_size;
	    int cur_frame = 0;
	    int val;

	    //WAV header:
            fwrite( (void*)"RIFF", 1, 4, f );
            val = 4 + 24 + 8 + song_len_bytes; fwrite( &val, 4, 1, f );
            fwrite( (void*)"WAVE", 1, 4, f );

            //WAV FORMAT:
            fwrite( (void*)"fmt ", 1, 4, f );
            val = 16; fwrite( &val, 4, 1, f );
            val = 1; if( g_sv_sample_type == 4 ) val = 3; fwrite( &val, 2, 1, f ); //format
            val = g_sv_channels_num; fwrite( &val, 2, 1, f ); //channels
            val = g_sv_sample_rate; fwrite( &val, 4, 1, f ); //frames per second
            val = g_sv_sample_rate * frame_size; fwrite( &val, 4, 1, f ); //bytes per second
            val = frame_size; fwrite( &val, 2, 1, f ); //block align
    	    val = g_sv_sample_type * 8; fwrite( &val, 2, 1, f ); //bits

            //WAV DATA:
            fwrite( (void*)"data", 1, 4, f );
            fwrite( &song_len_bytes, 4, 1, f );
            int pos = 0;
	    while( keep_running && cur_frame < song_len_frames )
	    {
		//Get the next piece of audio:
		int frames_num = g_sv_buffer_size;
		if( cur_frame + frames_num > song_len_frames )
		    frames_num = song_len_frames - cur_frame;
		sv_audio_callback( buf, frames_num, 0, sv_get_ticks() );
		cur_frame += frames_num;

		//Save this data to the file:
		fwrite( buf, 1, frames_num * frame_size, f );

		//Print some info:
		int new_pos = (int)( ( (float)cur_frame / (float)song_len_frames ) * 100 );
		if( pos != new_pos )
		{
		    printf( "Playing position: %d %%\n", pos );
		    pos = new_pos;
		}
	    }
	    fclose( f );
	    free( buf );
	}
	else
	{
	    printf( "Can't open the file\n" );
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
