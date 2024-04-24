/*
 * UDP.h
 *
 *  Created on: 11 Aug 2021
 *      Author: wangy
 */

#ifndef UDP_H_
#define UDP_H_

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

#include <mmcpplib.h>

//states enumeration for state machine
typedef enum {
	eSOCKET_CREAT_STATE = 0,
	eSOCKET_ACCEPT_STATE,
	eSOCKET_IDLE_STATE,
	eSOCKET_READ_STATE,
	eSOCKET_SEND_STATE,
	eSOCKET_CONNECT_STATE,
	eSOCKET_WRITABLE_STATE
} EDEMO_SOCK_STATE;

//message type definition - user proprietary.
typedef struct _sockdemo_msg_t {
	int _iID;
	char _szUserData[128];
} dsock_msg_t;

typedef struct {
	float drill_speed_m;
	float drill_torque_m;
	float drill_potentiometer_val;
	float joint_angle_m;
	float joint_torque_m;
	short joint_potentiometer_val; //range: +-10 V. unit: mV
	unsigned short foot_pedal; //0 - off; 1 - on
} dsock_msg_elmo2pc_t;

typedef struct {
	float joint_target_counts; // -1.0 to 1.0
	float drill_speed_scaled; // -1.0 to 1.0
	unsigned short spin_on; // 0 - off; 1 - on. Priority higher than foot_pedal
	unsigned short pc_take_overs;
} dsock_msg_pc2elmo_t;

extern dsock_msg_elmo2pc_t sock_msg_elmo2pc;
extern dsock_msg_pc2elmo_t sock_msg_pc2elmo;

extern short is_udp_cmd_valid;

extern void UDPDemoServer();
#endif /* UDP_H_ */
