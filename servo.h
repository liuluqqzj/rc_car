/*================================================================
*   Copyright (C) 2019 Ltd. All rights reserved.
*   
*   File Name ：servo.h
*   Author    ：Hamilton
*   Date      ：2019-03-19
*   Descriptor：
*
================================================================*/


#pragma onc

#define KEEP_TICK_LIMIT 100 //

typedef struct
{
    int duty_maximum;
    int duty_minimum;
    int duty_standby;

} servo_cfg_t;

typedef enum
{
    SERVO_NONE,
    SERVO_DUTY_MIN,
    SERVO_DUTY_MAX,
    SERVO_STANDBY,
    SERVO_STOP
} servo_state_t;

int servo_init(servo_cfg_t *cfg);
void servo_update_state(servo_state_t state);
void servo_process_per_1ms(void);
int servo_read_duty(void);

