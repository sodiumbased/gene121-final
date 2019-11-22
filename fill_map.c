#include <stdio.h>
#include <stdbool.h>
bool map[49][49];

void fill_map(){
    for (int i = 0; i < 49; i++){
        for (int a = 0; a < 49; a++){
            if (map[i][a] && !map[i][a+1]){
                int length = 0;
                do{
                    a++;
                    length++;
                    if (a == 48 && !map[i][a]){
                        length = -1;
                        break;
                    }
                } while (!map[i][a]);
                for (int c = a-length; c < a; c++){
                    map[i][c] = 1;
                }
            }
        }
    }
}

void print(){
    for (int i = 0; i < 49; i++){
        for (int a = 0; a < 49; a++){
            printf("%d", map[i][a]);
        }
        printf("\n");
    }
}

int main(){
    for (int i = 2; i < 47; i++){
        map[i][2] = 1;
        map[46][i] = 1;
        if (i > 24){
            map[25][i] = 1;
            map[1][i-24] = 1;
            map[i-24][25] = 1;
            map[i][47] = 1;
        }
    }
    map[1][24] = map[24][25] = 1;
    print();
    fill_map();
    print();
}