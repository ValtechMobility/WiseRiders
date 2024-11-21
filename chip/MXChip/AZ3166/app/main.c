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
#include <stdbool.h>
#include "tx_api.h"

#include "board_init.h"
#include "cmsis_utils.h"
#include "screen.h"
#include "sntp_client.h"
#include "wwd_networking.h"

#include "cloud_config.h"
#include "sensor.h"
#include "shared.h"

#include "nx_api.h"
#include "mqtt_client.h"

/*
 * Comment in to use logger
 */
// #define USE_LOGGER

TX_EVENT_FLAGS_GROUP mqtt_app_flag;

#define DEMO_QUEUE_SIZE 24
#define QUEUE_BYTE_POOL_SIZE 1024

/*
 * Queue setup
*/
UCHAR memory_area[QUEUE_BYTE_POOL_SIZE];
TX_QUEUE queue_mqtt;
TX_BYTE_POOL byte_pool_queue;

/*
* Define Thread configurations:
*   - Stack sizes
*/
#define ECLIPSETX_THREAD_STACK_SIZE 4096
#define ECLIPSETX_THREAD_PRIORITY 4
#define CRASH_THREAD_PRIORITY 3


TX_THREAD eclipsetx_thread;
ULONG eclipsetx_thread_stack[ECLIPSETX_THREAD_STACK_SIZE / sizeof(ULONG)];

TX_THREAD flash_thread;
ULONG flash_thread_stack[ECLIPSETX_THREAD_STACK_SIZE / sizeof(ULONG)];

TX_THREAD logger_thread;
ULONG logger_thread_stack[ECLIPSETX_THREAD_STACK_SIZE / sizeof(ULONG)];

TX_THREAD read_sensors_thread;
char read_sensors_thread_stack[1024];

void read_sensors_task(ULONG _unused);

TX_THREAD crash_thread;
ULONG crash_thread_stack[ECLIPSETX_THREAD_STACK_SIZE / sizeof(ULONG)];
void crash_detect_task(ULONG _unused);

/*
* Application funcations. 
*/

// Implements the blink part as reaction to an incoming command message.
void blink_5_s()
{
    // Each iteration takes 200ms (ON, OFF) so blinking for 6seconds means 6000/200 = 30
    for (int i = 0; i <= 30; i++)
    {
        RGB_LED_SET_R(0);
        tx_thread_sleep(10);
        RGB_LED_SET_R(255);
        tx_thread_sleep(10);
    }
}

static void start_blink_thread(ULONG parameter)
{
    ULONG received_message = 0;
    UINT queue_status;

    while (1)
    {
        RGB_LED_SET_R(0);
        queue_status = tx_queue_receive(&queue_mqtt, &received_message, TX_WAIT_FOREVER);
        if (queue_status != TX_SUCCESS)
        {
            printf("Queue receive failed \r\n");
            tx_thread_sleep(200);
        }
        if (received_message == 1)
        {
            blink_5_s();
        }
    }
}

#ifdef USE_LOGGER
static void logger_thread_entry(ULONG parameter)
{
    while (1)
    {
        // printf("ThreadX Test-Loop\n");

        hts221_data_t s_data = hts221_data_read();
        int hum = (int)s_data.humidity_perc;
        int temp = (int)s_data.temperature_degC;
        printf("*** hts221 ***\n");
        printf("hum %i\n", hum);
        printf("hts temp %i\n", temp);
        printf("\n");

        lsm6dsl_data_t lsm_data = lsm6dsl_data_read();
        int acc1 = (int)lsm_data.acceleration_mg[0];
        int acc2 = (int)lsm_data.acceleration_mg[1];
        int acc3 = (int)lsm_data.acceleration_mg[2];
        int ar1 = (int)lsm_data.angular_rate_mdps[0];
        int ar2 = (int)lsm_data.angular_rate_mdps[1];
        int ar3 = (int)lsm_data.angular_rate_mdps[2];
        int temp1 = (int)lsm_data.temperature_degC;
        printf("*** lsm6ds ***l\n");
        printf("acceleration_mg1 %i\n", acc1);
        printf("acceleration_mg2 %i\n", acc2);
        printf("acceleration_mg3 %i\n", acc3);
        printf("angular_rate_mdps1 %i\n", ar1);
        printf("angular_rate_mdps2 %i\n", ar2);
        printf("angular_rate_mdps3 %i\n", ar3);
        printf("lms temp %i\n", temp1);
        printf("\n");

        lps22hb_t lps_data = lps22hb_data_read();
        int press = (int)lps_data.pressure_hPa;
        int temp2 = (int)lps_data.temperature_degC;
        printf("*** lps22hb ***\n");
        printf("press %i\n", press);
        printf("lps temp %i\n", temp2);
        printf("\n\n\n");

        tx_thread_sleep(100);
    }
}
#endif

