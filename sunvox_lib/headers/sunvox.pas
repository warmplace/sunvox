unit sunvox;
//sunvox.h 1.7.3b translation by laggyluk.
//tested on windows and linux.
//2013 - 2019: updated by NightRadio

{$mode objfpc}{$H+}

{$define SUNVOX_STATIC_LIB}//comment out for dynamic loading

interface

uses
  {$IFDEF Windows}
  windows,
  {$endif}
  Classes, SysUtils, dynlibs;

const
{$IFDEF Windows}
    LIBNAME = 'sunvox.dll';
{$ENDIF}
{$IFDEF Darwin}
    LIBNAME = 'sunvox.dylib' ;
{$ENDIF}
{$IFDEF LINUX}
    LIBNAME = './sunvox.so';
{$ENDIF}

   NOTECMD_NOTE_OFF = 128;
   NOTECMD_ALL_NOTES_OFF = 129; //* notes of all synths off */
   NOTECMD_CLEAN_SYNTHS	= 130; //* stop and clean all synths */
   NOTECMD_STOP	=	131;
   NOTECMD_PLAY	=	132;

   SV_INIT_FLAG_NO_DEBUG_OUTPUT =	( 1 << 0 );
   SV_INIT_FLAG_USER_AUDIO_CALLBACK =	( 1 << 1 ); //* Interaction with sound card is on the user side */
   SV_INIT_FLAG_AUDIO_INT16 =		( 1 << 2 );
   SV_INIT_FLAG_AUDIO_FLOAT32 	=	( 1 << 3 );
   SV_INIT_FLAG_ONE_THREAD	=		( 1 << 4 ); //* Audio callback and song modification functions are in single thread */

   SV_MODULE_FLAG_EXISTS= ( 1 << 0 );
   SV_MODULE_FLAG_EFFECT= ( 1 << 1 );
   SV_MODULE_FLAG_MUTE= ( 1 << 2 );
   SV_MODULE_FLAG_SOLO= ( 1 << 3 );
   SV_MODULE_FLAG_BYPASS= ( 1 << 4 );
   SV_MODULE_INPUTS_OFF= 16;
   SV_MODULE_INPUTS_MASK= ( 255 << SV_MODULE_INPUTS_OFF ) ;
   SV_MODULE_OUTPUTS_OFF= ( 16 + 8 );
   SV_MODULE_OUTPUTS_MASK =( 255 << SV_MODULE_OUTPUTS_OFF );

   SV_STYPE_INT16 =0;
   SV_STYPE_INT32 =1;
   SV_STYPE_FLOAT32 =2;
   SV_STYPE_FLOAT64 =3;

type
//sv_send_event() parameters:
//  slot;
//  track_num: from 0 to 15;
//  note: 0 - nothing; 1..127 - note num; 128 - note off; 129, 130... - see NOTECMD_xxx defines;
//  vel: velocity 1..129; 0 - default;
//  module: 0 - nothing; 1..255 - module number;
//  ctl: CCXX. CC - number of controller. XX - std effect;
//  ctl_val: value of controller.

  sunvox_note = record
    note:ShortInt;           //0 - nothing; 1..127 - note num; 128 - note off; 129, 130... - see NOTECMD_xxx defines
    vel:ShortInt;            //Velocity 1..129; 0 - default
    module:ShortInt;         //0 - nothing; 1..255 - module number
    nothing:ShortInt;
    ctl:ShortInt;            //CCEE. CC - number of controller. EE - std effect
    ctl_val:ShortInt;        //XXYY. Value of controller
  end;
  pSunvox_note = ^sunvox_note;

