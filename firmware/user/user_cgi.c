/**
 * @file user_cgi.c
 * @brief Application-specific CGI handlers
 *  
 * @author Antti Nykänen <aon@umetronics.com>
 */

#include "esp_common.h"

#include "json/cJSON.h"

#include "user_cgi.h"
#include "user_dimmer.h"

/**
 * Set the dimmer on or off based on the "status" get parameter
 *
 * @param conn_data HTTP connection data
 */
int user_cgi_set_status(HttpdConnData *conn_data)
{
    int  len;
    char buf[4];

    len = httpdFindArg(conn_data->getArgs, "status", buf, sizeof(buf));

    if (len > 0) {
        if (!strcmp(buf, "on")) {
            dimmer_set_power_on(true);
        } else if (!strcmp(buf, "off")) {
            dimmer_set_power_on(false);
        }
    }

    return HTTPD_CGI_DONE;
}

/**
 * Set the dimmer percentage based on the "dimValue" get parameter
 *
 * @param conn_data HTTP connection data
 */
int user_cgi_set_dimmer(HttpdConnData *conn_data)
{
    int  len;
    char buf[4];

    len = httpdFindArg(conn_data->getArgs, "dimValue", buf, sizeof(buf));

    if (len > 0) {
        dimmer_set_dim_pct((uint8_t)atoi(buf));
    }

    return HTTPD_CGI_DONE;
    
}

/**
 * Return a JSON structure with the current power state and percentage
 *
 * @param conn_data HTTP connection data
 */
int user_cgi_get_dimmer(HttpdConnData *conn_data)
{
    cJSON *json;
    char  *out;
    dimmer_status_t status;
    
    dimmer_get_status(&status);
    
    json = cJSON_CreateObject();

    if (status.power_on == true) {
        cJSON_AddStringToObject(json, "status", "on");
    } else {
        cJSON_AddStringToObject(json, "status", "off");
    }

    cJSON_AddNumberToObject(json, "dimValue", status.dim_pct);

    out = cJSON_Print(json);

    httpdStartResponse(conn_data, 200);
    httpdHeader(conn_data, "Content-Type", "text/json");
    httpdEndHeaders(conn_data);

    httpdSend(conn_data, out, strlen(out));

    return HTTPD_CGI_DONE;
}