static void eclipsetx_thread_entry(ULONG parameter)
{
    bool status;

    printf("Starting Eclipse ThreadX thread\r\n\r\n");

    // Initialize the network
    if ((status = wwd_network_init(WIFI_SSID, WIFI_PASSWORD, WIFI_MODE)))
    {
        printf("ERROR: Failed to initialize the network (0x%08x)\r\n", status);
    }

    wwd_network_connect();

    thread_mqtt_entry(&nx_ip, nx_pool);
}

void tx_application_define(void *first_unused_memory)
{
    systick_interval_set(TX_TIMER_TICKS_PER_SECOND);
    CHAR *pointer = TX_NULL;
    /* Allocate the message queue.  */
    tx_byte_pool_create(&byte_pool_queue, "byte pool 0", memory_area, QUEUE_BYTE_POOL_SIZE);

    tx_byte_allocate(&byte_pool_queue, (VOID **)&pointer, DEMO_QUEUE_SIZE * sizeof(ULONG), TX_NO_WAIT);
    UINT status = tx_queue_create(&queue_mqtt, "queue 0", TX_1_ULONG, pointer, DEMO_QUEUE_SIZE * sizeof(ULONG));
    if (status != TX_SUCCESS)
    {
        printf("ERROR: Queue Creation failed with %d \r\n", status);
    }
    // Create network thread
    status = tx_thread_create(&eclipsetx_thread,
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
        printf("ERROR: Eclipse (Network) ThreadX thread creation failed\r\n");
    }

    // Create blinky thread
    status = tx_thread_create(&flash_thread,
                              "LED flash thread",
                              start_blink_thread,
                              0,
                              flash_thread_stack,
                              ECLIPSETX_THREAD_STACK_SIZE,
                              ECLIPSETX_THREAD_PRIORITY,
                              ECLIPSETX_THREAD_PRIORITY,
                              TX_NO_TIME_SLICE,
                              TX_AUTO_START);

    if (status != TX_SUCCESS)
    {
        printf("ERROR: LED-Flash thread creation failed\r\n");
    }

#ifdef USE_LOGGER
    // Create logger thread
    status = tx_thread_create(&logger_thread,
                              "Eclipse ThreadX Thread",
                              logger_thread_entry,
                              0,
                              logger_thread_stack,
                              ECLIPSETX_THREAD_STACK_SIZE,
                              ECLIPSETX_THREAD_PRIORITY,
                              ECLIPSETX_THREAD_PRIORITY,
                              TX_NO_TIME_SLICE,
                              TX_AUTO_START);

    if (status != TX_SUCCESS)
    {
        printf("ERROR: logger thread creation failed\r\n");
    }
#endif

    // Create the thread
    status = tx_thread_create(
        &read_sensors_thread,              // Thread control block
        "Read Sensors",                    // Thread name
        read_sensors_task,                 // Thread entry function
        0,                                 // Input parameter
        read_sensors_thread_stack,         // Thread stack pointer
        sizeof(read_sensors_thread_stack), // Stack size
        ECLIPSETX_THREAD_PRIORITY,         // Priority
        ECLIPSETX_THREAD_PRIORITY,         // Preemption threshold
        TX_NO_TIME_SLICE,                  // No time-slicing
        TX_AUTO_START                      // Auto-start the thread
    );

    if (status != TX_SUCCESS)
    {
        printf("Error creating thread: %d\n", status);
    }
    else
    {
        printf("Thread created : %d\n", status);
    }

    // Create ThreadX thread
    status = tx_thread_create(&crash_thread,
                              "Crash Detector",
                              crash_detect_task,
                              0,
                              crash_thread_stack,
                              ECLIPSETX_THREAD_STACK_SIZE,
                              CRASH_THREAD_PRIORITY,
                              CRASH_THREAD_PRIORITY,
                              TX_NO_TIME_SLICE,
                              TX_AUTO_START);

    if (status != TX_SUCCESS)
    {
        printf("ERROR: Crash Detector thread creation failed. Reason %d\r\n", status);
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
