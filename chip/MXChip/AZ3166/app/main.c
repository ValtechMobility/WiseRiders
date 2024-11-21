/*
 * Copyright (c) Microsoft
 * Copyright (c) 2024 Eclipse Foundation
 *
 *  This program and the accompanying materials are made available
 *  under the terms of the MIT license which is available at
 *  https://opensource.org/license/mit.
 *
 *  SPDX-License-Identifier: MIT
 *
 *  Contributors:
 *     Microsoft         - Initial version
 *     Frédéric Desbiens - 2024 version.
 */

#include <stdio.h>

#include "tx_api.h"

#include "nx_api.h"
#include "nx_icmp.h"

#include "board_init.h"
#include "cmsis_utils.h"
#include "screen.h"
#include "sntp_client.h"
#include "wwd_networking.h"

#include "cloud_config.h"
#include <nxd_mqtt_client.h>


#define ECLIPSETX_THREAD_STACK_SIZE 4096
#define ECLIPSETX_THREAD_PRIORITY   4
#define MQTT_THREAD_STACK_SIZE 4096
#define MQTT_THREAD_PRIORITY   4

#define MQTT_CLIENT_STACK_SIZE 2048
static ULONG mqtt_client_stack[MQTT_CLIENT_STACK_SIZE / sizeof(ULONG)];
static ULONG client_memory[2000 / sizeof(ULONG)];

#define IP_ADDRESS_NUMBER IP_ADDRESS(0, 0, 0, 0)
#define NETX_IPV4_MASK IP_ADDRESS(255, 255, 255, 0)

NX_IP ip_instance;
NX_PACKET_POOL packet_pool;

TX_THREAD eclipsetx_thread;
ULONG eclipsetx_thread_stack[ECLIPSETX_THREAD_STACK_SIZE / sizeof(ULONG)];

TX_THREAD mqtt_thread;
ULONG mqtt_thread_stack[MQTT_THREAD_STACK_SIZE / sizeof(ULONG)];

// MQTT Client
static NXD_MQTT_CLIENT mqtt_client;

// MQTT Broker Details
#define MQTT_BROKER_ADDRESS IP_ADDRESS(5, 196, 78, 28) // 5.196.78.28
#define MQTT_BROKER_PORT 1883
#define MQTT_TOPIC "telemetry/temperature"
#define MQTT_USERNAME "benedikt" // Optional
#define MQTT_PASSWORD "1234aBcd" // Optional
#define CLIENT_ID_STRING "awesome_test_client"
#define STRLEN(p) (sizeof(p) - 1)
#define MQTT_KEEP_ALIVE_TIMER 300

TX_EVENT_FLAGS_GROUP                mqtt_app_flag;
#define DEMO_MESSAGE_EVENT          1
#define DEMO_ALL_EVENTS             3

/* Declare buffers to hold message and topic. */
static UCHAR message_buffer[NXD_MQTT_MAX_MESSAGE_LENGTH];
static UCHAR topic_buffer[NXD_MQTT_MAX_TOPIC_NAME_LENGTH];


static VOID my_notify_func(NXD_MQTT_CLIENT* client_ptr, UINT number_of_messages)
{
    printf("Got a message.");
    NX_PARAMETER_NOT_USED(client_ptr);
    NX_PARAMETER_NOT_USED(number_of_messages);
    tx_event_flags_set(&mqtt_app_flag, DEMO_MESSAGE_EVENT, TX_OR);
    return;
}