{$ifdef SUNVOX_STATIC_LIB}
//sv_audio_callback() - get the next piece of SunVox audio.
//With sv_audio_callback() you can ignore the built-in SunVox sound output mechanism and use some other sound system.
//Set SV_INIT_FLAG_USER_AUDIO_CALLBACK flag in sv_init() if you want to use sv_audio_callback() function.
//Parameters:
//  buf - destination buffer of type signed short (if SV_INIT_FLAG_AUDIO_INT16 used in sv_init())
//        or float (if SV_INIT_FLAG_AUDIO_FLOAT32 used in sv_init());
//  	  stereo data will be interleaved in this buffer: LRLR... ; where the LR is the one frame (Left+Right channels);
//  frames - number of frames in destination buffer;
//  latency - audio latency (in frames);
//  out_time - output time (in ticks).
function sv_audio_callback( buf:pointer; frames, latency:integer; out_time:cardinal):integer;stdcall; external LIBNAME;
function sv_audio_callback2( buf:pointer; frames, latency:integer; out_time:cardinal; in_type, in_channels:integer; in_buf:pointer):integer;stdcall; external LIBNAME;
function sv_open_slot( slot:integer ):integer;  stdcall; external LIBNAME;
function sv_close_slot( slot:integer ):integer;  stdcall; external LIBNAME;
function sv_lock_slot( slot:integer ):integer;  stdcall; external LIBNAME;
function sv_unlock_slot( slot:integer ):integer;  stdcall; external LIBNAME;
function sv_init( const config:pchar; freq, channels, flags:integer ):integer; stdcall; external LIBNAME;
function sv_deinit:integer; stdcall; external LIBNAME;
function sv_get_sample_rate:integer; stdcall; external LIBNAME;
function sv_update_input:integer; stdcall; external LIBNAME;
function sv_load( slot:integer; const name:pchar ):integer; stdcall; external LIBNAME;
function sv_load_from_memory( slot:integer; data:pointer; data_size:cardinal ):integer; stdcall; external LIBNAME;
function sv_play( slot:integer ):integer; stdcall; external LIBNAME;
function sv_play_from_beginning( slot:integer):integer; stdcall; external LIBNAME;
function sv_stop( slot:integer ):integer; stdcall; external LIBNAME;
//autostop values: 0 - disable autostop; 1 - enable autostop.
//When disabled, song is playing infinitely in the loop.
function sv_set_autostop( slot, autostop :integer):integer; stdcall; external LIBNAME;
function sv_get_autostop( slot:integer ):integer; stdcall; external LIBNAME;
//sv_end_of_song() return values: 0 - song is playing now; 1 - stopped.
function sv_end_of_song(slot:integer):integer;  stdcall; external LIBNAME;
function sv_rewind(slot:integer; line_num:integer ):integer; stdcall; external LIBNAME;
function sv_volume( slot:integer; vol:integer ):integer; stdcall; external LIBNAME;
function sv_set_event_t(slot:integer; set, t :integer):integer; stdcall; external LIBNAME;
function sv_send_event(slot:integer; track_num, note, vel, module, ctl, ctl_val :integer):integer; stdcall; external LIBNAME;
function sv_get_current_line( slot:integer):integer; stdcall; external LIBNAME;
function sv_get_current_line2( slot:integer):integer; stdcall; external LIBNAME;
function sv_get_current_signal_level(slot:integer; channel:integer ):integer; stdcall; external LIBNAME; //From 0 to 255
function sv_get_song_name( slot:integer):pchar; stdcall; external LIBNAME;
function sv_get_song_bpm(slot:integer):integer; stdcall; external LIBNAME;
function sv_get_song_tpl(slot:integer):integer; stdcall; external LIBNAME;
//Frame is one discrete of the sound. Sample rate 44100 Hz means, that you hear 44100 frames per second.
function sv_get_song_length_frames( slot:integer ):cardinal; stdcall; external LIBNAME;
function sv_get_song_length_lines( slot:integer ):cardinal; stdcall; external LIBNAME;
function sv_get_number_of_modules( slot:integer ):integer; stdcall; external LIBNAME;
function sv_find_module( slot:integer; const name:pchar ):integer; stdcall; external LIBNAME;
function sv_get_module_flags( slot:integer; mod_num:integer ):integer; stdcall; external LIBNAME;
function sv_get_module_inputs( slot:integer; mod_num:integer):pinteger; stdcall; external LIBNAME;
function sv_get_module_outputs( slot:integer; mod_num:integer):pinteger; stdcall; external LIBNAME;
function sv_get_module_name( slot:integer; mod_num:integer):pchar; stdcall; external LIBNAME;
function sv_get_module_xy( slot:integer; mod_num:integer):cardinal; stdcall; external LIBNAME;
function sv_get_module_color( slot:integer; mod_num:integer):integer; stdcall; external LIBNAME;
function sv_get_module_finetune( slot:integer; mod_num:integer):cardinal; stdcall; external LIBNAME;
//sv_get_module_scope2() return value = received number of samples (may be less or equal to samples_to_read).
function sv_get_module_scope2(slot, mod_num, channel:integer; read_buf:pSmallInt; samples_to_read:cardinal ):cardinal; stdcall; external LIBNAME;
function sv_get_number_of_patterns( slot:integer ):integer; stdcall; external LIBNAME;
function sv_find_pattern( slot:integer; const name:pchar ):integer; stdcall; external LIBNAME;
function sv_get_pattern_x( slot,pat_num:integer):integer; stdcall; external LIBNAME;
function sv_get_pattern_y(slot,pat_num:integer):integer; stdcall; external LIBNAME;
function sv_get_pattern_tracks(slot,pat_num:integer):integer; stdcall; external LIBNAME;
function sv_get_pattern_lines(slot,pat_num:integer):integer; stdcall; external LIBNAME;
function sv_get_pattern_name( slot:integer; pat_num:integer):pchar; stdcall; external LIBNAME;
function sv_get_pattern_data(slot,pat_num:integer):pSunvox_note; stdcall; external LIBNAME;
function sv_pattern_mute( slot, pat_num, mute:integer ):integer; stdcall; external LIBNAME; //Use it with sv_lock_slot() and sv_unlock_slot()
//SunVox engine uses its own time space, measured in ticks.
//Use sv_get_ticks() to get current tick counter (from 0 to 0xFFFFFFFF).
//Use sv_get_ticks_per_second() to get the number of SunVox ticks per second.
function sv_get_ticks:cardinal; stdcall; external LIBNAME;
function sv_get_ticks_per_second:cardinal; stdcall; external LIBNAME;
function sv_get_log( size:integer ):pchar; stdcall; external LIBNAME;

