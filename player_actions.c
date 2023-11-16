/** @file   player_actions.c
    @author Geordie Gibson and Emma Badger
    @date   12 October 2022
    @brief  Implements player action functionality for our Dodgeball Game, ENCE260 Assignment
*/

#include "system.h"
#include "pacer.h"
#include "tinygl.h"
#include "../fonts/font3x5_1.h"
#include "pio.h"
#include "player_actions.h"
#include "navswitch.h"
#include "ball.h"
#include "ir_uart.h"
#include "usart1.h"

#define PACER_RATE 500
#define MESSAGE_RATE 10
#define MAX_ACTIVE_BALLS 10

// Initialises new player with starting values. Creates arrays of inactive incoming and outgoing balls. Takes player number 1 or 2 as parameter
// Returns initialised player
player_t set_player(char player_no) {

    player_t player;
    player.north_end = tinygl_point (4, 2);
    player.south_end = tinygl_point (4, 4);
    player.hit_count = 0;
    player.player_no = player_no;
    player.gameover = 0;

    for (int i = 0; i < MAX_ACTIVE_BALLS; i++) {
        player.balls[i] = create_ball();
    }
    for (int i = 0; i < MAX_ACTIVE_BALLS; i++) {
        player.incomingballs[i] = create_ball();
    }

    return player;
}

// Makes a ball active in player's ball array, starts it moving across screen starting from player's position, takes pointer to player object as parameter
void player_throw(player_t* player) {

    for (int i = 0; i<MAX_ACTIVE_BALLS; i++) {
        if (player->balls[i].active == 0) {
            player->balls[i].active = 1;
            
            // Changes the balls position to the players current position
            tinygl_point_t newPoint = {player->north_end.x, player->north_end.y+1};
            player->balls[i].pos = newPoint;

            ball_move(&(player->balls[i]));
            break;
        }
    }
}

// Updates old position of incoming ball as it moves along screen, takes pointer to ball object and pointer to player object as parameters
void incomingball_move(ball_t* ball, player_t* player) {

    const int back_board_index = 4;
    
    tinygl_coord_t x = ball->pos.x;
    tinygl_coord_t y = ball->pos.y;
    if (x!=back_board_index) {
        tinygl_point_t point = {x, y};
        tinygl_draw_point(point,0);
    } else {
        ball->active=0;
        if ((y>player->south_end.y) || (y<player->north_end.y)) {
            tinygl_point_t point = {x, y};
            tinygl_draw_point(point,0);
        } else {
            player->hit_count++;
        }
    }

    ball->pos.x ++;

    tinygl_coord_t x1 = ball->pos.x;
    tinygl_coord_t y1 = ball->pos.y;
    tinygl_point_t newpoint = {x1, y1};
    tinygl_draw_point(newpoint,1);

}

// Updates all balls that have been thrown onto player's screen, takes a pointer to player as parameter
void update_incoming_balls(player_t* player) {

    if (ir_uart_read_ready_p ()) {

        char incoming = ir_uart_getc ();

        // Check if character is indicating game is over
        if (incoming == 'W') {
            player->gameover = 1;
        } else {
            int column = 6 - incoming;
            tinygl_point_t point = {0, column};
            for (int i = 0; i<MAX_ACTIVE_BALLS; i++) {

                if (player->incomingballs[i].active == 0) {
                    player->incomingballs[i].active = 1;
                    player->incomingballs[i].pos = point;
                    tinygl_draw_point(player->incomingballs[i].pos,1);
                    break;
                }
            }
        }
    }
    
    for (int i = 0; i<MAX_ACTIVE_BALLS; i++) {

        if (player->incomingballs[i].active == 1) {
            incomingball_move(&(player->incomingballs[i]), player);
        }
    }
}

// Updates position of all active thrown balls of player, takes pointer to player object as parameter
void update_active_balls(player_t* player) {

    for (int i = 0; i<MAX_ACTIVE_BALLS; i++) {
        if (player->balls[i].active == 1) {
            if (player->balls[i].pos.x==0) {
                ir_uart_putc (player->balls[i].pos.y);
                player->balls[i].active=0;
                tinygl_draw_point(player->balls[i].pos,0);
            } else {
                ball_move(&(player->balls[i]));
            }
        }
    }
}

