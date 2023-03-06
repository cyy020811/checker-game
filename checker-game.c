/* Program to print and play checker games.

  Skeleton program written by Artem Polyvyanyy, artem.polyvyanyy@unimelb.edu.au,
  September 2021, with the intention that it be modified by students
  to add functionality, as required by the assignment specification.

  Student Authorship Declaration:

  (1) I certify that except for the code provided in the initial skeleton file,
  the program contained in this submission is completely my own individual
  work, except where explicitly noted by further comments that provide details
  otherwise. I understand that work that has been developed by another student,
  or by me in collaboration with other students, or by non-students as a result
  of request, solicitation, or payment, may not be submitted for assessment in
  this subject. I understand that submitting for assessment work developed by
  or in collaboration with other students or non-students constitutes Academic
  Misconduct, and may be penalized by mark deductions, or by other penalties
  determined via the University of Melbourne Academic Honesty Policy, as
  described at https://academicintegrity.unimelb.edu.au.

  (2) I also certify that I have not provided a copy of this work in either
  softcopy or hardcopy or any other form to any other student, and nor will I
  do so until after the marks are released. I understand that providing my work
  to other students, regardless of my intention or any undertakings made to me
  by that other student, is also Academic Misconduct.

  (3) I further understand that providing a copy of the assignment specification
  to any form of code authoring or assignment tutoring service, or drawing the
  attention of others to such services and code that may have been made
  available via such a service, may be regarded as Student General Misconduct
  (interfering with the teaching activities of the University and/or inciting
  others to commit Academic Misconduct). I understand that an allegation of
  Student General Misconduct may arise regardless of whether or not I personally
  make use of such solutions or sought benefit from such actions.

  Signed by: Chi-Yuan Yang 1169908
  Dated:     03/10/2021

*/

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>

/* some #define's from my sample solution ------------------------------------*/
#define BOARD_SIZE          8       // board size
#define ROWS_WITH_PIECES    3       // number of initial rows with pieces
#define CELL_EMPTY          '.'     // empty cell character
#define CELL_BPIECE         'b'     // black piece character
#define CELL_WPIECE         'w'     // white piece character
#define CELL_BTOWER         'B'     // black tower character
#define CELL_WTOWER         'W'     // white tower character
#define COST_PIECE          1       // one piece cost
#define COST_TOWER          3       // one tower cost
#define TREE_DEPTH          3       // minimax tree depth
#define COMP_ACTIONS        10      // number of computed actions
#define ERROR_SOUR_OUTSIDE  1       // source cell is outside of the board
#define ERROR_TAR_OUTSIDE   2       // target cell is outside of the board
#define ERROR_SOUR_EMPTY    3       // source cell is empty
#define ERROR_TAR_NOT_EMPTY 4       // target cell is not empty
#define ERROR_SOUR_OPPONENT 5       // source cell holds opponent’s piece/tower
#define ERROR_ILLEGAL       6       // illegal action
#define DIRECTION           4       // number of possible directions
#define UP_RIGHT            0       // direction up-right
#define DOWN_RIGHT          1       // direction down-right
#define DOWN_LEFT           2       // direction down-left
#define UP_LEFT             3       // direction up-left
#define MAX                 1       // Minimax(max)
#define MIN                 0       // Minimax(min)
#define UP                 -1       // vertical moving direction for up
#define DOWN                1       // vertical moving direction for down
#define RIGHT               1       // horizontal moving direction for right
#define LEFT               -1       // horizontal moving direction for left
#define NOACTION            'N'     // command for read input
#define NEXTACTION          'A'     // command for compute next action
#define MACHINEGAME         'P'     // command for machine games

/* one type definition from my sample solution -------------------------------*/
typedef unsigned char board_t[BOARD_SIZE][BOARD_SIZE];  // board type

/* Type definition for node*/
typedef struct node node_t;
/* Include a board, board cost, source row/col, target row/col, childrennum,
round, parent, and children*/
struct node {
	board_t board;
	int cost, sourrow, tarrow, childrennum, round;
	char sourcol, tarcol;
	node_t* parent;
	node_t** children;
};

/* Function prototypes -------------------------------------------------------*/

