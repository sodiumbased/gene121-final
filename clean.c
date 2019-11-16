
#include "PC.FileIO.c"
void clean (state* cur_state)
{
    for (int cycle = 0; cycle < cur_state -> clean_cycles; cycle++)
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
                goto(x_coord, y_coord, cur_state);
            }
            closeFilePC(fin);
        }   
    }

    
    
    
}