#include <iostream>
#include "E101.h"
#include "isColour.h"
#include <unistd.h>
#include <vector>
#include <iterator>
#include <algorithm>
#include <numeric>

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


// get vector of center line black pixels for quad2
std::vector<int> get_center_line(int row, int cols) {
    std::vector<int> line;
    for (int col = 0; col < cols; col++) {
	if (isBlack(row, col)) {
	    line.push_back(1);
	} else {
	    line.push_back(0);
	}
    }
    return line;
}

// returns an int vector where each value is the index of that value minus the index of the middle value
std::vector<int>  generate_error_vec(int size) {
    // init vector and fill using iota
    std::vector<int> v(size);
    std::iota(v.begin(), v.end(), 0);
    
    // create iterator and find mid value
    std::vector<int>::iterator i;
    int mid = v[v.size()/2];
    
    // subtract mid from each value
    for (i = v.begin(); i < v.end(); i++) {
	*i -= mid;
    }
    
    return v;
}

// multiplies all values in one vector by their corresponding value in another vector
void convert_line_to_error(std::vector<int> &line, std::vector<int> error_vec) {
    for (int i = 0; i < line.size(); i++) {
	line[i] *= error_vec[i];
    }
}

// returns how far off the centre the black line is for quad2
int get_quad1_error() {
    int img_height = 240;
    int img_width = 320;
    
    //dont necessarily want to be making a new error vector every time - may make parameter
    std::vector<int> error_vec = generate_error_vec(img_width);

    std::vector<int> line = get_center_line(img_height/2, img_width);
    convert_line_to_error(line, generate_error_vec(line.size()));
    return std::accumulate(line.begin(), line.end(), 0);
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
