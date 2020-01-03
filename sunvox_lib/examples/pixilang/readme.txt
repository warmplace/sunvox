SunVox modular synth engine is built into the Pixilang since v3.8.
SunVox API in Pixilang is a little different from the cross-platform SunVox 
library for other languages.
Let's take a look at simple examples showing the main differences.

//
// SunVox library (sunvox.dll, sunvox.so, etc.)
//

sv_load_dll();
int slot = 0;
int sunvox_version = sv_init( config_str, sample_rate, 2, init_flags );
int version_major = ( ver >> 16 ) & 255;
int version_minor1 = ( ver >> 8 ) & 255;
int version_minor2 = ver & 255;
int version_minor3 = 0;
sv_open_slot( slot );
sv_load( slot, sunvox_file_name );
//OR sv_load_from_memory( slot, data_bytes, data_size );
const char* name = sv_get_song_name( slot );
int BPM = sv_get_song_bpm( slot );
int TPL = sv_get_song_tpl( slot );
int sample_rate = sv_get_sample_rate();
sv_set_autostop( slot, 1 );
sv_play_from_beginning( slot );
while( 1 )
{
    if( sv_end_of_song( slot ) )
    {
	//End of song. Playback stopped
	break;
    }
    sleep( 1 );
}
sv_stop( slot );
sv_close_slot( slot );
sv_deinit();
sv_unload_dll();

//
// Pixilang
//

//Removed:
//  sv_load_dll, sv_unload_dll;
//  sv_init, sv_deinit;
//  sv_open_slot, sv_close_slot;
//  sv_load_from_memory;
//  sv_end_of_song;
//Renamed:
//  sv_get_song_name -> sv_get_name;
//  sv_get_song_bpm -> sv_get_bpm;
//  sv_get_song_tpl -> sv_get_tpl;
//  sv_get_ticks_per_second -> get_tps;
//  sv_get_ticks -> get_ticks;
//  sv_get_module_scope2 -> sv_get_module_scope;
//  SV_INIT_FLAG_USER_AUDIO_CALLBACK -> SV_INIT_FLAG_OFFLINE
//New:
//  sv_new, sv_remove;
//  sv_fload;
//  sv_get_status;

sv = sv_new( sample_rate, init_flags )
version_major = ( SV_VERSION >> 24 ) & 255
version_minor1 = ( SV_VERSION >> 16 ) & 255
version_minor2 = ( SV_VERSION >> 8 ) & 255
version_minor3 = SV_VERSION & 255
sv_load( sv, sunvox_file_name )
//OR sv_load( sv, f ) where f is the file stream opened by fopen()
name = sv_get_name( sv ); //don't forget to remove this name later...
BPM = sv_get_bpm( sv );
TPL = sv_get_tpl( sv );
sample_rate = sv_get_sample_rate( sv )
sv_set_autostop( sv, 1 )
sv_play( sv, 0 ) //play from beginning (second parameter is the position (line number))
while 1
{
    if sv_get_status( sv ) == 0
    {
	//Playback stopped
	break
    }
    sleep( 1000 )
}
sv_stop( sv )
sv_remove( sv )
