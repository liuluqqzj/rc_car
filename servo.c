/*================================================================
 *   Copyright (C) 2019 Ltd. All rights reserved.
 *   
 *   File Name ：servo.c
 *   Author    ：Hamilton
 *   Date      ：2019-03-18
 *   Descriptor：
 *
 ================================================================*/

#include "servo.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

static servo_cfg_t servo_cfg;
static servo_state_t servo_state;

static int duty = 0;
static int idle_tick = 0;

int servo_init(servo_cfg_t *cfg)
{

    if (cfg == 0)
    {
        return -1;
    }

    memcpy(&servo_cfg, cfg, sizeof(servo_cfg_t));

    duty = servo_cfg.duty_standby;
    servo_state = SERVO_STANDBY;
    return 0;
}

void servo_update_state(servo_state_t state)
{
    servo_state = state;
}

static servo_state_t read_servo_state(void)
{
    servo_state_t state = servo_state;
    servo_state = SERVO_NONE;
    return state;
}

int servo_read_duty(void)
{
    return duty;
}

void servo_process_per_1ms(void)
{
    servo_state_t sstate;

    sstate = read_servo_state();

    if (sstate == SERVO_NONE)
    {
        if (++idle_tick > KEEP_TICK_LIMIT)
        {
            sstate = SERVO_STOP;
        }
    }
    else
    {
        idle_tick = 0;
    }

    switch (sstate)
    {
        case SERVO_DUTY_MAX:
                duty = servo_cfg.duty_maximum;
            break;

        case SERVO_DUTY_MIN:
                duty = servo_cfg.duty_minimum;
            break;
        
        case SERVO_STOP:
                duty = 0;
            break;
    }
   // printf("tick: %d \r\n", idle_tick);
}