// Initialize the game board
void boardsetup(board_t);
// Print out the board
void printboard(board_t);
// Print the board size and number of pieces
void printsetup();
// Report action
void reportaction(char, int, char, int, int, board_t, char);
// Read the input(action)
int action(char, int, char, int, int, board_t);
// Calculate the board cost
int calculatecost(board_t);
// Tell if the action is legal
int isillegal(char, int, char, int, int, board_t);
// Tell if the direction is north
int isnorth(int, int);
// Tell if the action is a capture
int iscapture(char, char);
// Copy a board
void boardcpy(board_t, board_t);
// Calculate all the possible boards using revursive call
node_t* recursiveboard(board_t, node_t*, int, int);
// Find available move based on a piece/tower
int findmove(board_t, char, int, int, int, int*, int*);
// Find the propogated cost based on minimax rule
int calculateminimax(node_t*, int, int);
// Free the dynamic memories
void recursivefree(node_t*);
// Compute the next action
node_t* computedaction(node_t*, int, int);
// Print the next action
void printcomputedaction(node_t*, int);
// Tell if one side has won
int win(node_t*);

int
main(int argc, char* argv[]) {
	// YOUR IMPLEMENTATION OF STAGES 0-2
	// Stage 0
	// Set up the game
	int round = 1;
	board_t board;
	boardsetup(board);
	printsetup();
	printboard(board);
	int sourrow, tarrow;
	char sourcol, tarcol;
	// Read the input
	while (scanf("%c%d-%c%d", &sourcol, &sourrow, &tarcol, &tarrow) == 4) {
		// Terminate the game if the action is illegal
		if (action(sourcol, sourrow, tarcol, tarrow, round, board)
			== EXIT_FAILURE) {
			exit(1);
		}
		// Report the action
		reportaction(sourcol, sourrow, tarcol, tarrow, round, board, NOACTION);
		// Print the board
		printboard(board);
		// Skip the input that has already been read
		scanf("%*d-%*c%*d");
		// Increment the round number by 1
		round++;
	}
	// Stage 1 & Stage 2
	// Turn the last input being read into a command
	char command = sourcol;
	// If no command the program ends without computing the next action
	int actionnum = 0;
	// If command is 'A' then only compute the next action
	if (command == NEXTACTION) {
		actionnum = COMP_ACTIONS / COMP_ACTIONS;
	}
	/* If command is 'P' then compute the ten next actions or actions until
	there is a winner*/
	else if (command == MACHINEGAME) {
		actionnum = COMP_ACTIONS;
	}
	int i;
	// Compute the next action for actionnum times
	for (i = 0; i < actionnum; i++) {
		// Create the tree
		node_t* root = recursiveboard(board, NULL, 0, round);
		if (win(root)) {
			if (round % 2 == 0) {
				printf("BLACK WIN!\n");
			}
			else {
				printf("WHITE WIN!\n");
			}
			// Free the dynamic memories
			recursivefree(root);
			// End the game
			break;
		}
		// Compute the next action
		node_t* nextaction = computedaction(root, root->childrennum, round % 2);
		/* Copy the board from nextaction to the current board that is being
		played*/
		boardcpy(nextaction->board, board);
		// Print the computed next action
		printcomputedaction(nextaction, round);
		// If there is a winner then print the winner message and end the game
		// Free the dynamic memories
		recursivefree(root);
		// Increment the round number by 1
		round++;
	}
	return EXIT_SUCCESS;            // exit program with the success code
}
//Function definitions -------------------------------------------------------

void
boardsetup(board_t board) {
	int i, j;
	for (i = 0; i < BOARD_SIZE; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			// Identify possible cells that should have pieces
			if ((i % 2 == 0 && j % 2 != 0) || (i % 2 != 0 && j % 2 == 0)) {
				// Put white pieces in Row 1-3
				if (i < ROWS_WITH_PIECES) {
					board[i][j] = CELL_WPIECE;
				}
				// Fill the empty rows Row 4 and 5
				else if (i < BOARD_SIZE - ROWS_WITH_PIECES) {
					board[i][j] = CELL_EMPTY;
				}
				// Put black pieces in Row 6-8
				else {
					board[i][j] = CELL_BPIECE;
				}
			}
			else {
				// Fill the rest of the empty cells
				board[i][j] = CELL_EMPTY;
			}
		}
	}
}

