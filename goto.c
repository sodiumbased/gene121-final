#include "PC_FileIO.c"

bool map[10][13] =
	{{0,0,0,0,0,0,0,0,1,1,1,1,0},
	 {0,0,0,0,0,0,0,0,1,1,1,1,0},
	 {0,0,0,0,0,0,0,0,1,1,1,1,0},
	 {0,0,0,0,0,0,0,0,1,1,1,1,0},
	 {0,0,0,0,0,0,0,0,1,1,1,1,0},
	 {0,0,0,0,0,0,0,0,1,1,1,1,0},
	 {0,1,1,1,1,1,1,1,1,1,1,1,0},
	 {0,0,0,0,1,1,1,1,1,1,1,1,0},
	 {0,0,0,0,1,1,1,1,1,1,1,1,0},
	 {0,0,0,0,0,0,0,0,0,0,0,0,0}};

int paths[10][13] = {{0}};

// bool map[49][49];
// int paths[49][49];

typedef struct{
	int pos[2];
	int dir;
	bool* interrupt;
	int mode;
	int clean_cycles;
	bool* hasFinished;
}state;

void initialize_paths(){
	for (int i = 0; i < 10; i++){
		for (int a = 0; a < 10; a++){
			paths[i][a] = 0;
		}
	}
}

int identifyTouch()
{
	int light = getColorAmbient(S1);
	displayString(3, "%d ", light);
	int identify = 0;
	if (light == 2)
		identify = 4; // red - back
	else if (light == 5 || light == 6)
		identify = 1; // blue - right side
	else if (light == 15)
		identify = 3; // green - left
	else if (light == 12)
		identify = 2; // white - front
	writeDebugStreamLine("%d identified", identify);
	return identify;
}

bool triggered ()
{
	if (getColorAmbient(S1) > 2)
		return true;
	return false;
}

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

void drive(int distance, state* cur_state, bool canInterrupt){
	const int BLOCK_SIZE = 10;
	writeDebugStreamLine("Intended to go to %d cm @%d",distance,cur_state->dir);
	float encoderLimit = - distance * 180 / PI / 4;
	nMotorEncoder[motorA] = 0;
	if (distance > 0){
		motor[motorA] = motor[motorD] = -25;
		int initial_angle = getGyroDegrees(S2);
		while(!(*(cur_state->interrupt) && canInterrupt) && nMotorEncoder[motorA] > encoderLimit){
			if(getGyroDegrees(S2) - initial_angle < -1){
				motor[motorA] = -27;
			}
			else if(getGyroDegrees(S2) - initial_angle > 1){
				motor[motorD] = -27;
			}
			else {
				motor[motorA] = motor[motorD] = -25;
			}

		}
	}
	else {
		motor[motorA] = motor[motorD] = 25;
		while (!(*(cur_state->interrupt) && canInterrupt) && nMotorEncoder[motorA] < encoderLimit);
	}
	motor[motorA] = motor[motorD] = 0;

	// TODO: make sure that in the event of obstacle, it should back up to the nearest block
	if (*(cur_state->interrupt) && canInterrupt){
		int temp_rot = nMotorEncoder[motorA] % (int)(BLOCK_SIZE/4.*180/PI);
		motor[motorA] = motor[motorD] = 25;
		while (nMotorEncoder[motorA] < temp_rot);
		distance -= temp_rot*PI/180*4;
		motor[motorA] = motor[motorD] = 0;
	}
	if(cur_state->dir == 0){
		cur_state->pos[1] = cur_state->pos[1] - distance / BLOCK_SIZE;
	}
	else if(cur_state->dir == 1){
		cur_state->pos[0] = cur_state->pos[0] + distance / BLOCK_SIZE;
	}
	else if(cur_state->dir == 2){
		cur_state->pos[1] = cur_state->pos[1] + distance / BLOCK_SIZE;
	}
	else {
		cur_state->pos[0] = cur_state->pos[0] - distance / BLOCK_SIZE;
	}
	writeDebugStreamLine("Changed to (%d,%d)",cur_state->pos[0],cur_state->pos[1]);
	wait1Msec(100);
}

