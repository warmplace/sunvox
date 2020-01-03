/*
   SunVox Library (modular synthesizer)
   Copyright (c) 2008 - 2019, Alexander Zolotov <nightradio@gmail.com>, WarmPlace.ru
*/

#ifndef __SUNVOX_H__
#define __SUNVOX_H__

#include <stdio.h>
#include <stdint.h>

/*
   Constants, data types and macros
*/

#define NOTECMD_NOTE_OFF	128
#define NOTECMD_ALL_NOTES_OFF	129 /* send "note off" to all modules */
#define NOTECMD_CLEAN_SYNTHS	130 /* put all modules into standby state (stop and clear all internal buffers) */
#define NOTECMD_STOP		131
#define NOTECMD_PLAY		132
#define NOTECMD_SET_PITCH       133 /* set pitch ctl_val */

typedef struct
{
    uint8_t	note;           /* NN: 0 - nothing; 1..127 - note num; 128 - note off; 129, 130... - see NOTECMD_xxx defines */
    uint8_t	vel;            /* VV: Velocity 1..129; 0 - default */
    uint16_t	module;         /* MM: 0 - nothing; 1..65535 - module number + 1 */
    uint16_t	ctl;            /* 0xCCEE: CC: 1..127 - controller number + 1; EE - effect */
    uint16_t	ctl_val;        /* 0xXXYY: value of controller or effect */
} sunvox_note;

/* Flags for sv_init(): */
#define SV_INIT_FLAG_NO_DEBUG_OUTPUT 		( 1 << 0 )
#define SV_INIT_FLAG_USER_AUDIO_CALLBACK 	( 1 << 1 ) /* Offline mode: */
							   /* system-dependent audio stream will not be created; */
							   /* user calls sv_audio_callback() to get the next piece of sound stream */
#define SV_INIT_FLAG_OFFLINE			( 1 << 1 ) /* Same as SV_INIT_FLAG_USER_AUDIO_CALLBACK */
#define SV_INIT_FLAG_AUDIO_INT16 		( 1 << 2 ) /* Desired sample type of the output sound stream : int16_t */
#define SV_INIT_FLAG_AUDIO_FLOAT32 		( 1 << 3 ) /* Desired sample type of the output sound stream : float */
							   /* The actual sample type may be different, if SV_INIT_FLAG_USER_AUDIO_CALLBACK is not set */
#define SV_INIT_FLAG_ONE_THREAD			( 1 << 4 ) /* Audio callback and song modification are in single thread */
							   /* Use it with SV_INIT_FLAG_USER_AUDIO_CALLBACK only */

/* Flags for sv_get_time_map(): */
#define SV_TIME_MAP_SPEED	0
#define SV_TIME_MAP_FRAMECNT	1
#define SV_TIME_MAP_TYPE_MASK	3

/* Flags for sv_get_module_flags(): */
#define SV_MODULE_FLAG_EXISTS 	( 1 << 0 )
#define SV_MODULE_FLAG_EFFECT 	( 1 << 1 )
#define SV_MODULE_FLAG_MUTE	( 1 << 2 )
#define SV_MODULE_FLAG_SOLO	( 1 << 3 )
#define SV_MODULE_FLAG_BYPASS	( 1 << 4 )
#define SV_MODULE_INPUTS_OFF 	16
#define SV_MODULE_INPUTS_MASK 	( 255 << SV_MODULE_INPUTS_OFF )
#define SV_MODULE_OUTPUTS_OFF 	( 16 + 8 )
#define SV_MODULE_OUTPUTS_MASK 	( 255 << SV_MODULE_OUTPUTS_OFF )

/*
   Macros
*/
#define SV_GET_MODULE_XY( in_xy, out_x, out_y ) out_x = in_xy & 0xFFFF; if( out_x & 0x8000 ) out_x -= 0x10000; out_y = ( in_xy >> 16 ) & 0xFFFF; if( out_y & 0x8000 ) out_y -= 0x10000;
#define SV_GET_MODULE_FINETUNE( in_finetune, out_finetune, out_relative_note ) out_finetune = in_finetune & 0xFFFF; if( out_finetune & 0x8000 ) out_finetune -= 0x10000; out_relative_note = ( in_finetune >> 16 ) & 0xFFFF; if( out_relative_note & 0x8000 ) out_relative_note -= 0x10000;
#define SV_PITCH_TO_FREQUENCY( in_pitch ) ( pow( 2, ( 30720.0F - (in_pitch) ) / 3072.0F ) * 16.3339 )
#define SV_FREQUENCY_TO_PITCH( in_freq ) ( 30720 - log2( (in_freq) / 16.3339 ) * 3072 )

#if defined(_WIN32) || defined(_WIN32_WCE) || defined(__WIN32__) || defined(_WIN64)
    #define OS_WIN
    #define LIBNAME "sunvox.dll"
    typedef LPCTSTR LIBNAME_STR_TYPE;
#else
    typedef const char* LIBNAME_STR_TYPE;
#endif
#if defined(__APPLE__) 
    #include "TargetConditionals.h"
    #ifdef TARGET_OS_MAC
        #define OS_MACOS
    #else
        #define OS_IOS
    #endif
    #define OS_APPLE
    #define LIBNAME "sunvox.dylib"
#endif
#if defined(__linux__) || defined(linux)
    #define OS_LINUX
    #define LIBNAME "./sunvox.so"
#endif
#if defined(OS_APPLE) || defined(OS_LINUX)
    #define OS_UNIX
#endif

#ifdef OS_WIN
    #ifdef __GNUC__
	#define SUNVOX_FN_ATTR __attribute__((stdcall))
    #else
	#define SUNVOX_FN_ATTR __stdcall
    #endif
#endif
#ifndef SUNVOX_FN_ATTR
    #define SUNVOX_FN_ATTR /**/
#endif

#ifdef SUNVOX_STATIC_LIB

