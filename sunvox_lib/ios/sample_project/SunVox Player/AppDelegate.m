//
//  AppDelegate.m
//  SunVox Player
//
//  Created by Alexander Zolotov on 29.04.13.
//  Copyright (c) 2013 Alexander Zolotov. All rights reserved.
//

#import "AppDelegate.h"

#import "ViewController.h"

#define SUNVOX_STATIC_LIB
#import "sunvox.h"

int g_sunvox_version = -1;

@implementation AppDelegate

- (void)dealloc
{
    [_window release];
    [_viewController release];
    [super dealloc];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    self.window = [[[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]] autorelease];
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
	self.viewController = [[[ViewController alloc] initWithNibName:@"ViewController_iPhone" bundle:nil] autorelease];
    } else {
	self.viewController = [[[ViewController alloc] initWithNibName:@"ViewController_iPad" bundle:nil] autorelease];
    }
    self.window.rootViewController = self.viewController;
    [self.window makeKeyAndVisible];
    
    g_sunvox_version = sv_init( 0, 44100, 2, 0 );
    while( g_sunvox_version >= 0 )
    {
	int major = ( g_sunvox_version >> 16 ) & 255;
	int minor1 = ( g_sunvox_version >> 8 ) & 255;
	int minor2 = ( g_sunvox_version ) & 255;
	printf( "SunVox lib version: %d.%d.%d\n", major, minor1, minor2 );
	
	int rv = sv_open_slot( 0 );
	if( rv ) { printf( "sv_open_slot() error %d\n", rv ); break; }
	
	printf( "Loading SunVox song from file...\n" );
	const char* song_path_utf8 = [ [ NSString stringWithFormat:@"%@/test.sunvox", [ [ NSBundle mainBundle ] resourcePath ] ] UTF8String ];
	if( sv_load( 0, song_path_utf8 ) == 0 )
	    printf( "Loaded.\n" );
	else
	    printf( "Load error.\n" );
	sv_volume( 0, 256 );
	
	sv_play_from_beginning( 0 );
	
	break;
    }
    
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    if( g_sunvox_version >= 0 )
    {
	sv_stop( 0 );
	sv_close_slot( 0 );
	sv_deinit();
	printf( "SunVox closed.\n" );
    }
}

@end
