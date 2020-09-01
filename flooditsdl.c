#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include "neillsdl2.h"

#define MAXNUMCOLOURS 9
#define MINCOLOURS 2
#define MAXBOARDSIZE 14
#define MINBOARDSIZE 2
#define DEFAULTSIZE 14
#define DEFAULTCOLOURS 6
#define BASETEN 10
#define RECTSIZE 20
#define MILLISECONDDELAY 20
#define WRONGINPUT "Incorrect format, please try for example ./flooditsdl\n"

void populateBoard(int** board, int boardSize, int numColours);
int compareBoard(int** initialBoard, int** newBoard, int boardSize);
void fill(int** board, int y, int x, int currentFill, int newFill, int boardSize);
void printBoard(SDL_Simplewin sw, fntrow fontdata[FNTCHARS][FNTHEIGHT], int** board, int *boardSize);
int verifyInput(int input, int numColours);
void runGame(int** board, int *boardSize, int *numColours);
int checkWin(int** board, int boardSize);
void test(void);
int** initBoard( int boardSize);
int setupBoard( int argc, char *argv[], int *boardSize, int *numColours);
int** readFile(int** board, int *boardSize, char* filename, int *numColours);
int** loadBoard(int** board,int *boardSize, char* filename);
int freeBoard(int** board,int boardSize);
int process2Arg(int arg1, char** argv, int* boardSize, int *numColours);
int process3Arg(int arg1, int arg2, char** argv, int *boardSize, int *numColours);
void SDL_KeyPressed(SDL_Simplewin *sw, int* keyPressed);
void countNum(FILE *boardFile, int *rowCount, int *numColours, int *numberCount);


int main (int argc, char** argv)
{
	

	int** startBoard = NULL;
	int boardSize = 0, numColours = 0, *b, *n;

	b = &boardSize;
	n = &numColours;
	

	srand(time(NULL));
	

	if(setupBoard(argc, argv, b, n) == 1){
		startBoard = initBoard(boardSize);
		populateBoard(startBoard, boardSize, numColours);
		runGame(startBoard, b, n); 
	}else if (setupBoard(argc, argv, b, n) == 2){
		startBoard = readFile(startBoard, b, "neill.txt", n);
		if(startBoard != NULL){
			runGame(startBoard, b, n);
		}
	}
	

	freeBoard(startBoard, boardSize);
	return 0;
}

void runGame(int** startBoard, int *boardSize, int *numColours)
{

	fntrow fontdata[FNTCHARS][FNTHEIGHT];
	SDL_Simplewin sw;
	int input = 0, *keyPressed;
	keyPressed = &input;
	Neill_SDL_ReadFont(fontdata, "mode7.fnt");
	Neill_SDL_Init(&sw);
	do{
		printBoard(sw, fontdata, startBoard, boardSize);
		Neill_SDL_DrawString(&sw, fontdata, "Flood It", 300, 20, 10);
		Neill_SDL_DrawString(&sw, fontdata, "Press q to exit the game ", 200, 570, 10);
		Neill_SDL_DrawString(&sw, fontdata, "Press keys 1-9 to fill the board", 150, 540, 10);
		Neill_SDL_UpdateScreen(&sw);	
		SDL_KeyPressed(&sw, keyPressed);
		Neill_SDL_Events(&sw);

		if(verifyInput(input, *numColours) == 1){
			fill(startBoard, 0, 0, startBoard[0][0], input, *boardSize);
		}
		if(checkWin(startBoard, *boardSize) == 1){
			Neill_SDL_DrawString(&sw, fontdata, "YOU WIN", 320, 500, 10);
		}
	}while(!sw.finished);

   /* Clear up graphics subsystems */
	SDL_Quit();
	atexit(SDL_Quit);
}

