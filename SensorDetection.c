
//white -> greater than 9
//green -> 5,6
//red --> 2
//blue --> 3,4
//none --> 1


//identifying touch:
/*
1 - blue and right
2 - white and front
3 - green and left
4 - red and back
*/
bool triggered ()
{
	if (getColorAmbient(S1) > 1)
		return false;

	return true;
}
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
	if(light==2)
		identify = 4;
	else if (light> 2& light < 5)
		identify = 1;
	else if (light>4 & light < 7)
		identify =3;
	else if (light>8)
		identify = 2;

	return identify;


}

task main()
{
	SensorType[S1] = sensorEV3_Color;
	wait1Msec(50);
	SensorMode[S1] = modeEV3Color_Ambient;
	wait1Msec(50);

	while (!triggered()){}

	while (1){
		int x = identifyTouch();
		displayString(3, "%d", x);

	}


}
