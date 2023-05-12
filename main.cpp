#include <iostream>
#include "E101.h"
#include "isColour.h"
#include <unistd.h>
#include <vector>
#include <iterator>
#include <algorithm>

void open_gate() {
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

void move_forward(int time) {
    set_motors(1, 30);
    set_motors(3, 65);
    hardware_exchange();
    sleep(time);
    set_motors(1, 48);
    set_motors(3, 48);
    hardware_exchange();
}

std::vector<int> get_line(int &row) {
    std::vector<int> line;
    for (int col = 0; col < 320; col++) {
	if (isBlack(row, col)) {
	    line.push_back(1);
	} else {
	    line.push_back(0);
	}
    }
    row++;
    return line;
}

std::vector<int> generate_error_vec(int base_size) {
    int mid = base_size/2;
    std::vector<int> error_vec(base_size);
    //std::iota(error_vec.begin(), error_vec.end()
    
    for (int i = 0; i < base_size; i++) {
	error_vec.push_back(i-mid);
    }
    
    return error_vec;
}


void convert_line_to_error(std::vector<int> &line, std::vector<int> error_vec) {
    for (int i = 0; i < line.size(); i++) {
	line[i] *= error_vec[i];
    }
}


void move_on_line() {
}


int main() {
    int err;
    err = init(0);

    // quad1
    open_gate();

    //move forward for an amount of time until reach quad2

    // quad2

    // quad 3

    // quad4


    return err;
}
