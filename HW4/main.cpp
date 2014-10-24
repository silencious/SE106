#include <stdio.h>
#include <stdlib.h>
#include "Chessboard.h"
#include "KQueens.h"

int main()
{
    Chessboard *c = new Chessboard(25, 30);
    printf("%d\n",KQueens(c));
    int rows = c->rows;
    int cols = c->cols;
    int **board = c->board;
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            if(board[i][j]==Empty)
                printf(".");
            else
                printf("Q");
        }
        printf("\n");
    }
    delete c;
    return 0;
}