void SDL_KeyPressed(SDL_Simplewin *sw, int *keyPressed)
{
	SDL_Event event;
	while(SDL_PollEvent(&event)) 
	{      
		switch (event.type){
			case SDL_KEYDOWN:
			switch(event.key.keysym.sym)
			{
				case SDLK_1:
				*keyPressed = 1;
				break;

				case SDLK_2:
				*keyPressed = 2;
				break;

				case SDLK_3:
				*keyPressed = 3;
				break;

				case SDLK_4:
				*keyPressed = 4;
				break;

				case SDLK_5:
				*keyPressed = 5;
				break;

				case SDLK_6:
				*keyPressed = 6;
				break;

				case SDLK_7:
				*keyPressed = 7;
				break;

				case SDLK_8:
				*keyPressed = 8;
				break;

				case SDLK_9:
				*keyPressed = 9;
				break;

				case SDLK_q:
				sw->finished = 1;
				break;

				default:
				break;
			}
		}


	}
}

void printBoard(SDL_Simplewin sw, fntrow fontdata[FNTCHARS][FNTHEIGHT], int** board, int *boardSize)
{
	/*resizers center the board depending on board size*/
	int i, j, resizerW = 0, resizerH;
	resizerW = (WWIDTH/2) - (*boardSize*12.5);
	resizerH = (WHEIGHT/2) - (*boardSize*12.5);
	if(board == NULL){
		fprintf(stderr, "ERROR: board is empty\n");
		return;
	}
	for(i = 0; i < *boardSize; i++){
		for(j = 0; j < *boardSize; j++){
			Neill_SDL_DrawChar(&sw, fontdata, board[j][i]+'0', resizerW+(i*25), resizerH+(j*25), board[i][j]);
		}
	}
}




int setupBoard(int argc, char *argv[], int* boardSize, int* numColours)
{
	int arg1 = 0;
	int arg2 = 0;

	if(argv == NULL || strcmp(argv[0], "./flooditsdl") != 0
	|| boardSize == NULL || numColours == NULL){
		fprintf(stderr, WRONGINPUT);
		return 0;
	}
	if (argc == 1) {
		*boardSize = DEFAULTSIZE;
		*numColours = DEFAULTCOLOURS;
		return 1;
	}else if(argc == 2){
		return process2Arg(arg1, argv, boardSize, numColours);
	}else if(argc == 3){
		return process3Arg(arg1, arg2, argv, boardSize, numColours);
	}else{fprintf(stderr, WRONGINPUT);}
	return 0;
}


int process2Arg(int arg1, char** argv, int* boardSize, int *numColours)
{
	if(argv == NULL || boardSize == NULL || numColours == NULL){
		return 0;
	}
	/*convert input to base ten integer*/
	arg1 = (int)strtol(argv[1], (char**)NULL, BASETEN);

	if(arg1 >= MINBOARDSIZE && arg1 <= MAXBOARDSIZE){
		*boardSize = arg1;
		*numColours = DEFAULTCOLOURS;
		return 1;
	}else if(strcmp(argv[1], "neill.txt") == 0){
		return 2;
	}else{
		fprintf(stderr, "ERROR: the board size must be between %d and %d\n",
		MINBOARDSIZE, MAXBOARDSIZE);
		return 0;
	}
}

int process3Arg(int arg1, int arg2, char** argv, int* boardSize, int *numColours)
{		
	if(argv == NULL || boardSize == NULL || numColours == NULL){
		return 0;
	}

	arg1 = (int)strtol(argv[1], (char**)NULL, BASETEN);
	arg2 = (int)strtol(argv[2], (char**)NULL, BASETEN);
	if(arg1 >= 2 && arg1 <= MAXBOARDSIZE && arg2 > 0 && arg2 <= MAXNUMCOLOURS){
		*boardSize = arg1;
		*numColours = arg2;
		return 1;
	}else{
		fprintf(stderr, "ERROR: Max board size is %d and max colours are %d\n", 
			MAXBOARDSIZE, MAXNUMCOLOURS);
		fprintf(stderr, "ERROR: Min board size is %d and min colours are %d\n",
			MINBOARDSIZE, MINCOLOURS);
		return 0;
	}
}


int** initBoard(int boardSize){
	int i = 0;
	int** board;
	board = (int **) malloc(sizeof(int*)*boardSize);

	if(board == NULL){
		fprintf(stderr, "Malloc of board failed\n");
		exit(1);
	}

	for (i = 0; i < boardSize; i++) {
		board[i] = (int*) malloc(sizeof(int)*boardSize);
		if (board[i] == NULL){
			fprintf(stderr, "Malloc of board failed\n");
			exit(1);
		}
	}
	return board;
}

