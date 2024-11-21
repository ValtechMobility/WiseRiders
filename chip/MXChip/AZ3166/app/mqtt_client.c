#include "mqtt_client.h"
#include "shared.h"
#include <stdint.h>
#include "umessage.pb.h"
#include "uri.pb.h"
#include "umessage.pb.h"
#include "uattributes.pb.h"
#include "uuid.pb.h"
#include "pb_encode.h"
#include "pb_decode.h"

/* MQTT Demo defines */

/* IP Address of the server (test.mosquitto.org). */
#define LOCAL_SERVER_ADDRESS (IP_ADDRESS(5, 196, 78, 28))

#define STRLEN(p) (sizeof(p) - 1)


// #define  LOCAL_SERVER_ADDRESS (IP_ADDRESS(52, 31, 149, 80))
/*****************************************************************************************/
/* MQTT Local Server IoT Client example.                                                 */
/*****************************************************************************************/

/* Declare buffers to hold message and topic. */

/* Declare the MQTT thread stack space. */
static ULONG mqtt_client_stack[MQTT_CLIENT_STACK_SIZE / sizeof(ULONG)];


static UCHAR message_buffer[NXD_MQTT_MAX_MESSAGE_LENGTH];
static UCHAR topic_buffer[NXD_MQTT_MAX_TOPIC_NAME_LENGTH];


/* Declare the MQTT client control block. */
static NXD_MQTT_CLIENT mqtt_client;

/* Define the symbol for signaling a received message. */

/* Define the test threads.  */
#define TOPIC_NAME "d/Cloud/3039/1/8001"
#define PUBLISH_TOPIC_NAME "d/MXChip/5BA0/1/8001"

/* Define the priority of the MQTT internal thread. */
#define MQTT_THREAD_PRIORTY 2

/* Define the MQTT keep alive timer for 5 minutes */
#define MQTT_KEEP_ALIVE_TIMER 300

#define QOS0 0
#define QOS1 1


int marshall_umsg_to_buffer(uprotocol_v1_UMessage msg, uint8_t *buffer, int len)
{
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, len);

    pb_encode(&stream, uprotocol_v1_UMessage_fields, &msg);

    return stream.bytes_written;
}

/*
 * Builds u-Protocol message indicating a crash
 */
uprotocol_v1_UMessage build_umessage()
{
    uprotocol_v1_UMessage_payload_t u_pl = {.size = 1, .bytes = {0x01}};
    uprotocol_v1_UMessage msg = uprotocol_v1_UMessage_init_zero;

    uprotocol_v1_UAttributes u_att = uprotocol_v1_UAttributes_init_zero;

    uprotocol_v1_UUID uuid = {.lsb = 1234, .msb = 123451};
    u_att.has_id = true;
    u_att.id = uuid;

    u_att.type = uprotocol_v1_UMessageType_UMESSAGE_TYPE_PUBLISH;

    uprotocol_v1_UUri source = {.ue_id = 1, .authority_name = "Car01", .ue_version_major = 1};
    uprotocol_v1_UUri sink = {.ue_id = 1, .authority_name = "CrashApp", .ue_version_major = 1};
    u_att.has_sink = true;
    u_att.sink = sink;
    u_att.has_source = true;
    u_att.source = source;

    msg.has_attributes = true;
    msg.attributes = u_att;

    msg.has_payload = true;
    msg.payload = u_pl;

    return msg;
}

// Unmarshall UMesssage from buffer
uprotocol_v1_UMessage unmarshall_umsg(uint8_t *buffer, int msg_len)
{
    uprotocol_v1_UMessage message = uprotocol_v1_UMessage_init_zero;
    bool status;

    /* Create a stream that reads from the buffer. */
    pb_istream_t stream = pb_istream_from_buffer(buffer, msg_len);

    /* Now we are ready to decode the message. */
    status = pb_decode(&stream, uprotocol_v1_UMessage_fields, &message);

    /* Check for errors... */
    if (!status)
    {
        printf("Decoding failed: %s \r\n", PB_GET_ERROR(&stream));
    }
    /* Print the data contained in the message. */
    // printf("Message has type: %d! \r\n", message.attributes.type);

    return message;
}


/* Declare the disconnect notify function. */
static VOID my_disconnect_func(NXD_MQTT_CLIENT *client_ptr)
{
    NX_PARAMETER_NOT_USED(client_ptr);
    printf("client disconnected from server\n");
}

