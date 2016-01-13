/**
 * @file  user_button.c
 * @brief Button UI
 * 
 * @author Antti Nykänen <aon@umetronics.com>
 */

#include "esp_common.h"

#include "user_button.h"
#include "user_dimmer.h"

#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define BUTTON_PIN_REG GPIO_PIN_REG_4
#define BUTTON_PIN_NO 4
#define BUTTON_PIN_FUNC FUNC_GPIO4

#define HOLD_TRESHOLD (2000 / 50)

/**
 * Button handler task
 * 
 * @param parameters task parameters
 */
static void s_button_task(void *parameters)
{
    portTickType last_wake_time;
    const portTickType poll_frequency = 50 / portTICK_RATE_MS;
    uint8 button_state, last_button_state;
    uint16_t hold_counter;
    uint8_t dim_pct;
    
    last_wake_time = xTaskGetTickCount();
    
    while (1) {
        vTaskDelayUntil(&last_wake_time, poll_frequency);

        button_state = GPIO_INPUT_GET(BUTTON_PIN_NO);

        if (button_state == 0 && last_button_state == 1) {
            /* Button down, do nothing. */
        } else if (button_state == 1 && last_button_state == 0) {
            /*
             * Button up.
             */
            if (hold_counter < HOLD_TRESHOLD) {
                /*
                 * Held for less than HOLD_TRESHOLD:
                 * Toggle lamp power.
                 */
                if (dimmer_get_power_on() == true) {
                    dimmer_set_power_on(false);
                } else {
                    dimmer_set_power_on(true);
                }
            } else {
                /* Do nothing when releasing a held button. */
            }
            
            /* Reset hold counter in either case. */
            hold_counter = 0;
        } else if (button_state == 0 && last_button_state == 0) {
            if (hold_counter < HOLD_TRESHOLD) {
                /*
                 * Button held for less than HOLD_TRESHOLD:
                 * Increase hold counter.
                 */
                
                hold_counter ++;
            } else {
                /*
                 * Button held for >= HOLD_TRESHOLD:
                 * Increase dimmer percentage, start from 10%
                 * if 100% has been reached.
                 */
                
                dim_pct = dimmer_get_dim_pct();

                if (dim_pct >= 100) {
                    dim_pct = 0;
                } else {
                    dim_pct += 2;
                }
                
                dimmer_set_dim_pct(dim_pct);
            }
        }

        last_button_state = button_state;
    }
}

/**
 * Initialize the button GPIO and start the poller task
 */
void button_init(void)
{
    static bool initialized = false;
    
    /* Return if already initialized */
    if (initialized == true)
        return;

    /* Initialize GPIO */
    PIN_PULLUP_DIS(BUTTON_PIN_REG);
    PIN_FUNC_SELECT(BUTTON_PIN_REG, BUTTON_PIN_FUNC);
    GPIO_DIS_OUTPUT(BUTTON_PIN_NO);
    GPIO_AS_INPUT(BUTTON_PIN_NO);
    
    /* Create the button task */
    xTaskCreate(s_button_task, "Button", configMINIMAL_STACK_SIZE,
                NULL, 4, NULL);

    /* Prevent further initialization */
    initialized = true;
}
