#include "PC_FileIO.c"

const int ROW_SIZE = 10, COL_SIZE  = 10;
bool map[ROW_SIZE][COL_SIZE];

void calculate_route(){
	TFileHandle fout;
	bool okay = openWritePC(fout, "route.txt");

	bool reverse = true;
	int j = 0;

	for(int i = 0; i < COL_SIZE; i++){
		while(j < ROW_SIZE){
			while(map[(1 - 2 * reverse) * j + reverse * (ROW_SIZE - 1)][i]){
				writeLongPC(fout, i);
				writeTextPC(fout, " ");
				writeLongPC(fout, (1 - 2 * reverse) * j + reverse * (ROW_SIZE - 1));
				writeEndlPC(fout);
				j++;

				while(map[(1 - 2 * reverse) * j + reverse * (ROW_SIZE - 1)][i]){
					j++;
				}
				writeLongPC(fout, i);
				writeTextPC(fout, " ");
				writeLongPC(fout, ((1 - 2 * reverse) * j + reverse * (ROW_SIZE - 1)) - (1 - 2 * reverse));
				writeEndlPC(fout);
			}
			j++;
		}
		j = 0;
		reverse = !reverse;
	}

	closeFilePC(fout);
}

task main()
{
	calculate_route();
}