void turn(int angle, state* cur_state){
	const int OVERSHOOT_CORRECTION = 2;
	int initAngle = getGyroDegrees(S2);

	if(angle > 0){
		motor[motorA] = -10;
		motor[motorD] = 10;
		while(getGyroDegrees(S2) < (initAngle + angle - OVERSHOOT_CORRECTION)){}
	}
	else if(angle < 0){
		motor[motorA] = 10;
		motor[motorD] = -10;
		while(getGyroDegrees(S2) > (initAngle + angle + OVERSHOOT_CORRECTION)){}
	}

	motor[motorA] = motor[motorD] = 0;
	cur_state->dir = (cur_state->dir + angle / 90 + 4) % 4;
	writeDebugStreamLine("Direction to %d",cur_state->dir);
	wait1Msec(100);
}

void wet()
{
    const int TIMES_WET = 2; //number of times the robot will wet the mop
    const int WATER_LEVEL = 10;    // distance of water from the peak of the mop in cm
    const int WHEEL_RADIUS = 4; //radius of the wheels

    for (int wet =0; wet < TIMES_WET; wet++)
    {
        nMotorEncoder[motorD] =0;
        motor[motorD] = 20;
        while (((nMotorEncoder[motorD]*WHEEL_RADIUS*PI)/180) < WATER_LEVEL);
        motor[motorD] = 0;
        wait1Msec(2000);
        motor[motorD] = -20;
        while(nMotorEncoder[motorD]> 0);
        motor[motorD] = 0;
    }
}

void search(int cur_x, int cur_y, int dest_x, int dest_y, int weight){
	if (!map[cur_y][cur_x] || paths[cur_y][cur_x] <= weight && paths[cur_y][cur_x])
		return;
	paths[cur_y][cur_x] = weight;
	weight++;
	search(cur_x, cur_y+1, dest_x, dest_y, weight);
	search(cur_x+1, cur_y, dest_x, dest_y, weight);
	search(cur_x, cur_y-1, dest_x, dest_y, weight);
	search(cur_x-1, cur_y, dest_x, dest_y, weight);
}

int find_dir(int x, int y){
	int weight = paths[y][x];
	int dir[4];
	dir[0]=paths[y-1][x];
	dir[1]=paths[y][x+1];
	dir[2]=paths[y+1][x];
	dir[3]=paths[y][x-1];
	// This is the correct order {up, right, down, left}
	for (int i = 0; i < 4; i++){
		if (dir[i]+1 == weight)
			return i;
	}
	return 0;
}

void find_nav(int dest_x, int dest_y, int initial_dir){
	int temp_x = dest_x, temp_y = dest_y;
	int preferred_dir = find_dir(temp_x, temp_y);
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
		writeLongPC(output, temp_disp*10); // 10 for testing, 25 for actual
		writeEndlPC(output);
		if (paths[temp_y][temp_x] > 1){
			int temp = find_dir(temp_x, temp_y);
			int dir_difference = preferred_dir - temp;
			// printf("Turn %d degrees\n", dir_difference*90);
			writeLongPC(output, dir_difference*90);
			writeEndlPC(output);
			preferred_dir = temp;
		}
	}
	// printf("Turn %d degrees\n", (initial_dir-preferred_dir)*90);
	writeLongPC(output, (initial_dir-preferred_dir)*90);
	writeEndlPC(output);
	closeFilePC(output);
}

void read_instructions(state* cur_state){
	const int MAX_INS_SIZE = 30;
	int args[MAX_INS_SIZE] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	TFileHandle input;
	openReadPC(input, "goto_temp.txt");
	int arg;
	for (int i = 0; readIntPC(input, arg); i++){
		args[i] = arg;
	}
	for (int i = 0; i < MAX_INS_SIZE/2; i++){
		int itemp = args[i];
		args[i] = args[MAX_INS_SIZE-1-i];
		args[MAX_INS_SIZE-1-i] = itemp;
	}
	for (int i = 0; i < MAX_INS_SIZE; i++){
		if (args[i]){
			turn(args[i], cur_state);
			i++;
			drive(args[i], cur_state, 1);
		}
	}
	closeFilePC(input);
}