function sv_load_dll:integer;
function sv_unload_dll:integer;

implementation
function sv_load_dll:integer;
begin
  //static lib so..
  result:=0;
end;

function sv_unload_dll:integer;
begin
  result:=0;
end;
begin
end.

{$else}
type
  tsv_audio_callback = function ( buf:pointer; frames, latency:integer; out_time:cardinal):integer;stdcall;
  tsv_audio_callback2 = function ( buf:pointer; frames, latency:integer; out_time:cardinal; in_type, in_channels:integer; in_buf:pointer):integer;stdcall;
  tsv_open_slot = function ( slot:integer ):integer;  stdcall;
  tsv_close_slot = function ( slot:integer ):integer;  stdcall ;
  tsv_lock_slot = function ( slot:integer ):integer;  stdcall ;
  tsv_unlock_slot = function ( slot:integer ):integer;  stdcall ;
  tsv_init = function ( const config:pchar; freq, channels, flags:integer ):integer; stdcall ;
  tsv_deinit = function :integer; stdcall ;
  tsv_get_sample_rate = function :integer; stdcall ;
  tsv_update_input = function :integer; stdcall ;
  tsv_load = function ( slot:integer; const name:pchar ):integer; stdcall ;
  tsv_load_from_memory = function ( slot:integer; data:pointer; data_size:cardinal ):integer; stdcall ;
  tsv_play = function ( slot:integer ):integer; stdcall ;
  tsv_play_from_beginning = function ( slot:integer):integer; stdcall ;
  tsv_stop = function ( slot:integer ):integer; stdcall ;
  //autostop values: 0 - disable autostop; 1 - enable autostop.
  //When disabled, song is playing infinitely in the loop.
  tsv_set_autostop = function ( slot, autostop :integer):integer; stdcall ;
  tsv_get_autostop = function ( slot:integer ):integer; stdcall ;
  //sv_end_of_song() return values: 0 - song is playing now; 1 - stopped.
  tsv_end_of_song = function (slot:integer):integer;  stdcall ;
  tsv_rewind = function (slot:integer; t:integer ):integer; stdcall ;
  tsv_volume = function ( slot:integer; vol:integer ):integer; stdcall ;
  tsv_set_event_t = function (slot:integer; set, t :integer):integer; stdcall ;
  tsv_send_event = function (slot:integer; track_num, note, vel, module, ctl, ctl_val :integer):integer; stdcall ;
  tsv_get_current_line = function ( slot:integer):integer; stdcall ;
  tsv_get_current_line2 = function ( slot:integer):integer; stdcall ;
  tsv_get_current_signal_level = function (slot:integer; channel:integer ):integer; stdcall ; //From 0 to 255
  tsv_get_song_name = function ( slot:integer):pchar; stdcall ;
  tsv_get_song_bpm = function (slot:integer):integer; stdcall ;
  tsv_get_song_tpl = function (slot:integer):integer; stdcall ;
  //Frame is one discrete of the sound. Sample rate 44100 Hz means, that you hear 44100 frames per second.
  tsv_get_song_length_frames = function ( slot:integer ):cardinal; stdcall ;
  tsv_get_song_length_lines = function ( slot:integer ):cardinal; stdcall ;
  tsv_get_number_of_modules = function ( slot:integer ):integer; stdcall ;
  tsv_find_module = function ( slot:integer; const name:pchar ):integer; stdcall ;
  tsv_get_module_flags = function ( slot:integer; mod_num:integer ):integer; stdcall ;
  tsv_get_module_inputs = function ( slot:integer; mod_num:integer):pinteger; stdcall ;
  tsv_get_module_outputs =  function ( slot:integer; mod_num:integer):pinteger; stdcall ;
  tsv_get_module_name = function ( slot:integer; mod_num:integer):pchar; stdcall ;
  tsv_get_module_xy = function ( slot:integer; mod_num:integer):cardinal; stdcall ;
  tsv_get_module_color = function ( slot:integer; mod_num:integer):integer; stdcall ;
  tsv_get_module_finetune = function ( slot:integer; mod_num:integer):cardinal; stdcall ;
  //sv_get_module_scope2() return value = received number of samples (may be less or equal to samples_to_read).
  tsv_get_module_scope2 = function (slot, mod_num, channel:integer; read_buf:pSmallInt; samples_to_read:cardinal ):cardinal; stdcall ;
  tsv_get_number_of_patterns = function ( slot:integer ):integer; stdcall ;
  tsv_find_pattern = function ( slot:integer; const name:pchar ):integer; stdcall ;
  tsv_get_pattern_x = function ( slot,pat_num:integer):integer; stdcall ;
  tsv_get_pattern_y = function (slot,pat_num:integer):integer; stdcall ;
  tsv_get_pattern_tracks = function (slot,pat_num:integer):integer; stdcall ;
  tsv_get_pattern_lines = function (slot,pat_num:integer):integer; stdcall ;
  tsv_get_pattern_name = function (slot,pat_num:integer):pchar; stdcall ;
  tsv_get_pattern_data = function (slot,pat_num:integer):pSunvox_note; stdcall ;
  tsv_pattern_mute = function ( slot, pat_num, mute:integer ):integer; stdcall ; //Use it with sv_lock_slot() and sv_unlock_slot()
  //SunVox engine uses its own time space, measured in ticks.
  //Use sv_get_ticks() to get current tick counter (from 0 to 0xFFFFFFFF).
  //Use sv_get_ticks_per_second() to get the number of SunVox ticks per second.
  tsv_get_ticks = function :cardinal; stdcall ;
  tsv_get_ticks_per_second = function :cardinal; stdcall ;
  tsv_get_log = function :cardinal; stdcall ;

