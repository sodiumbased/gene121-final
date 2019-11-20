//
// Created by Josh Cai on 2019-11-18.
//

typedef struct{
	bool interrupt;
} state;

void turn(int angle, state* cur_state, bool canInterrupt){

    if(angle > 0){
        motor[motorA] = -15;
        motor[motorD] = 15;
        while(!(cur_state->interrupt && canInterrupt) && (getGyroDegrees(S2) < (getGyroDegrees(S2) + angle))){}
    } else if(angle < 0){
        motor[motorA] = 15;
        motor[motorD] = -15;
        while(!(cur_state->interrupt && canInterrupt) && (getGyroDegrees(S2) > (getGyroDegrees(S2) + angle))){}
    }

    motor[motorA] = motor[motorD] = 0;

}
task main(){
	state cur_state;
	cur_state.interrupt = false;
	turn(90, cur_state, true);
}
