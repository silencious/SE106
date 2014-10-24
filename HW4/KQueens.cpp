// Name: Tian Jiahe
// Student ID: 5130379056

#include <stdio.h>
#include <stdlib.h>
#include "KQueens.h"
#include "Chessboard.h"

/*
 * You should modify this function. Before you start, make
 * sure you have a clear idea of what you are doing.
 * Good Luck!
 */

inline bool diagonal(int r1, int c1, int r2, int c2){
    return (r1 + c2 == r2 + c1) || (r1 + c1 == r2 + c2);
}

inline bool attack(int r1, int c1, int r2, int c2){
    return (r1 == r2) || (c1 == c2) || diagonal(r1, c1, r2, c2);
}

bool saferow(Chessboard *c, int row, int col){
    int **board = c->board;
    for(int i=0; i<row; i++){
        for(int j=0; j<c->cols; j++){
            if(board[i][j] == Queen){
                if(attack(i, j, row, col))
                    return false;
            }
        }        
    }
    return true;
}

bool safecol(Chessboard *c, int row, int col){
    int **board = c->board;
    for(int j=0; j<col; j++){
        for(int i=0; i<c->rows; i++){
            if(board[i][j] == Queen){
                if(attack(i, j, row, col))
                    return false;
            }
        }        
    }
    return true;
}

bool rowfind(Chessboard *c, int k, int row, int col){
    if(!saferow(c, row, col)){
        if(col == c->cols-1)
            return false;
        return rowfind(c, k, row, col+1);
    }
    int **board = c->board;
    board[row][col] = Queen;
    if(row == k-1)
        return true;
    if(rowfind(c, k, row+1, 0)){
        return true;
    }
    else{
        board[row][col] = Empty;
        if(col == c->cols-1)
            return false;
        return rowfind(c, k, row, col+1);
    }
}

bool colfind(Chessboard *c, int k, int row, int col){
    if(!safecol(c, row, col)){
        if(row == c->rows-1)
            return false;
        return colfind(c, k, row+1, col);
    }
    int **board = c->board;
    board[row][col] = Queen;
    if(col == k-1)
        return true;
    if(colfind(c, k, 0, col+1)){
        return true;
    }
    else{
        board[row][col] = Empty;
        if(row == c->rows-1)
            return false;
        return colfind(c, k, row+1, col);
    }
}

int KQueens(Chessboard *c){
    int rows = c->rows;
    int cols = c->cols;
    int k = (rows<=cols)? rows:cols;
    for(; k>0; k--){
        if(rows<=cols){
            if(rowfind(c, k, 0, 0))
                return k;   
        }
        else{
            if(colfind(c, k, 0, 0))
                return k;
        }
    }
    return -1;
}
