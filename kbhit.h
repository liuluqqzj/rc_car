/*================================================================
 *   Copyright (C) 2019 Ltd. All rights reserved.
 *   
 *   File Name ：kbhit.h
 *   Author    ：Hamilton
 *   Date      ：2019-03-18
 *   Descriptor：
 *
 ================================================================*/


#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>


void set_conio_terminal_mode();
int kbhit(struct timeval val);
int getch();
