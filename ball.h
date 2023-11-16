/** @file   ball.h
    @author Geordie Gibson and Emma Badger
    @date   12 October 2022
    @brief  This is the interface for the balls in our Dodgeball Game, ENCE260 Assignment
*/

#ifndef BALL_H
#define BALL_H

#include <stdint.h>
#include <stdbool.h>
#include "tinygl.h"

typedef struct
{
    tinygl_point_t pos;
    bool active;

} ball_t;

ball_t create_ball(void);

void ball_move(ball_t* ball);

#endif