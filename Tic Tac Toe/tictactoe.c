/* 
University of Calgary
	CPSC 599.82 Retro Games Fall 2016
	Assignment 2
	Nizar Maan 10103889
*/
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "duk_src/duktape.h"

#define CELL_WIDTH 4
#define CELL_HEIGHT 3
#define DEFAULT_STRAT "Random"

//defining Cell objects that make up the board
typedef struct{
	int top_left_x;		//top left x cord of cell
	int top_left_y;		//top left y cord of cell
	int char_x;	//xcord where symbol is to be drawn
	int char_y;	//ycord where symbol is to be drawn
	char symbol; //symbol to draw in the cell O or X
} Cell;

void INIT_BOARD(Cell Board[]);
void INIT_CELL(int i, Cell Board[]);
void DRAW_BORDERS();
void SET_UP_SCREEN();
void AI_makeMove(Cell Board[]);
void Draw_Cell_Content(Cell Board[]);
void RESET_BOARD(Cell Board[]);
void COOKED_MODE();
void DISP_STRAT();
void Change_Strat(char newStrat[]);
void DISP_WARNING(int id); //id = 0 for error in strategy file id = 1 for invalid strategy file provided
void ERASE_WARNING();
bool LOAD_STRAT();
bool CHECK_FULL(Cell Board[]);
int Check_Win(Cell Board[]);

//file scope variables
int _BOARD_TOP_LEFT_Y;	//defining top left corner coordinates of the board
int _BOARD_TOP_LEFT_X;
char AI_SYMB, USER_SYMB; //'O' and 'X' respectively
int winner; //variable to check who won -1 tie, 0 player, 1 CPU, 2 means game quit
char currentStrat[40];	//will hold name of javascript file used as the current strategy used by AI
duk_context * ctx;
char board_state[9]; //0-2 first row, 3-5 second row, 6-8 bottom row
bool warning_on;

