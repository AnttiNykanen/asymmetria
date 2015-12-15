/**
 * @file  user_dimmer.h
 * @brief Dimmer control definitions
 * 
 * @author Antti Nykänen <aon@umetronics.com>
 */

#pragma once

typedef struct {
    bool    power_on;
    uint8_t dim_pct;
} dimmer_status_t;

void dimmer_get_status(dimmer_status_t *status);
void dimmer_set_status(dimmer_status_t *status);
void dimmer_set_dim_pct(uint8_t dim_pct);
void dimmer_set_power_on(bool power_on);
uint8_t dimmer_get_dim_pct(void);
bool dimmer_get_power_on(void);


void dimmer_init(dimmer_status_t *initial_status, uint32_t pwm_period,
                 uint32_t pwm_channel_num);
                 
