#include "chess.h"
int command_check(BOARD (*board)[SIZE], char *s) {
	int i1, j1, i2, j2, piece, absi, absj, k, p;
	castling = 0;
	piece = *s++;
	j1 = *s++ - 'a';
	i1 = SIZE - (*s++ - '0');
	j2 = *s++ - 'a';
	i2 = SIZE - (*s++ - '0');
	
	if(board[i1][j1].player != player) {
		puts("error: you are trying to move an empty/enemy piece.");
		return 0;
	}
	if(board[i1][j1].type != piece) {
		printf("error: you tried to move '%c', but '%c' is on position '%c%c'.\n"
				, piece, board[i1][j1].type, (j1+'a'), (-i1+SIZE+'0'));
		return 0;
	}
	per = 1;
	switch(piece) {
		case 'P': 	if(!pawn_check(board, i1, j1, i2, j2)) return 0; break;
		case 'S':	if(!knight_check(board, i1, j1, i2, j2)) return 0; break;	
		case 'K':	if(!king_check(board, i1, j1, i2, j2)) return 0; break;	
		case 'L':	if(!bishop_check(board, i1, j1, i2, j2)) return 0; break;
		case 'T':	if(!rook_check(board, i1, j1, i2, j2)) return 0; break;
		case 'D':	if(!queen_check(board, i1, j1, i2, j2)) return 0; break;
	}
	
	if(board[i2][j2].type == 'K') {
		printf("\nPlayer %d has won!\n", player+1);
		exit(EXIT_SUCCESS);
	}
	rm_danger(board, piece, i1, j1);
	
	//[i1][j1] piece is being moved to [i2][j2] spot
	board[i2][j2].type = board[i1][j1].type;
	board[i2][j2].player = board[i1][j1].player;
	board[i2][j2].state = 1;	
	rm_piece(&board[i1][j1]);
	
	//castling is being done if called and conditions are fullfiled
	if(castling) {
		p = j1 < j2 ? SIZE - 1 : 0;
		k = j1 < j2 ? j2 - 1 : j2 + 1;
		board[i2][k].type = board[i2][p].type;
		board[i2][k].player = board[i2][p].player;
		board[i2][k].state = 1;
		rm_piece(&board[i2][p]);
		castling = 0;
	}
	/*Note: add En passant move  later*/
	return 1;
}

int pawn_check(BOARD (*board)[SIZE], int i1, int j1, int i2, int j2) {
	int absi, absj;
	if(board[i2][j2].player == player) {
		if(per) puts("error: friendly piece is blocking the path.");
		return 0;
	}
	absi = abs(i1 - i2);
	absj = abs(j1 - j2);
	if(j1!=j2) {	
		if(absj > 1) {
			if(per) printf("error: pawn can only move vertical. (diagonally is exeption for eating enemy piece).\n");
			return 0;
		}
		else if(board[i2][j2].type == 0) {
			if(per) puts("error: pawn can move diagonally only to eat enemys piece.");
			return 0;
		}
	}
	if((player == 0) ? i1 < i2 : i1 > i2) {
		if(per) puts("error: pawn can't go backwards!");
		return 0;
	}
	if(board[i1][j1].state == 0 ? absi>2 : absi>1) {
		if(per) puts("error: pawn can move 2 steps forward if first move, and 1 step if not.");
		return 0;
	}
	if(board[i1][j1].state == 0 && absi == 2) {
		if(i1 > i2 ? board[i1-1][j1].type != 0 : board[i1+1][j1].type != 0) {
			if(per) puts("error: another piece is blocking the path.");
			return 0;
		}
	}
	if(j1 == j2 && board[i2][j2].type != 0) {
		if(per) puts("error: another piece is blocking the path.");
		return 0;
	}
	
	return 1;
}

int knight_check(BOARD (*board)[SIZE], int i1, int j1, int i2, int j2) {
	int absi, absj;
	if(board[i2][j2].player == player) {
		if(per) puts("error: friendly piece is blocking the path.");
		return 0;
	}
	absi = abs(i1 - i2);
	absj = abs(j1 - j2);
	if(absi < 1 || absi > 2 || absj < 1 || absj > 2) {
		if(per) puts("error: knight can only move in 'L'-shape.");
		return 0;
	}
	if(!((absi == 1 && absj ==2) || (absi == 2 && absj == 1))) {
		if(per) puts("error: knight can only move in 'L'-shape. ");
		return 0;
	} 
	return 1;
}

