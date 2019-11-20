//
// Created by Josh Cai on 2019-11-18.
//

bool get_mode(){
    bool mode =  true; //true for mop, false for UV
    bool selected = false;

    while(!selected) {
        eraseDisplay();

        displayString(4, "Please select mop or UV light for cleaning");

        if (mode) {
            displayString(6, "Mop");
        } else {
            displayString(6, "UV");
        }

        while (!getButtonPress(buttonLeft) && !getButtonPress(buttonRight) && !getButtonPress(buttonEnter)) {}

        if (getButtonPress(buttonLeft) || getButtonPress(buttonRight)) {
            while (getButtonPress(buttonLeft) || getButtonPress(buttonRight)) {}
            mode = !mode;
        } else {
            selected = true;
        }
    }

    return mode;
}

task main(){
	get_mode();
}
