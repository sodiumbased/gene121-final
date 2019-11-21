int map[49][49];
void fInit();
void fMappingMain();
bool fCheckSensorFailure();

// TODO //
enum MappingState;
void fTurnUS(int degrees);
void fBasicTurn(int degree, int *bearing);
//void fNavAdjust(int speed, enum MappingReference ref);

typedef struct {
	int acc_position_x;
	int acc_position_y;
	int current_position_x;
	int current_position_y;
} TempMapData;


void fTransitionState(TempMapData *data, enum MappingState *state, enum MappingState target);

void fInit() {
	for (int idx = 0; idx < 49; idx++) {
		for (int idx2 = 0; idx2 < 49; idx2++) {
			map[idx][idx2] = 0;
		}
	}
}

bool triggered() {
	return false;
}

void fTurnUS (int degrees){
	nMotorEncoder[motorB] = 0;
	if (degrees > 0){
		motor[motorB] = -15;
		while (nMotorEncoder[motorB] > -degrees);
	}
	else{
		motor[motorB] = 15;
		while (nMotorEncoder[motorB] < -degrees);
	}
	motor[motorB] = 0;
}

void fBasicTurn(int degree, int *bearing) {

	if (degree > 0) {
		motor[motorA] = -5;
		motor[motorD] = 5;
		while (SensorValue[S2] > degree) {}
	} else {
		motor[motorA] = 5;
		motor[motorD] = -5;
		while (SensorValue[S2] < degree) {}
	}

	*bearing += degree;
}

bool fCheckSensorFailure() {
	if (triggered()) {
		return true;
	} else {
		return false;
	}
}

enum MappingReference {
	NOT_SET,RIGHT,LEFT
};


void fNavAdjust(int speed, enum MappingReference ref) {
	if ((SensorValue[S3]*10 < 280) || (SensorValue[S3]*10 > 320)) {
		if (SensorValue[S3]*10 < 280) {
			if (ref != RIGHT) {
				motor[motorA] = speed + 3;
			} else {
				motor[motorD] = speed + 3;
			}
		} else {
			if (ref != LEFT) {
				motor[motorA] = speed + 3;
			} else {
				motor[motorD] = speed + 3;
			}
		}
	} else {
		if (SensorValue[S2] < -1) {
			motor[motorA] = speed + 2;
		} else if (SensorValue[S2] > 1) {
			motor[motorD] = speed + 3;
		} else {
			// set both motors to speed
			if (motor[motorA] != motor[motorD]) {
				motor[motorA] = motor[motorD] = speed;
			}
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
	resetGyro(S2);
	nMotorEncoder[motorA] = 0;
}

void fLogNav(TempMapData *data) {
	data->acc_position_x = data->current_position_x;
	data->acc_position_y = data->current_position_y;

	fResetNav();
}

void fTransitionState(TempMapData *data, enum MappingState *state, enum MappingState target) {
  
  writeDebugStreamLine("transitioning now to %d", target);
	*state = target;
	fLogNav(data);

}

void fExportPosition(int cur_x, int cur_y) {
	int square_x = 25 + cur_x/250;
	int square_y = 25 + cur_y/250;

	if (!map[square_x][square_y]) {
		map[square_x][square_y] = 1;
	}
}

void fGo(int speed) {
	motor[motorA] = motor[motorD] = -speed;
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

	int last_us_distance = -1;



	while (!exit) {

		wait1msec(100);
		temp_distance = -nMotorEncoder[motorA]/180.0*40.0*PI;
		writeDebugStreamLine("%d", temp_distance);
		pos_data.current_position_x = pos_data.acc_position_x + temp_distance*cos(90 - bearing_angle);
		pos_data.current_position_y = pos_data.acc_position_y + temp_distance*sin(90 - bearing_angle);

		fExportPosition(pos_data.current_position_x,pos_data.current_position_y);

		
		if (mapping_state == STATE_STARTING) {
			writeDebugStreamLine("Loop entered");
			fTransitionState(&pos_data, &mapping_state, STATE_UNDOCKING);
			writeDebugStreamLine("Should be firing the motors now");
			fGo(25);

		} else if (mapping_state == STATE_UNDOCKING) {

			if (fCheckSensorFailure()) {
				fGo(0);
				fTransitionState(&pos_data, &mapping_state, STATE_ERROR);
				strcpy(error_message,"ran into object while undocking");
			}

			if (temp_distance > 600) {
				fGo(0);
				fTransitionState(&pos_data, &mapping_state, STATE_CALIBRATION);

			}

		} else if (mapping_state == STATE_CALIBRATION) {

			fTurnUS(90);
			float distLeft = SensorValue[S3]*10;
			fTurnUS(-180);
			float distRight = SensorValue[S3]*10;
			fTurnUS(90);
			if (distRight > distLeft) {
				reference = LEFT;
				reference_direction_adjust = -1;
			} else {
				reference = RIGHT;
				reference_direction_adjust = 1;
			}

			


			fBasicTurn(15*reference_direction_adjust,&bearing_angle);
			fTurnUS(-105*reference_direction_adjust);

			fTransitionState(&pos_data,&mapping_state, STATE_DEPLOYMENT);
			fGo(25);



		} else if (mapping_state == STATE_DEPLOYMENT) {

			if (fCheckSensorFailure()) {
				fTransitionState(&pos_data, &mapping_state, STATE_ERROR);
				strcpy(error_message,"ran into object while deploying for mapping");
				fGo(0);
			}

			if (SensorValue[S4]*10 <= 300) {
				fTransitionState(&pos_data, &mapping_state, STATE_ERROR);
				strcpy(error_message,"not enough space to deploy for mapping");
				fGo(0);
			}

			if (SensorValue[S3]*10 >= 300) {
				fGo(0);

				if ((SensorValue[S3]*10 - last_us_distance) >= 300) {

					fBasicTurn(-15*reference_direction_adjust, &bearing_angle);
					fTurnUS(15*reference_direction_adjust);
					fTransitionState(&pos_data, &mapping_state, STATE_EXECUTING_OUTWARD_TURN);
					fGo(25);

				} else {
					fBasicTurn((-15)*reference_direction_adjust, &bearing_angle);
					fTransitionState(&pos_data, &mapping_state, STATE_MOVING);
					fGo(25);
				}
			}
		} else if (mapping_state == STATE_MOVING) {
			fNavAdjust(25, reference);

			if ((SensorValue[S3]*10 - last_us_distance) >= 300) {
				fGo(0);
				fTransitionState(&pos_data, &mapping_state, STATE_EXECUTING_OUTWARD_TURN);
				fGo(25);
			}

			if (SensorValue[S4]*10 >= 300) {
				fGo(0);
				fBasicTurn(90*reference_direction_adjust, &bearing_angle);
				fLogNav(&pos_data);
				fGo(25);

			}

		} else if (mapping_state == STATE_EXECUTING_OUTWARD_TURN) {
			fNavAdjust(25, reference);

			if (SensorValue[S3] >= 300) {
				fGo(0);
				fBasicTurn((-90)*reference_direction_adjust,&bearing_angle);
				fTransitionState(&pos_data, &mapping_state, STATE_MOVING);
				fGo(25);
			}
		}



		// state switch complete

		last_us_distance = SensorValue[S3]*10; // for determining delta distances


	}
}

task main(){
	fInit();
	fMappingMain();
}