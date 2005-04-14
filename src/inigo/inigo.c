#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>

#include <framework/mlt.h>

#include "io.h"

static void transport_action( mlt_producer producer, char *value )
{
	mlt_properties properties = MLT_PRODUCER_PROPERTIES( producer );
	mlt_multitrack multitrack = mlt_properties_get_data( properties, "multitrack", NULL );

	mlt_properties_set_int( properties, "stats_off", 0 );

	if ( strlen( value ) == 1 )
	{
		switch( value[ 0 ] )
		{
			case 'q':
				mlt_properties_set_int( properties, "done", 1 );
				break;
			case '0':
				mlt_producer_set_speed( producer, 1 );
				mlt_producer_seek( producer, 0 );
				break;
			case '1':
				mlt_producer_set_speed( producer, -10 );
				break;
			case '2':
				mlt_producer_set_speed( producer, -5 );
				break;
			case '3':
				mlt_producer_set_speed( producer, -2 );
				break;
			case '4':
				mlt_producer_set_speed( producer, -1 );
				break;
			case '5':
				mlt_producer_set_speed( producer, 0 );
				break;
			case '6':
			case ' ':
				mlt_producer_set_speed( producer, 1 );
				break;
			case '7':
				mlt_producer_set_speed( producer, 2 );
				break;
			case '8':
				mlt_producer_set_speed( producer, 5 );
				break;
			case '9':
				mlt_producer_set_speed( producer, 10 );
				break;
			case 'd':
				if ( multitrack != NULL )
				{
					int i = 0;
					mlt_position last = -1;
					fprintf( stderr, "\n" );
					for ( i = 0; 1; i ++ )
					{
						mlt_position time = mlt_multitrack_clip( multitrack, mlt_whence_relative_start, i );
						if ( time == last )
							break;
						last = time;
						fprintf( stderr, "%d: %d\n", i, time );
					}
				}
				break;

			case 'g':
				if ( multitrack != NULL )
				{
					mlt_position time = mlt_multitrack_clip( multitrack, mlt_whence_relative_current, 0 );
					mlt_producer_seek( producer, time );
				}
				break;
			case 'H':
				if ( producer != NULL )
				{
					mlt_position position = mlt_producer_position( producer );
					mlt_producer_seek( producer, position - ( mlt_producer_get_fps( producer ) * 60 ) );
				}
				break;
			case 'h':
				if ( producer != NULL )
				{
					mlt_position position = mlt_producer_position( producer );
					mlt_producer_set_speed( producer, 0 );
					mlt_producer_seek( producer, position - 1 );
				}
				break;
			case 'j':
				if ( multitrack != NULL )
				{
					mlt_position time = mlt_multitrack_clip( multitrack, mlt_whence_relative_current, 1 );
					mlt_producer_seek( producer, time );
				}
				break;
			case 'k':
				if ( multitrack != NULL )
				{
					mlt_position time = mlt_multitrack_clip( multitrack, mlt_whence_relative_current, -1 );
					mlt_producer_seek( producer, time );
				}
				break;
			case 'l':
				if ( producer != NULL )
				{
					mlt_position position = mlt_producer_position( producer );
					mlt_producer_set_speed( producer, 0 );
					mlt_producer_seek( producer, position + 1 );
				}
				break;
			case 'L':
				if ( producer != NULL )
				{
					mlt_position position = mlt_producer_position( producer );
					mlt_producer_seek( producer, position + ( mlt_producer_get_fps( producer ) * 60 ) );
				}
				break;
		}
	}

	mlt_properties_set_int( properties, "stats_off", 0 );
}

static mlt_consumer create_consumer( char *id, mlt_producer producer )
{
	char *arg = id != NULL ? strchr( id, ':' ) : NULL;
	if ( arg != NULL )
		*arg ++ = '\0';
	mlt_consumer consumer = mlt_factory_consumer( id, arg );
	if ( consumer != NULL )
	{
		mlt_properties properties = MLT_CONSUMER_PROPERTIES( consumer );
		mlt_properties_set_data( properties, "transport_callback", transport_action, 0, NULL, NULL );
		mlt_properties_set_data( properties, "transport_producer", producer, 0, NULL, NULL );
	}
	return consumer;
}

