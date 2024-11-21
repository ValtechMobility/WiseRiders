
#include "sensor.h"
#include "screen.h"

#include "tx_api.h"

#include <stdbool.h>
#include <stdio.h>
#include "buttons.h"
#include "board_init.h"
#include "ssd1306.h"
#include "shared.h"

// typedef struct
// {
//     char type;
//     double value;

// } SENSOR_DATA;
// static TX_QUEUE sensor_queue;

#define LED_STEP 5     // Change in brightness per step
#define LED_MAX 255    // Maximum brightness
#define LED_MIN 0      // Minimum brightness
#define SLEEP_TICKS 10 // Sleep time in ticks for smooth transitions

#define SCREEN_WIDTH 128
#define LINE_SPACING 10 // Horizontal spacing between points

// static SENSOR_DATA_QUEUE_MEM[sizeof(SENSOR_DATA)*10];

static bool CRASH_FUSE = false;

void read_sensors_task(ULONG _unused)
{

    // UINT success = tx_queue_create(&sensor_queue, "Sensors", sizeof(SENSOR_DATA), &SENSOR_DATA_QUEUE_MEM, 10);
    // if (success != TX_SUCCESS) {
    //     printf("Oh no queue could not be created: %d", success);
    //     return;
    // }
    int rgb_state = 0;                                  // 0: Red -> Green, 1: Green -> Blue, 2: Blue -> Red
    int red = LED_MAX, green = LED_MIN, blue = LED_MIN; // Initial colors

    do
    {
        char buf[4][20];

        if (CRASH_FUSE)
        {
            screen_print("You crashed your car", L1);
            return;
        }

        switch (button_a_pressed_count)
        {
        case 0:

            lps22hb_t lps_data = lps22hb_data_read();
            hts221_data_t data = hts221_data_read();
            int temp = (int)data.temperature_degC;
            int hum = (int)data.humidity_perc;
            int pres = (int)lps_data.pressure_hPa;

            // 100 ticks sind eine 1 sec
            tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND * 2);

            sprintf(buf[0], "Weather");
            sprintf(buf[1], "#     %d&C", temp);
            sprintf(buf[2], "$     %d %%", hum);
            sprintf(buf[3], "\"    %d Pa", pres);
            screen_print_lines(buf, 4, L0, -3);
            break;
        case 1:
            switch (button_b_pressed_count % 7)
            {
            case 0:
                RGB_LED_SET_R(0);
                RGB_LED_SET_G(0);
                RGB_LED_SET_B(0);

                sprintf(buf[0], "Ambient");
                sprintf(buf[1], "Off");
                screen_print_lines(buf, 2, L0, -3);
                break;
            case 1:
                RGB_LED_SET_R(255);
                RGB_LED_SET_G(0);
                RGB_LED_SET_B(0);

                sprintf(buf[0], "Ambient");
                sprintf(buf[1], "Red");
                screen_print_lines(buf, 2, L0, -3);
                break;
            case 2:
                RGB_LED_SET_R(0);
                RGB_LED_SET_G(255);
                RGB_LED_SET_B(0);

                sprintf(buf[0], "Ambient");
                sprintf(buf[1], "Green");
                screen_print_lines(buf, 2, L0, -3);
                break;
            case 3:
                RGB_LED_SET_R(0);
                RGB_LED_SET_G(0);
                RGB_LED_SET_B(255);

                sprintf(buf[0], "Ambient");
                sprintf(buf[1], "Blue");
                screen_print_lines(buf, 2, L0, -3);
                break;
            case 4:
                RGB_LED_SET_R(255);
                RGB_LED_SET_G(255);
                RGB_LED_SET_B(0);

                sprintf(buf[0], "Ambient");
                sprintf(buf[1], "Yellow");
                screen_print_lines(buf, 2, L0, -3);
                break;
            case 5:
                RGB_LED_SET_R(255);
                RGB_LED_SET_G(255);
                RGB_LED_SET_B(255);

                sprintf(buf[0], "Ambient");
                sprintf(buf[1], "White");
                screen_print_lines(buf, 2, L0, -3);
                break;
            case 6:
                switch (rgb_state)
                {
                case 0: // Transition: Red -> Green
                    green += LED_STEP;
                    if (green >= LED_MAX)
                    {
                        green = LED_MAX;
                        rgb_state = 1; // Move to the next transition
                    }
                    red -= LED_STEP;
                    if (red <= LED_MIN)
                        red = LED_MIN;
                    break;

                case 1: // Transition: Green -> Blue
                    blue += LED_STEP;
                    if (blue >= LED_MAX)
                    {
                        blue = LED_MAX;
                        rgb_state = 2; // Move to the next transition
                    }
                    green -= LED_STEP;
                    if (green <= LED_MIN)
                        green = LED_MIN;
                    break;

                case 2: // Transition: Blue -> Red
                    red += LED_STEP;
                    if (red >= LED_MAX)
                    {
                        red = LED_MAX;
                        rgb_state = 0; // Move to the next transition
                    }
                    blue -= LED_STEP;
                    if (blue <= LED_MIN)
                        blue = LED_MIN;
                    break;
                }

                // Set the LED colors
                RGB_LED_SET_R(red);
                RGB_LED_SET_G(green);
                RGB_LED_SET_B(blue);

                // Sleep for a while to create a visible effect
                tx_thread_sleep(SLEEP_TICKS);

                sprintf(buf[0], "Ambient");
                sprintf(buf[1], "Rainbow");
                screen_print_lines(buf, 2, L0, -3);
                break;
                break;
            }
            break;

        case 2:
            tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 2);

            lsm6dsl_data_t lsm_data = lsm6dsl_data_read();
            int acc1 = (int)lsm_data.acceleration_mg[0];
            int acc2 = (int)lsm_data.acceleration_mg[1];
            int acc3 = (int)lsm_data.acceleration_mg[2];
            int ar1 = (int)lsm_data.angular_rate_mdps[0];
            int ar2 = (int)lsm_data.angular_rate_mdps[1];
            int ar3 = (int)lsm_data.angular_rate_mdps[2];

            switch (button_b_pressed_count % 3)
            {
            case 0:
                sprintf(buf[0], "Orientation");
                sprintf(buf[1], "X: %d", acc1);
                sprintf(buf[2], "Y: %d", acc2);
                sprintf(buf[3], "Z: %d", acc3);
                screen_print_lines(buf, 4, L0, -3);
                break;
            case 1:
                sprintf(buf[0], "Accelometer");
                sprintf(buf[1], "X: %d", ar1);
                sprintf(buf[2], "Y: %d", ar2);
                sprintf(buf[3], "Z: %d", ar3);
                screen_print_lines(buf, 4, L0, -3);
                
                break;
            case 2:
                ssd1306_Fill(Black); // Clear the screen
                ssd1306_SetCursor(2, 0);
                ssd1306_WriteString("Graph", Font_11x18, 1);
                int threshold = (ar1 + ar2 + ar3) / 100000;
                int prev_x = 0;
                int prev_y = rand() * threshold; // Initial random y value

                for (int x = LINE_SPACING; x < SCREEN_WIDTH; x += LINE_SPACING)
                {
                    int new_y = rand() * threshold; // Generate a new random y value within the threshold

                    // Draw line from previous point to the new point
                    ssd1306_Line(prev_x, prev_y + 30, x, new_y + 30, 1);

                    // Update the previous point to the current point
                    prev_x = x;
                    prev_y = new_y;
                }

                ssd1306_UpdateScreen(); // Render the lines on the screen
                break;
            }
            break;
        case 3:
            screen_print("Sensitivity", 0);
            break;
        }
    } while (true);
}

