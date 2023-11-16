/** @file   ball.c
    @author Geordie Gibson and Emma Badger
    @date   12 October 2022
    @brief  This implements the functions needed for the balls for our Dodgeball Game, ENCE260 Assignment
*/

#include "system.h"
#include "tinygl.h"
#include "ball.h"
#include <stdint.h>

// Creates an instance of a ball and returns it
ball_t create_ball() {

    tinygl_point_t position = {0,0};
    ball_t ball = {position, 0};
    return ball;
}


// Removes old ball position on screen, and updates new position, takes pointer to ball object as parameter
void ball_move(ball_t* ball) {
    
    const int back_board_index = 4;

    tinygl_coord_t x = ball->pos.x;
    tinygl_coord_t y = ball->pos.y;
    if (x!=back_board_index) {
        tinygl_point_t point = {x, y};
        tinygl_draw_point(point,0);
    }

    ball->pos.x --;

    tinygl_coord_t x1 = ball->pos.x;
    tinygl_coord_t y1 = ball->pos.y;
    tinygl_point_t newpoint = {x1, y1};
    tinygl_draw_point(newpoint,1);
}