#ifdef __cplusplus
extern "C" {
#endif

/*
   Functions
   (use the functions with the label "USE LOCK/UNLOCK" within the sv_lock_slot() / sv_unlock_slot() block only!)
*/

/*
   sv_init(), sv_deinit() - global sound system init/deinit
   Parameters:
     config - string with additional configuration in the following format: "option_name=value|option_name=value";
              example: "buffer=1024|audiodriver=alsa|audiodevice=hw:0,0";
              use null if you agree to the automatic configuration;
     freq - desired sample rate (Hz); min - 44100;
            the actual rate may be different, if SV_INIT_FLAG_USER_AUDIO_CALLBACK is not set;
     channels - only 2 supported now;
     flags - mix of the SV_INIT_FLAG_xxx flags.
*/
int sv_init( const char* config, int freq, int channels, uint32_t flags ) SUNVOX_FN_ATTR;
int sv_deinit( void ) SUNVOX_FN_ATTR;

/*
   sv_get_sample_rate() - get current sampling rate (it may differ from the frequency specified in sv_init())
*/
int sv_get_sample_rate( void ) SUNVOX_FN_ATTR;

/*
   sv_update_input() - 
   handle input ON/OFF requests to enable/disable input ports of the sound card
   (for example, after the Input module creation).
   Call it from the main thread only, where the SunVox sound stream is not locked.
*/
int sv_update_input( void ) SUNVOX_FN_ATTR;

/*
   sv_audio_callback() - get the next piece of SunVox audio from the Output module.
   With sv_audio_callback() you can ignore the built-in SunVox sound output mechanism and use some other sound system.
   SV_INIT_FLAG_USER_AUDIO_CALLBACK flag in sv_init() mus be set.
   Parameters:
     buf - destination buffer of type int16_t (if SV_INIT_FLAG_AUDIO_INT16 used in sv_init())
           or float (if SV_INIT_FLAG_AUDIO_FLOAT32 used in sv_init());
           stereo data will be interleaved in this buffer: LRLR... ; where the LR is the one frame (Left+Right channels);
     frames - number of frames in destination buffer;
     latency - audio latency (in frames);
     out_time - buffer output time (in system ticks, SunVox time space);
   Return values: 0 - silence (buffer filled with zeroes); 1 - some signal.
   Example 1 (simplified, without accurate time sync) - suitable for most cases:
     sv_audio_callback( buf, frames, 0, sv_get_ticks() );
   Example 2 (accurate time sync) - when you need to maintain exact time intervals between incoming events (notes, commands, etc.):
     user_out_time = ... ; //output time in user time space (depends on your own implementation)
     user_cur_time = ... ; //current time in user time space
     user_ticks_per_second = ... ; //ticks per second in user time space
     user_latency = user_out_time - user_cur_time; //latency in user time space
     uint32_t sunvox_latency = ( user_latency * sv_get_ticks_per_second() ) / user_ticks_per_second; //latency in SunVox time space
     uint32_t latency_frames = ( user_latency * sample_rate_Hz ) / user_ticks_per_second; //latency in frames
     sv_audio_callback( buf, frames, latency_frames, sv_get_ticks() + sunvox_latency );
*/
int sv_audio_callback( void* buf, int frames, int latency, uint32_t out_time ) SUNVOX_FN_ATTR;

/*
   sv_audio_callback2() - send some data to the Input module and receive the filtered data from the Output module.
   It's the same as sv_audio_callback() but you also can specify the input buffer.
   Parameters:
     ...
     in_type - input buffer type: 0 - int16_t (16bit integer); 1 - float (32bit floating point);
     in_channels - number of input channels;
     in_buf - input buffer; stereo data must be interleaved in this buffer: LRLR... ; where the LR is the one frame (Left+Right channels);
*/
int sv_audio_callback2( void* buf, int frames, int latency, uint32_t out_time, int in_type, int in_channels, void* in_buf ) SUNVOX_FN_ATTR;

/*
   sv_open_slot(), sv_close_slot(), sv_lock_slot(), sv_unlock_slot() - 
   open/close/lock/unlock sound slot for SunVox.
   You can use several slots simultaneously (each slot with its own SunVox engine).
   Use lock/unlock when you simultaneously read and modify SunVox data from different threads (for the same slot); 
   example:
     thread 1: sv_lock_slot(0); sv_get_module_flags(0,mod1); sv_unlock_slot(0);
     thread 2: sv_lock_slot(0); sv_remove_module(0,mod2); sv_unlock_slot(0);
   Some functions (marked as "USE LOCK/UNLOCK") can't work without lock/unlock at all.
*/
int sv_open_slot( int slot ) SUNVOX_FN_ATTR;
int sv_close_slot( int slot ) SUNVOX_FN_ATTR;
int sv_lock_slot( int slot ) SUNVOX_FN_ATTR;
int sv_unlock_slot( int slot ) SUNVOX_FN_ATTR;

/*
   sv_load(), sv_load_from_memory() - 
   load SunVox project from the file or from the memory block.
*/
int sv_load( int slot, const char* name ) SUNVOX_FN_ATTR;
int sv_load_from_memory( int slot, void* data, uint32_t data_size ) SUNVOX_FN_ATTR;

/*
   sv_play() - play from the current position;
   sv_play_from_beginning() - play from the beginning (line 0);
   sv_stop(): first call - stop playing; second call - reset all SunVox activity and switch the engine to standby mode.
*/
int sv_play( int slot ) SUNVOX_FN_ATTR;
int sv_play_from_beginning( int slot ) SUNVOX_FN_ATTR;
int sv_stop( int slot ) SUNVOX_FN_ATTR;

/*
   sv_set_autostop(), sv_get_autostop() -
   autostop values: 0 - disable autostop; 1 - enable autostop.
   When disabled, song is playing infinitely in the loop.
*/
int sv_set_autostop( int slot, int autostop ) SUNVOX_FN_ATTR;
int sv_get_autostop( int slot ) SUNVOX_FN_ATTR;

/*
   sv_end_of_song() return values: 0 - song is playing now; 1 - stopped. 
*/
int sv_end_of_song( int slot ) SUNVOX_FN_ATTR;

/*
*/
int sv_rewind( int slot, int line_num ) SUNVOX_FN_ATTR;

/*
   sv_volume() - set volume from 0 (min) to 256 (max 100%);
   negative values are ignored;
   return value: previous volume;
*/
int sv_volume( int slot, int vol ) SUNVOX_FN_ATTR;

/*
   sv_set_event_t() - set the time of events to be sent by sv_send_event()
   Parameters:
     slot;
     set: 1 - set; 0 - reset (use automatic time setting - the default mode);
     t: the time when the events occurred (in system ticks, SunVox time space).
   Examples:
     sv_set_event_t( slot, 1, 0 ) //not specified - further events will be processed as quickly as possible
     sv_set_event_t( slot, 1, sv_get_ticks() ) //time when the events will be processed = NOW + sound latancy * 2
*/
int sv_set_event_t( int slot, int set, int t ) SUNVOX_FN_ATTR;

/*
   sv_send_event() - send an event (note ON, note OFF, controller change, etc.)
   Parameters:
     slot;
     track_num - track number within the pattern;
     note: 0 - nothing; 1..127 - note num; 128 - note off; 129, 130... - see NOTECMD_xxx defines;
     vel: velocity 1..129; 0 - default;
     module: 0 (empty) or module number + 1 (1..65535);
     ctl: 0xCCEE. CC - number of a controller (1..255). EE - effect;
     ctl_val: value of controller or effect.
*/
int sv_send_event( int slot, int track_num, int note, int vel, int module, int ctl, int ctl_val ) SUNVOX_FN_ATTR;

/*
*/
int sv_get_current_line( int slot ) SUNVOX_FN_ATTR; /* Get current line number */
int sv_get_current_line2( int slot ) SUNVOX_FN_ATTR; /* Get current line number in fixed point format 27.5 */
int sv_get_current_signal_level( int slot, int channel ) SUNVOX_FN_ATTR; /* From 0 to 255 */
const char* sv_get_song_name( int slot ) SUNVOX_FN_ATTR;
int sv_get_song_bpm( int slot ) SUNVOX_FN_ATTR;
int sv_get_song_tpl( int slot ) SUNVOX_FN_ATTR;

/*
   sv_get_song_length_frames(), sv_get_song_length_lines() -
   get the project length.
   Frame is one discrete of the sound. Sample rate 44100 Hz means, that you hear 44100 frames per second.
*/
uint32_t sv_get_song_length_frames( int slot ) SUNVOX_FN_ATTR;
uint32_t sv_get_song_length_lines( int slot ) SUNVOX_FN_ATTR;

/*
   sv_get_time_map()
   Parameters:
     slot;
     start_line - first line to read (usually 0);
     len - number of lines to read;
     dest - pointer to the buffer (size = len*sizeof(uint32_t)) for storing the map values;
     flags:
       SV_TIME_MAP_SPEED: dest[X] = BPM | ( TPL << 16 ) (speed at the beginning of line X);
       SV_TIME_MAP_FRAMECNT: dest[X] = frame counter at the beginning of line X;
   Return value: 0 if successful, or negative value in case of some error.
*/
int sv_get_time_map( int slot, int start_line, int len, uint32_t* dest, int flags ) SUNVOX_FN_ATTR;

/*
   sv_new_module() - create a new module;
   sv_remove_module() - remove selected module;
   sv_connect_module() - connect the source to the destination;
   sv_disconnect_module() - disconnect the source from the destination;
   sv_load_module() - load a module or sample; supported file formats: sunsynth, xi, wav, aiff;
                      return value: new module number or negative value in case of some error;
   sv_load_module_from_memory() - load a module or sample from the memory block;
   sv_sampler_load() - load a sample to already created Sampler; to replace the whole sampler - set sample_slot to -1;
   sv_sampler_load_from_memory() - load a sample from the memory block;
*/
int sv_new_module( int slot, const char* type, const char* name, int x, int y, int z ) SUNVOX_FN_ATTR; /* USE LOCK/UNLOCK! */
int sv_remove_module( int slot, int mod_num ) SUNVOX_FN_ATTR; /* USE LOCK/UNLOCK! */
int sv_connect_module( int slot, int source, int destination ) SUNVOX_FN_ATTR; /* USE LOCK/UNLOCK! */
int sv_disconnect_module( int slot, int source, int destination ) SUNVOX_FN_ATTR; /* USE LOCK/UNLOCK! */
int sv_load_module( int slot, const char* file_name, int x, int y, int z ) SUNVOX_FN_ATTR;
int sv_load_module_from_memory( int slot, void* data, uint32_t data_size, int x, int y, int z ) SUNVOX_FN_ATTR;
int sv_sampler_load( int slot, int sampler_module, const char* file_name, int sample_slot ) SUNVOX_FN_ATTR;
int sv_sampler_load_from_memory( int slot, int sampler_module, void* data, uint32_t data_size, int sample_slot ) SUNVOX_FN_ATTR;

/*
*/
int sv_get_number_of_modules( int slot ) SUNVOX_FN_ATTR;

/*
   sv_find_module() - find a module by name;
   return value: module number or -1 (if not found);
*/
int sv_find_module( int slot, const char* name ) SUNVOX_FN_ATTR;

/*
*/
uint32_t sv_get_module_flags( int slot, int mod_num ) SUNVOX_FN_ATTR; /* SV_MODULE_FLAG_xxx */

/*
   sv_get_module_inputs(), sv_get_module_outputs() - 
   get pointers to the int[] arrays with the input/output links.
   Number of inputs = ( module_flags & SV_MODULE_INPUTS_MASK ) >> SV_MODULE_INPUTS_OFF.
   Number of outputs = ( module_flags & SV_MODULE_OUTPUTS_MASK ) >> SV_MODULE_OUTPUTS_OFF.
*/
int* sv_get_module_inputs( int slot, int mod_num ) SUNVOX_FN_ATTR;
int* sv_get_module_outputs( int slot, int mod_num ) SUNVOX_FN_ATTR;

/*
*/
const char* sv_get_module_name( int slot, int mod_num ) SUNVOX_FN_ATTR;

/*
   sv_get_module_xy() - get module XY coordinates packed in a single uint32 value:
   ( x & 0xFFFF ) | ( ( y & 0xFFFF ) << 16 )
   Normal working area: 0x0 ... 1024x1024
   Center: 512x512
   Use SV_GET_MODULE_XY() macro to unpack X and Y.
*/
uint32_t sv_get_module_xy( int slot, int mod_num ) SUNVOX_FN_ATTR;

/*
   sv_get_module_color() - get module color in the following format: 0xBBGGRR
*/
int sv_get_module_color( int slot, int mod_num ) SUNVOX_FN_ATTR;

/*
   sv_get_module_finetune() - get the relative note and finetune of the module;
   return value: ( finetune & 0xFFFF ) | ( ( relative_note & 0xFFFF ) << 16 ).
   Use SV_GET_MODULE_FINETUNE() macro to unpack finetune and relative_note.
*/
uint32_t sv_get_module_finetune( int slot, int mod_num ) SUNVOX_FN_ATTR;

/*
   sv_get_module_scope2() return value = received number of samples (may be less or equal to samples_to_read).
   Example:
     int16_t buf[ 1024 ];
     int received = sv_get_module_scope2( slot, mod_num, 0, buf, 1024 );
     //buf[ 0 ] = value of the first sample (-32768...32767);
     //buf[ 1 ] = value of the second sample;
     //...
     //buf[ received - 1 ] = value of the last received sample;
*/
uint32_t sv_get_module_scope2( int slot, int mod_num, int channel, int16_t* dest_buf, uint32_t samples_to_read ) SUNVOX_FN_ATTR;

/*
   sv_module_curve() - access to the curve values of the specified module
   Parameters:
     slot;
     mod_num - module number;
     curve_num - curve number;
     data - destination or source buffer;
     len - number of items to read/write;
     w - read (0) or write (1).
   return value: number of items processed successfully.
*/
int sv_module_curve( int slot, int mod_num, int curve_num, float* data, int len, int w ) SUNVOX_FN_ATTR;

/*
*/
int sv_get_number_of_module_ctls( int slot, int mod_num ) SUNVOX_FN_ATTR;
const char* sv_get_module_ctl_name( int slot, int mod_num, int ctl_num ) SUNVOX_FN_ATTR;
int sv_get_module_ctl_value( int slot, int mod_num, int ctl_num, int scaled ) SUNVOX_FN_ATTR;

/*
*/
int sv_get_number_of_patterns( int slot ) SUNVOX_FN_ATTR;

/*
   sv_find_pattern() - find a pattern by name;
   return value: pattern number or -1 (if not found);
*/
int sv_find_pattern( int slot, const char* name ) SUNVOX_FN_ATTR;

/*
   sv_get_pattern_xxxx - get pattern information
   x - time (line number);
   y - vertical position on timeline;
   tracks - number of pattern tracks;
   lines - number of pattern lines;
   name - pattern name or NULL;
*/
int sv_get_pattern_x( int slot, int pat_num ) SUNVOX_FN_ATTR;
int sv_get_pattern_y( int slot, int pat_num ) SUNVOX_FN_ATTR;
int sv_get_pattern_tracks( int slot, int pat_num ) SUNVOX_FN_ATTR;
int sv_get_pattern_lines( int slot, int pat_num ) SUNVOX_FN_ATTR;
const char* sv_get_pattern_name( int slot, int pat_num ) SUNVOX_FN_ATTR;

/*
   sv_get_pattern_data() - get the pattern buffer (for reading and writing)
   containing notes (events) in the following order:
     line 0: note for track 0, note for track 1, ... note for track X;
     line 1: note for track 0, note for track 1, ... note for track X;
     ...
     line X: ...
   Example:
     int pat_tracks = sv_get_pattern_tracks( slot, pat_num ); //number of tracks
     sunvox_note* data = sv_get_pattern_data( slot, pat_num ); //get the buffer with all the pattern events (notes)
     sunvox_note* n = &data[ line_number * pat_tracks + track_number ];
     ... and then do someting with note n ...
*/
sunvox_note* sv_get_pattern_data( int slot, int pat_num ) SUNVOX_FN_ATTR;

/*
   sv_pattern_mute() - mute (1) / unmute (0) specified pattern;
   negative values are ignored;
   return value: previous state (1 - muted; 0 - unmuted) or -1 (error);
*/
int sv_pattern_mute( int slot, int pat_num, int mute ) SUNVOX_FN_ATTR; /* USE LOCK/UNLOCK! */

/*
   SunVox engine uses its own time space, measured in system ticks (don't confuse it with the project ticks);
   required when calculating the out_time parameter in the sv_audio_callback().
   Use sv_get_ticks() to get current tick counter (from 0 to 0xFFFFFFFF).
   Use sv_get_ticks_per_second() to get the number of SunVox ticks per second.
*/
uint32_t sv_get_ticks( void ) SUNVOX_FN_ATTR;
uint32_t sv_get_ticks_per_second( void ) SUNVOX_FN_ATTR;

/*
   sv_get_log() - get the latest messages from the log
   Parameters:
     size - max number of bytes to read.
   Return value: pointer to the null-terminated string with the latest log messages.
*/
const char* sv_get_log( int size ) SUNVOX_FN_ATTR;

#ifdef __cplusplus
} /* ...extern "C" */
#endif