void
printboard(board_t board) {
	int i, j;
	// Print the hori axis A - H
	printf("     A   B   C   D   E   F   G   H\n");
	for (i = 0; i < BOARD_SIZE; i++) {
		// Print hori grid line
		printf("   +---+---+---+---+---+---+---+---+\n");
		// Print the vert axis 1 - 8
		printf(" %d |", i + 1);
		for (j = 1; j <= BOARD_SIZE; j++) {
			// Print the cells
			printf(" %c |", board[i][j - 1]);
			if (j % BOARD_SIZE == 0) {
				printf("\n");
			}
		}
	}
	// Print the last grid line
	printf("   +---+---+---+---+---+---+---+---+\n");
}

void
printsetup() {
	// Black & white
	int colors = 2;
	// Print the board size
	printf("BOARD SIZE: %dx%d\n", BOARD_SIZE, BOARD_SIZE);
	// Print the number of black pieces
	printf("#BLACK PIECES: %d\n", BOARD_SIZE * ROWS_WITH_PIECES / colors);
	// Print the number of white pieces
	printf("#WHITE PIECES: %d\n", BOARD_SIZE * ROWS_WITH_PIECES / colors);
}

void
reportaction(char sourcol, int sourrow, char tarcol, int tarrow, int round,
	board_t board, char command) {
	// Print the seperation line
	printf("=====================================\n");
	// Print *** if the action is computed
	if (command == NEXTACTION || command == MACHINEGAME) {
		printf("*** ");
	}
	// Odd round number means it is black's turn
	if (round % 2 != 0) {
		printf("BLACK ACTION #%d:", round);
	}
	// Even round number means it is white's turn
	else {
		printf("WHITE ACTION #%d:", round);
	}
	// Print the source cell and the targeted cell
	printf(" %c%d-%c%d\n", sourcol, sourrow, tarcol, tarrow);
	// Print the board cost
	printf("BOARD COST: %d\n", calculatecost(board));
}

int
action(char sourcol, int sourrow, char tarcol, int tarrow, int round,
	board_t board) {
	// Turn char coordinates into int
	int numsourcol = sourcol - 'A', numtarcol = tarcol - 'A';
	// Find the column of the middle cell for capture
	int midcol = (numsourcol + numtarcol) / 2;
	// Find the row of the middle cell for capture
	int midrow = (sourrow - 1 + tarrow - 1) / 2;
	// Find the error code the action
	int error = isillegal(sourcol, sourrow, tarcol, tarrow, round, board);
	// Return EXIT_FAILURE if the action is not legal
	if (error) {
		if (error == ERROR_SOUR_OUTSIDE) {
			printf("ERROR: Source cell is outside of the board.\n");
		}
		else if (error == ERROR_TAR_OUTSIDE) {
			printf("ERROR: Target cell is outside of the board.\n");
		}
		else if (error == ERROR_SOUR_EMPTY) {
			printf("ERROR: Source cell is empty.\n");
		}
		else if (error == ERROR_TAR_NOT_EMPTY) {
			printf("ERROR: Target cell is not empty.\n");
		}
		else if (error == ERROR_SOUR_OPPONENT) {
			printf("ERROR: Source cell holds opponent's piece/tower.\n");
		}
		else if (error == ERROR_ILLEGAL) {
			printf("ERROR: Illegal action.\n");
		}
		return EXIT_FAILURE;
	}
	// Execute only if the move is a capture
	if (iscapture(tarcol, sourcol)) {
		// Set the mid cell to empty after capture
		board[midrow][midcol] = CELL_EMPTY;
	}
	// Move the piece/tower from the source cell to the target cell
	board[tarrow - 1][numtarcol] = board[sourrow - 1][numsourcol];
	/* Turn a piece into a tower if successfully reaches the other side of
	the board*/
	if (board[tarrow - 1][numtarcol] == CELL_WPIECE && tarrow == BOARD_SIZE) {
		board[tarrow - 1][numtarcol] = CELL_WTOWER;
	}
	else if (board[tarrow - 1][numtarcol] == CELL_BPIECE && tarrow == 1) {
		board[tarrow - 1][numtarcol] = CELL_BTOWER;
	}
	// Set the source cell to empty after move
	board[sourrow - 1][numsourcol] = CELL_EMPTY;
	return EXIT_SUCCESS;
}

