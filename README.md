# Tic-tac-toe for Linux
University of Calgary
	CPSC 599.82 Retro Games Fall 2016
	Assignment 2
	Nizar Maan 10103889

MAKE SURE YOU HAVE THE NCURSES LIBRARY INSTALLED IN YOUR UNIX SYSTEM
http://tldp.org/HOWTO/NCURSES-Programming-HOWTO/intro.html#WHERETOGETIT

LINK TO VIDEO
https://www.youtube.com/watch?v=40WRFK_gfnQ&feature=youtu.be

						RUNNING THE GAME

-navigate through command line to the location of tictactoe.c and enter the following to compile:
	
	gcc -std=c99 -otictactoe.out -Isrc/ duk_src/duktape.c tictactoe.c -lm -lncurses

then run with:

	./tictactoe.out <optional js file>

or (more easily)

-Make use of "make run" command to compile and run tictactoe.c
	-running without setting ARGS will default the game's AI to use a Random move strategy
	-use 

        make ARGS="<javascript file name>" run

    to run the game with a different strategy

-Javascript strategy files must have a ReadBoard(x) function in order for duk_tape context to work
-If a javascript file is loaded but its structure is invalid the game will default back to Random strategy
-Compilation errors with the javascript file will terminate the game

MODES

	COOK
	Pressing the '`' or '~' key will take the user to command line mode, entering "exit" will take the user back to where they left off

	RESET
	Pressing 'r' will reset the game board

	LOAD
	Pressing 'l' will activate load mode where a user can enter the name or path to a javascript file to change the AI strategy
	*strategy will remain unchanged if invalid file is provided, also, providing a name that is too long will exit the program

	EXIT
	Pressing 'esc' will exit the game

NOTES

I make use of blinking "you win/lose/tie" messages however please note that character Blinking is not supported on the standard UNIX terminal, for blinking text support use the XTERM terminal

IMPORTANT
**Please note that mouse clicks should be kept steady in order for them to register on the console

MY STRATEGY FILES

	center_strat.js - this strategy will more than likely always end in a tie if you let the AI take the middle cell
					  the AI's first move will always be to take the middle cell if it's available
					  you should always play the middle cell first if you want to win

	first_available.js - the AI just takes the first available cell it finds