/* ... SUNVOX_STATIC_LIB */
#else
/* DYNAMIC LIBRARY (DLL, SO, etc.) ... */

typedef int (SUNVOX_FN_ATTR *tsv_audio_callback)( void* buf, int frames, int latency, uint32_t out_time );
typedef int (SUNVOX_FN_ATTR *tsv_audio_callback2)( void* buf, int frames, int latency, uint32_t out_time, int in_type, int in_channels, void* in_buf );
typedef int (SUNVOX_FN_ATTR *tsv_open_slot)( int slot );
typedef int (SUNVOX_FN_ATTR *tsv_close_slot)( int slot );
typedef int (SUNVOX_FN_ATTR *tsv_lock_slot)( int slot );
typedef int (SUNVOX_FN_ATTR *tsv_unlock_slot)( int slot );
typedef int (SUNVOX_FN_ATTR *tsv_init)( const char* config, int freq, int channels, uint32_t flags );
typedef int (SUNVOX_FN_ATTR *tsv_deinit)( void );
typedef int (SUNVOX_FN_ATTR *tsv_get_sample_rate)( void );
typedef int (SUNVOX_FN_ATTR *tsv_update_input)( void );
typedef int (SUNVOX_FN_ATTR *tsv_load)( int slot, const char* name );
typedef int (SUNVOX_FN_ATTR *tsv_load_from_memory)( int slot, void* data, uint32_t data_size );
typedef int (SUNVOX_FN_ATTR *tsv_play)( int slot );
typedef int (SUNVOX_FN_ATTR *tsv_play_from_beginning)( int slot );
typedef int (SUNVOX_FN_ATTR *tsv_stop)( int slot );
typedef int (SUNVOX_FN_ATTR *tsv_set_autostop)( int slot, int autostop );
typedef int (SUNVOX_FN_ATTR *tsv_get_autostop)( int slot );
typedef int (SUNVOX_FN_ATTR *tsv_end_of_song)( int slot );
typedef int (SUNVOX_FN_ATTR *tsv_rewind)( int slot, int t );
typedef int (SUNVOX_FN_ATTR *tsv_volume)( int slot, int vol );
typedef int (SUNVOX_FN_ATTR *tsv_set_event_t)( int slot, int set, int t );
typedef int (SUNVOX_FN_ATTR *tsv_send_event)( int slot, int track_num, int note, int vel, int module, int ctl, int ctl_val );
typedef int (SUNVOX_FN_ATTR *tsv_get_current_line)( int slot );
typedef int (SUNVOX_FN_ATTR *tsv_get_current_line2)( int slot );
typedef int (SUNVOX_FN_ATTR *tsv_get_current_signal_level)( int slot, int channel );
typedef const char* (SUNVOX_FN_ATTR *tsv_get_song_name)( int slot );
typedef int (SUNVOX_FN_ATTR *tsv_get_song_bpm)( int slot );
typedef int (SUNVOX_FN_ATTR *tsv_get_song_tpl)( int slot );
typedef uint32_t (SUNVOX_FN_ATTR *tsv_get_song_length_frames)( int slot );
typedef uint32_t (SUNVOX_FN_ATTR *tsv_get_song_length_lines)( int slot );
typedef int (SUNVOX_FN_ATTR *tsv_get_time_map)( int slot, int start_line, int len, uint32_t* dest, int flags );
typedef int (SUNVOX_FN_ATTR *tsv_new_module)( int slot, const char* type, const char* name, int x, int y, int z );
typedef int (SUNVOX_FN_ATTR *tsv_remove_module)( int slot, int mod_num );
typedef int (SUNVOX_FN_ATTR *tsv_connect_module)( int slot, int source, int destination );
typedef int (SUNVOX_FN_ATTR *tsv_disconnect_module)( int slot, int source, int destination );
typedef int (SUNVOX_FN_ATTR *tsv_load_module)( int slot, const char* file_name, int x, int y, int z );
typedef int (SUNVOX_FN_ATTR *tsv_load_module_from_memory)( int slot, void* data, uint32_t data_size, int x, int y, int z );
typedef int (SUNVOX_FN_ATTR *tsv_sampler_load)( int slot, int sampler_module, const char* file_name, int sample_slot );
typedef int (SUNVOX_FN_ATTR *tsv_sampler_load_from_memory)( int slot, int sampler_module, void* data, uint32_t data_size, int sample_slot );
typedef int (SUNVOX_FN_ATTR *tsv_get_number_of_modules)( int slot );
typedef int (SUNVOX_FN_ATTR *tsv_find_module)( int slot, const char* name );
typedef uint32_t (SUNVOX_FN_ATTR *tsv_get_module_flags)( int slot, int mod_num );
typedef int* (SUNVOX_FN_ATTR *tsv_get_module_inputs)( int slot, int mod_num );
typedef int* (SUNVOX_FN_ATTR *tsv_get_module_outputs)( int slot, int mod_num );
typedef const char* (SUNVOX_FN_ATTR *tsv_get_module_name)( int slot, int mod_num );
typedef uint32_t (SUNVOX_FN_ATTR *tsv_get_module_xy)( int slot, int mod_num );
typedef int (SUNVOX_FN_ATTR *tsv_get_module_color)( int slot, int mod_num );
typedef uint32_t (SUNVOX_FN_ATTR *tsv_get_module_finetune)( int slot, int mod_num );
typedef uint32_t (SUNVOX_FN_ATTR *tsv_get_module_scope2)( int slot, int mod_num, int channel, int16_t* dest_buf, uint32_t samples_to_read );
typedef int (SUNVOX_FN_ATTR *tsv_module_curve)( int slot, int mod_num, int curve_num, float* data, int len, int w );
typedef int (SUNVOX_FN_ATTR *tsv_get_number_of_module_ctls)( int slot, int mod_num );
typedef const char* (SUNVOX_FN_ATTR *tsv_get_module_ctl_name)( int slot, int mod_num, int ctl_num );
typedef int (SUNVOX_FN_ATTR *tsv_get_module_ctl_value)( int slot, int mod_num, int ctl_num, int scaled );
typedef int (SUNVOX_FN_ATTR *tsv_get_number_of_patterns)( int slot );
typedef int (SUNVOX_FN_ATTR *tsv_find_pattern)( int slot, const char* name );
typedef int (SUNVOX_FN_ATTR *tsv_get_pattern_x)( int slot, int pat_num );
typedef int (SUNVOX_FN_ATTR *tsv_get_pattern_y)( int slot, int pat_num );
typedef int (SUNVOX_FN_ATTR *tsv_get_pattern_tracks)( int slot, int pat_num );
typedef int (SUNVOX_FN_ATTR *tsv_get_pattern_lines)( int slot, int pat_num );
typedef const char* (SUNVOX_FN_ATTR *tsv_get_pattern_name)( int slot, int pat_num );
typedef sunvox_note* (SUNVOX_FN_ATTR *tsv_get_pattern_data)( int slot, int pat_num );
typedef int (SUNVOX_FN_ATTR *tsv_pattern_mute)( int slot, int pat_num, int mute );
typedef uint32_t (SUNVOX_FN_ATTR *tsv_get_ticks)( void );
typedef uint32_t (SUNVOX_FN_ATTR *tsv_get_ticks_per_second)( void );
typedef const char* (SUNVOX_FN_ATTR *tsv_get_log)( int size );

