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

#include "tx_api.h"
#include <nxd_mqtt_client.h>

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