int main(int argc, char * argv[]){	
	//initialize some values
	int ch, turn; //0 for Player 1 (User, always goes first), 1 for Player 2(AI)
	MEVENT mouseEvent;
	FILE * fp;
	Cell Board[9]; //0-2 first row, 3-5 second row, 6-8 bottom row
	srand(time(NULL));	//set up random number generator
	strcpy(currentStrat, DEFAULT_STRAT);
	ctx = NULL;	//initialize duk_context
	warning_on = false;

	AI_SYMB = 'O';
	USER_SYMB = 'X';

	//exit program if too many args provided
	if(argc > 2)
	{
		printf("Usage: %s <empty> or <path to .js file>\n", argv[0]);
		exit(1);
	}

	if(argv[1] != NULL){
		fp = fopen(argv[1], "r");
		if(fp == NULL){
			perror("Cannot open file");
			exit(1);
		}
		if(strlen(argv[1]) > 40){
			printf("File name too long\n");
			exit(1);
		}

		strcpy(currentStrat, argv[1]);	//set current strat name to input file

		/*get last 3 chars of file name (should be length of 3 ".js")*/
		int i;
		char extension[3];
		for(i = 0; i < 3; i++){
			extension[i] = currentStrat[strlen(currentStrat)-3+i];
		}

		/*Check if file is a javascript file*/
		if(strcmp(extension,".js") != 0){
			printf("Invalid file type: '%s'\n", extension);
			printf("Usage: %s <empty> or <.js file name>\n", argv[0]);
			exit(1);
		}
	}
	
	initscr();				//Start curses mode

	noecho();				//dont display user keystrokes

	attron(A_BOLD);			//bold all text

	_BOARD_TOP_LEFT_Y = (LINES/2)-CELL_HEIGHT*3;
	_BOARD_TOP_LEFT_X = (COLS/2)-CELL_WIDTH*2;

	mousemask(ALL_MOUSE_EVENTS, NULL);	//turning on all mouse events
	curs_set(0);			//make cursor invisible
	keypad(stdscr, TRUE);	//enable use of special keys

	//Game loop
	while(true)	
	{
		SET_UP_SCREEN();

		INIT_BOARD(Board);

		turn = 0; //0 for Player 1 (User always goes first), 1 for Player 2(AI)

		while(true){
			/*Check if board is at a stalemate*/
			if(CHECK_FULL(Board)){
				break;	//if board is full loop will break with -1 in "winner" variable
			}
			if(turn == 0){
				ch = getch();
				if(ch == 27){//ASCII valuefor ESC
					endwin();				//End curses mode
					return 0;
				}
				if(ch == '`'){
					COOKED_MODE();
					SET_UP_SCREEN();
					DRAW_BORDERS();
					Draw_Cell_Content(Board);
				}
				/*reset game*/
				if(ch == 'r'){
					clear();
					SET_UP_SCREEN();
					INIT_BOARD(Board);
				}
				if(ch == 'l'){
					clear();
					bool success = LOAD_STRAT();
                    clear();
					curs_set(0);				//make cursor visible
					noecho();					//display keystrokes
					SET_UP_SCREEN();
					DRAW_BORDERS();
					Draw_Cell_Content(Board);
					//if unsuccessful file load display warning
					if (!success){
						DISP_WARNING(1);
					}
				}
				if(ch == KEY_MOUSE){
					if(getmouse(&mouseEvent) == OK){
						if(mouseEvent.bstate & BUTTON1_CLICKED){	//check for Left-mouse button click
							int i;
							for(i = 0; i < 9; i++){
								//here we check if a mouse click is inside any of the board's cells
								if((mouseEvent.x < Board[i].top_left_x + CELL_WIDTH) && (mouseEvent.x > Board[i].top_left_x)){
									if((mouseEvent.y < Board[i].top_left_y + CELL_HEIGHT + 1) && (mouseEvent.y > Board[i].top_left_y)){
										if(Board[i].symbol == ' '){
											Board[i].symbol = USER_SYMB;	//update Cell's symbol
											turn = 1;	//AI's turn now
											if(warning_on){
												//erase the strategy change warning if it was being displayed
												warning_on = false;
												ERASE_WARNING();
											}
										}
									}
								}
							}
						}
					}
				}
			}
			else{
				//AI make move
				AI_makeMove(Board);
				turn = 0;
			}

			Draw_Cell_Content(Board);

			winner = Check_Win(Board);
			if(winner == 0 || winner == 1){
				break;
			}
		}

		clear();
		DRAW_BORDERS();
		Draw_Cell_Content(Board);
		
		//BLINKING text support only on XTERM terminal, not on regular unix terminal
		if(winner == 1){
			move(LINES-8, (COLS/2) - 5);
			attron(A_BLINK);
			printw("CPU WINS!");
			attroff(A_BLINK);
		}
		else if (winner == 0){
			move(LINES-8, (COLS/2) - 5);
			attron(A_BLINK);
			printw("YOU WIN!");
			attroff(A_BLINK);
		}
		else if (winner == -1){
			move(LINES-8, (COLS/2) - 2);
			attron(A_BLINK);
			printw("TIE");
			attroff(A_BLINK);
		}

		move(LINES-1,0);
		printw("Press any key to play again or ESC to exit");

		ch = getch();

		if(ch == 27){			
			endwin();				//End curses mode
			return 0;
		}
		
		clear();
		RESET_BOARD(Board);			//reset and loop back for another round
	}
}

/*Cooked Mode, i.e. terminal mode, enter "exit" to get back to game*/
void COOKED_MODE(){
	clear();
	def_prog_mode();		/* Save the tty modes		  */
	endwin();				/* End curses mode temporarily	  */
	system("/bin/sh");		/* Do whatever you like in cooked mode */
	reset_prog_mode();		/* Return to the previous tty mode*/
	refresh();
}