int
calculatecost(board_t board) {
	int i, j, cost = 0;
	for (i = 0; i < BOARD_SIZE; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			// Black piece +1 
			if (board[i][j] == CELL_BPIECE) {
				cost += COST_PIECE;
			}
			// White piece -1 
			else if (board[i][j] == CELL_WPIECE) {
				cost -= COST_PIECE;
			}
			// Black tower +3 
			else if (board[i][j] == CELL_BTOWER) {
				cost += COST_TOWER;
			}
			// White tower -3 
			else if (board[i][j] == CELL_WTOWER) {
				cost -= COST_TOWER;
			}
		}
	}
	// Return the board cost
	return cost;
}

int
isillegal(char sourcol, int sourrow, char tarcol, int tarrow, int round,
	board_t board) {
	// Turn char coordinates into int
	int numsourcol = sourcol - 'A', numtarcol = tarcol - 'A';
	// Find the column of the middle cell for capture
	int midcol = (numsourcol + numtarcol) / 2;
	// Find the row of the middle cell for capture
	int midrow = (sourrow - 1 + tarrow - 1) / 2;
	// Define the source cell
	char sourcell = board[sourrow - 1][numsourcol];
	// Define the target cell
	char tarcell = board[tarrow - 1][numtarcol];
	// Define the mid cell for capture
	char midcell = board[midrow][midcol];
	// Source cell's colunm or row goes out of bounds
	if (sourcol < 'A' || sourcol > 'H' || sourrow < 1 || sourrow > BOARD_SIZE) {
		return ERROR_SOUR_OUTSIDE;
	}
	// Target cell's colunm or row goes out of bounds
	if (tarcol < 'A' || tarcol > 'H' || tarrow < 1 || tarrow > BOARD_SIZE) {
		return ERROR_TAR_OUTSIDE;
	}
	// Missing piece or tower in the source cell
	if (sourcell == CELL_EMPTY) {
		return ERROR_SOUR_EMPTY;
	}
	// Target cell is occupied
	if (tarcell != CELL_EMPTY) {
		return ERROR_TAR_NOT_EMPTY;
	}
	// Black's turn
	if (round % 2 != 0) {
		// Source cell is a white piece/tower
		if (sourcell == CELL_WPIECE || sourcell == CELL_WTOWER) {
			return ERROR_SOUR_OPPONENT;
		}
		// Source cell is a black piece but is moving toward south
		else if (sourcell == CELL_BPIECE && isnorth(sourrow, tarrow) < 0) {
			return ERROR_ILLEGAL;
		}
	}
	// White's turn
	else if (round % 2 == 0) {
		// Source cell is a black piece/tower
		if (sourcell == CELL_BPIECE || sourcell == CELL_BTOWER) {
			return ERROR_SOUR_OPPONENT;
		}
		// Source cell is a white piece but is moving toward north
		else if (sourcell == CELL_WPIECE && isnorth(sourrow, tarrow) > 0) {
			return ERROR_ILLEGAL;
		}
	}
	// Target colunm is unreachable
	if (abs(sourcol - tarcol) > 2) {
		return ERROR_ILLEGAL;
	}
	// Target row is unreachable
	if (abs(sourrow - tarrow) > 2) {
		return ERROR_ILLEGAL;
	}
	// Same row or col
	if (sourrow == tarrow || sourcol == tarcol) {
		return ERROR_ILLEGAL;
	}
	// Not a diagonal move
	if (abs((sourcol - tarcol) / (sourrow - tarrow)) != 1) {
		return ERROR_ILLEGAL;
	}
	// Trying to capture an ally
	if (iscapture(tarcol, sourcol)) {
		// Both source cell and mid cell are black pieces/towers
		if (sourcell == CELL_BPIECE || sourcell == CELL_BTOWER) {
			if (midcell == CELL_BPIECE || midcell == CELL_BTOWER) {
				return ERROR_ILLEGAL;
			}
		}
		// Both source cell and mid cell are white pieces/towers
		else if (sourcell == CELL_WPIECE || sourcell == CELL_WTOWER) {
			if (midcell == CELL_WPIECE || midcell == CELL_WTOWER) {
				return ERROR_ILLEGAL;
			}
		}
	}
	// No error detected
	return EXIT_SUCCESS;
}

