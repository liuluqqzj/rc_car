/*================================================================
 *   Copyright (C) 2019 Ltd. All rights reserved.
 *   
 *   File Name ：main.c
 *   Author    ：Hamilton
 *   Date      ：2019-03-18
 *   Descriptor：
 *
 ================================================================*/


#include "main.h"
#include "pthread.h"
#include "unistd.h"
#include "stdbool.h"

static pthread_t state_thread, esc_thread, servo_thread;
static int esc_pwm = -1, servo_pwm = -1;
static bool running = 1;

void *state_process(void *arg)
{
    while (running)
    {
        esc_process_per_1ms();
        servo_process_per_1ms();
        usleep(1000);
    }
}

void *esc_process(void *arg)
{
    int speed = 0, speed_tmp = 0;

    while (running)
    {
        usleep(1000);
        speed_tmp = esc_read_speed();
        if (speed != speed_tmp)
        {
            speed = speed_tmp;
            pwm_set_duty_in_clk(esc_pwm, speed);
            printf("%d \r\n", speed);
        }
    }
}

void *servo_process(void *arg)
{
    int duty = 0, duty_tmp = 0;

    while (running)
    {
        usleep(1000);
        duty_tmp = servo_read_duty();
        if (duty != duty_tmp)
        {
            duty = duty_tmp;
            pwm_set_duty_in_clk(servo_pwm, duty);
            printf("%d \r\n", duty);
        }
    }
}


int main(int argc, char *argv[])
{
    int ret;
    char ch;
    struct timeval val = {0, 1000};

    esc_cfg_t gcfg = 
    {
        .gear_num = 5
    };

    pwm_cfg_t pcfg = 
    {
        .freq = 50,
        .pin_num = 1
    };

    set_conio_terminal_mode();

    esc_pwm = pwm_init(&pcfg); // esc pwm
    gcfg.speed_forward_limit = pwm_get_duty_in_clk(esc_pwm, 10); // 2/20
    gcfg.speed_back_limit    = pwm_get_duty_in_clk(esc_pwm, 5); // 1/20
    gcfg.speed_standby       = pwm_get_duty_in_clk(esc_pwm, 7.5); // 1.5/20
    esc_init(&gcfg);

    pcfg.pin_num = 23;
    servo_pwm = pwm_init(&pcfg); // servo pwm
    servo_cfg_t scfg;
    scfg.duty_maximum = pwm_get_duty_in_clk(servo_pwm, 10 ); // 2.5/20
    scfg.duty_minimum = pwm_get_duty_in_clk(servo_pwm, 5  ); // 0.5/20
    scfg.duty_standby = pwm_get_duty_in_clk(servo_pwm, 7.5);   // 1.5/20
    servo_init(&scfg);

    if (pthread_create(&state_thread, NULL, state_process, "state thread") != 0)
    {
        perror("pthread_create() error");
        exit(1);
    }
    if (pthread_create(&esc_thread, NULL, esc_process, "esc thread") != 0)
    {
        perror("pthread_create() error");
        exit(1);
    }
    if (pthread_create(&servo_thread, NULL, servo_process, "servo thread") != 0)
    {
        perror("pthread_create() error");
        exit(1);
    }

    while (1)
    {
        if ((ret = kbhit(val)) < 0)
        {
            break;
        }
        if (ret > 0)
        {
            if ((ch = getch()) == 'q')
            {
                break;
            }
    //        printf("%c %x\r\n", ch, ch);

            switch (ch)
            {
                // esc control
                case 'w':
                    esc_update_driver_state(DRIVER_FORWARD);
                    break;
                case 's':
                    esc_update_driver_state(DRIVER_BACK);
                    break;
                case 'r':
                    esc_update_gear_state(GEAR_UP);
                    break;
                case 'f':
                    esc_update_gear_state(GEAR_DOWN);
                    break;
                case 0x20: // space button
                    esc_update_driver_state(DRIVER_BRAKE);
                    break;

                // servo control
                case 'a':
                    servo_update_state(SERVO_DUTY_MAX);
                    break;
                case 'd':
                    servo_update_state(SERVO_DUTY_MIN);
                    break;

            }
        }
    }

    running = 0;
    printf("finish.\r\n");

    sleep(1);
    return 0;
}