static void transport( mlt_producer producer, mlt_consumer consumer )
{
	mlt_properties properties = MLT_PRODUCER_PROPERTIES( producer );
	int silent = mlt_properties_get_int( MLT_CONSUMER_PROPERTIES( consumer ), "silent" );
	struct timespec tm = { 0, 40000 };

	if ( mlt_properties_get_int( properties, "done" ) == 0 && !mlt_consumer_is_stopped( consumer ) )
	{
		if ( !silent )
		{
			term_init( );

			fprintf( stderr, "+-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+\n" );
			fprintf( stderr, "|1=-10| |2= -5| |3= -2| |4= -1| |5=  0| |6=  1| |7=  2| |8=  5| |9= 10|\n" );
			fprintf( stderr, "+-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+\n" );

			fprintf( stderr, "+---------------------------------------------------------------------+\n" );
			fprintf( stderr, "|               H = back 1 minute,  L = forward 1 minute              |\n" );
			fprintf( stderr, "|                 h = previous frame,  l = next frame                 |\n" );
			fprintf( stderr, "|           g = start of clip, j = next clip, k = previous clip       |\n" );
			fprintf( stderr, "|                0 = restart, q = quit, space = play                  |\n" );
			fprintf( stderr, "+---------------------------------------------------------------------+\n" );
		}

		while( mlt_properties_get_int( properties, "done" ) == 0 && !mlt_consumer_is_stopped( consumer ) )
		{
			int value = silent ? -1 : term_read( );

			if ( value != -1 )
				transport_action( producer, ( char * )&value );

			if ( !silent && mlt_properties_get_int( properties, "stats_off" ) == 0 )
				fprintf( stderr, "Current Position: %10d\r", mlt_producer_position( producer ) );

			if ( silent )
				nanosleep( &tm, NULL );
		}

		if ( !silent )
			fprintf( stderr, "\n" );
	}
}

int main( int argc, char **argv )
{
	int i;
	mlt_consumer consumer = NULL;
	mlt_producer inigo = NULL;
	FILE *store = NULL;
	char *name = NULL;
	struct sched_param scp;

	// Use realtime scheduling if possible
	memset( &scp, '\0', sizeof( scp ) );
	scp.sched_priority = sched_get_priority_max( SCHED_FIFO ) - 1;
#ifndef __DARWIN__
	sched_setscheduler( 0, SCHED_FIFO, &scp );
#endif

	// Construct the factory
	mlt_factory_init( NULL );

	// Check for serialisation switch first
	for ( i = 1; i < argc; i ++ )
	{
		if ( !strcmp( argv[ i ], "-serialise" ) )
		{
			name = argv[ ++ i ];
			if ( strstr( name, ".inigo" ) )
				store = fopen( name, "w" );
		}
	}

	// Get inigo producer
	if ( argc > 1 )
		inigo = mlt_factory_producer( "inigo", &argv[ 1 ] );

	if ( argc > 1 && inigo != NULL && mlt_producer_get_length( inigo ) > 0 )
	{
		// Get inigo's properties
		mlt_properties inigo_props = MLT_PRODUCER_PROPERTIES( inigo );

		// Get the last group
		mlt_properties group = mlt_properties_get_data( inigo_props, "group", 0 );

		// Parse the arguments
		for ( i = 1; i < argc; i ++ )
		{
			if ( !strcmp( argv[ i ], "-consumer" ) )
			{
				consumer = create_consumer( argv[ ++ i ], inigo );
				while ( argv[ i + 1 ] != NULL && strstr( argv[ i + 1 ], "=" ) )
					mlt_properties_parse( group, argv[ ++ i ] );
			}
			else if ( !strcmp( argv[ i ], "-serialise" ) )
			{
				i ++;
			}
			else
			{
				if ( store != NULL )
					fprintf( store, "%s\n", argv[ i ] );

				i ++;

				while ( argv[ i ] != NULL && argv[ i ][ 0 ] != '-' )
				{
					if ( store != NULL )
						fprintf( store, "%s\n", argv[ i ] );
					i += 1;
				}

				i --;
			}
		}

		// If we have no consumer, default to sdl
		if ( store == NULL && consumer == NULL )
			consumer = create_consumer( NULL, inigo );

		if ( consumer != NULL && store == NULL )
		{
			// Apply group settings
			mlt_properties properties = MLT_CONSUMER_PROPERTIES( consumer );
			mlt_properties_inherit( properties, group );

			// Connect consumer to inigo
			mlt_consumer_connect( consumer, MLT_PRODUCER_SERVICE( inigo ) );

			// Start the consumer
			mlt_consumer_start( consumer );

			// Transport functionality
			transport( inigo, consumer );

			// Stop the consumer
			mlt_consumer_stop( consumer );
		}
		else if ( store != NULL )
		{
			fprintf( stderr, "Project saved as %s.\n", name );
			fclose( store );
		}
	}
	else
	{
		fprintf( stderr, "Usage: inigo [ -group [ name=value ]* ]\n"
						 "             [ -consumer id[:arg] [ name=value ]* ]\n"
						 "             [ -filter filter[:arg] [ name=value ] * ]\n"
						 "             [ -attach filter[:arg] [ name=value ] * ]\n"
						 "             [ -mix length [ -mixer transition ]* ]\n"
						 "             [ -transition id[:arg] [ name=value ] * ]\n"
						 "             [ -blank frames ]\n"
						 "             [ -track ]\n"
						 "             [ -split relative-frame ]\n"
						 "             [ -join clips ]\n"
						 "             [ -repeat times ]\n"
						 "             [ producer [ name=value ] * ]+\n" );
	}

	// Close the consumer
	if ( consumer != NULL )
		mlt_consumer_close( consumer );

	// Close the producer
	if ( inigo != NULL )
		mlt_producer_close( inigo );

	// Close the factory
	mlt_factory_close( );

	return 0;
}