#ifdef SUNVOX_MAIN
    #define SV_FN_DECL
    #define SV_FN_DECL2 =0
#else
    #define SV_FN_DECL extern
    #define SV_FN_DECL2
#endif

SV_FN_DECL tsv_audio_callback sv_audio_callback SV_FN_DECL2;
SV_FN_DECL tsv_audio_callback2 sv_audio_callback2 SV_FN_DECL2;
SV_FN_DECL tsv_open_slot sv_open_slot SV_FN_DECL2;
SV_FN_DECL tsv_close_slot sv_close_slot SV_FN_DECL2;
SV_FN_DECL tsv_lock_slot sv_lock_slot SV_FN_DECL2;
SV_FN_DECL tsv_unlock_slot sv_unlock_slot SV_FN_DECL2;
SV_FN_DECL tsv_init sv_init SV_FN_DECL2;
SV_FN_DECL tsv_deinit sv_deinit SV_FN_DECL2;
SV_FN_DECL tsv_get_sample_rate sv_get_sample_rate SV_FN_DECL2;
SV_FN_DECL tsv_update_input sv_update_input SV_FN_DECL2;
SV_FN_DECL tsv_load sv_load SV_FN_DECL2;
SV_FN_DECL tsv_load_from_memory sv_load_from_memory SV_FN_DECL2;
SV_FN_DECL tsv_play sv_play SV_FN_DECL2;
SV_FN_DECL tsv_play_from_beginning sv_play_from_beginning SV_FN_DECL2;
SV_FN_DECL tsv_stop sv_stop SV_FN_DECL2;
SV_FN_DECL tsv_set_autostop sv_set_autostop SV_FN_DECL2;
SV_FN_DECL tsv_get_autostop sv_get_autostop SV_FN_DECL2;
SV_FN_DECL tsv_end_of_song sv_end_of_song SV_FN_DECL2;
SV_FN_DECL tsv_rewind sv_rewind SV_FN_DECL2;
SV_FN_DECL tsv_volume sv_volume SV_FN_DECL2;
SV_FN_DECL tsv_set_event_t sv_set_event_t SV_FN_DECL2;
SV_FN_DECL tsv_send_event sv_send_event SV_FN_DECL2;
SV_FN_DECL tsv_get_current_line sv_get_current_line SV_FN_DECL2;
SV_FN_DECL tsv_get_current_line2 sv_get_current_line2 SV_FN_DECL2;
SV_FN_DECL tsv_get_current_signal_level sv_get_current_signal_level SV_FN_DECL2;
SV_FN_DECL tsv_get_song_name sv_get_song_name SV_FN_DECL2;
SV_FN_DECL tsv_get_song_bpm sv_get_song_bpm SV_FN_DECL2;
SV_FN_DECL tsv_get_song_tpl sv_get_song_tpl SV_FN_DECL2;
SV_FN_DECL tsv_get_song_length_frames sv_get_song_length_frames SV_FN_DECL2;
SV_FN_DECL tsv_get_song_length_lines sv_get_song_length_lines SV_FN_DECL2;
SV_FN_DECL tsv_get_time_map sv_get_time_map SV_FN_DECL2;
SV_FN_DECL tsv_new_module sv_new_module SV_FN_DECL2;
SV_FN_DECL tsv_remove_module sv_remove_module SV_FN_DECL2;
SV_FN_DECL tsv_connect_module sv_connect_module SV_FN_DECL2;
SV_FN_DECL tsv_disconnect_module sv_disconnect_module SV_FN_DECL2;
SV_FN_DECL tsv_load_module sv_load_module SV_FN_DECL2;
SV_FN_DECL tsv_load_module_from_memory sv_load_module_from_memory SV_FN_DECL2;
SV_FN_DECL tsv_sampler_load sv_sampler_load SV_FN_DECL2;
SV_FN_DECL tsv_sampler_load_from_memory sv_sampler_load_from_memory SV_FN_DECL2;
SV_FN_DECL tsv_get_number_of_modules sv_get_number_of_modules SV_FN_DECL2;
SV_FN_DECL tsv_find_module sv_find_module SV_FN_DECL2;
SV_FN_DECL tsv_get_module_flags sv_get_module_flags SV_FN_DECL2;
SV_FN_DECL tsv_get_module_inputs sv_get_module_inputs SV_FN_DECL2;
SV_FN_DECL tsv_get_module_outputs sv_get_module_outputs SV_FN_DECL2;
SV_FN_DECL tsv_get_module_name sv_get_module_name SV_FN_DECL2;
SV_FN_DECL tsv_get_module_xy sv_get_module_xy SV_FN_DECL2;
SV_FN_DECL tsv_get_module_color sv_get_module_color SV_FN_DECL2;
SV_FN_DECL tsv_get_module_finetune sv_get_module_finetune SV_FN_DECL2;
SV_FN_DECL tsv_get_module_scope2 sv_get_module_scope2 SV_FN_DECL2;
SV_FN_DECL tsv_module_curve sv_module_curve SV_FN_DECL2;
SV_FN_DECL tsv_get_number_of_module_ctls sv_get_number_of_module_ctls SV_FN_DECL2;
SV_FN_DECL tsv_get_module_ctl_name sv_get_module_ctl_name SV_FN_DECL2;
SV_FN_DECL tsv_get_module_ctl_value sv_get_module_ctl_value SV_FN_DECL2;
SV_FN_DECL tsv_get_number_of_patterns sv_get_number_of_patterns SV_FN_DECL2;
SV_FN_DECL tsv_find_pattern sv_find_pattern SV_FN_DECL2;
SV_FN_DECL tsv_get_pattern_x sv_get_pattern_x SV_FN_DECL2;
SV_FN_DECL tsv_get_pattern_y sv_get_pattern_y SV_FN_DECL2;
SV_FN_DECL tsv_get_pattern_tracks sv_get_pattern_tracks SV_FN_DECL2;
SV_FN_DECL tsv_get_pattern_lines sv_get_pattern_lines SV_FN_DECL2;
SV_FN_DECL tsv_get_pattern_name sv_get_pattern_name SV_FN_DECL2;
SV_FN_DECL tsv_get_pattern_data sv_get_pattern_data SV_FN_DECL2;
SV_FN_DECL tsv_pattern_mute sv_pattern_mute SV_FN_DECL2;
SV_FN_DECL tsv_get_ticks sv_get_ticks SV_FN_DECL2;
SV_FN_DECL tsv_get_ticks_per_second sv_get_ticks_per_second SV_FN_DECL2;
SV_FN_DECL tsv_get_log sv_get_log SV_FN_DECL2;

