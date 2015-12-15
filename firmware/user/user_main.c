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

int authGetUserPw(HttpdConnData *connData, int no, char *user, int userLen, char *pass, int passLen) {
    int rv = 0;
    
    if (no == 0) {
        strncpy(user, "testaaja", userLen);
        strncpy(pass, "testi", passLen);
        rv = 1;
    }

    return rv;
}

HttpdBuiltInUrl g_builtInUrls[] = {
    {"*", authBasic, authGetUserPw },
    {"/", cgiRedirect, "/index.html"},
    {"/setStatus", user_cgi_set_status, NULL },
    {"/setDimmer", user_cgi_set_dimmer, NULL },
    {"/getDimmer", user_cgi_get_dimmer, NULL },
    {"*", cgiEspFsHook, NULL},
    {NULL, NULL, NULL}
};


#define DEMO_AP_SSID "ESP8266"
#define DEMO_AP_PASSWORD "espressif"

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{
    UART_ConfigTypeDef uart_config;
    struct softap_config *config;
    dimmer_status_t *dim_initial_status;

    uart_config.baud_rate = BIT_RATE_115200;
    uart_config.data_bits = UART_WordLength_8b;
    uart_config.parity = USART_Parity_None;
    uart_config.stop_bits = USART_StopBits_1;
    uart_config.flow_ctrl = USART_HardwareFlowControl_None;
    uart_config.UART_RxFlowThresh = 120;
    uart_config.UART_InverseMask = UART_None_Inverse;

    UART_ParamConfig(UART0, &uart_config);

    wifi_set_opmode(SOFTAP_MODE);
    
    config = (struct softap_config *)zalloc(sizeof(struct softap_config));
    wifi_softap_get_config(config);
    sprintf(config->ssid, DEMO_AP_SSID);
    sprintf(config->password, DEMO_AP_PASSWORD);
    config->authmode = AUTH_WPA_WPA2_PSK;
    config->ssid_len = 0;
    config->max_connection = 4;
    wifi_softap_set_config(config);
    free(config);

    espFsInit((void*)webpages_espfs_start);
    httpdInit(g_builtInUrls, 80);

    dim_initial_status = (dimmer_status_t *)zalloc(sizeof(dimmer_status_t));
    dim_initial_status->power_on = true;
    dim_initial_status->dim_pct  = 85;
    dimmer_init(dim_initial_status, 100, 0);
    free(dim_initial_status);

    button_init();
}
