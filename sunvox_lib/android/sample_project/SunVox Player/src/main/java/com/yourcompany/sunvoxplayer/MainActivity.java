package com.yourcompany.sunvoxplayer;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;

import android.os.Bundle;
import android.app.Activity;
import android.content.res.Resources.NotFoundException;
import android.util.Log;
import android.view.Menu;
import android.view.View;

import nightradio.sunvoxlib.SunVoxLib;

public class MainActivity extends Activity {

	int sunvox_version = 0;
	
	@Override
	protected void onCreate( Bundle savedInstanceState ) 
	{
		super.onCreate( savedInstanceState );
		setContentView( R.layout.activity_main );
		
		sunvox_version = SunVoxLib.init( null, 44100, 2, 0 );
		if( sunvox_version > 0 )
	    {
	        int major = ( sunvox_version >> 16 ) & 255;
	        int minor1 = ( sunvox_version >> 8 ) & 255;
	        int minor2 = ( sunvox_version ) & 255;
	        Log.i( "SunVoxPlayer", "SunVox lib version: " + major + " " + minor1 + " " + minor2 );
	        
	        //Open audio slot #0:
	        SunVoxLib.open_slot( 0 );
	        
	        //Load test song from raw resource:
	        byte[] song_data = null;
	        try {
				song_data = convertStreamToByteArray( getResources().openRawResource( R.raw.test ) );
			} catch( NotFoundException e ) {
				Log.e( "SunVoxPlayer", "test.sunvox not found" );
			} catch( IOException e ) {
				Log.e( "SunVoxPlayer", "openRawResource error" );
			}
	        if( song_data != null )
	        {
	        	int rv = SunVoxLib.load_from_memory( 0, song_data );
	        	if( rv == 0 )
	        		Log.i( "SunVoxPlayer", "Song loaded" );
	        	else
	        		Log.e( "SunVoxPlayer", "Song load error " + rv );
	        }	        
	    }
		else
		{
			Log.e( "SunVoxPlayer", "Can't open SunVox library" );
		}
	}

	@Override
	public boolean onCreateOptionsMenu( Menu menu ) 
	{
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate( R.menu.activity_main, menu );
		return true;
	}
	
	@Override
	protected void onDestroy ()
	{
		if( sunvox_version > 0 )
		{
	        SunVoxLib.close_slot( 0 );
			SunVoxLib.deinit();
			Log.i( "SunVoxPlayer", "SunVox engine closed" );
		}
		super.onDestroy();
	}
	
