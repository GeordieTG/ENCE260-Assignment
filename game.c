/** @file   game.c
    @author Geordie Gibson and Emma Badger
    @date   12 October 2022
    @brief  Dodgeball Game, ENCE260 Assignment
*/

// Magic Numbers were declared inside functions if that was the only place it was used
// If was used in multiple functions it was declared using #define

#include "system.h"
#include "pacer.h"
#include "tinygl.h"
#include "../fonts/font3x5_1.h"
#include "pio.h"
#include "player_actions.h"
#include "navswitch.h"
#include "ir_uart.h"
#include "usart1.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "led.h"
#include <avr/io.h>

#define PACER_RATE 500
#define MESSAGE_RATE 10

// Enum storing all the states of the game
typedef enum {

    INTRO,
    PLAYER_ASSIGNMENT,
    GAME,
    OUTRO_LOSS,
    OUTRO_WIN,
    IDLE

} GAME_STATE;

// Sets the intial state of the game
static GAME_STATE state = INTRO;
static GAME_STATE previous = IDLE;

// Checks that the player is in bounds of the led matrix, takes tinygl poiint of each end of player as parameters
// Returns true if in bounds, false if not.
bool inbounds (tinygl_point_t posNorth, tinygl_point_t posSouth)
{
    const int screen_boundary = TINYGL_WIDTH + 1;
    return posNorth.y >= 0 && posSouth.y <= screen_boundary;    
}

// Function to move the player side to side, takes displacement of player and pointer to player object as parameters
void move_player (int dy, player_t* player)
{
    // Creates proposed new start and end points of the player
    tinygl_point_t newNorth = {player->north_end.x, player->north_end.y + dy};
    tinygl_point_t newSouth = {player->south_end.x, player->south_end.y + dy};

    // Checks if the player is in bounds of the led matrix, returns without updating player if they're not
    if (!inbounds (newNorth, newSouth)) {
        return;
    }

    // Otherwise player is in bounds and position needs updating

    // Deletes old position
    tinygl_draw_line (player->north_end, player->south_end,0); 
    tinygl_update();

    // Sets new position
    player->north_end = newNorth;
    player->south_end = newSouth;

    // Draws player on the screen
    tinygl_draw_line (player->north_end, player->south_end,1); 
    tinygl_update();
}

// Draws the player on the screen when the game first starts, takes a player object as parameter
void init_player(player_t player) {
    tinygl_draw_line (player.north_end, player.south_end,1);
}

