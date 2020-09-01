#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>

#define MAXNUMCOLOURS 9
#define MINCOLOURS 2
#define MAXBOARDSIZE 20
#define MINBOARDSIZE 2
#define DEFAULTSIZE 14
#define DEFAULTCOLOURS 6
#define BASETEN 10
#define MAXINPUT 100


int setupBoard( int argc, char *argv[], int *boardSize, int *numColours);
int process2Arg(int arg1, char** argv, int* boardSize, int *numColours);
int process3Arg(int arg1, int arg2, char** argv, int *boardSize, int *numColours);
int** initBoard( int boardSize);
void populateBoard(int** board, int boardSize, int numColours);
void runGame(int boardSize, int numColours, int** board);
void printBoard(int** board, int boardSize);
void fill(int** board, int y, int x, int currentFill, int newFill, int boardSize);
int verifyInput(int input, int numColours);
int checkWin(int** board, int boardSize);
void test(void);
int** readFile(int** board, int *boardSize, char* filename, int *numColours);
int** loadBoard(int** board,int *boardSize, char* filename);
int freeBoard(int** board,int boardSize);
int compareBoard(int** initialBoard, int** newBoard, int boardSize);
void countNum(FILE *boardFile, int *rowCount, int *numColours, int *numberCount);


int main (int argc, char** argv)
{
	int** startBoard = NULL;
	int boardSize = 0, numColours = 0, *b, *n;
	b = &boardSize;
	n = &numColours;
	srand(time(NULL));
	/*test();*/

	if(setupBoard(argc, argv, b, n) == 1){
		startBoard = initBoard(boardSize);
		populateBoard(startBoard, boardSize, numColours);
		runGame(boardSize, numColours, startBoard);

	}else if (setupBoard(argc, argv, b, n) == 2){
		startBoard = readFile(startBoard, b, "neill.txt", n);
		if(startBoard != NULL){
		runGame(boardSize, numColours, startBoard);
		}
	}
	freeBoard(startBoard, boardSize);
	return 0;
}


