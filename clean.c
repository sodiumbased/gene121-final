
void clean (state* cur_state, ifstream & fin)
{
    int x_coord =0; y_coord =0;

    while(fin >> x_coord >> y_coord && cur_state -> clean_cycle)
    {
        goto(x_coord, y_coord, cur_state);
    }
    
}