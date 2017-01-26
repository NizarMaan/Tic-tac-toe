/* 
University of Calgary
	CPSC 599.82 Retro Games Fall 2016
	Assignment 2
	Nizar Maan 10103889

	Strategy file to be loaded onto the tictactoe.c game making use of the Duktape and ncurses libraries
*/
var board_state;
var empty = ' ';
var AI = 'O';
var user = 'X';

function ReadBoard(board_state){
	this.board_state = board_state;
	return First_Available();
}

//return first available move
function First_Available(){
	for(i = 0; i < 9; i++){
		if(board_state[i] === empty){
			return i;
		}
	}
}