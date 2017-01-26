#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

WINDOW * create_window(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);

int main(int argc, char * argv[]){
	WINDOW * my_win;
	int startx, starty, width, height, ch, max_row, max_col;
	MEVENT mouseEvent;

	initscr();				/* Start curses mode */

	mousemask(ALL_MOUSE_EVENTS, NULL);

	keypad(stdscr, TRUE);		/* I need that nifty F1 	*/
	curs_set(0);		//make cursor invisible
	height = 3;
	width = 10;
	starty = (LINES - height) / 2;	/* Calculating for a center placement */
	startx = (COLS - width) / 2;	/* of the window		*/
	printw("Press ESC to exit");
	refresh();
	my_win = create_window(height, width, starty, startx);

	while((ch = getch()) != 27) //ASCII value for ESC is 27
	{	
		if(ch == KEY_MOUSE){
			if(getmouse(&mouseEvent) == OK){
				move(2, 2);
				printw("x-cord:%i", mouseEvent.x);
				move(3, 2);
				printw("y-cord:%i", mouseEvent.y);
				move(4,2);
				if(mouseEvent.bstate & BUTTON1_PRESSED){
					printw("button pressed: Left Btn");
				}
			}
		}
		switch(ch)
		{	case KEY_LEFT:			
				if(startx-1 > 0){
					destroy_win(my_win);
					my_win = create_window(height, width, starty,--startx);
				}
				break;
			case KEY_RIGHT:			
				if(startx+width < COLS){
					destroy_win(my_win);
					my_win = create_window(height, width, starty,++startx);
				}
				break;
			case KEY_UP:
				if(starty-1 > 0){
					destroy_win(my_win);
					my_win = create_window(height, width, --starty,startx);
				}
				break;
			case KEY_DOWN:
				if(starty+height < LINES){
					destroy_win(my_win);
					my_win = create_window(height, width, ++starty,startx);
				}
				break;	
		}
	}

	endwin();				/* End curses mode*/
	return 0;
}

WINDOW * create_window(int height, int width, int starty, int startx){
	WINDOW * new_window;
	new_window = newwin(height, width, starty, startx);
	box(new_window, 0, 0); //0, 0 default chars for vertial and horz lines
	wrefresh(new_window);
	return new_window;
}

void destroy_win(WINDOW * window){
	/* box(local_win, ' ', ' '); : This won't produce the desired
	 * result of erasing the window. It will leave it's four corners 
	 * and so an ugly remnant of window. 
	 */
	wborder(window, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	/* The parameters taken are 
	 * 1. win: the window on which to operate
	 * 2. ls: character to be used for the left side of the window 
	 * 3. rs: character to be used for the right side of the window 
	 * 4. ts: character to be used for the top side of the window 
	 * 5. bs: character to be used for the bottom side of the window 
	 * 6. tl: character to be used for the top left corner of the window 
	 * 7. tr: character to be used for the top right corner of the window 
	 * 8. bl: character to be used for the bottom left corner of the window 
	 * 9. br: character to be used for the bottom right corner of the window
	 */
	wrefresh(window);
	delwin(window);
}