void start_mqtt()
{
    UINT status;
    NXD_ADDRESS server_ip;
    ULONG events;
    UINT topic_length, message_length;

    printf("Starting MQTT.");

    // Create the client instance
    status = nxd_mqtt_client_create(&mqtt_client, "mqtt_client", CLIENT_ID_STRING, STRLEN(CLIENT_ID_STRING), &nx_ip, nx_pool, (VOID *)mqtt_client_stack, sizeof(mqtt_client_stack), MQTT_THREAD_PRIORITY, (UCHAR *)client_memory, sizeof(client_memory));

    if (status != TX_SUCCESS)
        {
            printf("ERROR: MQTT client creation failed\r\n");
        }

    printf("MQTT Trying to create flags.");
    tx_event_flags_create(&mqtt_app_flag, "my app event");

    server_ip.nxd_ip_version = 4;
    server_ip.nxd_ip_address.v4 = MQTT_BROKER_ADDRESS;

    printf("MQTT Trying to connect.");
    status = nxd_mqtt_client_connect(&mqtt_client, &server_ip, MQTT_BROKER_PORT, MQTT_KEEP_ALIVE_TIMER, 0, NX_WAIT_FOREVER);

    if (status != TX_SUCCESS)
        {
            printf("ERROR: MQTT client connect failed\r\n");
        }

    printf("MQTT Trying to publish.");
    status = nxd_mqtt_client_publish(&mqtt_client, MQTT_TOPIC, STRLEN(MQTT_TOPIC), "Device Booted.", STRLEN("Hello World!"), 0, 0, NX_WAIT_FOREVER);

    if (status != TX_SUCCESS)
        {
            printf("ERROR: MQTT client publish failed\r\n");
        }

    printf("MQTT Trying to subscribe.");
    status = nxd_mqtt_client_subscribe(&mqtt_client, MQTT_TOPIC, STRLEN(MQTT_TOPIC), 0);

    if (status != TX_SUCCESS)
        {
            printf("ERROR: MQTT client subscribe failed\r\n");
        }

    printf("MQTT Trying to set callback.");
    status = nxd_mqtt_client_receive_notify_set(&mqtt_client, my_notify_func);

    if (status != TX_SUCCESS)
        {
            printf("ERROR: MQTT client set callback failed\r\n");
        }

    printf("MQTT Trying to get flags.");
    tx_event_flags_get(&mqtt_app_flag, DEMO_ALL_EVENTS, TX_OR_CLEAR, &events, TX_WAIT_FOREVER);
    if(events & DEMO_MESSAGE_EVENT)
    {
        status = nxd_mqtt_client_message_get(&mqtt_client, topic_buffer, sizeof(topic_buffer), &topic_length, 
                                             message_buffer, sizeof(message_buffer), &message_length);
        if(status == NXD_MQTT_SUCCESS)
        {
            topic_buffer[topic_length] = 0;
            message_buffer[message_length] = 0;
            printf("topic = %s, message = %s\n", topic_buffer, message_buffer);
        }
    }

    printf("Listening for messages.");

    while (1) {}
}

static void eclipsetx_thread_entry(ULONG parameter)

{
    UINT status;

    printf("Starting Eclipse ThreadX thread\r\n\r\n");

    // Initialize the network
    if ((status = wwd_network_init(WIFI_SSID, WIFI_PASSWORD, WIFI_MODE)))
    {
        printf("ERROR: Failed to initialize the network (0x%08x)\r\n", status);
    }

    wwd_network_connect();

    start_mqtt();
}

void tx_application_define(void* first_unused_memory)
{
    systick_interval_set(TX_TIMER_TICKS_PER_SECOND);

    // Create ThreadX thread
    UINT status = tx_thread_create(&eclipsetx_thread,
        "Eclipse ThreadX Thread",
        eclipsetx_thread_entry,
        0,
        eclipsetx_thread_stack,
        ECLIPSETX_THREAD_STACK_SIZE,
        ECLIPSETX_THREAD_PRIORITY,
        ECLIPSETX_THREAD_PRIORITY,
        TX_NO_TIME_SLICE,
        TX_AUTO_START);

    if (status != TX_SUCCESS)
    {
        printf("ERROR: Eclipse ThreadX thread creation failed\r\n");
    }
}


int main(void)
{
    // Initialize the board
    board_init();

    // Enter the ThreadX kernel
    tx_kernel_enter();

    return 0;
}
