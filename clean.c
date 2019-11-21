typedef struct{
	bool interrupt;
	int clean_cycles;
} state;

void go_to(int x, int y, state* cur_state){
}

void clean (state* cur_state)
{
	for (int cycle = 0; cycle < cur_state->clean_cycles; cycle++)
	{
		TFileHandle fin;
		if (!openReadPC(fin, "data.txt"))
		{
			displayString(5, "Error!"); //I would like to use the speaker here
			wait1Msec(5000);
		}
		else
		{
			int x_coord = 0, y_coord = 0;

			while (readIntPC(fin, x_coord))
			{
				readIntPC(fin, y_coord);
				while (cur_state->pos[0] != x_coord && cur_state->pos[1] != y_coord){
					go_to(x_coord, y_coord, cur_state);
					while (cur_state->interrupt);
				}
			}
			closeFilePC(fin);
		}
	}

}

task main(){
	state cur_state;
	cur_state.interrupt = false;
	cur_state.clean_cycles = 3;

	clean(cur_state);
}