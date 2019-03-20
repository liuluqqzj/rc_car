/*================================================================
*   Copyright (C) 2019 Ltd. All rights reserved.
*   
*   File Name ：hard_pwm.h
*   Author    ：Hamilton
*   Date      ：2019-03-18
*   Descriptor：
*
================================================================*/


#pragma once

#include "stdlib.h"
#include "stdbool.h"

#define PWM_NUM_LIMIT   2

typedef struct
{
    int freq;
    int pin_num;

} pwm_cfg_t;

typedef struct
{
    int instant;
    bool in_use;
    pwm_cfg_t cfg;
    int range_in_clk;
    int cycle_in_ms;
} pwm_inode_t;

int pwm_init (pwm_cfg_t *cfg);
int pwm_set_duty_in_percent(int inst, float percent);
int pwm_get_duty_in_clk(int inst, float percent);
int pwm_set_duty_in_clk(int inst, int clk);