void crash_detect_task(ULONG _unused)
{

    const int THRESHOLD = 60000;
    do
    {

        // printf("Crash check loop\n");

        lsm6dsl_data_t lsm_data = lsm6dsl_data_read();
        int ar1 = (int)lsm_data.angular_rate_mdps[0];
        int ar2 = (int)lsm_data.angular_rate_mdps[1];
        int ar3 = (int)lsm_data.angular_rate_mdps[2];

        // int acc_x = (int)lsm_data.acceleration_mg[0];
        // int acc_y = (int)lsm_data.acceleration_mg[1];
        // int acc_z = (int)lsm_data.acceleration_mg[2];
        // // printf("%d, %d, %d", acc_x, acc_y, acc_z);

        if (ar1 > THRESHOLD || ar1 < -THRESHOLD || ar2 > THRESHOLD || ar2 < -THRESHOLD || ar3 > THRESHOLD || ar3 < -THRESHOLD)

        // if (acc_x > 0 || acc_y > 0 || acc_z > 0)
        {

            printf("Detected crash, trigger ecall... \r\n");

            tx_event_flags_set(&mqtt_app_flag, MESSAGE_READY, TX_OR);

            CRASH_FUSE = true;

            // crash
            break;
        }

        tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND);
    } while (true);
}
