#define MAPPING_DIST 400

int map[49][49];
bool travel_map[49][49];
void fInit();
void fMapPerimeter();
bool fCheckSensorFailure();

// TODO //
enum PerimeterMappingState;
void fTurnUS(int degrees);
void fBasicTurn(int degree, int *bearing);
//void fNavAdjust(int speed, enum MappingReference ref);

typedef struct {
    int acc_position_x;
    int acc_position_y;
    int current_position_x;
    int current_position_y;
} TempMapData;


void fTransitionState(TempMapData *data, enum PerimeterMappingState *state, enum PerimeterMappingState target);

void fInit() {
    for (int idx = 0; idx < 49; idx++) {
        for (int idx2 = 0; idx2 < 49; idx2++) {
            map[idx][idx2] = 0;
            travel_map[idx][idx2] = 0;
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
    wait1msec(250);
    resetGyro(S2);
    wait1msec(50)

    if (degree > 0) {
        motor[motorA] = -5;
        motor[motorD] = 5;
        while (SensorValue[S2] < degree) {}
    } else {
        motor[motorA] = 5;
        motor[motorD] = -5;
        while (SensorValue[S2] > degree) {}
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
    if ((SensorValue[S3]*10 < MAPPING_DIST - 30) || (SensorValue[S3]*10 > MAPPING_DIST + 30)) {
        if (SensorValue[S3]*10 < MAPPING_DIST - 30) {
            if (ref == LEFT) {
                motor[motorA] = -(speed + 4);
            } else {
                motor[motorD] = -(speed + 4);
            }
        } else {
            if (ref == RIGHT) {
                motor[motorA] = -(speed + 4);
            } else {
                motor[motorD] = -(speed + 4);
            }
        }
    } else {
        if (SensorValue[S2] < -1) {
            motor[motorA] = -(speed + 3);
        } else if (SensorValue[S2] > 1) {
            motor[motorD] = -(speed + 3);
        } else {
            // set both motors to speed
            if (motor[motorA] != motor[motorD]) {
                motor[motorA] = motor[motorD] = -speed;
            }
        }
    }
}

enum PerimeterMappingState {
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

void fTransitionState(TempMapData *data, enum PerimeterMappingState *state, enum PerimeterMappingState target) {
  
  writeDebugStreamLine("transitioning now to %d", target);
    *state = target;
    fLogNav(data);

}

void fGo(int speed) {
    motor[motorA] = motor[motorD] = -speed;
}

void fMapPerimeter() {

    TempMapData pos_data;
    pos_data.acc_position_x = 0;
    pos_data.acc_position_y = 0;
    pos_data.current_position_x = 0;
    pos_data.current_position_y = 0;
    int square_x = 0;
    int square_y = 0;

    enum PerimeterMappingState mapping_state = STATE_STARTING;

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

        square_x = 25 + pos_data.current_position_x/250;
        square_y = 25 + pos_data.current_position_y/250;

        if (!map[square_x][square_y]) {
            map[square_x][square_y] = 1;
        }

        if (square_x == 25 && square_y == 26 && false) {
            exit = true;
            writeDebugStreamLine("Finished");
        }



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
            float distRight = SensorValue[S3]*10;

            fTurnUS(-180);
            float distLeft = SensorValue[S3]*10;

            fTurnUS(90);
            if (distRight > distLeft) {
                reference = LEFT;
                writeDebugStreamLine("reference set to left, 1");
                reference_direction_adjust = 1;
            } else {
                reference = RIGHT;
                writeDebugStreamLine("reference set to right, -1");
                reference_direction_adjust = -1;
            }


            fBasicTurn(10*reference_direction_adjust,&bearing_angle);
            fTurnUS(-100*reference_direction_adjust);

            fTransitionState(&pos_data,&mapping_state, STATE_DEPLOYMENT);
            fGo(25);



        } else if (mapping_state == STATE_DEPLOYMENT) {

            if (fCheckSensorFailure()) {
                fTransitionState(&pos_data, &mapping_state, STATE_ERROR);
                strcpy(error_message,"ran into object while deploying for mapping");
                fGo(0);
            }

            if (SensorValue[S4]*10 <= MAPPING_DIST - 100) {
                fTransitionState(&pos_data, &mapping_state, STATE_ERROR);
                strcpy(error_message,"not enough space to deploy for mapping");
                fGo(0);
            }

            if (SensorValue[S3]*10 >= MAPPING_DIST) {
                fGo(0);

                if ((SensorValue[S3]*10 - last_us_distance) >= MAPPING_DIST) {

                    fBasicTurn(-10*reference_direction_adjust, &bearing_angle);
                    fTurnUS(10*reference_direction_adjust);
                    fTransitionState(&pos_data, &mapping_state, STATE_EXECUTING_OUTWARD_TURN);
                    fGo(25);

                } else {
                    fBasicTurn((-10)*reference_direction_adjust, &bearing_angle);
                    fTurnUS(10*reference_direction_adjust);
                    fTransitionState(&pos_data, &mapping_state, STATE_MOVING);
                    fGo(25);
                }
            }
        } else if (mapping_state == STATE_MOVING) {
            fNavAdjust(25, reference);

            if ((SensorValue[S3]*10 - last_us_distance) >= MAPPING_DIST) {
                fGo(0);
                fTransitionState(&pos_data, &mapping_state, STATE_EXECUTING_OUTWARD_TURN);
                fGo(25);
            }

            if (SensorValue[S4]*10 <= MAPPING_DIST - 100) {
                fGo(0);
                fBasicTurn(90*reference_direction_adjust, &bearing_angle);
                writeDebugStreamLine("Performing short turn");
                fLogNav(&pos_data);
                fGo(25);

            }

        } else if (mapping_state == STATE_EXECUTING_OUTWARD_TURN) {
            fNavAdjust(25, reference);

            if (SensorValue[S4] <= MAPPING_DIST - 100) {
                fGo(0);
                fBasicTurn((90)*reference_direction_adjust,&bearing_angle);
                writeDebugStreamLine("Aborting long turn, performing short turn");
                fTransitionState(&pos_data, &mapping_state, STATE_MOVING);
                fGo(25);
            }

            if (temp_distance > MAPPING_DIST) {
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

void fTestNav() {
	for (int i = 0; i < 100; i++) {
		wait1msec(100);
		fNavAdjust(25);
	}
}

void hasSurround(int compare_target,int x_value, int y_value) {
	if (map[x_value - 1][y_value - 1] == compare ||
			map[x_value    ][y_value - 1] == compare ||
			map[x_value + 1][y_value - 1] == compare ||
			map[x_value - 1][y_value    ] == compare ||
			// no value for same index
			map[x_value + 1][y_value    ] == compare ||
			map[x_value - 1][y_value + 1] == compare ||
			map[x_value    ][y_value + 1] == compare ||
			map[x_value + 1][y_value + 1] == compare) {

		return true;

	}
}

void markArray() {
	for (idx1 = 0; idx1 < 49; idx1++) {
		for (int idx2 = 0; idx2 < 49; idx2++) {
			if (hasSurround(1,idx1,idx2)) {
				travel_map[idx1][idx2] = true;
			}
		}
	}
};

enum MappingDirection {
	DIRECTION_LEFT,DIRECTION_RIGHT,DIRECTION_FORWARDS,DIRECTION_BACKWARDS
}

enum InteriorMappingState {
	STATE_MARKING, STATE_PATHING, STATE_ALIGNING
}

void fMapInterior() {
	enum InteriorMappingState mapping_state = STATE_MARKING;
	enum MappingDirection current_direction = DIRECTION_FORWARDS;


}


task main(){
    fInit();
    int no;
    //fMapPerimeter();
    //fBasicTurn(15, &no);
    //fBasicTurn(-15, &no);
    //fBasicTurn(90, &no);
    //fBasicTurn(-90, &no);
    fTestNav();
}
