//
// Created by Josh Cai on 2019-11-18.
//

#include <stdbool.h>


bool get_cycles(){
    int cycles = 1;
    bool selected = false;

    while(!selected) {
        eraseDisplay();

        displayString(4, "Please select the number of cleaning cycles");
        displayString(6, "%i", cycles);

        while (!getButtonPress(buttonLeft) && !getButtonPress(buttonRight) && !getButtonPress(buttonEnter)) {}

        if (getButtonPress(buttonLeft)) {
            while (getButtonPress(buttonLeft)) {}
            if(cycles == 1){
                cycles = 10;
            } else {
                cycles--;
            }
        }
        else if(getButtonPress(buttonRight)){
            while (getButtonPress(buttonRight)) {}
            if(cycles == 10){
                cycles = 1;
            } else {
                cycles++;
            }
        } else {
            selected = true;
        }
    }

    return cycles;
}