//set and display up on-screen text
void SET_UP_SCREEN(){
	char welcomeMSG[] = "WELCOME TO TIC-TAC-TOE!";
	move(LINES-6, (COLS/2)-(strlen(welcomeMSG)/2)-1);	//centering the game title
	start_color();
	init_pair(1, COLOR_YELLOW, COLOR_BLACK);	//set up a color pair, fore-ground color and background color
	attron(COLOR_PAIR(1));
	printw("%s", welcomeMSG);
	attroff(COLOR_PAIR(1));

	move(LINES-1, 0);
	printw("Press '~' key to go back to command line mode (enter 'exit' to continue game)");

	init_pair(4, COLOR_RED, COLOR_BLACK);
	attron(COLOR_PAIR(4));
	move(LINES-2,0);
	printw("'ESC' to exit game, 'r' to reset, 'l' to load Javascript file                   ");
	attroff(COLOR_PAIR(4));

	attroff(A_BOLD);
	char msg[] = "*Keep mouse clicks steady in order for them to register";
	move(LINES-4, (COLS/2)-(strlen(msg)/2));
	printw("%s", msg);
	attron(A_BOLD);

	move(0,0);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	attron(COLOR_PAIR(2));
	printw("P1:  X");
	attroff(COLOR_PAIR(2));

	init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
	attron(COLOR_PAIR(3));
	printw("\nCPU: O");
	attroff(COLOR_PAIR(3));

	DISP_STRAT();

	refresh();
}

//function that sets a new strategy and redisplays its name
void Change_Strat(char newStrat[]){
	strcpy(currentStrat, newStrat);
	DISP_STRAT();
}

//function to display an error message when there was an error with the chosen strategy file's output
//id = 0 for Warning user of an error in strategy file 
//id = 1 for invalid strategy file provided
void DISP_WARNING(int id){
	warning_on = true;
	init_pair(8, COLOR_RED, COLOR_BLACK);
	attron(COLOR_PAIR(8));
	move(0,8);
	if(id == 0){
		printw("*ERROR IN STRATEGY FILE; STRAT SET TO RANDOM");
	}
	if(id == 1){
		printw("*STRATEGY UNCHANGED - INVALID FILE");
	}
	attroff(COLOR_PAIR(8));
}

//function to erase the "Error in file" message
void ERASE_WARNING(){
	move(0,8);
	printw("                                            ");
}

//print current AI strategy to screen
void DISP_STRAT(){
	move(2,0);
	printw("                                                                          ");
	move(2,0);
	printw("Current AI Strategy: ");

	init_pair(5, COLOR_BLUE, COLOR_BLACK);
	attron(COLOR_PAIR(5));
	printw("%s", currentStrat);
	attroff(COLOR_PAIR(5));
}

//applies the AI's move according to what strategy is currently set
void AI_makeMove(Cell Board[]){
	int move;
	//use Randomness if strategy is still default
	if(strcmp(currentStrat, DEFAULT_STRAT) == 0){
		move = rand() % 9;
		bool valid_move = false;
		while(!valid_move){
			if(Board[move].symbol == ' '){
				Board[move].symbol = AI_SYMB;
				valid_move = true;
			}
			else{
				move = rand() % 9;
			}
		}
	}
	else{/*Strategy javascript file provided*/
		/*attempt to create Duktape Context*/
		ctx = duk_create_heap_default();
		if (!ctx) {
			clear();
		    printf("Failed to create a Duktape heap.\n");
		    exit(1);
		}
		/*register the file as an Ecmacscript global object for later use*/
		if (duk_peval_file(ctx, currentStrat) != 0) {
			clear();
    		printf("%s\n", duk_safe_to_string(ctx, -1));
    		exit(1);
		}
		duk_pop(ctx);  /* ignore result */

		memset(board_state, 0, sizeof(board_state));

		duk_push_global_object(ctx);		//pushing global objec to value stack
		duk_get_prop_string(ctx, -1, "ReadBoard"); //look up ReadBoard function of script object

		/*Copy symbols on the board to board_state var to pass info to the script*/
		int i;
		for(i=0;i<9;i++){
			board_state[i] = Board[i].symbol;
		}

		duk_push_string(ctx, board_state);	//push var board_state to value stack

		if (duk_pcall(ctx, 1 /*nargs*/) != 0) {
			DISP_WARNING(0);
			Change_Strat(DEFAULT_STRAT);			    //strategy file invalid, default back to random
			AI_makeMove(Board);
		} else {
			//-1 refers to top-most element in value stack	which in this case is the return value of the Javascript file			
			move = atoi(duk_safe_to_string(ctx, -1)); 	//store javascript move result	    
			Board[move].symbol = AI_SYMB;  				//apply the move to Board											
		}												 

		duk_pop(ctx);  /* pop result/error */

		duk_destroy_heap(ctx); //destroy duktape context to free up resources when done with a move
	}
}

