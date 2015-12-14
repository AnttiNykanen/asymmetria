/**
 * @file user_cgi.h
 * @brief Application-specific CGI handler definitions
 *
 * @author Antti Nykänen <aon@umetronics.com>
 */

#include "httpd.h"

int user_cgi_set_status(HttpdConnData *conn_data);
int user_cgi_set_dimmer(HttpdConnData *conn_data);
int user_cgi_get_dimmer(HttpdConnData *conn_data);
