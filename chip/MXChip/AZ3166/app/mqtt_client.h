#include "nxd_mqtt_client.h"

#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H
/* Declare event flag, which is used in this demo. */
#endif

#undef NXD_MQTT_MAX_TOPIC_NAME_LENGTH
#undef NXD_MQTT_MAX_MESSAGE_LENGTH
#define NXD_MQTT_MAX_TOPIC_NAME_LENGTH 68
#define NXD_MQTT_MAX_MESSAGE_LENGTH 128

#define DEMO_ALL_EVENTS 3

#define DEMO_STACK_SIZE 2048
#define CLIENT_ID_STRING "car_id_0_j"
#define MQTT_CLIENT_STACK_SIZE 4096


void thread_mqtt_entry(NX_IP *ip_ptr, NX_PACKET_POOL *pool_ptr);
