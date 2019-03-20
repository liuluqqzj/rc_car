/*================================================================
 *   Copyright (C) 2019 Ltd. All rights reserved.
 *   
 *   File Name ：esc.c
 *   Author    ：Hamilton
 *   Date      ：2019-03-18
 *   Descriptor：
 *
 ================================================================*/

#include "esc.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

static esc_cfg_t  esc_cfg;
static driver_state_t driver_state;
static gear_state_t   gear_state;

static int speed = 0;
static int gear_index = 0;
static int forward_speed_table[GEARS_NUM_LIMIT] = {1600, 1700, 1800, 1900, 2000}; 
static int forward_accel_unit[GEARS_NUM_LIMIT]  = {10, 20, 30, 40, 50};     // 10ms完成加速
static int back_speed_table[GEARS_NUM_LIMIT]    = {1400, 1300, 1200, 1100, 1000}; 
static int back_accel_unit[GEARS_NUM_LIMIT]     = {10, 20, 30, 40, 50};     // 10ms完成加速
static int idle_tick = 0;

int esc_init(esc_cfg_t *cfg)
{

    int forward_range, forward_gear_unit;
    int back_range, back_gear_unit;
    int i = 0;

    if (cfg == 0)
    {
        return -1;
    }

    memcpy(&esc_cfg, cfg, sizeof(esc_cfg_t));

    forward_range      = esc_cfg.speed_forward_limit - esc_cfg.speed_standby;
    forward_gear_unit  = forward_range/esc_cfg.gear_num;
    for (i = 0; i < esc_cfg.gear_num; i++)
    {
        forward_speed_table[i] = esc_cfg.speed_standby + forward_gear_unit*(i+1);
        forward_accel_unit [i] = (forward_gear_unit*(i+1))/ACCEL_LOOP;
    }

    back_range     = esc_cfg.speed_standby - esc_cfg.speed_back_limit;
    back_gear_unit = back_range/esc_cfg.gear_num;
    for (i = 0; i < esc_cfg.gear_num; i++)
    {
        back_speed_table[i] = esc_cfg.speed_standby - back_gear_unit*(i+1);
        back_accel_unit [i] = (back_gear_unit*(i+1))/ACCEL_LOOP;
    }

    speed = esc_cfg.speed_standby;
    gear_index = 0;
    idle_tick = 0;

    driver_state = DRIVER_NONE;
    gear_state = GEAR_NONE;

    printf("%d   %d   %d \r\n   %d  %d  %d  %d   \r\n", esc_cfg.speed_forward_limit,  esc_cfg.speed_back_limit, esc_cfg.speed_standby, 
            forward_speed_table[0], forward_accel_unit[0], back_speed_table[0], back_accel_unit[0] );
    return 0;
}

void esc_update_driver_state(driver_state_t state)
{
    driver_state = state;
}

static driver_state_t read_driver_state(void)
{
    driver_state_t state = driver_state;
    driver_state = DRIVER_NONE;
    return state;
}

void esc_update_gear_state(gear_state_t state)
{
    gear_state = state;
}

static gear_state_t read_gear_state(void)
{
    gear_state_t state = gear_state;
    gear_state = GEAR_NONE;
    return state;
}

int esc_read_speed(void)
{
    return speed;
}

void esc_process_per_1ms(void)
{
    gear_state_t gstate;
    driver_state_t dstate;

    gstate = read_gear_state();

    switch (gstate)
    {
        case GEAR_UP:
            if (gear_index < esc_cfg.gear_num - 1)
            {
                gear_index++;
            }
            break;

        case GEAR_DOWN:
            if (gear_index > 0)
            {
                gear_index--;
            }
            break;
    }

    dstate = read_driver_state();

    if (dstate == DRIVER_NONE)
    {
        if (++idle_tick > IDLE_TICK_LIMIT)
        {
            dstate = DRIVER_BRAKE;
        }
    }
    else
    {
        idle_tick = 0;
    }

    switch (dstate)
    {
        case DRIVER_FORWARD:
            if (speed <  (forward_speed_table[gear_index] - forward_accel_unit[gear_index]))
            {
                speed += forward_accel_unit[gear_index];
            }
            else
            {
                speed = forward_speed_table[gear_index];
            }
            break;

        case DRIVER_BACK:
            if (speed > (back_speed_table[gear_index] + back_accel_unit[gear_index]))
            {
                speed -= back_accel_unit[gear_index];
            }
            else
            {
                speed = back_speed_table[gear_index];
            }
            break;

        case DRIVER_BRAKE:
            speed = esc_cfg.speed_standby;
    }

}