//return -1 if no win, 0 if Player wins 1 if AI wins
int Check_Win(Cell Board[]){
	//check for possible win conditions
	/*Check rows */
	int i;
	for(i = 0; i < 3; i++){
		if(Board[i*3].symbol != ' '){
			if(Board[i*3].symbol == Board[(i*3)+1].symbol){
				if(Board[(i*3)+1].symbol == Board[(i*3)+2].symbol){
					if(Board[i*3].symbol == USER_SYMB){
						return 0; //Player won
					}
					else{
						return 1; //AI won
					}
				}
			}
		}	
	}
	/*Check columns */
	for(i = 0; i < 3; i++){
		if(Board[i].symbol != ' '){
			if(Board[i].symbol == Board[i+3].symbol){
				if(Board[i+3].symbol == Board[i+6].symbol){
					if(Board[i].symbol == USER_SYMB){
						return 0; //Player won
					}
					else{
						return 1; //AI won
					}
				}
			}
		}	
	}
	/*Check diagonals*/
	//left to right diagonal
	if(Board[0].symbol != ' '){
		if(Board[0].symbol == Board[4].symbol){
			if(Board[4].symbol == Board[8].symbol){
				if(Board[0].symbol == USER_SYMB){
					return 0; // Player won
				}
				else{
					return 1; //AI won
				}
			}
		}
	}
	//right to left diagonal
	if(Board[2].symbol != ' '){
		if(Board[2].symbol == Board[4].symbol){
			if(Board[4].symbol == Board[6].symbol){
				if(Board[2].symbol == USER_SYMB){
					return 0; // Player won
				}
				else{
					return 1; //AI won
				}
			}
		}
	}
	return -1;
}

//draw color coded content of a cell according to who played the move
void Draw_Cell_Content(Cell Board[]){
	int i, c;
	for(i = 0; i < 9; i++){
		if(Board[i].symbol != ' '){
			move(Board[i].char_y, Board[i].char_x);
			if(Board[i].symbol == 'X'){
				init_pair(6, COLOR_GREEN, COLOR_BLACK);	
				c = 6;		
			}
			else{
				init_pair(7, COLOR_MAGENTA, COLOR_BLACK);
				c = 7;
			}	
			attron(COLOR_PAIR(c));
			printw("%c", Board[i].symbol);
			attroff(COLOR_PAIR(c));
		}
	}
}

//Reset Cell's content
void RESET_BOARD(Cell Board[]){
	int i;
	for(i = 0; i < 9; i++){
		Board[i].symbol = ' ';
		move(Board[i].char_y, Board[i].char_x);
		printw("%c", Board[i].symbol);
	}
}

//initialize cells to empty and draw board
void INIT_BOARD(Cell Board[]){
	int i = 0;
	DRAW_BORDERS();
	for(i;i<9;i++){
		INIT_CELL(i, Board);
		Board[i].symbol = ' ';		//initialize Cells' content to an empty char
	}
}

