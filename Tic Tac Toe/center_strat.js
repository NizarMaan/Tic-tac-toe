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
	return Decide_Move();
}

function Decide_Move(){
	var move;
	/*first checks to see if there are any plays that need to be blocked otherwise,
	attempt to play the middle Cell if it is not already taken
	*/
	move = Block();

	if(move === null){
		//attempt to play center first
		if(board_state[4] === empty){
			move = 4;
		}/*if middle cell was successfully played then play the corners to try and get diagonals
		   if corners are not available then play available cells adjacent to center cell*/
		else if(board_state[4] === AI){
			/*there are no corners available, play available cells adjacent to center cell*/
			if(board_state[0] != empty && board_state[2] != empty && board_state[6] != empty && board_state[8] != empty){
				//attempt to fill middle column first, then middle row
				if(board_state[1] === empty && board_state[7] === empty){
					move = 1;
				}
				else if(board_state[3] === empty && board_state[5] === empty){
					move = 3;
				}
				else if(board_state[1] === AI){
					move = 7;
				}
				else if(board_state[3] === AI){
					move = 5;
				}
				else{
					move = First_Available();
				}
			}
			/*play the corners 0, 2, 6, 8, will play highest available corner first
			unless a diagonal can be completed*/
			else{
				for(i = 0; i < 5; i++){
					if((i*2) % 2 == 0){
						if(board_state[i*2] === empty){
							if((i*2 === 6) && board_state[8] === AI && board_state[0] === empty){
								move = 0;
							}
							else if((i * 2 === 2) && board_state[8] === AI && board_state[0] === empty){
								move = 0; 
							}
							else if(i*2 === 8 && board_state[2] === AI && board_state[6] === empty){
								move = 6;
							}
							else{
								move = i*2;
							}
						}
					}
				}
			}
		}
		//Dumb-Strategy; if center cell was not filled just play first available cell
		else{
			move = First_Available();
		}
	}
	return move; 
}

//return first available move
function First_Available(){
	for(i = 0; i < 9; i++){
		if(board_state[i] === empty){
			return i;
		}
	}
}

//block user from making a winning column or row sequence
function Block(){
	var move = null;
	//check for columns to block
	for(i = 0; i < 3; i++){
		if(board_state[i] === user && board_state[i+3] === user){
			if(board_state[i+6] === empty){
				move = i+6;
			}
		}
		else if(board_state[i] === user && board_state[i+6] === user){
			if(board_state[i+3] === empty){
				move = i+3;
			}
		}
		else if(board_state[i+3] === user && board_state[i+6] === user){
			if(board_state[i] === empty){
				move = i;
			}
		}
	}

	//check for rows to block
	for(i = 0; i < 3; i++){
		if(board_state[i*3] === user && board_state[(i*3)+2] === user){
			if(board_state[(i*3)+1] === empty){
				move = (i*3)+1;
			}
		}
		else if(board_state[i*3] === user && board_state[(i*3)+1] === user){		
			if(board_state[(i*3)+2] === empty){
				move = (i*3)+2;
			}
		}
		else if(board_state[(i*3)+1] === user && board_state[(i*3)+2] === user){
			if(board_state[i*3] === empty){
				move = i*3;
			}
		}
	}

	/*DISABLED DIAGONAL BLOCKING, makes game too boring
	//check for diagonals to block
	//left to right diagonal
	if(board_state[0] === user && board_state[4] === user){
		if(board_state[8] === empty){
			move = 8;
		}
	}
	else if(board_state[0] === user && board_state[8] === user){
		if(board_state[4] === empty){
			move = 4;
		}
	}
	else if(board_state[4] === user && board_state[8] === user){
		if(board_state[0] === empty){
			move = 0;
		}
	}

	//right to left diagonal
	if(board_state[2] === user && board_state[4] === user){
		if(board_state[6] === empty){
			move = 6;
		}
	}
	else if(board_state[2] === user && board_state[6] === user){
		if(board_state[4] === empty){
			move = 4;
		}
	}
	else if(board_state[4] === user && board_state[6] === user){
		if(board_state[2] === empty){
			move = 2;
		}
	}
	*/

	return move;
}