var
   sv_audio_callback:tsv_audio_callback;
   sv_audio_callback2:tsv_audio_callback2;
   sv_open_slot:tsv_open_slot;
   sv_close_slot:tsv_close_slot;
   sv_lock_slot:tsv_lock_slot;
   sv_unlock_slot:tsv_unlock_slot;
   sv_init:tsv_init;
   sv_deinit:tsv_deinit;
   sv_get_sample_rate:tsv_get_sample_rate;
   sv_update_input:tsv_update_input;
   sv_load:tsv_load;
   sv_load_from_memory: tsv_load_from_memory;
   sv_play : tsv_play;
   sv_play_from_beginning : tsv_play_from_beginning;
   sv_stop : tsv_stop;
   sv_set_autostop : tsv_set_autostop;
   sv_get_autostop : tsv_get_autostop;
   sv_end_of_song : tsv_end_of_song;
   sv_rewind : tsv_rewind;
   sv_volume : tsv_volume;
   sv_set_event_t : tsv_set_event_t ;
   sv_send_event : tsv_send_event ;
   sv_get_current_line :tsv_get_current_line ;
   sv_get_current_line2 :tsv_get_current_line2 ;
   sv_get_current_signal_level: tsv_get_current_signal_level;
   sv_get_song_name : tsv_get_song_name;
   sv_get_song_bpm : tsv_get_song_bpm;
   sv_get_song_tpl: tsv_get_song_tpl ;
   sv_get_song_length_frames : tsv_get_song_length_frames;
   sv_get_song_length_lines : tsv_get_song_length_lines;
   sv_get_number_of_modules : tsv_get_number_of_modules;
   sv_find_module : tsv_find_module;
   sv_get_module_flags : tsv_get_module_flags;
   sv_get_module_inputs : tsv_get_module_inputs;
   sv_get_module_outputs: tsv_get_module_outputs;
   sv_get_module_name: tsv_get_module_name;
   sv_get_module_xy : tsv_get_module_xy;
   sv_get_module_color: tsv_get_module_color;
   sv_get_module_finetune : tsv_get_module_finetune;
   sv_get_module_scope2 : tsv_get_module_scope2;
   sv_get_number_of_patterns : tsv_get_number_of_patterns;
   sv_find_pattern : tsv_find_pattern;
   sv_get_pattern_x: tsv_get_pattern_x;
   sv_get_pattern_y : tsv_get_pattern_y;
   sv_get_pattern_tracks: tsv_get_pattern_tracks;
   sv_get_pattern_lines: tsv_get_pattern_lines;
   sv_get_pattern_name: tsv_get_pattern_name;
   sv_get_pattern_data : tsv_get_pattern_data;
   sv_pattern_mute : tsv_pattern_mute;
   sv_get_ticks : tsv_get_ticks;
   sv_get_ticks_per_second : tsv_get_ticks_per_second;
   sv_get_log : tsv_get_log;

