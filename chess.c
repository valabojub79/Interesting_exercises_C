/*Chess made in C programming language (serbian piece names). 
 K - kralj (king), D - dama(queen), T - top (rook), L - lovac (bishop), S - skakac/konj (knight), P - pesak (pawn)
 Autor: Danilo Novakovic */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#define SIZE 8
#define MAX_BUFFER 7
#define RED "\x1b[37;3;1m"
#define BLUE  "\x1b[36;3;1m"
#define BG_WHITE "\x1b[40m"
#define BG_YELLOW "\x1b[41m"
#define CL_RESET  "\x1b[0m"
typedef struct board_st{
	int type;
	int player;
	int state;
} BOARD;
void print_matrix(BOARD (*board)[SIZE]);
void init_board(BOARD (*board)[SIZE]);
void usage_tip(void);
int syntax_check(char *p);
int command_check(BOARD (*board)[SIZE], char *p);
void remove_piece(BOARD *piece);

int winner = 0;
int player = 0;

int main () {
	printf(BLUE "Player 1"   CL_RESET "\n");
	printf(RED  "Player 2"  CL_RESET "\n");
	
	int i = 0, j = 0;
	BOARD board [SIZE][SIZE];
	
	init_board(board);
	print_matrix(board);
	usage_tip();
	char buffer[MAX_BUFFER], promotion;
	do{
		i = 0;
		do {
			printf(player == 0 ? BLUE "Player 1: " CL_RESET : RED  "Player 2: "  CL_RESET);
			fgets(buffer, MAX_BUFFER, stdin);
			if(!strncmp(buffer, "/ff", 3)) {
				printf("\nPlayer %d won!\n", player == 0 ? 2 : 1);
				exit(EXIT_SUCCESS);
			}
			i = syntax_check(buffer);
			if(!i) {
				usage_tip();
				continue;
			}
			i = command_check(board, buffer);
		} while(!i);
		print_matrix(board);
		
		promotion = 0;
		i = SIZE - (buffer[4] - '0');
		j = buffer[3] - 'a';
		if(board[i][j].type == 'P' && (player == 0 ? i == 0 : i == SIZE - 1)) {
			printf("Promotion!\n");
			do {
				printf("With what piece (D/S/L/T) do you wish to exchange pawn at '%c%c': ", buffer[3], buffer[4]);
				scanf(" %c", &promotion);
			} while(promotion != 'D' && promotion != 'S' && promotion != 'L' && promotion != 'T');
			board[i][j].type = promotion;
			print_matrix(board);
		}
		
		player = (player+1)%2;
	}while(!winner);
	
	printf("\nPlayer %d won!\n", winner);
	
	return EXIT_SUCCESS;
}
int command_check(BOARD (*board)[SIZE], char *s) {
	int i1, j1, i2, j2, piece, k, p, castling = 0;
	piece = *s++;
	j1 = *s++ - 'a';
	i1 = SIZE - (*s++ - '0');
	j2 = *s++ - 'a';
	i2 = SIZE - (*s++ - '0');
	
	/*Following if and switch check if the given command is valid.*/
	if(board[i1][j1].player != player) {
		puts("error: you are trying to move an empty/enemy piece.");
		return 0;
	}
	if(board[i1][j1].type != piece) {
		printf("error: you tried to move '%c', but '%c' is on position '%c%c'.\n", piece, board[i1][j1].type, (j1+'a'), (-i1+SIZE+'0'));
		return 0;
	}
	switch(piece) {
		case 'P': 
			if(j1!=j2) {	
				if(abs(j1-j2) > 1) {
					printf("error: pawn can only move vertical. (diagonally is exeption for eating enemy piece).\n");
					return 0;
				}
				else if(board[i2][j2].type == 0) {
					puts("error: pawn can move diagonally only to eat enemys piece.");
					return 0;
				}
			}
			if((player == 0) ? i1 < i2 : i1 > i2) {
				puts("error: pawn can't go backwards!");
				return 0;
			}
			if(board[i1][j1].state == 0 ? abs(i1-i2)>2 : abs(i1-i2)>1) {
				puts("error: pawn can move 2 steps forward if first move, and 1 step if not.");
				return 0;
			}
			if(board[i1][j1].state == 0 && abs(i1-i2) == 2) {
				if(i1 > i2 ? board[i1-1][j1].type != 0 : board[i1+1][j1].type != 0) {
					puts("error: another piece is blocking the path.");
					return 0;
				}
			}
			if(j1 == j2 && board[i2][j2].type != 0) {
				puts("error: another piece is blocking the path.");
				return 0;
			}
			break;
		case 'S':
			if(abs(i1-i2) < 1 || abs(i1-i2) > 2 || abs(j1-j2) < 1 || abs(j1-j2) > 2) {
				puts("error: knight can only move in 'L'-shape.");
				return 0;
			}
			if(!((abs(i1-i2) == 1 && abs(j1-j2) ==2) || (abs(i1 -i2) == 2 && abs(j1 - j2) == 1))) {
				puts("error: knight can only move in 'L'-shape. ");
				return 0;
			} 
			break;
		case 'K':
			if (i1 == i2 && abs(j1-j2) == 2 && board[i1][j1].state == 0) {
				if(j1 < j2) {
					if(board[i1][SIZE-1].type == 'T' && board[i1][SIZE-1].state == 0) {
						for(k = 1; k < 2; k++) {
							if(board[i1][j1+k].type != 0) {
								puts("error: obstacle blocking path for castling move.");
								return 0;
							}
						}
						castling = 1;
					}
					else {
						puts(board[i1][j1].state == 0 ? "error: rook has already been moved" 
																  : "error: king can only move one step in any direction.");
						return 0;
					}
				}
				else {
					if(board[i1][0].type == 'T' && board[i1][0].state == 0) {
						for(k = 1; k < j1; k++) {
							if(board[i1][j1-k].type != 0) {
								puts("error: obstacle blocking path for castling move.");
								return 0;
							}
						}
						castling = 1;
					}
					else {
						puts(board[i1][j1].state == 0 ? "error: rook has already been moved" 
																  : "error: king can only move one step in any direction.");
						return 0;
					}
				}
			}
			else if(abs(i1-i2)>1 || abs(j1-j2)>1) {
				puts("error: king can only move one step in any direction.");
				return 0;
			}
			/*note: upgrade castling move later*/
			break;
		case 'L':
			if(abs(i1-i2) != abs(j1-j2)) {
				puts("error: bishop can only move diagonally.");
				return 0;
			}
			/*checks if there is an obsticle in diagonal path*/
			for(k = 1, p = abs(i1-i2); k < p; k++) {
					if(i1 > i2) {
						if(j1 < j2 ? board[i1 - k][j1+k].type != 0 : board[i1 - k][j1-k].type != 0) {
							puts("error: another piece is blocking the path.");
							return 0;
						}
					}
					else if(j1 < j2 ? board[i1 + k][j1+k].type != 0 : board[i1 + k][j1-k].type != 0) {
							puts("error: another piece is blocking the path.");
							return 0;
					}
			}
			break;
		case 'T':
			if((i1!=i2)&&(j1!=j2)) {
				puts("error: rook can only move vertically or horizontally");
				return 0;
			}
			for(k = 1, p = (i1 == i2) ? abs(j1-j2) : abs(i1-i2); k < p; k++) 
				if(i1 == i2) {
					if(j1 > j2 ? board[i1][j1-k].type != 0 : board[i1][j1+k].type != 0) {
						puts("error: another piece is blocking the path.");
						return 0;
					}
				}
				else if(i1 > i2 ? board[i1-k][j1].type != 0 : board[i1+k][j1].type != 0) {
						puts("error: another piece is blocking the path.");
						return 0;
				}
			break;
		case 'D':
			if(abs(i1-i2) == abs(j1-j2)) {
				for(k = 1, p = abs(i1-i2); k < p; k++) 
					if(i1 > i2) {
						if(j1 < j2 ? board[i1 - k][j1+k].type != 0 : board[i1 - k][j1-k].type != 0) {
							puts("error: another piece is blocking the path.");
							return 0;
						}
					}
					else if(j1 < j2 ? board[i1 + k][j1+k].type != 0 : board[i1 + k][j1-k].type != 0) {
							puts("error: another piece is blocking the path.");
							return 0;
					}
			}
			else if((i1==i2)^(j1==j2)) {
				for(k = 1, p = (i1 == i2) ? abs(j1-j2) : abs(i1-i2); k < p; k++) 
					if(i1 == i2) {
						if(j1 > j2 ? board[i1][j1-k].type != 0 : board[i1][j1+k].type != 0) {
							puts("error: another piece is blocking the path.");
							return 0;
						}
					}
					else if(i1 > i2 ? board[i1-k][j1].type != 0 : board[i1+k][j1].type != 0) {
							puts("error: another piece is blocking the path.");
							return 0;
					}
			}
			else {
				puts("error: queen can move vertically, horizontally or diagonally.");
				return 0;
			}
			break;
	}
	
	if(board[i2][j2].player == player) {
		puts("error: friendly piece is blocking the path.");
		return 0;
	}
	
	if(board[i2][j2].type == 'K') {
		printf("\nPlayer %d has won!\n", player+1);
		exit(EXIT_SUCCESS);
	}
	
	board[i2][j2] = board[i1][j1];
	board[i2][j2].state = 1;	
	remove_piece(&board[i1][j1]);
	
	if(castling) {
		p = j1 < j2 ? SIZE - 1 : 0;
		k = j1 < j2 ? j2 - 1 : j2 + 1;
		board[i2][k] = board[i2][p];
		board[i2][k].state = 1;
		remove_piece(&board[i2][p]);
	}
	/*Note: add check later*/
	/*Note: add En passant move  later*/
	return 1;
}
void remove_piece(BOARD *piece) {
	piece->type = 0;
	piece->player = -1;
	piece->state = 0;
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
void print_matrix(BOARD (*board)[SIZE]) {
	int i, j, k = 0;
	printf("\n\t    ");
	for(i = 0; i < SIZE; printf("%c ", 'a' + i++));
		
	printf("\n\t    ");
	for(i = 0; i < SIZE; i++) 
		printf("--");
	
	putchar('\n');
	for(i = 0; i < SIZE; i++) {
		printf(CL_RESET "\t%d | ", SIZE - i);
		for(j = 0; j < SIZE; j++) {
			switch(board[i][j].type) {
				case 0 : printf(k%2 == 1 ? BG_YELLOW  "  " : BG_WHITE  "  "); break;
				case 'K' : case 'L' : case 'S' : case 'D' : case 'T' : case 'P':
					printf(k%2 == 1 ? BG_YELLOW : BG_WHITE);
					printf(board[i][j].player == 1 ? RED "%c " CL_RESET 
														: BLUE  "%c " CL_RESET , board[i][j].type); break;
			}
			k = (k+1)%2;
		}
		k = (k+1)%2;
		printf(CL_RESET "| %d\n", SIZE - i);
	}
	printf("\t    ");
	for(i = 0; i < SIZE; i++) 
		printf("--");
	printf("\n\t    ");
	for(i = 0; i < SIZE; printf("%c ", 'a' + i++));
	puts("\n");
}
void init_board(BOARD (*board)[SIZE]) {
	int i, j;
	
	for(i = 0; i < SIZE; i++) {
		for(j = 0; j < SIZE; j++) {
			board[i][j].state = 0;
		}
	}
	for(i = 0; i < SIZE; i++) {
		board[0][i].player = 1;
	}
	board[0][0].type = 'T';
	board[0][1].type = 'S';
	board[0][2].type = 'L';
	board[0][3].type = 'D';
	board[0][4].type = 'K';
	board[0][5].type = 'L';
	board[0][6].type = 'S';
	board[0][7].type = 'T';
	for(j = 0; j < SIZE; j++) {
		board[1][j].type = 'P';
		board[1][j].player = 1;
	}
	for(i = 2; i < SIZE - 2; i++) {
		for(j = 0; j < SIZE; j++) {
			board[i][j].type = 0;
			board[i][j].player = -1;
		}
	}
	for(j = 0; j < SIZE; j++) {
		board[6][j].type = 'P';
		board[6][j].player = 0;
	}
	for(i = 0; i < SIZE; i++) {
		board[7][i].player = 0;
	}
	board[7][0].type = 'T' ;
	board[7][1].type = 'S' ;
	board[7][2].type = 'L' ;
	board[7][3].type = 'D' ;
	board[7][4].type = 'K' ;
	board[7][5].type = 'L' ;
	board[7][6].type = 'S' ;
	board[7][7].type = 'T' ;
}
void usage_tip(void) {
	printf("usage example: Pa2a3 (first letter of piece, from (column/row) - to(column/row) *case sensitive*)\n");
}