#include <iostream>
#include "E101.h"

void opengate() {
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

    // done! hopefully
}

// Attempts connect to server until successful
/*void connect(char) {
    bool connected = false;
    while (!connected) {
        connected = connect_to_server( , 1024);
    }
}*/

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
