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
	if (!map[cur_y][cur_x] || paths[cur_y][cur_x] <= weight && paths[cur_y][cur_x])
		return;
	paths[cur_y][cur_x] = weight;
	weight++;
	search(cur_x, cur_y+1, dest_x, dest_y, map, paths, weight);
	search(cur_x+1, cur_y, dest_x, dest_y, map, paths, weight);
	search(cur_x, cur_y-1, dest_x, dest_y, map, paths, weight);
	search(cur_x-1, cur_y, dest_x, dest_y, map, paths, weight);
}
	
int find_dir(int x, int y, int paths[10][10]){
	int weight = paths[y][x];
	int dir[4] ={paths[y-1][x], paths[y][x+1], paths[y+1][x], paths[y][x-1]};
	// This is the correct order {up, right, down, left}
	for (int i = 0; i < 4; i++){
		if (dir[i]+1 == weight)
			return i;
	}
}

void find_nav(int dest_x, int dest_y, int paths[10][10], int initial_dir){
	int temp_x = dest_x, temp_y = dest_y;
	int preferred_dir = find_dir(temp_x, temp_y, paths);
	TFileHandle output;
	openWritePC(output, "goto_temp.txt");
	while (paths[temp_y][temp_x] > 1){
		int temp_disp = 0;
		if (preferred_dir == 0){
			while (paths[temp_y-1][temp_x] == paths[temp_y][temp_x]-1 && paths[temp_y][temp_x] > 1){
				temp_y--;
				temp_disp++;
			}
		}
		else if (preferred_dir == 1){
			while (paths[temp_y][temp_x+1] == paths[temp_y][temp_x]-1 && paths[temp_y][temp_x] > 1){
				temp_x++;
				temp_disp++;
			}
		}
		else if (preferred_dir == 2){
			while (paths[temp_y+1][temp_x] == paths[temp_y][temp_x]-1 && paths[temp_y][temp_x] > 1){
				temp_y++;
				temp_disp++;
			}
		}
		else if (preferred_dir == 3){
			while (paths[temp_y][temp_x-1] == paths[temp_y][temp_x]-1 && paths[temp_y][temp_x] > 1){
				temp_x--;
				temp_disp++;
			}
		}
		// printf("Drive for %d cm\n", temp_disp*25);
		writeIntPC(output, temp_disp*10); // 10 for testing, 25 for actual
		writeEndlPC(output);
		if (paths[temp_y][temp_x] > 1){
			int temp = find_dir(temp_x, temp_y, paths);
			int dir_difference = preferred_dir - temp;
			// printf("Turn %d degrees\n", dir_difference*90);
			writeIntPC(output, dir_difference*90);
			writeEndlPC(output);
			preferred_dir = temp;
		}
	}
	// printf("Turn %d degrees\n", (initial_dir-preferred_dir)*90);
	writeCharPC(output, 't');
	writeEndlPC(output);
	writeIntPC(output, (initial_dir-preferred_dir)*90);
	writeEndlPC(output);
	closeFilePC(output);
}

void read_instructions(state* cur_state){
	const int MAX_INS_SIZE = 30;
	int args[MAX_INS_SIZE] = {0};
	TFileHandle input;
	openReadPC(input, "goto_temp.txt");
	int arg;
	for (int i = 0; readCharPC(input, arg); i++){
		args[i] = arg;
	}
	for (int i = 0; i < MAX_INS_SIZE/2; i++){
		int itemp = args[i];
		args[i] = args[MAX_INS_SIZE-1-i];
		args[MAX_INS_SIZE-1-i] = itemp;
	}
	for (int i = 0; i < MAX_INS_SIZE; i++){
		if (args[i]){
			turn(args[i], cur_state, 0);
			i++;
			drive(args[i], cur_state, 0);
		}
	}
	closeFilePC(input);
}

void go_to(int x, int y, bool map[10][10], state* cur_state){
	bool isTrivial = 1;
	for (int delta_x = min(cur_state->pos[0],x); delta_x < max(cur_state->pos[0],x); delta_x++){
		if (!map[cur_state->pos[1]][delta_x])
			isTrivial = 0;
	}
	for (int delta_y = min(cur_state->pos[1],y); delta_y < max(cur_state->pos[1],y); delta_y++){
		if (!map[delta_y][x])
			isTrivial = 0;
	}
	if (isTrivial){
		if (y - cur_state->pos[1] > 0) // This is delta x
			turn((1-cur_state->dir)*90);
		else 
			turn((3-cur_state->dir)*90);
		drive(abs(y-cur_state->pos[1])*10, cur_state, 0); // 10 for testing, 25 for actual
		if (x - cur_state->pos[0] > 0) // This is delta y
			turn(-cur_state->dir*90);
		else 
			turn((2-cur_state->dir)*90);
		drive(abs(x-cur_state->pos[0])*10, cur_state, 0); // 10 for testing, 25 for actual
	}

	int paths[10][10] = {{0}};
	search(cur_state->pos[0], cur_state->pos[1], x, y, map, paths, 1);
	find_nav(x, y, paths, cur_state->dir);
	read_instructions(cur_state);
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
	current_state.pos[0] = 1;
	current_state.pos[1] = 8;
	current_state.dir = 1;
	go_to(3,2,map,&current_state);

	return 0;
}