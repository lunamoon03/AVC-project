#include <iostream>
#include "E101.h"
#include "isColour.h"
#include <unistd.h>
#include <vector>
#include <iterator>
#include <algorithm>
#include <numeric>

void open_gate() {
    std::cout<<"Opening Gate"<<std::endl<<std::endl;
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

void stop_motors() {
    set_motors(1, 48);
    set_motors(1, 48);
    hardware_exchange();
}

void move_forward(int time) {
    set_motors(1, 31);
    set_motors(3, 65);
    hardware_exchange();
    sleep(time);
    stop_motors();
}

// get vector of center line black pixels for quad2
void get_center_line(int row, int cols, std::vector<int>& line) {
    for (int col = 0; col < cols; col++) {
        if (isBlack(row, col)) {
            line.push_back(1);
        } else {
            line.push_back(0);
        }
    }
}

// multiplies all values in one vector by their corresponding value in another vector
void convert_line_to_error(std::vector<int>& line, std::vector<int>& error_vec) {
    int j = -160;
    for (unsigned int i = 0; i < line.size(); i++) {
	line[i] *= error_vec[j];
        j++;
    }
}

// returns how far off the centre the black line is for quad2
int get_quad2_error(int img_height, int img_width, std::vector<int>& error_vec) {
    //dont necessarily want to be making a new error vector every time - may make parameter
    
    std::vector<int> line;
    get_center_line(img_height/2, img_width, line);
    convert_line_to_error(line, error_vec);
    int result = 0;
    for (int i = 0; i < img_width; i++) {
        result += line[i];
        std::cout << line[i] << std::endl;
        std::cout << result << std::endl;
    }
    return result;
    //return std::accumulate(line.begin(), line.end(), 0);
}

void make_error_vec(std::vector<int> &vector, int size) {
    int j = -160;
    for (int i = 0; i < 320; i++) {
        vector.push_back(j);
        j++;
    }
}

void quad2() {
    // set up unchanging quad1 fields
    int img_height = 240;
    int img_width = 320;
    //create error vec -160 to 159 values
    std::vector<int> error_vec(img_width);
    make_error_vec(error_vec, img_width);
    
    int v_go_r = 42; // 6 less than midpoint
    int v_go_l = 54; // 6 more than midpoint
    double tick = 1; //seconds
    double Kp = 0.0005; // emphasis on current error

    // set up other fields
    int curr_error;
    int  v_l = v_go_l;
    int  v_r = v_go_r;
    double delta_vel;
    
    set_motors(1, v_l);
    set_motors(3, v_r);
    hardware_exchange();
    
    std::cout<<"start scanning"<<std::endl;
    while (true) {
	// turn proportional to error while still going forward
	// adjust speed of appropriate wheel - assume 1 is left and 3 is right
	// midpoint is 48
	
	// adjustment = Kp*error + Kd*de/dt
	// adjustment = difference between left and right speeds
        take_picture();
        curr_error = get_quad2_error(img_height, img_width, error_vec);

        if (true) { //if there is black in the image
            delta_vel = Kp * curr_error;
            v_l = v_go_l + delta_vel;
            v_r = v_go_r + delta_vel;
        } else {
            // reverse
            v_l = v_go_r;
            v_r = v_go_l;
        }
        std::cout<<curr_error<<std::endl;
        std::cout<<v_r<<std::endl;
        std::cout<<v_l<<std::endl;

        set_motors(1, v_l);
        set_motors(3, v_r);

        hardware_exchange();

        sleep(tick);
    }
}
    

/*bool quadrantChangeDetector() {
    /**
     * This function counts the number of red pixels
     * If there are enough red pixels it returns true
     * All other cases false
    
    int count = 0;
    const int MIN_REQ_RED = 0;
    for (int row = 0; row < 240; row++){
        for (int col = 0; col < 320; col++){
            if (isRed(row, col)) count++;
        }
    }
    if (count >= MIN_REQ_RED) return true;
    return false;
}

void findBoundingBox(int& boundingBox) {
    /**
     * Takes a Reference (Might need to be a pointer) to an array of ints. And changes the values to represent the bounding box of black lines
     * The array should have four elements in it.
    
   int leftMostBlack = 400;
   int rightMostBlack = -100;
   int topMostBlack = 400;
   int bottomMostBlack = 0;
   take_picture();
   for (int row = 0; row <240; row++){
        for (int col = 0; col < 320; col++){
            if (isBlack(row, col)) {
                // Update the bounding box based on the location of the pixels
                if (leftMostBlack > col) leftMostBlack = col;
                if (rightMostBlack < col) rightMostBlack = col;
                if (topMostBlack > row) topMostBlack = row;
                if (bottomMostBlack < row) bottomMostBlack = row;
            }
        }
   }
   // Update the array with the bounding box
   boundingBox[0] = leftMostBlack;
   boundingBox[1] = rightMostBlack;
   boundingBox[2] = topMostBlack;
   boundingBox[3] = bottomMostBlack;
}

void branchingPathDetection(bool& directions){
    /**
     * Takes a reference to an array of strings.
     * Gets the bounding box and determines what directions can be moved in
     * Then updates the array of strings with that.
     * This function expects the robot to be aligned with the path
     * Directions 0 is left, 1 is forward, 2 is right
    
   int boundingBox[4];
   findBoundingBox(boundingBox);
   if (boundingBox[0] < 5) directions[0] = true;
   if (boundingBox[1] > 315) directions[2] = true;
   if (boundingBox[2] < 5) directions[1] = true;

}*/


int main() {
    int err;
    err = init(0);

    // quad1
    open_gate();
    
    //move forward for an amount of time until reach quad2

    // quad2
    quad2();
    // quad 3

    // quad4


    return err;
}
