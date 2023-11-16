/** @file   player_actions.h
    @author Geordie Gibson and Emma Badger
    @date   12 October 2022
    @brief  Is an interface for player actions in our Dodgeball Game, ENCE260 Assignment
*/

#ifndef GAME_INIT_H
#define GAME_INIT_H

#include "ball.h"

typedef struct {

    tinygl_point_t north_end;
    tinygl_point_t south_end;
    ball_t balls[10];
    ball_t incomingballs[10];
    int hit_count;
    char player_no;
    bool gameover;
    int gamespeed;

} player_t;

player_t set_player(char player_no);

void player_throw(player_t* player);

void incomingball_move(ball_t* ball, player_t* player);

void update_incoming_balls(player_t* player);

void update_active_balls(player_t* player);

#endif