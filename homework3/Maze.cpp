// Name: Tian Jiahe
// Student ID: 5130379056

// I know that x should represent column number and y row number
// but since it's tedious to fix this I'll just leave it here
// Anyway it's just sinple homework ~_~

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
using namespace std;

struct Point{
    int x;
    int y;
    Point(int a, int b)
        :x(a), y(b){}
};

class Maze{
public:
    Maze(string filename);
    Point getStartPoint();
    bool solve(Point);
    void outputPath();

private:
    int row;
    int col;
    vector< vector<char> > maze;
    vector<Point> path;

    enum Direction{NORTH, EAST, SOUTH, WEST};

    bool reachEnd(Point);

    Point next(Point, Direction);
    bool reachWall(Point, Direction);

    bool isMarked(Point);
    void mark(Point);

};

Maze::Maze(string filename){
    ifstream fin(filename.c_str());
    fin >> col >> row;
    string line;
    getline(fin, line);
    for(int i=0; i<row; i++){
        vector<char>v;
        getline(fin, line);
        for(int j=0; j<col; j++){
            v.push_back(line[j]);
        }
        maze.push_back(v);
    }
}

Point Maze::getStartPoint(){
    for(int i=0; i<row; i++)
        for(int j=0; j<col; j++)
            if(maze[i][j]=='S')
                return Point(i, j);
    return Point(-1, -1);
}

bool Maze::reachEnd(Point p){
    return maze[p.x][p.y]=='E';
}

Point Maze::next(Point p, Direction dir){
    switch(dir){
      case NORTH:
        return Point(p.x, p.y-1);
      case EAST:
        return Point(p.x+1, p.y);
      case SOUTH:
        return Point(p.x, p.y+1);
      default:
        return Point(p.x-1, p.y);
    }
}

bool Maze::reachWall(Point p, Direction dir){
    Point n = next(p, dir);
    char c = maze[n.x][n.y];
    return ((c == '-') || (c == '|') || (c == '+'));
}

bool Maze::isMarked(Point p){
    return maze[p.x][p.y] == '*';
}

void Maze::mark(Point p){
    maze[p.x][p.y] = '*';
}

bool Maze::solve(Point p){
    if(reachEnd(p)){
        path.push_back(p);
        return true;
    }
    if(isMarked(p))
        return false;
    mark(p);
    for(int i = 0; i < 4; i++){
        Direction dir = Direction(i);
        if(!reachWall(p, dir)){
            if(solve(next(p, dir))){
                path.push_back(p);
                return true;
            }
        }
    }
    return false;
}

void Maze::outputPath(){
    for(int i=path.size()-2; i>=0; i--)
        cout << " (" << path[i].y << ',' << path[i].x << ')';
}

int main()
{
	int fileNum = 3;
    for(int num=1; num<=fileNum; num++){
        stringstream ss;
        string filename;
        ss << "testmap_" << num << ".txt";
        ss >> filename;
        Maze maze(filename);
        if(maze.solve(maze.getStartPoint())){
            cout << "testmap_" << num << ':';
            maze.outputPath();
            cout << endl;
        }
    }
	return 0;
}