int setupBoard(int argc, char *argv[], int* boardSize, int* numColours)
{
	int arg1 = 0;
	int arg2 = 0;

	if(argv == NULL || strcmp(argv[0], "./floodit") != 0
	|| boardSize == NULL || numColours == NULL){
		fprintf(stderr, "Incorrect format, please try for example ./floodit\n" );
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
	}else{fprintf(stderr, "Incorrect format, please try for example ./floodit\n" );}
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

void runGame(int boardSize, int numColours, int** startBoard)
{
	char input[MAXINPUT], *p;
	int inputNum = 0;
	int count = 1;

	while(inputNum != -1){
		printBoard(startBoard, boardSize);
		printf("Turn %d : What number ? ", count);
		/*scanf sends into infinite loop if input is not int*/
		/*check input is int; this method is long, but works*/
		fgets(input, sizeof(input), stdin);
        if ((p = strchr(input, '\n')) != NULL) {
            *p = '\0';
        }
        inputNum = (int)strtol(input, &p, BASETEN);
        if (p == input || *p != '\0')  {
            printf("Sorry, (%s) is not a number. try a number between 1 and %d ", input, numColours);
        } else {
        	/*if input is in the colour range, fill and check for win*/
			if(verifyInput(inputNum, numColours) == 1){
			fill(startBoard, 0, 0, startBoard[0][0], inputNum, boardSize);
			if(checkWin(startBoard, boardSize) == 1){
				printf("You Win!\n");
				inputNum = -1;
			}
		}
	}
		printf("\n");
			count++;
	}
	printf("Game ended\n");
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


void printBoard(int** board, int boardSize)
{
	int i, j;

	if(board == NULL){
		fprintf(stderr, "ERROR: board is empty\n");
		return;
	}
	for(i = 0; i < boardSize; i++){
		for(j = 0; j < boardSize; j++){
			printf("%d", board[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

int verifyInput(int input, int numColours)
{
	if(input < 1 || input > numColours){
		fprintf(stderr, "You can only input a number between 1 and %d \n", numColours);
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


/*function made to make testing boards easier*/
int compareBoard(int** initialBoard, int** newBoard, int boardSize)
{
	int i, j;

	for(i = 0; i < boardSize; i++){
		for(j = 0; j < boardSize; j++){
			if (initialBoard[i][j] != newBoard[i][j]){

				return 0;
			}
		}
	}
	return 1;
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


void test(void){

	int boardSize = 0, numColours = 0, *b, *n;
	int arg1 = 0, arg2 =0;
	int numberCount = 0, rowCount = 0, *nC, *rC;
	FILE *boardFile;
	
	int** testBoard1 = NULL; 
	int** testBoard2 = NULL; 
	int** testBoard3 = NULL; 
	int** testBoard2Copy = NULL; 
	int** testBoard5 = NULL;
	int** testFromFile = NULL;

	/*create empty argvs*/
	char** my_argv1 = (char**) malloc(sizeof(char*));
	char** my_argv2 = (char**) malloc(sizeof(char*)*2);
	char** my_argv3 = (char**) malloc(sizeof(char*)*3);
	char** my_argv4 = (char**) malloc(sizeof(char*)*4);
	char** my_argv5 = (char**) malloc(sizeof(char*)*2);
	char** my_argv6 = (char**) malloc(sizeof(char*)*3);

	
	nC = &numberCount; 
	rC = &rowCount;
	b = &boardSize;
	n = &numColours;

	/*modify these values to test for other cases*/
	my_argv1[0] = "./floodit";

	my_argv2[0] = "./floodit";
	my_argv2[1] = "5";

	my_argv3[0] = "./floodit";
	my_argv3[1] = "7";
	my_argv3[2] = "9";


	my_argv4[0] = "./floodit";
	my_argv4[1] = "8";
	my_argv4[2] = "";
	my_argv4[3] = "3";

	my_argv5[0] = "./floodit";
	my_argv5[1] = "j";

	my_argv6[0] = "./floodit";
	my_argv6[1] = "25";
	my_argv6[2] = "-9";

	testBoard1 = initBoard(4);
	testBoard2 = initBoard(4);
	testBoard3 = initBoard(3);
	testBoard2Copy = initBoard(4);

	/*check that initialisation is successful*/
	assert(testBoard1 != NULL);
	assert(testBoard2 != NULL);
	assert(testBoard3 != NULL);
	assert(testBoard2Copy != NULL);

	populateBoard(testBoard1, 4, 4);
	populateBoard(testBoard2, 4, 4);
	populateBoard(testBoard3, 3, 2);

	/*after populating two boards of the same size, they should be 
	  different as they should be randomly populated. CompareBoard
	  function created for testing*/
	assert(compareBoard(testBoard1, testBoard2, 4) != 1);

	/*print boards to check that printBoard is working and verify
	  previous functions*/
	printBoard(testBoard1, 4);
	printBoard(testBoard2, 4);
	printBoard(testBoard3, 3);

	/*testboard3 is a 3x3 board populated with 1s and 2s.
	  whatever the initial matrix, it should all be one colour 
	  after four alternating fills*/
	fill(testBoard3, 0, 0, testBoard3[0][0], 2, 3);
	fill(testBoard3, 0, 0, testBoard3[0][0], 1, 3);
	fill(testBoard3, 0, 0, testBoard3[0][0], 2, 3);
	fill(testBoard3, 0, 0, testBoard3[0][0], 1, 3);
	printf("numColours is %d\n", numColours);
	/*test checkwin on testboards*/
	assert(checkWin(testBoard3, 3) == 1);
	assert(checkWin(testBoard1, 4) == 0);
	assert(checkWin(testBoard2, 4) == 0);

	/*the copy should be different to the original after fill*/
	testBoard2Copy = testBoard2;

	/*more fill tests*/
	fill(testBoard2, 0, 0, testBoard2[0][0], 1, 4);
	fill(testBoard2, 0, 0, testBoard2[0][0], 2, 4);
	fill(testBoard2, 0, 0, testBoard2[0][0], 3, 4);
	fill(testBoard2, 0, 0, testBoard2[0][0], 4, 4);

	assert(compareBoard(testBoard1, testBoard2, 4) != 1);

	assert(testBoard2[0][0] == 4);
	assert(testBoard2[1][0] == 4);
	assert(testBoard2[0][1] == 4);

	/*test verifyInput*/
	assert(verifyInput(1,6) == 1);
	assert(verifyInput(3,6) == 1);
	assert(verifyInput(4,3) == 0);
	assert(verifyInput(0,0) == 0);

	/*test different argv inputs*/
	assert(process2Arg(arg1, my_argv2, b, n) == 1);
	assert(process2Arg(arg1, my_argv5, b, n) == 0);
	assert(process2Arg(arg1, my_argv2, NULL, n) == 0);
	assert(process2Arg(arg1, my_argv2, b, NULL) == 0);
	assert(process2Arg(arg1, NULL, b, n) == 0);

	assert(process3Arg(arg1, arg2, my_argv3, b, n) == 1);
	assert(process3Arg(arg1, arg2, my_argv6, b, n) == 0);
	assert(process3Arg(arg1, arg2, NULL, b, n) == 0);
	assert(process3Arg(arg1, arg2, my_argv3, NULL, n) == 0);
	assert(process3Arg(arg1, arg2, NULL, NULL, n) == 0);

	/* further test if porcess args working with setupboard*/
	assert(setupBoard(1, my_argv1, b, n) == 1);
	assert(setupBoard(2, my_argv2, b, n) == 1);
	assert(setupBoard(3, my_argv3, b, n) == 1);
	assert(setupBoard(4, my_argv4, b, n) == 0);
	assert(setupBoard(2, my_argv4, NULL, n) == 0);
	assert(setupBoard(3, NULL, b, n) == 0);

	/*set boardSize to 13 to allow extra space for boardSize 12 boards that 
	shouldn't pass read from file but that should load*/
	boardSize=13;
	testFromFile = loadBoard(testFromFile, b, "testBoard1.txt");
	printBoard(testFromFile, boardSize);
	testFromFile = loadBoard(testFromFile, b, "testBoard2.txt");
	printBoard(testFromFile, boardSize);
	testFromFile = loadBoard(testFromFile, b, "testBoard4.txt");
	printBoard(testFromFile, boardSize);
	testFromFile = loadBoard(testFromFile, b, NULL);
	printBoard(testFromFile, boardSize);

	/*check that countNum is counting right numbers*/
	boardFile = fopen("testBoard1.txt", "r");
	countNum(boardFile, rC, n, nC);
	assert(rowCount == 13);
	assert(numColours == 9);
	assert(numberCount == 144);
	fclose(boardFile);

	boardFile = fopen("testBoard2.txt", "r");
	countNum(boardFile, rC, n, nC);
	assert(rowCount == 11);
	assert(numColours == 9);
	assert(numberCount == 145);
	fclose(boardFile);

	boardFile = fopen("testBoard3.txt", "r");
	countNum(boardFile, rC, n, nC);
	assert(rowCount == 11);
	assert(numColours == 9);
	assert(numberCount == 143);
	fclose(boardFile);

	boardFile = fopen("testBoard4.txt", "r");
	countNum(boardFile, rC, n, nC);
	assert(rowCount == 0);
	assert(numColours == 0);
	assert(numberCount == 0);
	fclose(boardFile);


	testFromFile = readFile(testFromFile, b, "testBoard1.txt", n);
	assert(testFromFile == NULL);
	testFromFile = readFile(testFromFile, b, "testBoard2.txt", n);
	assert(testFromFile == NULL);
	testFromFile = readFile(testFromFile, b, "testBoard3.txt", n);
	assert(testFromFile == NULL);
	testFromFile = readFile(testFromFile, b, "testBoard4.txt", n);
	assert(testFromFile == NULL);
	testFromFile = readFile(testFromFile, b, "testBoard5.txt", n);
	assert(testFromFile != NULL);
	printBoard(testFromFile, boardSize);
	assert(boardSize == 2);
	assert(numColours == 3);
	testFromFile = readFile(testFromFile, b, "testBoard6.txt", n);
	assert(testFromFile == NULL);
	
	assert(freeBoard(testBoard1, 4) == 1);
	assert(freeBoard(testBoard2, 4) == 1);
	assert(freeBoard(testBoard3, 3) == 1);
	assert(freeBoard(testBoard5, 4) == 0);

	free(my_argv1);
	free(my_argv2);
	free(my_argv3);
	free(my_argv4);
	free(my_argv5);
	free(my_argv6);



}