	public void playButtonClick( View view )
	{
		//PLAY:
		if( sunvox_version > 0 )
		{
	        //Disable autostop:
	        SunVoxLib.set_autostop( 0, 0 );
	        
	        //Rewind (go to the line #0):
	        SunVoxLib.rewind( 0, 0 );
	        
	        //Set volume:
	        SunVoxLib.volume( 0, 256 );
	        
			/*
	        //Send some test NoteON event:
	        SunVoxLib.send_event( 0, 0, 64, 128, 6, 0, 0 );
	        
	        //Get some song info:
	        Log.i( "SunVoxPlayer", "Current line = " + SunVoxLib.get_current_line( 0 ) );
	        Log.i( "SunVoxPlayer", "Current signal level (for left channel) = " + SunVoxLib.get_current_signal_level( 0, 0 ) );
	        Log.i( "SunVoxPlayer", "Song name = " + SunVoxLib.get_song_name( 0 ) );
	        Log.i( "SunVoxPlayer", "Song BPM (Beats Per Minute) = " + SunVoxLib.get_song_bpm( 0 ) );
	        Log.i( "SunVoxPlayer", "Song TPL (Ticks Per Line) = " + SunVoxLib.get_song_tpl( 0 ) );
	        Log.i( "SunVoxPlayer", "Song length (frames) = " + SunVoxLib.get_song_length_frames( 0 ) );
	        Log.i( "SunVoxPlayer", "Song length (lines) = " + SunVoxLib.get_song_length_lines( 0 ) );
	        int mods = SunVoxLib.get_number_of_modules( 0 );
	        Log.i( "SunVoxPlayer", "Number of modules = " + mods );
	        for( int mod_num = 0; mod_num < mods; mod_num++ )
	        {
	        	Log.i( "SunVoxPlayer", "Module " + mod_num + "; Flags:" + SunVoxLib.get_module_flags( 0, mod_num ) + "; Name: " + SunVoxLib.get_module_name( 0, mod_num ) );
	        	int inputs[] = SunVoxLib.get_module_inputs( 0, mod_num );
	        	int outputs[] = SunVoxLib.get_module_outputs( 0, mod_num );
	        	if( inputs != null )
	        		Log.i( "SunVoxPlayer", "First input = " + inputs[ 0 ] );
	        	if( outputs != null )
	        		Log.i( "SunVoxPlayer", "First output = " + outputs[ 0 ] );
	        	int xy = SunVoxLib.get_module_xy( 0, mod_num );
	        	int x = xy & 0xFFFF;
	        	if( ( x & 0x8000 ) != 0 ) x |= 0xFFFF0000;
	        	int y = ( xy >> 16 ) & 0xFFFF;
	        	if( ( y & 0x8000 ) != 0 ) y |= 0xFFFF0000;
	        	Log.i( "SunVoxPlayer", "Module position: " + x + " " + y );
	        	int color = SunVoxLib.get_module_color( 0, mod_num );
	        	Log.i( "SunVoxPlayer", "Module color: R=" + ( color & 255 ) + " G=" + ( ( color >> 8 ) & 255 ) + " B=" + ( ( color >> 16 ) & 255 ) );
	        }
	        int pats = SunVoxLib.get_number_of_patterns( 0 );
	        Log.i( "SunVoxPlayer", "Number of patterns = " + pats );
	        for( int pat_num = 0; pat_num < pats; pat_num++ )
	        {
	        	int x = SunVoxLib.get_pattern_x( 0, pat_num );
	        	int y = SunVoxLib.get_pattern_y( 0, pat_num );
	        	int tracks = SunVoxLib.get_pattern_tracks( 0, pat_num );
	        	int lines = SunVoxLib.get_pattern_lines( 0, pat_num );
	        	Log.i( "SunVoxPlayer", "Pattern " + pat_num + ": " + x + " " + y + " " + tracks + " " + lines );
	        }
	        Log.i( "SunVoxPlayer", "SunVox tick counter = " + SunVoxLib.get_ticks() );
	        Log.i( "SunVoxPlayer", "SunVox ticks per second = " + SunVoxLib.get_ticks_per_second() );
	        
	        //Get pattern data:
	        int pattern_num = 0;
	        byte pat_data[] = SunVoxLib.get_pattern_data( 0, pattern_num );
	        if( pat_data != null )
	        {
	        	int tracks = SunVoxLib.get_pattern_tracks( 0, pattern_num );
	        	//Get pattern event on track 0, line 4:
	        	int track = 0;
	        	int line = 4;
	        	int ptr = ( line * tracks + track ) * 8;
	        	int note = pat_data[ ptr ]; //0 - nothing; 1..127 - note num; 128 - note off; 129, 130... - see NOTECMD_xxx defines
	        	int vel = pat_data[ ptr + 1 ]; //Velocity 1..129; 0 - default
	        	int module = pat_data[ ptr + 2 ]; //0 - nothing; 1..255 - module number
	        	int eff = pat_data[ ptr + 4 ]; //Std. effect
	        	int ctl = pat_data[ ptr + 5 ]; //Number of controller
	        	int ctl_val = pat_data[ ptr + 6 ] + ( pat_data[ ptr + 7 ] << 8 ); //Controller value
	        	Log.i( "SunVoxPlayer", "EVT " + note + " " + vel + " " + module + " " + eff + " " + ctl + " " + ctl_val );
	        	
	        	//Save pattern data:
	        	SunVoxLib.set_pattern_data( 0, pattern_num, pat_data );
	        }
	        
	        //Get a scope buffer of the Output module:
	        short[] scope = new short[ 4096 ];
	        int received = SunVoxLib.get_module_scope( 0, 0, 0, scope, 4096 );
	        Log.i( "SunVoxPlayer", "Scope. Samples received: " + received );
	        if( received >= 4 )
	        	Log.i( "SunVoxPlayer", "Scope samples: " + scope[ 0 ] + " " + scope[ 1 ] + " " + scope[ 2 ] + " " + scope[ 3 ] );

	        //Read curve 1 from the MultiSynth module:
	        SunVoxLib.lock_slot( 0 );
	        int multisynth = SunVoxLib.new_module( 0, "MultiSynth", "MultiSynth", 0, 0, 0 );
			SunVoxLib.unlock_slot( 0 );
			float[] curve_data = new float[ 1024 ];
			received = SunVoxLib.module_curve( 0, multisynth, 1, curve_data, 0, 0 );
			Log.i( "SunVoxPlayer", "MultiSynth curve 1 length: " + received );
			for( int i = 0; i < received && i < 16; i++ )
				Log.i( "SunVoxPlayer", "curve 1. item " + i + " = " + curve_data[ i ] );
			*/

	        //Play from beginning:
	        SunVoxLib.play_from_beginning( 0 );	        
		}
	}

	public void stopButtonClick( View view )
	{
		//STOP:
		if( sunvox_version > 0 )
		{
			SunVoxLib.stop( 0 );
		}
	}
	
	private byte[] convertStreamToByteArray( InputStream is ) throws IOException 
	{
	    ByteArrayOutputStream baos = new ByteArrayOutputStream();
	    byte[] buff = new byte[ 10240 ];
	    int i = Integer.MAX_VALUE;
	    while( ( i = is.read( buff, 0, buff.length ) ) > 0 ) 
	    {
	        baos.write( buff, 0, i );
	    }
	    return baos.toByteArray(); // be sure to close InputStream in calling function
	}

}
