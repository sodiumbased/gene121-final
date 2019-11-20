//
// Created by Josh Cai on 2019-11-18.
//

typedef struct{
	bool interrupt;
	int pos[2];
	int dir;
} state;

void drive(float distance, state* cur_state, bool canInterrupt){
	float encoderLimit = distance / (4 * PI * 2) * 360;

	nMotorEncoder[motorA] = 0;
	motor[motorA] = motor[motorD] = 25;
	while(!(cur_state->interrupt && canInterrupt) && nMotorEncoder[motorA] < encoderLimit){
		if(getGyroDegrees(S2) < -1){
			motor[motorD] = 27;
		}
		else if(getGyroDegrees(S2) > 1){
			motor[motorA] = 27;
			} else {
			motor[motorA] = motor[motorD] = 25;
		}
	}

	motor[motorA] = motor[motorD] = 0;

	float distanceTravelled = nMotorEncoder[motorA] * 2 * 4 * PI / 360;

	if(cur_state->dir == 0){
		cur_state->pos[1] = cur_state->pos[1] + (distanceTravelled / 25);
	}
	else if(cur_state->dir == 1){
		cur_state->pos[0] = cur_state->pos[0] + (distanceTravelled / 25);
	}
	else if(cur_state->dir == 2){
		cur_state->pos[1] = cur_state->pos[1] - (distanceTravelled / 25);
	}
	else {
		cur_state->pos[0] = cur_state->pos[0] - (distanceTravelled / 25);
	}
}
task main(){
	state cur_state;
	cur_state.interrupt = false;
	cur_state.dir = 2;
	cur_state.pos[0] = 1;
	cur_state.pos[1] = 1;
	drive(40, cur_state, true);
}