int
isnorth(int sourrow, int tarrow) {
	// Return the relative direction of sourrow to tarrow
	if (tarrow - sourrow < 0) {
		return DOWN;
	}
	else {
		return UP;
	}
}

int
iscapture(char tarcol, char sourcol) {
	// Return 1 if the distance between target col and source col is 2
	if ((tarcol - sourcol) % 2 == 0) {
		return 1;
	}
	return 0;
}

void
boardcpy(board_t board, board_t newboard) {
	int i, j;
	// Copy the char one by one into the newboard
	for (i = 0; i < BOARD_SIZE; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			newboard[i][j] = board[i][j];
		}
	}
}

node_t
* recursiveboard(board_t board, node_t* parent, int depth, int round) {
	int i, j, nextround = round + 1, sourrow = 0, vertmove = 0, horimove = 0,
		currentsize = DIRECTION, chirdrennum = 0;
	char sourcol = 'A';
	node_t* selfnode = (node_t*)malloc(sizeof(node_t));
	assert(selfnode);
	selfnode->parent = parent;
	boardcpy(board, selfnode->board);
	// Stop recursion when reach depth = 3
	if (depth == TREE_DEPTH) {
		// End of the tree
		selfnode->children = NULL;
		selfnode->childrennum = 0;
		// Calculate the leaf board cost
		selfnode->cost = calculatecost(selfnode->board);
		return selfnode;
	}
	//printboard(board);
	selfnode->children = (node_t**)malloc(currentsize * sizeof(node_t*));
	assert(selfnode->children);
	// Scan through the board in row-major order
	for (i = 0; i < BOARD_SIZE * BOARD_SIZE; i++) {
		char sourcell = board[i / BOARD_SIZE][i % BOARD_SIZE];
		if (sourcell == CELL_EMPTY) {
			continue;
		}
		// Find a white piece/tower
		if (round % 2 == 0) {
			if (sourcell == CELL_BTOWER || sourcell == CELL_BPIECE) {
				continue;
			}
			if (sourcell == CELL_WTOWER || sourcell == CELL_WPIECE) {
				sourcol = i % BOARD_SIZE + 'A';
				sourrow = i / BOARD_SIZE;
			}
		}
		// Find a black piece/tower
		else {
			if (sourcell == CELL_WTOWER || sourcell == CELL_WPIECE) {
				continue;
			}
			if (sourcell == CELL_BTOWER || sourcell == CELL_BPIECE) {
				sourcol = i % BOARD_SIZE + 'A';
				sourrow = i / BOARD_SIZE;
			}
		}
		// Run a test for each direction
		for (j = 0; j < DIRECTION; j++) {
			// Find a possible action
			if (findmove(board, sourcol, sourrow, j, round, &vertmove,
				&horimove)) {
				// If no spaces then realloc
				if (chirdrennum == currentsize) {
					// Times four because there are four possible directions
					currentsize *= DIRECTION;
					selfnode->children = (node_t**)realloc(selfnode ->
						children, currentsize * sizeof(node_t*));
					assert(selfnode->children);
				}
				int tarrow = sourrow + vertmove;
				char tarcol = sourcol + horimove;
				// Action on a new board
				board_t newboard;
				boardcpy(board, newboard);
				action(sourcol, sourrow + 1, tarcol, tarrow + 1, round, 
					newboard);
				// Create a childnode with the newboard and recusive call
				node_t* childnode = recursiveboard(newboard, selfnode, 
					depth + 1, nextround);
				// Record the action made in the board to childnode
				childnode->sourcol = sourcol;
				childnode->sourrow = sourrow + 1;
				childnode->tarcol = tarcol;
				childnode->tarrow = tarrow + 1;
				// Assign dynamic memory spaces for the children
				selfnode->children[chirdrennum] = childnode;
				// Append the childnode into children
				chirdrennum++;
			}
		}
	}
	// Calculate the propogated cost based on minimax rule
	selfnode->cost = calculateminimax(selfnode, chirdrennum, round % 2);
	// Record the number of children
	selfnode->childrennum = chirdrennum;
	return selfnode;
}

