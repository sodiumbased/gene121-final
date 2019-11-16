#include <stdio.h>
#include <stdbool.h>

typedef struct{
	int pos[2];
	int dir;
	bool interrupt;
}state;

int min(int a, int b){
	if (a > b)
		return b;
	return a;
}

int max(int a, int b){
	if (a > b)
		return a;
	return b;
}

void display_array(int arg[10][10]){
	for (int i = 0; i < 10; i++){
		for (int a = 0; a < 10; a++){
			printf("%d ", arg[i][a]);
		}
		printf("\n");
	}
}

bool search(int cur_x, int cur_y, int dest_x, int dest_y, bool map[10][10], int paths[10][10], int weight){
	if (!map[cur_x][cur_y])
		return 0;
	bool isTrivial = 1;
	for (int delta_x = min(cur_x,dest_x); delta_x < max(cur_x,dest_x); delta_x++){
		if (!map[delta_x][dest_y])
			isTrivial = 0;
	}
	for (int delta_y = min(cur_y,dest_y); delta_y < max(cur_x,dest_y); delta_y++){
		if (!map[cur_y][delta_y])
			isTrivial = 0;
	}
	if (isTrivial){
		printf("Trivial solution\n");
		return 1;
	}
	if (paths[cur_x][cur_y] <= weight && paths[cur_x][cur_y] != 0)
		return 0;
	paths[cur_x][cur_y] = weight;
	weight++;
	search(cur_x, cur_y+1, dest_x, dest_y, map, paths, weight);
	search(cur_x+1, cur_y, dest_x, dest_y, map, paths, weight);
	search(cur_x, cur_y-1, dest_x, dest_y, map, paths, weight);
	search(cur_x-1, cur_y, dest_x, dest_y, map, paths, weight);
}
	
void go_to(int x, int y, bool map[10][10], state* cur_state){
	int paths[10][10] = {{0}};
	search(cur_state->pos[0], cur_state->pos[1], x, y, map, paths, 0);

	display_array(paths);
	//motor functions later lol
}


int main(){

	bool map[10][10] =
	{{0,0,0,0,0,0,0,0,0,0},
	 {0,1,1,1,0,0,1,1,1,0},
	 {0,1,1,1,0,0,1,1,1,0},
	 {0,1,1,1,0,0,1,1,1,0},
	 {0,1,1,1,1,1,1,1,1,0},
	 {0,1,1,1,1,1,1,0,1,0},
	 {0,1,1,1,1,1,1,0,1,0},
	 {0,1,0,0,0,0,0,0,1,0},
	 {0,1,1,1,1,1,1,1,1,0},
	 {0,0,0,0,0,0,0,0,0,0}};
	state current_state;
	current_state.pos[0] = 8;
	current_state.pos[1] = 8;
	go_to(2,3,map,&current_state);

	return 0;
}