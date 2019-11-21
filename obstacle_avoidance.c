#include "goto.c"
/*
1 - blue and right
2 - white and front
3 - green and left
4 - red and back
*/
int identifyTouch()
{
	int light = getColorAmbient(S1);
	int identify = 0;
	if (light == 2)
		identify = 4;
	else if (light > 2 && light < 5)
		identify = 1;
	else if (light > 4 && light < 7)
		identify = 3;
	else if (light > 8)
		identify = 2;

	return identify;


}

bool triggered ()
{
	if (getColorAmbient(S1) > 1)
		return false;

	return true;
}

task obstacle_avoidance(){
    state current_state;
    current_state.interrupt = &(map[48][48]);
    current_state.hasFinished = &(map[48][0]);
    while (! *(current_state.hasFinished)){
        
    }
}