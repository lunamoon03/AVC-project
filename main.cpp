#include <iostream>
#include "E101.h"
#include "isColour.h"
#include <unistd.h>

void openGate() {
    char server_address [15] = {'1', '3', '0', '.', '1', '9','5', '.', '3', '.', '9', '1', '\0'};
    int server_port = 1024;
    char message [24] = {'P', 'l', 'e', 'a', 's', 'e', '\0'};
    char password [24] = "";
    // Connect to server
    connect_to_server(server_address, server_port);
    // Initialise contact with server
    send_to_server(message);
    // recieve password from server
    // pass password by reference
    receive_from_server(password);
    // send password back to server
    send_to_server(password);
    // done!
}

void moveForward(int time) {
    set_motors(1, 30);
    set_motors(3, 60);
    hardware_exchange();
    sleep(time);
    set_motors(1, 45);
    set_motors(3, 45);
    hardware_exchange();
}


bool quadrant3Detector() {
    /**
     * This function counts the number of red pixels
     * If there are enough red pixels it returns true
     * All other cases false
    */
    int count = 0;
    const int MIN_REQ_RED = 0;
    
}


int main() {
    int err;
    err = init(0);

    // quad1
    opengate();
    moveForward(4);

    //move forward for an amount of time until reach quad2

    // quad2

    // quad 3

    // quad4


    return err;
}
