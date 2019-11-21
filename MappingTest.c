/*
 * MappingTest.c
 *
 *  Created on: Nov 18, 2019
 *      Author: Artem Sotnikov
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <MappingTest.h>

int map[85][85];
void fInit();
void fMappingMain();

bool fCheckSensorFailure();

// TODO //
void fTurnUS(int degree);
void fBasicTurn(int degree, int *bearing);
//void fNavAdjust(int, enum MappingReference);

typedef struct {
	int acc_position_x;
	int acc_position_y;
	int current_position_x;
	int current_position_y;
} TempMapData;

enum MappingState;
void fTransitionState(TempMapData *, enum MappingState *, enum MappingState);

int encoder = 0;
int gyro = 0;
int us_distance = 0;
int ir_distance = 0;


int main() {
	printf("function start \n");
	fInit();
	printf("%i", map[5][5]);
}

void fInit() {
	for (int idx = 0; idx < 85; idx++) {
		for (int idx2 = 0; idx2 < 85; idx2++) {
			map[idx][idx2] = 0;
		}
	}
}

void fTurnUS(int degree) {
	// turns ultrasonic sensor
}

void fBasicTurn(int degree, int *bearing) {
	// turn by while loop
	*bearing += degree;
}

bool fCheckSensorFailure() {
	// if any touch sensors are on
	return false;
}

enum MappingReference {
	NOT_SET,RIGHT,LEFT
};


void fNavAdjust(int speed, enum MappingReference ref) {
	if ((us_distance*10 < 280) || (us_distance*10 > 320)) {
		if (us_distance*10 < 280) {
			if (ref == RIGHT) {
				// set left  motor to speed + 3
			} else {
				// set right motor to speed + 3
			}
		} else {
			if (ref == LEFT) {
				// set left  motor to speed + 3
			} else {
				// set right motor to speed + 3
			}
		}
	} else {
		if (gyro < -1) {
			// set left motor to speed + 2
		} else if (gyro > 1) {
			// set right motor to speed + 2
		} else {
			// set both motors to speed
			// if (motor[motorA] != motor[motorD]) { motor[motorA] = motor[motorD] }
		}
	}
}

enum MappingState {
	STATE_STARTING,
	STATE_UNDOCKING,
	STATE_CALIBRATION,
	//STATE_CALIBRATION_RIGHT,
	//STATE_CALIBRATION_LEFT,
	STATE_DEPLOYMENT,
	STATE_MOVING,
	STATE_EXECUTING_OUTWARD_TURN,
	STATE_AVOIDANCE_BACKUP,
	STATE_AVOIDANCE_RIGHT,
	STATE_AVOIDANCE_LEFT,
	STATE_AVOIDANCE_FORWARD,
	STATE_ERROR
};

void fResetNav() {
	// reset gyro
	// reset encoder
}

void fLogNav(TempMapData *data) {
	data->acc_position_x = data->current_position_x;
	data->acc_position_y = data->current_position_y;

	fResetNav();
}

void fTransitionState(TempMapData *data, enum MappingState *state, enum MappingState target) {

	*state = target;
	fLogNav(data);

}

void fExportPosition(int cur_x, int cur_y) {
	int square_x = 43 + cur_x/250;
	int square_y = 43 + cur_y/250;

	if (!map[square_x][square_y]) {
		map[square_x][square_y] = 1;
	}
}


void fMappingMain() {

	TempMapData pos_data;
	pos_data.acc_position_x = 0;
	pos_data.acc_position_y = 0;
	pos_data.current_position_x = 0;
	pos_data.current_position_y = 0;

	enum MappingState mapping_state = STATE_STARTING;

	enum MappingReference reference = NOT_SET;
	int reference_direction_adjust = 0;

	bool exit = false;

	int temp_distance; // calculated based on encoder ticks every cycle
	int bearing_angle;

	bool mapping_blackout = false;

	char error_message[] = "no errors";
	char status_message[] = "initialized";

	int last_us_distance = -1;
	int last_ir_distance = -1;



	while (!exit) {
		// delay(100 msec); // game loop delay
		temp_distance = (encoder/180.0)*40.0;
		pos_data.current_position_x = pos_data.acc_position_x + temp_distance*cos(90 - bearing_angle);
		pos_data.current_position_y = pos_data.acc_position_y + temp_distance*sin(90 - bearing_angle);




		if (mapping_state == STATE_STARTING) {

			fTransitionState(&pos_data, &mapping_state, STATE_UNDOCKING);

			// motors to 90

		} else if (mapping_state == STATE_UNDOCKING) {

			if (fCheckSensorFailure()) {
				//kill motors
				fTransitionState(&pos_data, &mapping_state, STATE_ERROR);
				strcpy(error_message,"ran into object while undocking");
			}

			if (temp_distance > 600) {
				//kill motors
				fTransitionState(&pos_data, &mapping_state, STATE_CALIBRATION);

			}

		} else if (mapping_state == STATE_CALIBRATION) {

			fTurnUS(90);
			float distRight = us_distance*10;
			fTurnUS(-180);
			float distLeft = us_distance*10;

			if (distRight > distLeft) {
				reference = LEFT;
				reference_direction_adjust = -1;
			} else {
				reference = RIGHT;
				reference_direction_adjust = 1;
			}

			if (distRight < 400 )


			fBasicTurn(15*reference_direction_adjust,&bearing_angle);
			fTurnUS(-15*reference_direction_adjust);

			fTransitionState(&pos_data,&mapping_state, STATE_DEPLOYMENT);
			//motors to 90



		} else if (mapping_state == STATE_DEPLOYMENT) {

			if (fCheckSensorFailure()) {
				fTransitionState(&pos_data, &mapping_state, STATE_ERROR);
				strcpy(error_message,"ran into object while deploying for mapping");
				// kill motors
			}

			if (ir_distance <= 300) {
				fTransitionState(&pos_data, &mapping_state, STATE_ERROR);
				strcpy(error_message,"not enough space to deploy for mapping");
				// kill motors
			}

			if (us_distance*10 >= 300) {

				if ((us_distance*10 - last_us_distance) >= 300) {
					// kill motors
					fBasicTurn(-15*reference_direction_adjust, &bearing_angle);
					fTurnUS(15*reference_direction_adjust);
					fTransitionState(&pos_data, &mapping_state, STATE_EXECUTING_OUTWARD_TURN);
					// motors to 90
				} else {
					// kill motors
					fBasicTurn((-15)*reference_direction_adjust, &bearing_angle);
					fTransitionState(&pos_data, &mapping_state, STATE_MOVING);
					// motors to 90
				}
			}
		} else if (mapping_state == STATE_MOVING) {
			fNavAdjust(90, reference);

			if ((us_distance*10 - last_us_distance) >= 300) {
				// kill motors
				fTransitionState(&pos_data, &mapping_state, STATE_EXECUTING_OUTWARD_TURN);
				// motors to 90
			}

			if (ir_distance >= 300) {
				// kill motors
				fBasicTurn(90*reference_direction_adjust, &bearing_angle);
				fLogNav(&pos_data);
				// motors to 90

			}

		} else if (mapping_state == STATE_EXECUTING_OUTWARD_TURN) {
			fNavAdjust(90, reference);

			if (us_distance >= 300) {
				// kill motors
				fBasicTurn((-90)*reference_direction_adjust,&bearing_angle);
				fTransitionState(&pos_data, &mapping_state, STATE_MOVING);
				// motors to 90
			}
		}



		// state switch complete

		last_us_distance = us_distance*10; // for determining delta distances
		last_ir_distance = ir_distance;
	}
}


