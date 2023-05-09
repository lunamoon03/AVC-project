#include <iostream>
#include "E101.h"

void opengate() {
    // Connect to server
    connect();
    // Initialise contact with server
    send_to_server("Please");
    // recieve password from server
    // pass password by reference
    char password[24];
    recieve_from_server(&password);
    // send password back to server
    send_to_server(password);

    // done! hopefully
}

// Attempts connect to server until successful
void connect() {
    bool connected = false;
    while (!connected) {
        connected = connect_to_server( , 1024);
    }
}

void moveForward() {

}

int main() {
    int err;
    err = init(0);

    // quad1
    opengate();
    //move forward for an amount of time until reach quad2

    // quad2

    // quad 3

    // quad4


    return 0;
}
