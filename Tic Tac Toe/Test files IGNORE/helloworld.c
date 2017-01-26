#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char * argv[])
{	
	//exit program if not enough args provided
	if(argc != 2)
	{
	printf("Usage: %s <a c file name>\n", argv[0]);
	exit(1);
	}

	int test_cursor_col = 5;	//x
	int test_cursor_row = 10;	//y
	int max_col, max_row, ch, prev, cursor_col, cursor_row;
	char prompt[] = "Enter a message: ";
	char user_str[100];
	FILE * fp;
	prev = EOF;	//end of file var

	fp = fopen(argv[1], "r");
	if(fp == NULL){
		perror("Cannot open file");
		exit(1);
	}

	initscr();				/* Start curses mode 		  */

	getmaxyx(stdscr, max_row, max_col);	//get edges of screen

	printw("File name: %s", argv[1]);
	refresh();				/* Print it on to the real screen */
	
	halfdelay(60);			//wait 60 tenths of a second for input	
	
	ch = getch();			/* Wait for user input, uses ASCII value */
	mvaddch(test_cursor_row, test_cursor_col, ch);
	
	move(test_cursor_row+1, test_cursor_col);	//y first arg, x second arg
	addstr("Hi there");

	//PromptUser(max_row, max_col, prompt);
	mvprintw(max_row/2, (max_col-strlen(prompt))/2, prompt);
	getstr(user_str);

	printw("\nYou entered: %s", user_str);

	printw("\nFile content: ");
	while((ch = getc(fp)) != EOF){
		getyx(stdscr, cursor_row, cursor_col); //get current cursor position
		//resetting cursor and screen when screen is filled
		if(cursor_row == (max_row - 1)){
			printw("<-Press any key->");
			getch();
			clear();
			move(0,0);
		}
		printw("%c",ch);
	}

	printw("\npress any key to exit...");
	getch();
	
	endwin();				/* End curses mode		  */
	fclose(fp); 			//close filestream
	return 0;
}

/*void PromptUser(int max_row, int max_col, char prompt[], char user_str[]){
	mvprintw(max_row/2, (max_col-strlen(prompt))/2, prompt);
	getstr(user_str);
}*/