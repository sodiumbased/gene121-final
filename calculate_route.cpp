#include <iostream>
#include <cstdlib>
#include <fstream>

using namespace std;

const int ROW_SIZE = 10, COL_SIZE = 10;

bool map[10][10] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 1, 1, 1, 1, 1, 0},
        {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
        {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
        {0, 0, 0, 0, 0, 1, 1, 1, 1, 0},
        {0, 0, 0, 0, 0, 1, 1, 1, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

void calculate_route(){
    ofstream fout;
    fout.open("route.txt");

    bool reverse = true;
    int j = 0;

    for (int i = 0; i < COL_SIZE; ++i) {
        while (j < ROW_SIZE){
            while(map[(1 - 2 * reverse) * j + reverse * (ROW_SIZE- 1)][i]){
                fout << (1 - 2 * reverse) * j + reverse * (ROW_SIZE - 1) << " " << i << endl;
                j++;

                while(map[(1 - 2 * reverse) * j + reverse * (ROW_SIZE - 1)][i]){
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

int main() {

    calculate_route();

    return EXIT_SUCCESS;
}