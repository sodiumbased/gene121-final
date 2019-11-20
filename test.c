#include <stdio.h>
void display_array(int arg[20]){
	for (int i = 0; i < 20; i++){
        printf("%d ", arg[i]);
	}
    printf("\n");
}
void display_char_array(int arg[20]){
    for (int a = 0; a < 20; a++){
        printf("%c ", arg[a]);
    }
    printf("\n");
}
int main(){
    int actions[20]; int args[20];
    for (int i = 0; i < 20; i++){
        actions[i] = i+20;
        args[i] = i;
    }
    for (int i = 0; i < 10; i++){
		int ctemp = actions[i];
		int itemp = args[i];
		actions[i] = actions[19-i];
		args[i] = args[19-i];
		actions[19-i] = ctemp;
		args[19-i] = itemp;
	}
    display_array(args);
    display_array(actions);
}