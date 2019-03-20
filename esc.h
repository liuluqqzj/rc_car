/*================================================================
 *   Copyright (C) 2019 Ltd. All rights reserved.
 *   
 *   File Name ：esc.h
 *   Author    ：Hamilton
 *   Date      ：2019-03-18
 *   Descriptor：
 *
 ================================================================*/


#pragma once

#ifndef top_gear_h
#define top_gear_h

typedef enum
{
    DRIVER_NONE = 0,
    DRIVER_FORWARD,
    DRIVER_BACK,
    DRIVER_BRAKE
} driver_state_t;

typedef enum 
{
    GEAR_NONE = 0, 
    GEAR_UP, 
    GEAR_DOWN
} gear_state_t;


typedef struct 
{
    int speed_forward_limit;
    int speed_back_limit;
    int speed_standby;
    int gear_num;
} esc_cfg_t;

#define GEARS_NUM_LIMIT
#define IDLE_TICK_LIMIT         100 // ms
#define ACCEL_LOOP              1 // reach full speed in 1ms.

int esc_init(esc_cfg_t *gear_cfg);
void esc_update_driver_state(driver_state_t state);
driver_state_t esc_read_driver_state(void);
void esc_update_gear_state(gear_state_t state);
gear_state_t esc_read_gear_state(void);
int esc_read_speed(void);
void esc_process_per_1ms(void);

#endif /* top_gear_h */

