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
			if (arg[i][a] < 10)
				printf("%d  ", arg[i][a]);
			else
				printf("%d ", arg[i][a]);
		}
		printf("\n");
	}
}

void display_bool_array(bool arg[10][10]){
	for (int i = 0; i < 10; i++){
		for (int a = 0; a < 10; a++){
			printf("%d  ", arg[i][a]);
		}
		printf("\n");
	}
}

void search(int cur_x, int cur_y, int dest_x, int dest_y, bool map[10][10], int paths[10][10], int weight){
	if (!map[cur_x][cur_y] || paths[cur_x][cur_y] <= weight && paths[cur_x][cur_y])
		return;
	paths[cur_x][cur_y] = weight;
	weight++;
	search(cur_x, cur_y+1, dest_x, dest_y, map, paths, weight);
	search(cur_x+1, cur_y, dest_x, dest_y, map, paths, weight);
	search(cur_x, cur_y-1, dest_x, dest_y, map, paths, weight);
	search(cur_x-1, cur_y, dest_x, dest_y, map, paths, weight);
}
	
int find_dir(int x, int y, int paths[10][10]){
	int weight = paths[x][y];
	int dir[4] ={paths[x][y+1], paths[x+1][y], paths[x][y-1], paths[x-1][y]};
	for (int i = 0; i < 4; i++){
		if (min(weight, dir[i]) == dir[i])
			return i;
	}
}

void find_nav(int cur_x, int cur_y, int dest_x, int dest_y, int paths[10][10]){
	int temp_x = dest_x, temp_y = dest_y;
	int preferred_dir = find_dir(temp_x, temp_y, paths);
	while (paths[temp_x][temp_y] != 1){
		// Look at first the preferred direction then all four directions (0-3 protocol) if paths[temp_x][temp_y] is 1 less than the current weight. It should be noted that when the preferred direction changes that marks a turn and should be handled correctly. PS remember to reverse the driving order as it's writing instructions so it can be read then reversed later.
	}
}

void go_to(int x, int y, bool map[10][10], state* cur_state){
	bool isTrivial = 1;
	for (int delta_x = min(cur_state->pos[0],x); delta_x < max(cur_state->pos[0],x); delta_x++){
		if (!map[delta_x][cur_state->pos[1]])
			isTrivial = 0;
	}
	for (int delta_y = min(cur_state->pos[1],y); delta_y < max(cur_state->pos[1],y); delta_y++){
		if (!map[x][delta_y])
			isTrivial = 0;
	}
	if (isTrivial){
		if (x - cur_state->pos[0] > 0)
			turn((1-cur_state->dir)*90);
		else 
			turn((3-cur_state->dir)*90);
		drive(abs(x-cur_state->pos[0]));
		if (y - cur_state->pos[1] > 0)
			turn(-cur_state->dir*90);
		else 
			turn((2-cur_state->dir)*90);
		drive(abs(y-cur_state->pos[1]));
	}

	int paths[10][10] = {{0}};
	search(cur_state->pos[0], cur_state->pos[1], x, y, map, paths, 1);
	display_bool_array(map);
	printf("\n");
	display_array(paths);
	//motor functions later lol
	//output the "trivial solution" messages to a file so I can read the first one
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