int king_check(BOARD (*board)[SIZE], int i1, int j1, int i2, int j2) {
	int absi, absj, p, k;
	if(board[i2][j2].player == player) {
		if(per) puts("error: friendly piece is blocking the path.");
		return 0;
	}
	absi = abs(i1 - i2);
	absj = abs(j1 - j2);
	//Checks if castling move first
	if (!check && i1 == i2 && absj == 2 && board[i1][j1].state == 0) {
		if(j1 < j2) {
			if(board[i1][SIZE-1].type == 'T' && board[i1][SIZE-1].state == 0
				&& board[i1][SIZE-1].player == player && !board[i1][SIZE-1].danger[(player+1)%2]) {
				for(k = 1; k <= 2; k++) {
					if(board[i1][j1+k].type != 0 
						|| board[i1][j1+k].danger[(player+1)%2] != 0) {
						if(per) puts("error: path for castling move is blocked / under attack.");
						return 0;
					}
				}
			}
			else {
				if(per) puts("error: castling unavailable.");
				return 0;
			}
		}
		else {
			if(board[i1][0].type == 'T' && board[i1][0].state == 0
				&& board[i1][0].player == player && !board[i1][0].danger[(player+1)%2]) {
				for(k = 1; k < j1; k++) {
					if(board[i1][j1-k].type != 0
						|| board[i1][j1-k].danger[(player+1)%2] != 0) {
						if(per) puts("error: path for castling move is blocked / under attack.");
						return 0;
					}
				}
			}
			else {
				if(per) puts("error: castling unavailable.");
				return 0;
			}
		}
		castling = 1;
	}
	else if(absi>1 || absj>1) {
		if(per) puts("error: king can only move one step in any direction.");
		return 0;
	}
	else if(check && board[i2][j2].danger[(player+1)%2] != 0) {
		if(per) printf("error: Destination '%c%c' is under attack.\n", (j1+'a'), (-i1+SIZE+'0') );
		return 0;
	}
	return 1;
}

int bishop_check(BOARD (*board)[SIZE], int i1, int j1, int i2, int j2) {
	int absi, absj, p, k;
	if(board[i2][j2].player == player) {
		if(per) puts("error: friendly piece is blocking the path.");
		return 0;
	}
	absi = abs(i1 - i2);
	absj = abs(j1 - j2);
	if(absi != absj) {
		if(per) puts("error: bishop can only move diagonally.");
		return 0;
	}
	/*checks if there is an obsticle in diagonal path*/
	for(k = 1, p = absi; k < p; k++) {
			if(i1 > i2) {
				if(j1 < j2 ? board[i1 - k][j1+k].type != 0 : board[i1 - k][j1-k].type != 0) {
					if(per) puts("error: another piece is blocking the path.");
					return 0;
				}
			}
			else if(j1 < j2 ? board[i1 + k][j1+k].type != 0 : board[i1 + k][j1-k].type != 0) {
					if(per) puts("error: another piece is blocking the path.");
					return 0;
			}
	}
	
	return 1;
}

int rook_check(BOARD (*board)[SIZE], int i1, int j1, int i2, int j2) {
	int absi, absj, p, k;
	if(board[i2][j2].player == player) {
		if(per) puts("error: friendly piece is blocking the path.");
		return 0;
	}
	absi = abs(i1 - i2);
	absj = abs(j1 - j2);
	if((i1!=i2)&&(j1!=j2)) {
		if(per) puts("error: rook can only move vertically or horizontally");
		return 0;
	}
	for(k = 1, p = (i1 == i2) ? absj : absi; k < p; k++) 
		if(i1 == i2) {
			if(j1 > j2 ? board[i1][j1-k].type != 0 : board[i1][j1+k].type != 0) {
				if(per) puts("error: another piece is blocking the path.");
				return 0;
			}
		}
		else if(i1 > i2 ? board[i1-k][j1].type != 0 : board[i1+k][j1].type != 0) {
				if(per) puts("error: another piece is blocking the path.");
				return 0;
		}
	return 1;
}

int queen_check(BOARD (*board)[SIZE], int i1, int j1, int i2, int j2) {
	int absi, absj, p, k;
	if(board[i2][j2].player == player) {
		if(per) puts("error: friendly piece is blocking the path.");
		return 0;
	}
	absi = abs(i1 - i2);
	absj = abs(j1 - j2);
	if(absi == absj) {
		for(k = 1, p = absi; k < p; k++) 
			if(i1 > i2) {
				if(j1 < j2 ? board[i1 - k][j1+k].type != 0 : board[i1 - k][j1-k].type != 0) {
					if(per) puts("error: another piece is blocking the path.");
					return 0;
				}
			}
			else if(j1 < j2 ? board[i1 + k][j1+k].type != 0 : board[i1 + k][j1-k].type != 0) {
					if(per) puts("error: another piece is blocking the path.");
					return 0;
			}
	}
	else if((i1==i2)^(j1==j2)) {
		for(k = 1, p = (i1 == i2) ? absj : absi; k < p; k++) 
			if(i1 == i2) {
				if(j1 > j2 ? board[i1][j1-k].type != 0 : board[i1][j1+k].type != 0) {
					if(per) puts("error: another piece is blocking the path.");
					return 0;
				}
			}
			else if(i1 > i2 ? board[i1-k][j1].type != 0 : board[i1+k][j1].type != 0) {
					if(per) puts("error: another piece is blocking the path.");
					return 0;
			}
	}
	else {
		if(per) puts("error: queen can move vertically, horizontally or diagonally.");
		return 0;
	}
	return 1;
}

/*Checks if syntax of given command is correct, returns 1 if it is, and 0 if it's not*/
int syntax_check(char *p) {
	if(*p != 'S' && *p != 'P' && *p != 'D' && *p != 'L' && *p != 'K' && *p != 'T') return 0;
	if(*++p < 'a' || *p > 'h') return 0;
	if(*++p < '1' || *p > '8') return 0;
	if(*++p < 'a' || *p > 'h') return 0;
	if(*++p < '1' || *p > '8') return 0;
	return 1;
}