static VOID my_notify_func(NXD_MQTT_CLIENT *client_ptr, UINT number_of_messages)
{
    NX_PARAMETER_NOT_USED(client_ptr);
    NX_PARAMETER_NOT_USED(number_of_messages);
    tx_event_flags_set(&mqtt_app_flag, DEMO_MESSAGE_EVENT, TX_OR);
    return;
}

static ULONG error_counter;
void thread_mqtt_entry(NX_IP *ip_ptr, NX_PACKET_POOL *pool_ptr)
{
    UINT status;
    NXD_ADDRESS server_ip;
    ULONG events;
    UINT topic_length, message_length;
    ULONG message_sent = 0;

    printf("Creating MQTT client\r\n");
    /* Create MQTT client instance. */
    status = nxd_mqtt_client_create(&mqtt_client, "my_client", CLIENT_ID_STRING, STRLEN(CLIENT_ID_STRING),
                                    ip_ptr, pool_ptr, (VOID *)mqtt_client_stack, sizeof(mqtt_client_stack),
                                    MQTT_THREAD_PRIORTY, NX_NULL, 0);

    if (status)
    {
        printf("Error in creating MQTT client: 0x%02x\n", status);
        error_counter++;
    }

    // nxd_mqtt_client_login_set(&mqtt_client, "benedikt", 10,  "1234aBcd", 10);
    printf(" MQTT client created\r\n");

    /* Register the disconnect notification function. */
    nxd_mqtt_client_disconnect_notify_set(&mqtt_client, my_disconnect_func);

    /* Create an event flag for this demo. */
    status = tx_event_flags_create(&mqtt_app_flag, "my app event");
    if (status)
        error_counter++;

    server_ip.nxd_ip_version = 4;
    server_ip.nxd_ip_address.v4 = LOCAL_SERVER_ADDRESS;

    /* Start the connection to the server. */

    status = nxd_mqtt_client_connect(&mqtt_client, &server_ip, NXD_MQTT_PORT,
                                     MQTT_KEEP_ALIVE_TIMER, 0, NX_WAIT_FOREVER);
    if (status)
        error_counter++;

    /* Subscribe to the topic with QoS level 0. */
    status = nxd_mqtt_client_subscribe(&mqtt_client, TOPIC_NAME, STRLEN(TOPIC_NAME), QOS0);
    if (status)
        error_counter++;

    /* Set the receive notify function. */
    status = nxd_mqtt_client_receive_notify_set(&mqtt_client, my_notify_func);
    if (status)
        error_counter++;

    /* Now wait for the broker to publish the message. */
    printf("Waiting for messages\r\n");

    while (1)
    {
        tx_event_flags_get(&mqtt_app_flag, DEMO_ALL_EVENTS, TX_OR_CLEAR, &events, TX_WAIT_FOREVER);
        if (events & DEMO_MESSAGE_EVENT)
        {
            status = nxd_mqtt_client_message_get(&mqtt_client, topic_buffer, sizeof(topic_buffer), &topic_length,
                                                 message_buffer, sizeof(message_buffer), &message_length);
            printf("Got message and status: %d \r\n", status);
            if (status == NXD_MQTT_SUCCESS)
            {

                // Protocol is simple: 1 --> Start flash for 2 seconds
                topic_buffer[topic_length] = 0;
                message_buffer[message_length] = 0;
                message_sent = message_buffer[0];
                status = tx_queue_send(&queue_mqtt, &message_sent, TX_WAIT_FOREVER);
                printf("topic = %s, message = %d\n", topic_buffer, message_buffer[0]);
            }
        }
        else if (events & MESSAGE_READY)
        {
            /* Publish a message with QoS Level 1. */
            uint8_t buffer[128];
            uprotocol_v1_UMessage msg = build_umessage();
            int msg_len = marshall_umsg_to_buffer(msg, buffer, sizeof(buffer));

            status = nxd_mqtt_client_publish(&mqtt_client, PUBLISH_TOPIC_NAME, STRLEN(PUBLISH_TOPIC_NAME),
                                             (CHAR *)buffer, msg_len, 0, QOS1, NX_WAIT_FOREVER);

            if (status != NXD_MQTT_SUCCESS)
            {
                printf("Pubhlish failed with code: %d\r˜n", status);
            }
            printf("Published message");
        }
    }

    /* Now unsubscribe the topic. */
    nxd_mqtt_client_unsubscribe(&mqtt_client, TOPIC_NAME, STRLEN(TOPIC_NAME));

    /* Disconnect from the broker. */
    nxd_mqtt_client_disconnect(&mqtt_client);

    /* Delete the client instance, release all the resources. */
    nxd_mqtt_client_delete(&mqtt_client);

    return;
}