void populateBoard(int** board, int boardSize, int numColours) 
{

	int i, j;
	for(i = 0; i < boardSize; i++){
		for(j = 0; j < boardSize; j++){
			board[i][j] = (rand()%numColours)+1;
		}		
	}

}


int checkWin(int** board, int boardSize)
{
	int i, j;
	for(i = 0; i < boardSize; i++){
		for(j = 0; j < boardSize-1; j++){

			if(board[i][j] != board[i][j+1]){
				return 0;
			}
		}
	}
	return 1;
}



int verifyInput(int input, int numColours)
{
	if(input < 1 || input > numColours){
		return 0;
	}else{
		return 1;
	}
}





void fill(int** board, int y, int x, int currentFill, int newFill, int boardSize) {
	/*check out of bounds of board*/
	if(y > boardSize-1 || x > boardSize-1 || x < 0 || y < 0 || currentFill == newFill)
		return;
	/*current fill is fill starting from 0,0*/
	if(board[y][x] == currentFill) {
		board[y][x] = newFill;
		/*recursively check neighbors*/
		fill(board, y, x - 1, currentFill, newFill, boardSize);
		fill(board, y + 1, x, currentFill, newFill, boardSize);
		fill(board, y, x + 1, currentFill, newFill, boardSize);
		fill(board, y - 1, x, currentFill, newFill, boardSize);
	}
}

int freeBoard(int** board,int boardSize){
	int i = 0;
	if (board == NULL){
		return 0;
	}else{
		for (i = 0; i < boardSize; i++) {
			free(board[i]);
		}
		free(board);
		board = NULL;
		return 1;
	}
}


int** readFile(int** board, int *boardSize, char* filename, int *numColours) {
	
	int numberCount = 0, rowCount = 0, *nC, *rC;
	FILE *boardFile;

	nC = &numberCount; 
	rC = &rowCount;
	boardFile = fopen(filename, "r");

	/*check boardFile exists*/
	if (boardFile) {

		countNum(boardFile, rC, numColours, nC);
		/*Add an extra row because the last line of the file
		 won't have a \n character. This also stops empty boards from being valid */
		rowCount++;
		fclose(boardFile);
		/*a valid board should have rowcount squared equal to numberCount*/
		if(pow(rowCount, 2) == numberCount && rowCount <= MAXBOARDSIZE){
			*boardSize = rowCount;
			board=loadBoard(board, boardSize, filename);
			return board;
		}else{
			fprintf(stderr, "ERROR: the board's rows & columns have to be equal & less than %d\n", 
				MAXBOARDSIZE);
		}
	}
	return NULL;
}

void countNum(FILE *boardFile, int *rC, int *numColours, int *nC)
{
	char c;
	int highDigit = 0, rowCount = 0, count = 0;
	if(boardFile == NULL){
		return;
	}
		while( (c = getc(boardFile)) != EOF){
			/* \n character signifies end of a row */
			if(c != '\n'){
				if(isdigit(c)){
					count++;
					/*highest digit is the number of colours*/
					if(c - '0' > highDigit){
						highDigit = c - '0';
					}
				}else{
					fprintf(stderr, "ERROR: the board you read in must only contain numbers\n");
				}
			}
			if(c == '\n'){
				rowCount++;
			}
		}
		*rC = rowCount;
		*numColours = highDigit;
		*nC = count;
}

int** loadBoard(int** board,int *boardSize, char* filename){
	char c;
	FILE *boardFile;
	int container[MAXBOARDSIZE*MAXBOARDSIZE-1] = {0};
	int i =0, j =0, t = 0;
	board = initBoard(*boardSize);
	boardFile = fopen(filename, "r");
	
	if (boardFile) {
		while( (c = getc(boardFile)) != EOF){
			if(c != '\n'){
				container[i] = c - '0';		
				i++;
			}
		}
		for (i = 0; i < *boardSize; i++){
			for (j = 0; j < *boardSize; j++){
				board[i][j]	= container[t++];		
			}
		}
		fclose(boardFile);
		return board;
	}else{
		return NULL;
	}
}



