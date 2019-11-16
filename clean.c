
#include "PC.FileIO.c"
void clean (state* cur_state)
{
    TFileHandle fin;
    bool (fileOkay = openReadPC(fin, "data.txt"));
    if (!fileOkay)
    {
        displayString(5, "Error!"); //I would like to use the speaker here
        wait1Msec(5000);
    }
    else
    {
        int x_coord = 0, y_coord = 0;

        //infinite for loop, only ends when cur_state does not point to clean_cycle
        for (int cycle = 0; cycle < 1 && cur_state -> clean_cycle; cycle --)
        {
            while (readIntPC(fin, x_coord) && cur_state->clean_cycle)
            {
                readIntPC(fin, y_coord); // this is here because unknown how mant arguments readIntPC can take
                //if it can take more than one, this can be placed in while loop
                goto(x_coord, y_coord, cur_state);
            }
        }   
    }

    closeFilePC(fin);
    
    
}