//draw board
void DRAW_BORDERS(){
	int i,j;
	int horz_offset = 1;
	char horz_sym = '-';
	char vert_sym = '|';

	//draw horizontal lines
	for(i = 0;i < 2; i++){
		for(j = 0; j <= CELL_WIDTH*3; j++){
			if(i == 0){
				mvaddch((_BOARD_TOP_LEFT_Y+CELL_HEIGHT), _BOARD_TOP_LEFT_X+j, horz_sym);
			}
			else{
				mvaddch((_BOARD_TOP_LEFT_Y+CELL_HEIGHT*2) + 1, _BOARD_TOP_LEFT_X+j, horz_sym); //second horz line
			}
		}
	}
	//draw vertical lines
	for(i = 0;i < 2; i++){
		for(j = 0; j <= (CELL_HEIGHT*3)+1; j++){
			if((j+1)%4 == 0){
				vert_sym = '+';
			}
			else{
				vert_sym = '|';
			}
			if(i == 0){
				mvaddch((_BOARD_TOP_LEFT_Y + j), ((COLS/2)-CELL_WIDTH*horz_offset), vert_sym);
			}
			else{
				mvaddch((_BOARD_TOP_LEFT_Y + j), ((COLS/2)-(CELL_WIDTH*horz_offset)+CELL_WIDTH), vert_sym); //second vert line
			}
		}
	}
}

//set coordinate values of each cell to calculate click areas and char locations
void INIT_CELL(int i, Cell Board[]){
	//first row start
	if(i == 0){
		Board[i].top_left_x = _BOARD_TOP_LEFT_X;
		Board[i].top_left_y = _BOARD_TOP_LEFT_Y - 1;
		Board[i].char_x = Board[i].top_left_x+2;
		Board[i].char_y = Board[i].top_left_y+2;
	}
	//second row start
	else if(i == 3){
		Board[i].top_left_x = Board[0].top_left_x;
		Board[i].top_left_y = Board[0].top_left_y + CELL_HEIGHT + 1;
		Board[i].char_x = Board[i].top_left_x+2;
		Board[i].char_y = Board[i].top_left_y+2;
	}
	//third row start
	else if(i == 6){
		Board[i].top_left_x = Board[3].top_left_x;
		Board[i].top_left_y = Board[3].top_left_y + CELL_HEIGHT + 1;
		Board[i].char_x = Board[i].top_left_x+2;
		Board[i].char_y = Board[i].top_left_y+2;
	}
	else{ //set up row cells
		Board[i].top_left_x = Board[i-1].top_left_x + CELL_WIDTH;
		Board[i].top_left_y = Board[i-1].top_left_y;
		Board[i].char_x = Board[i].top_left_x+2;
		Board[i].char_y = Board[i].top_left_y+2;
	}
}

/*Function to check if stalemate*/
bool CHECK_FULL(Cell Board[]){
	int i;

	for(i=0;i<9;i++){
		if(Board[i].symbol == ' '){
			return false;
		}
	}

	return true;
}

bool LOAD_STRAT(){
	FILE * fp;
	bool valid = true;

	move(0,0);
	printw("file name <filename.js>: ");

	curs_set(1);			//make cursor visible
	echo();					//display keystrokes

	char new_file[40];
	getstr(new_file);

	fp = fopen(new_file, "r");
	if(fp == NULL){
		valid = false;
	}
	if(strlen(new_file) > 40){
		valid = false;
	}

	/*get last 3 chars of file name (should be length of 3 ".js")*/
	int i;
	char extension[] = "   ";
	for(i = 0; i < 3; i++){
		extension[i] = new_file[strlen(new_file)-3+i];
	}


    if(strcmp(extension,".js") != 0){
	    valid = false;
    }

	if(valid){
		Change_Strat(new_file);
		return valid;
	}
	return valid;
}