function sv_load_dll:integer;
function sv_unload_dll:integer;

implementation
var
 fn_not_found: pchar = nil;
 g_sv_dll: TLibHandle;

 function IMPORT( Functio:pchar):pointer;
 begin
   	result := GetProcedureAddress( g_sv_dll, Functio );
   	if result = nil then fn_not_found := Functio;
 end;

function sv_load_dll:integer;
begin
  g_sv_dll := LoadLibrary( LIBNAME);
  if( g_sv_dll = 0 )then
    begin
    //showmessage( 'some sunvox lib loading error ');
    result:= 1;
    exit;
  end;
   //while true do
  begin
  sv_audio_callback:=tsv_audio_callback(import('sv_audio_callback' ));
  sv_audio_callback2:=tsv_audio_callback2(import('sv_audio_callback2' ));
  sv_open_slot:=tsv_open_slot(import('sv_open_slot' ));
  sv_close_slot:=tsv_close_slot(import('sv_close_slot' ));
  sv_lock_slot:=tsv_lock_slot(import('sv_lock_slot' ));
  sv_unlock_slot:=tsv_unlock_slot(import('sv_unlock_slot' ));
  sv_init:=tsv_init(import( 'sv_init' ));
  sv_deinit:=tsv_deinit(import( 'sv_deinit' ));
  sv_get_sample_rate:=tsv_get_sample_rate(import( 'sv_get_sample_rate' ));
  sv_update_input:=tsv_update_input(import( 'sv_update_input' ));
  sv_load:=tsv_load(import('sv_load' ));
  sv_load_from_memory:=tsv_load_from_memory(import('sv_load_from_memory' ));
  sv_play:=tsv_play(import( 'sv_play' ));
  sv_play_from_beginning:=tsv_play_from_beginning(import('sv_play_from_beginning' ));
  sv_stop:=tsv_stop(import( 'sv_stop' ));
  sv_set_autostop:=tsv_set_autostop(import( 'sv_set_autostop' ));
  sv_get_autostop:=tsv_get_autostop(import( 'sv_get_autostop' ));
  sv_end_of_song:=tsv_end_of_song(import( 'sv_end_of_song' ));
  sv_rewind:=tsv_rewind(import( 'sv_rewind' ));
  sv_volume:=tsv_volume(import('sv_volume' ));
  sv_set_event_t:=tsv_set_event_t(import('sv_set_event_t' ));
  sv_send_event:=tsv_send_event(import('sv_send_event' ));
  sv_get_current_line:=tsv_get_current_line(import('sv_get_current_line' ));
  sv_get_current_line2:=tsv_get_current_line2(import('sv_get_current_line2' ));
  sv_get_current_signal_level:=tsv_get_current_signal_level(import('sv_get_current_signal_level' ));
  sv_get_song_name:=tsv_get_song_name(import('sv_get_song_name' ));
  sv_get_song_bpm:=tsv_get_song_bpm(import('sv_get_song_bpm' ));
  sv_get_song_tpl:=tsv_get_song_tpl(import('sv_get_song_tpl' ));
  sv_get_song_length_frames:=tsv_get_song_length_frames(import('sv_get_song_length_frames' ));
  sv_get_song_length_lines:=tsv_get_song_length_lines(import('sv_get_song_length_lines' ));
  sv_get_number_of_modules:=tsv_get_number_of_modules(import('sv_get_number_of_modules' ));
  sv_find_module:=tsv_find_module(import('sv_find_module' ));
  sv_get_module_flags:=tsv_get_module_flags(import('sv_get_module_flags' ));
  sv_get_module_inputs:=tsv_get_module_inputs(import('sv_get_module_inputs' ));
  sv_get_module_outputs:=tsv_get_module_outputs(import('sv_get_module_outputs' ));
  sv_get_module_name:=tsv_get_module_name(import('sv_get_module_name' ));
  sv_get_module_xy:=tsv_get_module_xy(import('sv_get_module_xy' ));
  sv_get_module_color:=tsv_get_module_color(import('sv_get_module_color' ));
  sv_get_module_finetune:=tsv_get_module_finetune(import('sv_get_module_finetune' ));
  sv_get_module_scope2:=tsv_get_module_scope2(import('sv_get_module_scope2' ));
  sv_get_number_of_patterns:=tsv_get_number_of_patterns(import('sv_get_number_of_patterns' ));
  sv_find_pattern:=tsv_find_pattern(import('sv_find_pattern' ));
  sv_get_pattern_x:=tsv_get_pattern_x(import('sv_get_pattern_x' ));
  sv_get_pattern_y:=tsv_get_pattern_y(import('sv_get_pattern_y' ));
  sv_get_pattern_tracks:=tsv_get_pattern_tracks(import('sv_get_pattern_tracks' ));
  sv_get_pattern_lines:=tsv_get_pattern_lines(import('sv_get_pattern_lines' ));
  sv_get_pattern_name:=tsv_get_pattern_name(import('sv_get_pattern_name' ));

  sv_get_pattern_data:=tsv_get_pattern_data(import('sv_get_pattern_data' ));
  sv_pattern_mute:=tsv_pattern_mute(import('sv_pattern_mute' ));
  sv_get_ticks:=tsv_get_ticks(import('sv_get_ticks' ));
  sv_get_ticks_per_second:=tsv_get_ticks_per_second(import('sv_get_ticks_per_second' ));
  sv_get_log:=tsv_get_log(import('sv_get_log' ));
  end;
  if( fn_not_found<>nil ) then
    begin
	//    showmessage('sunvox lib: not found');
    	result:= -1;
    end;
    result:= 0;
end;

function sv_unload_dll:integer;
begin
  result:= integer(UnloadLibrary( g_sv_dll ));
end;

begin

end.

{$endif}   //...SUNVOX_STATIC_LIB
//{$endif}