int
findmove(board_t board, char sourcol, int sourrow, int direction, int round,
	int* vertmove, int* horimove) {
	int i;
	// From up right to up left(clockwise), set the moving directions
	if (direction == UP_RIGHT) {
		*horimove = RIGHT;
		*vertmove = UP;
	}
	else if (direction == DOWN_RIGHT) {
		*horimove = RIGHT;
		*vertmove = DOWN;
	}
	else if (direction == DOWN_LEFT) {
		*horimove = LEFT;
		*vertmove = DOWN;
	}
	else if (direction == UP_LEFT) {
		*horimove = LEFT;
		*vertmove = UP;
	}
	// Capture is possible therefore tests the moves after timing two
	for (i = 1; i <= 2; i++) {
		*horimove *= i;
		*vertmove *= i;
		char tarcol = sourcol + *horimove;
		int tarrow = sourrow + *vertmove;
		/* Return true if the move is legal and pass the horimove and vertmove
		varibles back to the main function through pointers*/
		if (isillegal(sourcol, sourrow + 1, tarcol, tarrow + 1, round, board) 
			== EXIT_SUCCESS) {
			return 1;
		}
	}
	return 0;
}

int
calculateminimax(node_t* selfnode, int childrennum, int minimax) {
	int i, propcost = 0;
	// Set the propcost to INT_MIN or INT_MAX based on the minimax rule 
	if (minimax == MAX) {
		propcost = INT_MIN;
	}
	else if (minimax == MIN) {
		propcost = INT_MAX;
	}
	// Go through all the children to find the best cost
	for (i = 0; i < childrennum; i++) {
		if (i == 0) {
			propcost = selfnode->children[i]->cost;
		}
		// Find the highest cost
		if (minimax == MAX && selfnode->children[i]->cost > propcost) {
			propcost = selfnode->children[i]->cost;
		}
		// Find the lowest cost
		else if (minimax == MIN && selfnode->children[i]->cost < propcost) {
			propcost = selfnode->children[i]->cost;
		}
	}
	return propcost;
}

void
recursivefree(node_t* selfnode) {
	int i;
	// If these is no children free the selfnode
	if (selfnode->childrennum == 0) {
		free(selfnode);
		selfnode = NULL;
		return;
	}
	// Go backwardly to ensure all the children are being checked
	for (i = selfnode->childrennum - 1; i >= 0; i--) {
		// If there is child do the recursion
		if (selfnode->children[i] != NULL) {
			recursivefree(selfnode->children[i]);
		}
	}
	// Free the children
	free(selfnode->children);
	selfnode->children = NULL;
	// Free selfnode
	free(selfnode);
	selfnode = NULL;
	return;
}

node_t*
computedaction(node_t* root, int childrennum, int minimax) {
	int i, childnum = 0, bestcost;
	// Return NULL if there is no children
	// Find a child that has the best board cost
	for (i = 0; i < childrennum; i++) {
		// If there's only one child
		if (i == 0) {
			childnum = i;
			bestcost = root->children[i]->cost;
		}
		// Minimax = Max, therefore choose the child with highest cost
		if (minimax == MAX && root->children[i]->cost > bestcost) {
			childnum = i;
			bestcost = root->children[i]->cost;
		}
		// Minimax = Min, therefore choose the child with lowest cost
		else if (minimax == MIN && root->children[i]->cost < bestcost) {
			childnum = i;
			bestcost = root->children[i]->cost;
		}
	}
	// Return the best child
	return root->children[childnum];
}

void
printcomputedaction(node_t* nextaction, int round) {
	// Report the action based on the info of the next action
	reportaction(nextaction->sourcol, nextaction->sourrow,
		nextaction->tarcol, nextaction->tarrow, round, nextaction ->
		board, NEXTACTION);
	// Print the board after next action
	printboard(nextaction->board);
	return;
}

int
win(node_t* root) {
	// No children which means no possible moves can be make so return true
	if (root->childrennum == 0) {
		return 1;
	}
	return 0;
}
/* Algorithms are fun!--------------------------------------------------------*/
/* THE END -------------------------------------------------------------------*/