void go_to(int x, int y, state* cur_state){
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
			turn((1-cur_state->dir)*90, cur_state);
		else
			turn((3-cur_state->dir)*90, cur_state);
		drive(abs(y-cur_state->pos[1])*10, cur_state, 1); // 10 for testing, 25 for actual
		if (x - cur_state->pos[0] > 0) // This is delta y
			turn((2-cur_state->dir)*90, cur_state);
		else
			turn((-cur_state->dir)*90, cur_state);
		drive(abs(x-cur_state->pos[0])*10, cur_state, 1); // 10 for testing, 25 for actual
	}
	else {
	initialize_paths();
	search(cur_state->pos[0], cur_state->pos[1], x, y, 1);
	find_nav(x, y, cur_state->dir);
	read_instructions(cur_state);
	//motor functions later lol
	//output the "trivial solution" messages to a file so I can read the first one
	}
}

void calculate_route(){
	const int COL_SIZE = 13;
	const int ROW_SIZE = 10;
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


void clean (state* cur_state)
{
	for (int cycle = 0; cycle < cur_state->clean_cycles; cycle++)
	{
		TFileHandle fin;
		if (!openReadPC(fin, "route.txt"))
		{
			displayString(5, "Error!");
			wait1Msec(5000);
		}
		else
		{
			int x_coord = 0, y_coord = 0;
			while (readIntPC(fin, x_coord))
			{
				readIntPC(fin, y_coord);
				if ((x_coord >= 24 && x_coord <= 27) && (y_coord==24 || y_coord ==25)){
					y_coord = 26;
				}
				while (cur_state->pos[0] != x_coord && cur_state->pos[1] != y_coord){
					writeDebugStreamLine("going to (%d,%d)",x_coord, y_coord);
					go_to(x_coord, y_coord, cur_state);
					while (*(cur_state->interrupt));
				}
			}
			closeFilePC(fin);
		}
	}
}

void shutdown(state* cur_state){
	while (cur_state->pos[0] != 28 && cur_state->pos[1] != 24){
		go_to(28,24,cur_state);
		while(*(cur_state->interrupt));
	}
	turn((1-cur_state->dir)*90,cur_state);
	motor[motorA] = motor[motorD] = 30;
	while(identifyTouch() != 4);
	motor[motorA] = motor[motorD] = 0;
}

task obstacle_avoidance(){
    state current_state;
    current_state.interrupt = &(map[9][12]);
    current_state.hasFinished = &(map[9][0]);
    while (! *(current_state.hasFinished)){
        if (triggered()){
			*(current_state.interrupt) = 1;
			wait1Msec(50);
			int sensor = identifyTouch();
			wait1Msec(1000);
			if (sensor == 1){ //right
				drive(-25, &current_state, 0);
				turn(-90, &current_state);
				drive(25, &current_state, 0);
				turn(90, &current_state);
			}
			else if (sensor == 2 || getGyroDegrees(S3) < 20){ //front
			drive(-25, &current_state, 0);
				if (random(1)){
					turn(-90, &current_state);
					drive(25, &current_state, 0);
					turn(90, &current_state);
				}
				else {
					turn(90, &current_state);
					drive(25, &current_state, 0);
					turn(-90, &current_state);
				}
			}
			else if (sensor == 3){ //left
				drive(-25, &current_state, 0);
				turn(90, &current_state);
				drive(25, &current_state, 0);
				turn(-90, &current_state);
			}
			*(current_state.interrupt) = 0;
    	}
	}
}

task main(){
	SensorType[S2] = sensorEV3_Gyro;
	wait1Msec(50);
	SensorMode[S2] = modeEV3Gyro_RateAndAngle;
	wait1Msec(50);
	SensorType[S1] = sensorEV3_Color;
	wait1Msec(50);
	SensorMode[S1] = modeEV3Color_Ambient;
	wait1Msec(50);

	state current_state;
	current_state.pos[0] = 4;
	current_state.pos[1] = 8;
	current_state.dir = 1;
	current_state.interrupt = &(map[9][12]);
	current_state.hasFinished = &(map[9][0]);
	current_state.clean_cycles = 1;

	// calculate_route();
	startTask(obstacle_avoidance);
	// clean(&current_state);
	go_to(1,6,&current_state);
	// drive(-100, &current_state, 1);
	// turn(-90, &current_state, 1);
	// wait1Msec(500);
	// turn(90, &current_state, 1);
	*(current_state.hasFinished) = 1;

}