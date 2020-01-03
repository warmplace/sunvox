package nightradio.sunvoxlib;

public class SunVoxLib
{
	static 
	{
        System.loadLibrary( "sunvox" );
    }
	
	public static final int SV_INIT_FLAG_NO_DEBUG_OUTPUT = 1 << 0;
	public static final int SV_INIT_FLAG_USER_AUDIO_CALLBACK = 1 << 1;
	public static final int SV_INIT_FLAG_AUDIO_INT16 = 1 << 2;
	public static final int SV_INIT_FLAG_AUDIO_FLOAT32 = 1 << 3;
	public static final int SV_INIT_FLAG_ONE_THREAD = 1 << 4;

	public static final int SV_MODULE_FLAG_EXISTS = 1 << 0;
	public static final int SV_MODULE_FLAG_EFFECT = 1 << 1;
	public static final int SV_MODULE_FLAG_MUTE = 1 << 2;
	public static final int SV_MODULE_FLAG_SOLO = 1 << 3;
	public static final int SV_MODULE_FLAG_BYPASS = 1 << 4;
	public static final int SV_MODULE_INPUTS_OFF = 16;
	public static final int SV_MODULE_INPUTS_MASK = 255 << SV_MODULE_INPUTS_OFF;
	public static final int SV_MODULE_OUTPUTS_OFF = 16 + 8;
	public static final int SV_MODULE_OUTPUTS_MASK = 255 << SV_MODULE_OUTPUTS_OFF;

	public static final int SV_STYPE_INT16 = 0;
	public static final int SV_STYPE_INT32 = 1;
	public static final int SV_STYPE_FLOAT32 = 2;
	public static final int SV_STYPE_FLOAT64 = 3;

	public static native int init( String config, int freq, int channels, int flags );
	public static native int deinit();
	public static native int get_sample_rate();
	public static native int update_input();
	public static native int audio_callback( byte[] buf, int frames, int latency, int out_time );
	public static native int audio_callback2( byte[] buf, int frames, int latency, int out_time, int in_type, int in_channels, byte[] in_buf );
	public static native int open_slot( int slot );
	public static native int close_slot( int slot );
	public static native int lock_slot( int slot );
	public static native int unlock_slot( int slot );
	public static native int get_sample_type();
	public static native int load( int slot, String name );
	public static native int load_from_memory( int slot, byte[] data );
	public static native int play( int slot );
	public static native int play_from_beginning( int slot );
	public static native int stop( int slot );
	public static native int set_autostop( int slot, int autostop );
	public static native int get_autostop( int slot );
	public static native int end_of_song( int slot );
	public static native int rewind( int slot, int line_num );
	public static native int volume( int slot, int vol );
	public static native int set_event_t( int slot, int set, int t );
	public static native int send_event( int slot, int track_num, int note, int vel, int module, int ctl, int ctl_val );
	public static native int get_current_line( int slot );
	public static native int get_current_line2( int slot );
	public static native int get_current_signal_level( int slot, int channel );
	public static native String get_song_name( int slot );
	public static native int get_song_bpm( int slot );
	public static native int get_song_tpl( int slot );
	public static native int get_song_length_frames( int slot );
	public static native int get_song_length_lines( int slot );
	public static native int get_time_map( int slot, int start_line, int len, int[] dest, int flags );
	public static native int new_module( int slot, String type, String name, int x, int y, int z );
	public static native int remove_module( int slot, int mod_num );
	public static native int connect_module( int slot, int source, int destination );
	public static native int disconnect_module( int slot, int source, int destination );
	public static native int load_module( int slot, String name, int x, int y, int z );
	public static native int load_module_from_memory( int slot, byte[] data, int x, int y, int z );
	public static native int sampler_load( int slot, int sampler_module, String name, int sample_slot );
	public static native int sampler_load_from_memory( int slot, int sampler_module, byte[] data, int sample_slot );
	public static native int get_number_of_modules( int slot );
	public static native int find_module( int slot, String name );
	public static native int get_module_flags( int slot, int mod_num );
	public static native int[] get_module_inputs( int slot, int mod_num );
	public static native int[] get_module_outputs( int slot, int mod_num );
	public static native String get_module_name( int slot, int mod_num );
	public static native int get_module_xy( int slot, int mod_num );
	public static native int get_module_color( int slot, int mod_num );
	public static native int get_module_finetune( int slot, int mod_num );
	public static native int get_module_scope( int slot, int mod_num, int channel, short[] dest_buf, int samples_to_read );
	public static native int module_curve( int slot, int mod_num, int curve_num, float[] data, int len, int w );
	public static native int get_number_of_module_ctls( int slot, int mod_num );
	public static native String get_module_ctl_name( int slot, int mod_num, int ctl_num );
	public static native int get_module_ctl_value( int slot, int mod_num, int ctl_num, int scaled );
	public static native int get_number_of_patterns( int slot );
	public static native int find_pattern( int slot, String name );
	public static native int get_pattern_x( int slot, int pat_num );
	public static native int get_pattern_y( int slot, int pat_num );
	public static native int get_pattern_tracks( int slot, int pat_num );
	public static native int get_pattern_lines( int slot, int pat_num );
	public static native String get_pattern_name( int slot, int pat_num );
	public static native byte[] get_pattern_data( int slot, int pat_num );
	public static native int set_pattern_data( int slot, int pat_num, byte[] pat_data );
	public static native int pattern_mute( int slot, int pat_num, int mute );
	public static native int get_ticks();
	public static native int get_ticks_per_second();
	public static native String get_log( int size );
}

