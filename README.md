# ab-pruning

This is a demonstration of alpha-beta pruning for a tic tac toe game.

The program takes only one argument; a string representing the state of your tic tac toe game. The game is entered row by row, with the top row being entered first, and the bottom row last. For example if you have the following game,

O_X
OXX
___

you would represent it with a string like this: O_XOXX___

You would then call the program like this:

>ab-pruning O_XOXX___

