/*
 * pwm.c:
 *	This tests the hardware PWM channel.
 *
 * Copyright (c) 2012-2013 Gordon Henderson. <projects@drogon.net>
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "hard_pwm.h"

// default in pinMode() Set pin to PWM mode
// pwmSetMode  (PWM_MODE_BAL) ;	// Pi default mode
// pwmSetRange (1024) ;		// Default range of 1024
// pwmSetClock (32) ;		// 19.2 / 32 = 600KHz - Also starts the PWM
// #define PWM_MODE_MS     0
// 78 #define PWM_MODE_BAL        1

#define PWM_CLOCK       (600000)    // source clock, 600KHz default in this lib 

#define PWM_FREQ        50          // output frequence 50Hz
#define PWM_RANGE       (PWM_CLOCK/PWM_FREQ)    // output clock ticks/cycle = clock/freq
#define PWM_CYCLE       (1000/PWM_FREQ)         // output cycle = 1000/freq, ms
#define PWM_DUTY_1MS    (PWM_RANGE/(PWM_CYCLE/1))   // duty 1ms, get ticks in freq
#define PWM_DUTY_2MS    (PWM_RANGE/(PWM_CYCLE/2))   // duty 2ms

static pwm_inode_t pwm_cfg_tbl[PWM_NUM_LIMIT];

int pwm_init (pwm_cfg_t *cfg)
{
    int i = 0;

    if (cfg == 0 || cfg->pin_num == 0 || cfg->freq == 0)
    {
        return -1;
    }

    for (i = 0; i < PWM_NUM_LIMIT; i++)
    {
        if (!pwm_cfg_tbl[i].in_use)
        {           
            break;
        }
    }

    if (i == PWM_NUM_LIMIT)
    {
        return -2;
    }

    memcpy(&pwm_cfg_tbl[i].cfg, cfg, sizeof(pwm_cfg_t));
    pwm_cfg_tbl[i].in_use = true;
    pwm_cfg_tbl[i].instant = i;

    if (wiringPiSetup () == -1)
    { 
        exit (1) ;
    }

    pinMode (cfg->pin_num, PWM_OUTPUT) ;
    pwmSetMode(PWM_MODE_MS);
    pwm_cfg_tbl[i].range_in_clk = PWM_CLOCK/cfg->freq;
    pwmSetRange(pwm_cfg_tbl[i].range_in_clk);
    pwmWrite (cfg->pin_num, 0) ;
    
    return i ;
}

// percent : 0 ~100 refer to 0% ~100%
int pwm_set_duty_in_percent(int inst, float percent)
{
    int duty_in_clk;

    if (inst >= PWM_NUM_LIMIT || !pwm_cfg_tbl[inst].in_use)
    {
        return -1;
    }
    
    duty_in_clk = (percent * pwm_cfg_tbl[inst].range_in_clk)/100;
    pwmWrite(pwm_cfg_tbl[inst].cfg.pin_num, duty_in_clk);
    return 0;
}

// percent : 0 ~100 refer to 0% ~100%
int pwm_get_duty_in_clk(int inst, float percent)
{
    if (inst >= PWM_NUM_LIMIT || !pwm_cfg_tbl[inst].in_use)
    {
        return -1;
    }
    
    return (percent * pwm_cfg_tbl[inst].range_in_clk)/100;
}

// clk: refer to pwm source clock. 
int pwm_set_duty_in_clk(int inst, int clk)
{
    if (inst >= PWM_NUM_LIMIT || !pwm_cfg_tbl[inst].in_use)
    {
        return -1;
    }
    
    pwmWrite(pwm_cfg_tbl[inst].cfg.pin_num, clk);
    return 0;
}

int main_test (void)
{
    int bright ;

    printf ("Raspberry Pi wiringPi PWM test program\n") ;

    if (wiringPiSetup () == -1)
        exit (1) ;

    pinMode (1, PWM_OUTPUT) ;

    pwmSetMode(PWM_MODE_MS);
    pwmSetRange(PWM_RANGE);

    pwmWrite (1, PWM_DUTY_1MS) ;
    delay (1000) ;
    
    pwmWrite (1, PWM_DUTY_1MS+1000) ;
    delay (2000) ;
    

    for (;;)
    {
        for (bright = PWM_DUTY_1MS ; bright < PWM_DUTY_2MS ; ++bright)
        {
            pwmWrite (1, bright) ;
            delay (10) ;
        }
    }

    return 0 ;
}
