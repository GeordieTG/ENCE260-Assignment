# ENCE260-Assignment
Dodgeball Game on UCFK4 Micro-Controller for ENCE260 

# Instructions

Starting the game:

Both players cd into the directory group_221 in a terminal

Both players must push the reset button (S2)

Both players run the command 'make'

Both players run the command 'make program'
This will start the game.


To play:

Both screens will read dodgeball
To initialise the game, there are three levels to choose from.
One player needs to use the navswitch to start the game.
Easy - navswitch east
Medium - navswitch push
Hard - navswitch west

The game difficulty determines how fast the balls will travel.

One player will be assigned as Player 1, one as Player 2, this will be displayed on screens for short amount of time.

Then the game begins. Each player (3 dots on the back board) can move side to side by pushing the navswitch north and south, and throw a ball at the other player by pushing the navswitch west, towards their board.

Once you have been hit 15 times, you are on critical life and your blue LED will begin flashing to tell you this. The first player to be hit by a ball thrown by the other player 20 times loses and the game is over. The players boards will either scroll through the message “You win” or “You lose” depending on their result.

To restart the game, both players push the navswitch down.