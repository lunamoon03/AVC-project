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

int calc_error(std::vector<int>& line, std::vector<int>& error_vec) {
    int error = 0;
    for (int i = 0; i < 320; i++) {
        error += line[i] * error_vec[i];
    }
    return error;
}

int read_middle_line(std::vector<int>& line, int img_height, int img_width) {
    int num_hits = 0;
    for (int i = 0; i < img_width; i++) {
        if (isBlack(img_height/2, i)) {
            line.push_back(1);
            num_hits++;
        } else {
            line.push_back(0);
        }
    }
    return num_hits;
}

void make_error_vec(std::vector<int>& error_vec, int img_width) {
    int j = -(img_width/2);
    for (int i = 0; i < img_width; i++) {
        error_vec.push_back(j);
        j++;
    }
}

void quad2() {
    int img_height = 240;
    int img_width = 320;
    double tick = 0.01; // 10ms
    std::vector<int> error_vec;
    std::vector<int> line;
    int num_black_pixels;

    int zero_speed = 48;
    int left_base = zero_speed-6;
    int left_speed = 0;
    int right_base = zero_speed+6;
    int right_speed = 0;
    double kp = 0.3;

    make_error_vec(error_vec, img_width);

    while (true) {
        take_picture();
        // clear line so that new values may be entered
        line.clear();
        num_black_pixels = read_middle_line(line, img_height, img_width);
        int error = calc_error(line, error_vec);

        // normalise error
        if (num_black_pixels != 0) {
            error /= num_black_pixels;
            left_speed = left_base + (error * kp);
            right_speed = right_base - (error * kp);
            set_motors(1, left_speed);
            set_motors(3, right_speed);
        } else {
            // move backwards
            left_speed = right_base;
            right_speed = left_base;
            set_motors(1, left_speed);
            set_motors(3, right_speed);
        }
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
