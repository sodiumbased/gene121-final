//
// Created by Josh Cai on 2019-11-18.
//

typedef struct{
	bool interrupt;
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
}
task main(){
	state cur_state;
	cur_state.interrupt = false;
	drive(40, cur_state, true);
}
