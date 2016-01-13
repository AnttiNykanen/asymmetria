/**
 * @file  user_dimmer.c
 * @brief PT4115 dimmer control
 * 
 * @author Antti Nykänen <aon@umetronics.com>
 */

#include "esp_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "user_dimmer.h"

static xSemaphoreHandle s_status_mutex;
static dimmer_status_t s_status;

static xQueueHandle s_dimmer_queue;

static uint32_t s_pwm_period;

static uint8_t s_pwm_channel;

static uint8_t s_initialized = 0;

static const uint32_t DIMMER_QUEUE_MSG = 0x55;

uint32 pwm_init_info[1][3]={
    {PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12, 12}
};

uint32 pwm_init_dcs[1] = {0x00};

/**
 * Convert duty cycle percentaget to actual duty cycle value
 *
 * @param  pct duty cycle percentage
 *
 * @return scaled duty cycle value 
 */
static uint32_t s_dimmer_pct_to_dc(uint8_t pct)
{
    if (pct > 100)
        pct = 100;

    /*
     * TODO: investigate why the formula in the docs gives incorrect
     * results but this seems to work more or less correctly
     */
    return (s_pwm_period * 10 * pct / 100);
}

/**
 * Get dimmer status
 *
 * @param status structure where the current status is to be placed
 */
void dimmer_get_status(dimmer_status_t *status)
{
    xSemaphoreTake(s_status_mutex, portMAX_DELAY);
    status->power_on = s_status.power_on;
    status->dim_pct = s_status.dim_pct;
    xSemaphoreGive(s_status_mutex);
}

/**
 * Set dimmer status
 *
 * @param status structure where the status is to be read from
 */
void dimmer_set_status(dimmer_status_t *status)
{
    xSemaphoreTake(s_status_mutex, portMAX_DELAY);
    s_status.dim_pct = status->dim_pct;
    s_status.power_on = status->power_on;
    xQueueSend(s_dimmer_queue, &DIMMER_QUEUE_MSG, 0);
    xSemaphoreGive(s_status_mutex);
}

/**
 * Set dimmer percentage
 *
 * @param dim_pct dim percentage
 */
void dimmer_set_dim_pct(uint8_t dim_pct)
{
    xSemaphoreTake(s_status_mutex, portMAX_DELAY);
    s_status.dim_pct  = dim_pct;
    xSemaphoreGive(s_status_mutex);
    xQueueSend(s_dimmer_queue, &DIMMER_QUEUE_MSG, 0);
}

/**
 * Get dimmer percentage
 *
 * @return dimmer percentage (0-100)
 */
uint8_t dimmer_get_dim_pct(void)
{
    uint8_t rv;
    xSemaphoreTake(s_status_mutex, portMAX_DELAY);
    rv = s_status.dim_pct;
    xSemaphoreGive(s_status_mutex);
    return rv;
}


/**
 * Set power on or off
 *
 * @param power_on whether to set the power on or off
 */
void dimmer_set_power_on(bool power_on)
{
    xSemaphoreTake(s_status_mutex, portMAX_DELAY);
    s_status.power_on = power_on;
    xSemaphoreGive(s_status_mutex);
    xQueueSend(s_dimmer_queue, &DIMMER_QUEUE_MSG, 0);
}

/**
 * Get power status
 *
 * @return power status (true=on, false=off)
 */
bool dimmer_get_power_on(void)
{
    bool rv;
    xSemaphoreTake(s_status_mutex, portMAX_DELAY);
    rv = s_status.power_on;
    xSemaphoreGive(s_status_mutex);
    return rv;
}

/**
 * Dimmer FreeRTOS task
 *
 * @param parameters task parameters
 */
static void dimmer_task(void *parameters)
{
    portBASE_TYPE recv_status;
    portBASE_TYPE recv_value;

    printf("Dimmer: task created\r\n");
    
    while (1) {
        recv_status = xQueueReceive(s_dimmer_queue, &recv_value, portMAX_DELAY);
        
        if (recv_status == pdPASS) {
            if (recv_value & DIMMER_QUEUE_MSG) {
                printf("Dimmer: received update notification\r\n");

                xSemaphoreTake(s_status_mutex, portMAX_DELAY);
                if (s_status.power_on == true) {
                    pwm_set_duty(s_dimmer_pct_to_dc(s_status.dim_pct), s_pwm_channel);
                } else {
                    pwm_set_duty(0, s_pwm_channel);
                }
                xSemaphoreGive(s_status_mutex);
                
                pwm_start();
            }
        }
    }
}

/**
 * Initialize the dimmer and start the RTOS task
 *
 * @param initial_status  initial dimmer status structure
 * @param pwm_period      PWM period
 * @param pwm_channel_num PWM channel number
 */
void dimmer_init(dimmer_status_t *initial_status, uint32_t pwm_period,
                 uint32_t pwm_channel_num)
{
    /* There can be only one task, so return if already initialized. */
    if (s_initialized)
        return;

    /* Create status mutex */
    s_status_mutex = xSemaphoreCreateMutex();

    /* Create notification message queue */
    s_dimmer_queue = xQueueCreate(5, sizeof(dimmer_status_t));
    
    s_pwm_period = pwm_period;

    /* Initialize status struct */
    s_status.power_on = initial_status->power_on;
    s_status.dim_pct = initial_status->dim_pct;

    /* Init PWM */
    s_pwm_channel = pwm_channel_num;

    if (s_status.power_on == true) {
        pwm_init_dcs[0] = s_dimmer_pct_to_dc(s_status.dim_pct);
    }

    pwm_init(pwm_period, pwm_init_dcs, 1, pwm_init_info);
    pwm_start();

    printf("Dimmer: PWM initialized and started\r\n");

    printf("Ch0 DC: %d\r\n", pwm_get_duty(0));
    printf("Period: %d\r\n", pwm_get_period());

    /* Create dimmer task */
    xTaskCreate(dimmer_task, "Dimmer", configMINIMAL_STACK_SIZE,
                NULL, 3, NULL);

    /* Prevent further intialization */
    s_initialized = 1;
}
