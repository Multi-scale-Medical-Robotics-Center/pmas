/*
 * UDP.cpp
 *
 *  Created on: 11 Aug 2021
 *      Author: wangy
 */

#include "UDP.h"

static char g_szIpAddress[16];
static unsigned short g_usPort = 4000;

short is_udp_cmd_valid = 1;

dsock_msg_elmo2pc_t sock_msg_elmo2pc = { };
dsock_msg_pc2elmo_t sock_msg_pc2elmo = { };

void UDPDemoServer() {
	static EDEMO_SOCK_STATE eState = eSOCKET_CREAT_STATE;
	static CMMCUDP udpServer;
	static dsock_msg_t msg;
	int rc;

	switch (eState) {
	case eSOCKET_CREAT_STATE:
		//create listener.
		rc = udpServer.Create(g_usPort); //no callback, normal mode of operation
		fprintf(stderr, "%s, Create: rc = %d.\n", __func__, rc);
		eState = eSOCKET_READ_STATE; //normal mode of operation
		break;

	case eSOCKET_READ_STATE: //receive data from available clients (normal mode only)
		// check if readable
		if (udpServer.IsReadable()) {
			memset(&sock_msg_pc2elmo, 0, sizeof(dsock_msg_pc2elmo_t));
			rc = udpServer.Receive(&sock_msg_pc2elmo,
					sizeof(dsock_msg_pc2elmo_t));
			if (rc > 0) {
				is_udp_cmd_valid = 1;

//			cout << "received: ";
//					<< "DrillSpeed " << sock_msg_pc2elmo.drill_speed_d << ", "
//					<< "JointAngle "<< sock_msg_pc2elmo.joint_angle_d << ", "
//					<< "SpingOn "<< sock_msg_pc2elmo.spin_on << endl;
				eState = eSOCKET_SEND_STATE; //next stage for response available clients.
			} else {
				cout << "eSOCKET_READ_STATE receive error" << endl;
				is_udp_cmd_valid = 0;
			}
		} else {
			//otherwise repeat on read state.
//			cout << "eSOCKET_READ_STATE no readable data" << endl;
			is_udp_cmd_valid = 0; // switch back to independent control if timeout
		}

		break;
	case eSOCKET_SEND_STATE:               //send response to  available clients
		rc = udpServer.Send(&sock_msg_elmo2pc, sizeof(dsock_msg_elmo2pc_t));
		if (rc > 0)
//			fprintf(stderr, "%s, sending response id = %d : %s\n", __func__,
//					msg._iID, msg._szUserData);
//		else
//			fprintf(stderr, "%s, send: failed\n", __func__);
		eState = eSOCKET_READ_STATE;
		break;
	default:
		break;
	}
}