// Main for game, cycles through game states
int main (void)
{
    player_t player;
    system_init ();
    navswitch_init ();
    ir_uart_init ();
    led_init ();
    led_set(LED1, 0);
    pacer_init (PACER_RATE);
    int delay = 0;

    tinygl_init (PACER_RATE);
    tinygl_font_set(&font3x5_1);
    tinygl_text_speed_set (MESSAGE_RATE);
    tinygl_text_dir_set(TINYGL_TEXT_DIR_ROTATE);
    tinygl_text_mode_set(TINYGL_TEXT_MODE_SCROLL);
    int update_balls_delay;

    while (1)
    {
        pacer_wait ();
        tinygl_update ();
        navswitch_update ();


        switch (state) {

            // Shows name of the game, waits for either player to initialise the game by pushing the NAVSWITCH
            case INTRO:

                player = set_player('1');
                if (previous != INTRO) {
                    tinygl_text("DODGEBALL");
                }

                if (ir_uart_read_ready_p ()) {
                    char player_difficulty = ir_uart_getc ();
                    if ((player_difficulty=='E') | (player_difficulty == 'M') | (player_difficulty == 'H')) {
                        tinygl_clear();
                        player.player_no = '2';
                        if (player_difficulty == 'E') {
                            player.gamespeed = 300;
                        } else if (player_difficulty == 'H') {
                            player.gamespeed = 60;
                        } else {
                            player.gamespeed = 100;
                        }
                        state = PLAYER_ASSIGNMENT;
                    }
                }
                    
                if (navswitch_push_event_p (NAVSWITCH_PUSH)) {
                    player.gamespeed = 100;
                    tinygl_clear();
                    player.player_no = '1';
                    ir_uart_putc ('M');
                    state = PLAYER_ASSIGNMENT;
                }

                if (navswitch_push_event_p (NAVSWITCH_WEST)) {
                    player.gamespeed = 60;
                    tinygl_clear();
                    player.player_no = '1';
                    ir_uart_putc ('H');
                    state = PLAYER_ASSIGNMENT;
                }

                if (navswitch_push_event_p (NAVSWITCH_EAST)) {
                    player.gamespeed = 250;
                    tinygl_clear();
                    player.player_no = '1';
                    ir_uart_putc ('E');
                    state = PLAYER_ASSIGNMENT;
                }

                previous = INTRO;
                break;

            // Displays the player number of each player
            case PLAYER_ASSIGNMENT:

                delay++;
                const int no_display_time = 1000;
                tinygl_text_mode_set(TINYGL_TEXT_MODE_STEP);

                if (previous != PLAYER_ASSIGNMENT) {
                    if (player.player_no == '1') {
                        tinygl_text("P1");
                    } else {
                        tinygl_text("P2");
                    }
                }
                
                if (delay>=no_display_time) {
                    delay = 0;
                    tinygl_clear();
                    init_player(player);
                    state = GAME;
                }

                previous = PLAYER_ASSIGNMENT;
                break;
                
            // State where game is played, players can move and throw balls and scores are recorded
            case GAME:

                delay++;
                update_balls_delay = player.gamespeed;

                const int led_flash_time = 50;
                const int critical_hit_count = 15;
                const int max_hit_count = 20;

                if (player.gameover==0) {
                    if (delay>=update_balls_delay) {
                        delay = 0;
                        update_active_balls(&player);
                        update_incoming_balls(&player);
                        if (player.gameover == 1) {
                            state = OUTRO_WIN;
                        }
                    }

                    tinygl_update();
                    if (player.hit_count>=critical_hit_count) {
                        if (delay>=led_flash_time) {
                            led_set (LED1, 1);
                        } else {
                            led_set (LED1, 0);
                        }
                    } else {
                        led_set (LED1, 0);
                    }

                    if (player.hit_count>=max_hit_count) {
                        player.gameover = 1;
                        ir_uart_putc ('W');
                        state = OUTRO_LOSS;
                    }

                    // Move player to the right
                    if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
                        move_player (-1, &player);
                    }

                    // Move player to the left
                    if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {
                        move_player (1, &player);
                    }

                    // Throw a ball forward
                    if (navswitch_push_event_p (NAVSWITCH_WEST)) {
                        player_throw(&player);
                    }

                } else {
                    state = OUTRO_WIN;
                }

                break;

            // State for showing the losing player that they lost
            case OUTRO_LOSS:

                tinygl_text_mode_set(TINYGL_TEXT_MODE_SCROLL);
                led_set (LED1, 0);

                if (previous != OUTRO_LOSS) {
                    tinygl_text("YOU LOSE :(");
                }

                previous = OUTRO_LOSS;
                if (navswitch_push_event_p (NAVSWITCH_PUSH)) {
                    tinygl_clear();
                    state = INTRO;
                }
                break;

            // State for showing the winning player that they won
            case OUTRO_WIN:

                tinygl_text_mode_set(TINYGL_TEXT_MODE_SCROLL);
                led_set (LED1, 0);

                if (previous != OUTRO_WIN) {
                    tinygl_text("YOU WIN!");
                }

                previous = OUTRO_WIN;
                if (navswitch_push_event_p (NAVSWITCH_PUSH)) {
                    tinygl_clear();
                    delay = 0;
                    state = INTRO;
                }
                break;

            // IDLE state, state that previous is set as initially.
            case IDLE:
                break;
        }
    }
    return 0;
}
