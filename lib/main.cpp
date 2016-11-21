#include <stdio.h>
#include <string>
#include <cstring>
#include <iostream>
#include "DJI_guidance.h"
#include "DJI_utility.h"

using namespace std;



e_vbus_index sensor_id = e_vbus1;

DJI_lock    g_lock;
DJI_event   g_event;
char		key = 0;

std::ostream& operator<<(std::ostream& out, const e_sdk_err_code value){
	const char* s = 0;
	static char str[100]={0};
#define PROCESS_VAL(p) case(p): s = #p; break;
	switch(value){
		PROCESS_VAL(e_OK);     
		PROCESS_VAL(e_load_libusb_err);     
		PROCESS_VAL(e_sdk_not_inited);
		PROCESS_VAL(e_disparity_not_allowed);
		PROCESS_VAL(e_image_frequency_not_allowed);
		PROCESS_VAL(e_config_not_ready);
		PROCESS_VAL(e_online_flag_not_ready);
		PROCESS_VAL(e_stereo_cali_not_ready);
		PROCESS_VAL(e_libusb_io_err);
		PROCESS_VAL(e_timeout);
	default:
		strcpy(str, "Unknown error");
		s = str;
		break;
	}
#undef PROCESS_VAL

	return out << s;
}

int my_callback(int data_type, int data_len, char *content)
{
	g_lock.enter();

	if ( e_obstacle_distance == data_type && NULL != content )
	{
		obstacle_distance *oa = (obstacle_distance*)content;
		printf( "obstacle distance:" );
		for ( int i = 0; i < CAMERA_PAIR_NUM; ++i )
			printf( " %f ", 0.01f * oa->distance[i] );
		
		printf( "\n" );
		printf( "frame index:%d,stamp:%d\n", oa->frame_index, oa->time_stamp );
	}
	g_lock.leave();
	g_event.set_event();

	return 0;
}

#define RETURN_IF_ERR(err_code) { if( err_code ){ release_transfer(); \
std::cout<<"Error: "<<(e_sdk_err_code)err_code<<" at "<<__LINE__<<","<<__FILE__<<std::endl; return -1;}}

int main(int argc, const char** argv)
{

	reset_config();  // clear all data subscription

	int err_code = init_transfer(); //wait for board ready and init transfer thread
	RETURN_IF_ERR( err_code );
	select_obstacle_distance();
	err_code = set_sdk_event_handler( my_callback );
	RETURN_IF_ERR( err_code );
	err_code = start_transfer();
	RETURN_IF_ERR( err_code );



	while(1)
	{
		g_event.wait_event();
	}

	err_code = stop_transfer();
	RETURN_IF_ERR( err_code );
	//make sure the ack packet from GUIDANCE is received
	sleep( 1000000 );
	err_code = release_transfer();
	RETURN_IF_ERR( err_code );

	return 0;
}
