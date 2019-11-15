//assumes mop is connected to motorD
const int WATER_LEVEL = 10; // distance of water from the peak of the mop in cm
const int WHEEL_RADIUS = 2.75; //radius of the wheels
const int OVERSHOOT_ERROR = 3; //in terms of nMotorEncoder

void wet()
{

    const int TIMES_WET = 2; 
    for (int wet =0; wet < TIMES_WET; wet++)
    {
        nMotorEncoder[motorD] =0;
        motor[motorD] = 20;
        
        while (((nMotorEncoder[motorD]*WHEEL_RADIUS*PI)/180)- OVERSHOOT_ERROR < WATER_LEVLE)
        {}
        motor[motorD] = 0;
        wait1MSsec(2000);
        motor[motorD] = -20;
        while(nMotorEncoder> OVERSHOOT_ERROR)
        {}
        motor[motorD] = 0;

    }

}


void clean (state* cur_state)
{
    
}