#ifdef SUNVOX_MAIN

#ifdef OS_WIN
#define IMPORT( Handle, Type, Function, Store ) \
    { \
	Store = (Type)GetProcAddress( Handle, Function ); \
	if( Store == 0 ) { fn_not_found = Function; break; } \
    }
#define ERROR_MSG( msg ) MessageBoxA( 0, msg, "Error", MB_OK );
#endif

#ifdef OS_UNIX
#define IMPORT( Handle, Type, Function, Store ) \
    { \
	Store = (Type)dlsym( Handle, Function ); \
	if( Store == 0 ) { fn_not_found = Function; break; } \
    }
#define ERROR_MSG( msg ) printf( "ERROR: %s\n", msg );
#endif

#ifdef OS_UNIX
    void* g_sv_dll = NULL;
#endif

#ifdef OS_WIN
    HMODULE g_sv_dll = NULL;
#endif

int sv_load_dll2( LIBNAME_STR_TYPE filename )
{
#ifdef OS_WIN
    g_sv_dll = LoadLibrary( filename );
    if( g_sv_dll == 0 ) 
    {
        printf( "LoadLibrary() error %d\n", GetLastError() );
        ERROR_MSG( "can't load sunvox.dll" );
        return -1;
    }
#endif
#ifdef OS_UNIX
    g_sv_dll = dlopen( filename, RTLD_NOW );
    if( g_sv_dll == 0 )
    {
	printf( "%s\n", dlerror() );
        return -1;
    }
#endif
    const char* fn_not_found = NULL;
    while( 1 )
    {
	IMPORT( g_sv_dll, tsv_audio_callback, "sv_audio_callback", sv_audio_callback );
	IMPORT( g_sv_dll, tsv_audio_callback2, "sv_audio_callback2", sv_audio_callback2 );
	IMPORT( g_sv_dll, tsv_open_slot, "sv_open_slot", sv_open_slot );
	IMPORT( g_sv_dll, tsv_close_slot, "sv_close_slot", sv_close_slot );
	IMPORT( g_sv_dll, tsv_lock_slot, "sv_lock_slot", sv_lock_slot );
	IMPORT( g_sv_dll, tsv_unlock_slot, "sv_unlock_slot", sv_unlock_slot );
	IMPORT( g_sv_dll, tsv_init, "sv_init", sv_init );
	IMPORT( g_sv_dll, tsv_deinit, "sv_deinit", sv_deinit );
	IMPORT( g_sv_dll, tsv_get_sample_rate, "sv_get_sample_rate", sv_get_sample_rate );
	IMPORT( g_sv_dll, tsv_update_input, "sv_update_input", sv_update_input );
	IMPORT( g_sv_dll, tsv_load, "sv_load", sv_load );
	IMPORT( g_sv_dll, tsv_load_from_memory, "sv_load_from_memory", sv_load_from_memory );
	IMPORT( g_sv_dll, tsv_play, "sv_play", sv_play );
	IMPORT( g_sv_dll, tsv_play_from_beginning, "sv_play_from_beginning", sv_play_from_beginning );
	IMPORT( g_sv_dll, tsv_stop, "sv_stop", sv_stop );
	IMPORT( g_sv_dll, tsv_set_autostop, "sv_set_autostop", sv_set_autostop );
	IMPORT( g_sv_dll, tsv_get_autostop, "sv_get_autostop", sv_get_autostop );
	IMPORT( g_sv_dll, tsv_end_of_song, "sv_end_of_song", sv_end_of_song );
	IMPORT( g_sv_dll, tsv_rewind, "sv_rewind", sv_rewind );
	IMPORT( g_sv_dll, tsv_volume, "sv_volume", sv_volume );
	IMPORT( g_sv_dll, tsv_set_event_t, "sv_set_event_t", sv_set_event_t );
	IMPORT( g_sv_dll, tsv_send_event, "sv_send_event", sv_send_event );
	IMPORT( g_sv_dll, tsv_get_current_line, "sv_get_current_line", sv_get_current_line );
	IMPORT( g_sv_dll, tsv_get_current_line2, "sv_get_current_line2", sv_get_current_line2 );
	IMPORT( g_sv_dll, tsv_get_current_signal_level, "sv_get_current_signal_level", sv_get_current_signal_level );
	IMPORT( g_sv_dll, tsv_get_song_name, "sv_get_song_name", sv_get_song_name );
	IMPORT( g_sv_dll, tsv_get_song_bpm, "sv_get_song_bpm", sv_get_song_bpm );
	IMPORT( g_sv_dll, tsv_get_song_tpl, "sv_get_song_tpl", sv_get_song_tpl );
	IMPORT( g_sv_dll, tsv_get_song_length_frames, "sv_get_song_length_frames", sv_get_song_length_frames );
	IMPORT( g_sv_dll, tsv_get_song_length_lines, "sv_get_song_length_lines", sv_get_song_length_lines );
	IMPORT( g_sv_dll, tsv_get_time_map, "sv_get_time_map", sv_get_time_map );
	IMPORT( g_sv_dll, tsv_new_module, "sv_new_module", sv_new_module );
	IMPORT( g_sv_dll, tsv_remove_module, "sv_remove_module", sv_remove_module );
	IMPORT( g_sv_dll, tsv_connect_module, "sv_connect_module", sv_connect_module );
	IMPORT( g_sv_dll, tsv_disconnect_module, "sv_disconnect_module", sv_disconnect_module );
	IMPORT( g_sv_dll, tsv_load_module, "sv_load_module", sv_load_module );
	IMPORT( g_sv_dll, tsv_load_module_from_memory, "sv_load_module_from_memory", sv_load_module_from_memory );
	IMPORT( g_sv_dll, tsv_sampler_load, "sv_sampler_load", sv_sampler_load );
	IMPORT( g_sv_dll, tsv_sampler_load_from_memory, "sv_sampler_load_from_memory", sv_sampler_load_from_memory );
	IMPORT( g_sv_dll, tsv_get_number_of_modules, "sv_get_number_of_modules", sv_get_number_of_modules );
	IMPORT( g_sv_dll, tsv_find_module, "sv_find_module", sv_find_module );
	IMPORT( g_sv_dll, tsv_get_module_flags, "sv_get_module_flags", sv_get_module_flags );
	IMPORT( g_sv_dll, tsv_get_module_inputs, "sv_get_module_inputs", sv_get_module_inputs );
	IMPORT( g_sv_dll, tsv_get_module_outputs, "sv_get_module_outputs", sv_get_module_outputs );
	IMPORT( g_sv_dll, tsv_get_module_name, "sv_get_module_name", sv_get_module_name );
	IMPORT( g_sv_dll, tsv_get_module_xy, "sv_get_module_xy", sv_get_module_xy );
	IMPORT( g_sv_dll, tsv_get_module_color, "sv_get_module_color", sv_get_module_color );
	IMPORT( g_sv_dll, tsv_get_module_finetune, "sv_get_module_finetune", sv_get_module_finetune );
	IMPORT( g_sv_dll, tsv_get_module_scope2, "sv_get_module_scope2", sv_get_module_scope2 );
	IMPORT( g_sv_dll, tsv_module_curve, "sv_module_curve", sv_module_curve );
	IMPORT( g_sv_dll, tsv_get_number_of_module_ctls, "sv_get_number_of_module_ctls", sv_get_number_of_module_ctls );
	IMPORT( g_sv_dll, tsv_get_module_ctl_name, "sv_get_module_ctl_name", sv_get_module_ctl_name );
	IMPORT( g_sv_dll, tsv_get_module_ctl_value, "sv_get_module_ctl_value", sv_get_module_ctl_value );
	IMPORT( g_sv_dll, tsv_get_number_of_patterns, "sv_get_number_of_patterns", sv_get_number_of_patterns );
	IMPORT( g_sv_dll, tsv_find_pattern, "sv_find_pattern", sv_find_pattern );
	IMPORT( g_sv_dll, tsv_get_pattern_x, "sv_get_pattern_x", sv_get_pattern_x );
	IMPORT( g_sv_dll, tsv_get_pattern_y, "sv_get_pattern_y", sv_get_pattern_y );
	IMPORT( g_sv_dll, tsv_get_pattern_tracks, "sv_get_pattern_tracks", sv_get_pattern_tracks );
	IMPORT( g_sv_dll, tsv_get_pattern_lines, "sv_get_pattern_lines", sv_get_pattern_lines );
	IMPORT( g_sv_dll, tsv_get_pattern_name, "sv_get_pattern_name", sv_get_pattern_name );
	IMPORT( g_sv_dll, tsv_get_pattern_data, "sv_get_pattern_data", sv_get_pattern_data );
	IMPORT( g_sv_dll, tsv_pattern_mute, "sv_pattern_mute", sv_pattern_mute );
	IMPORT( g_sv_dll, tsv_get_ticks, "sv_get_ticks", sv_get_ticks );
	IMPORT( g_sv_dll, tsv_get_ticks_per_second, "sv_get_ticks_per_second", sv_get_ticks_per_second );
	IMPORT( g_sv_dll, tsv_get_log, "sv_get_log", sv_get_log );
	break;
    }
    if( fn_not_found )
    {
	char ts[ 256 ];
	sprintf( ts, "sunvox lib: %s() not found", fn_not_found );
	ERROR_MSG( ts );
	return -2;
    }
    
    return 0;
}

int sv_load_dll( void )
{
#ifdef OS_WIN
    return sv_load_dll2( TEXT(LIBNAME) );
#else
    return sv_load_dll2( LIBNAME );
#endif
    return -1111;
}

int sv_unload_dll( void )
{
#ifdef OS_UNIX
    if( g_sv_dll ) dlclose( g_sv_dll );
#endif
    return 0;
}

#else /* ... SUNVOX_MAIN */

int sv_load_dll2( LIBNAME_STR_TYPE filename );
int sv_load_dll( void );
int sv_unload_dll( void );

#endif /* ... not SUNVOX_MAIN */

#endif /* ... DYNAMIC LIBRARY */

#endif
