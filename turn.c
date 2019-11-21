//
// Created by Josh Cai on 2019-11-18.
//

typedef struct{
	bool interrupt;
	int pos[2];
	int dir;
} state;

void turn(int angle, state* cur_state, bool canInterrupt){

	int initAngle = getGyroDegrees(S2);

	if(angle > 0){
		motor[motorA] = -15;
		motor[motorD] = 15;
		while(!(cur_state->interrupt && canInterrupt) && (getGyroDegrees(S2) < (initAngle + angle))){}
	} else if(angle < 0){
		motor[motorA] = 15;
		motor[motorD] = -15;
		while(!(cur_state->interrupt && canInterrupt) && (getGyroDegrees(S2) > (initAngle + angle))){}
	}

	motor[motorA] = motor[motorD] = 0;

	cur_state->dir = (cur_state->dir + angle / 90 + 4) % 4;
}
task main(){
	state cur_state;
	cur_state.interrupt = false;
	cur_state.dir = 2;
	cur_state.pos[0] = 1;
	cur_state.pos[1] = 1;
	turn(90, cur_state, true);
}
