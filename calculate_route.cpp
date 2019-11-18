#include <iostream>
#include <fstream>

using namespace std;

const int ROW_SIZE = 85, COL_SIZE = 85;

bool map[ROW_SIZE][COL_SIZE];

bool surroundingGood(int row, int col){
    if(!map[row][col+1]){
        return false;
    }
    if(!map[row][col-1]){
        return false;
    }
    if(!map[row+1][col]){
        return false;
    }
    if(!map[row-1][col]){
        return false;
    }
    if(!map[row+1][col+1]){
        return false;
    }
    if(!map[row-1][col+1]){
        return false;
    }
    if(!map[row+1][col-1]){
        return false;
    }
    if(!map[row-1][col-1]){
        return false;
    }
    return true;
}

void calculate_route(){
    ofstream fout;
    fout.open("route.txt");

    bool reverse = true;
    int j = 0;

    for (int i = 0; i < COL_SIZE; i += 2) {
        while (j < ROW_SIZE){
            while(map[(1 - 2 * reverse) * j + reverse * (ROW_SIZE- 1)][i] && surroundingGood((1 - 2 * reverse) * j + reverse * (ROW_SIZE- 1), i)){
                fout << (1 - 2 * reverse) * j + reverse * (ROW_SIZE - 1) << " " << i << endl;
                j++;

                while(map[(1 - 2 * reverse) * j + reverse * (ROW_SIZE - 1)][i] && surroundingGood((1 - 2 * reverse) * j + reverse * (ROW_SIZE- 1), i)){
                    j++;
                }

                fout << ((1 - 2 * reverse) * j + reverse * (ROW_SIZE-1)) - (1 - 2 * reverse) << " " << i << endl;
            }
            j++;
        }
        j = 0;
        reverse = !reverse;
    }

    fout.close();
}