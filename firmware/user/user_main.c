/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/**
 * @file user_main.c
 * @brief User application main file
 */

#include "esp_common.h"
#include "driver/uart.h"

#include "httpd.h"
#include "httpdespfs.h"
#include "espfs.h"
#include "webpages-espfs.h"
#include "auth.h"

#include "user_cgi.h"
#include "user_dimmer.h"
#include "user_button.h"

/** HTTP authentication username */
#define AUTH_USER "testaaja"

/** HTTP authentication password */
#define AUTH_PASS "testi"

/** Wi-Fi SSID */
#define AP_SSID "ESP8266"

/** Wi-Fi Password */
#define AP_PASSWORD "none"

/** Wi-Fi Authentication mode */
#define AP_AUTHMODE AUTH_OPEN

/** Wi-Fi Soft-AP channel */
#define AP_CHANNEL 5

/**
 * HTTP authentication function
 */
int authGetUserPw(HttpdConnData *connData, int no, char *user, int userLen, char *pass, int passLen) {
    int rv = 0;
    
    if (no == 0) {
        strncpy(user, AUTH_USER, userLen);
        strncpy(pass, AUTH_PASS, passLen);
        rv = 1;
    }

    return rv;
}

/** HTTPD routes */
HttpdBuiltInUrl g_builtInUrls[] = {
    {"*", authBasic, authGetUserPw },
    {"/", cgiRedirect, "/index.html"},
    {"/setStatus", user_cgi_set_status, NULL },
    {"/setDimmer", user_cgi_set_dimmer, NULL },
    {"/getDimmer", user_cgi_get_dimmer, NULL },
    {"*", cgiEspFsHook, NULL},
    {NULL, NULL, NULL}
};


/**
 * Entry of user application, init user function here
 */
void user_init(void)
{
    UART_ConfigTypeDef uart_config;
    struct softap_config *config;
    dimmer_status_t *dim_initial_status;

    /* Set UART0 to 115200 bps, 8N1 */
    uart_config.baud_rate = BIT_RATE_115200;
    uart_config.data_bits = UART_WordLength_8b;
    uart_config.parity = USART_Parity_None;
    uart_config.stop_bits = USART_StopBits_1;
    uart_config.flow_ctrl = USART_HardwareFlowControl_None;
    uart_config.UART_RxFlowThresh = 120;
    uart_config.UART_InverseMask = UART_None_Inverse;

    UART_ParamConfig(UART0, &uart_config);

    /* Configure Wi-Fi */
    wifi_set_opmode(SOFTAP_MODE);
    
    config = (struct softap_config *)zalloc(sizeof(struct softap_config));
    wifi_softap_get_config(config);
    sprintf(config->ssid, AP_SSID);
    sprintf(config->password, AP_PASSWORD);
    config->authmode = AP_AUTHMODE;
    config->ssid_len = 0;
    config->max_connection = 4;
    config->channel = AP_CHANNEL;
    wifi_softap_set_config(config);
    free(config);

    /* Start httpd task */
    espFsInit((void*)webpages_espfs_start);
    httpdInit(g_builtInUrls, 80);

    /* Start dimmer task */
    dim_initial_status = (dimmer_status_t *)zalloc(sizeof(dimmer_status_t));
    dim_initial_status->power_on = true;
    dim_initial_status->dim_pct  = 85;
    dimmer_init(dim_initial_status, 50, 0);
    free(dim_initial_status);

    /* Start button task */
    button_init();
}
