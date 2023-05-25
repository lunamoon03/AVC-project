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

int calc_error(std::vector<int>& line) {
    int error = 0;
    int j = -1 * (int)(line.size()/2);
    for (int i = 0; i < 320; i++) {
        error += line[i] * j;
        j++;
    }
    return error;
}


int read_middle_line(std::vector<int>& line, int& num_red_pixels, int img_height, int img_width) {
    int num_black_pixels = 0;
    num_red_pixels = 0;
    for (int i = 0; i < img_width; i++) {
        if (isBlack(img_height/2, i)) {
            line.push_back(1);
            num_black_pixels++;
        } else {
            line.push_back(0);
        }
        if (isRed(img_height/2, i)) {
            num_red_pixels++;
        }
    }
    return num_black_pixels;
}

int read_n_line(std::vector<int>& line, int& num_red_pixels, int img_height, int img_width) {
    int num_black_pixels = 0;
    num_red_pixels = 0;
    for (int i = 0; i < img_height; i++) {
        if(isBlack(i, img_width/3)) {
            line.push_back(1);
            num_black_pixels++;
        } else {
            line.push_back(0);
        }
    }

    return num_black_pixels;
}

void make_error_vec(std::vector<int>& error_vec, int img_width) {
    int j = -(img_width/2);
    for (int i = 0; i < img_width; i++) {
        error_vec.push_back(j);
        j++;
    }
}

void quad2() {
    double tick = 0.01; // 10ms
    //std::vector<int> error_vec;
    std::vector<int> line;
    int num_black_pixels;
    int num_red_pixels;

    int zero_speed = 48;
    int left_motor = 1;
    int left_base = zero_speed-11; // 37
    int left_speed;
    int right_motor = 5;
    int right_base = zero_speed+6; // 54
    int right_speed;
    int img_width = 320;
    int img_height = 240;
    double kp = 0.1;

    //make_error_vec(error_vec, img_width);

    while (true) {
        take_picture();
        // clear line so that new values may be entered
        line.clear();
        num_black_pixels = read_middle_line(line, num_red_pixels, img_height, img_width);
        int error = calc_error(line);

        // normalise error
        if (num_red_pixels > 30) { //?? on the threshold value there
            set_motors(left_motor, zero_speed);
            set_motors(right_motor, zero_speed);
            hardware_exchange();
            break; // leave quad2 code
        } else if (num_black_pixels != 0) {
            error /= num_black_pixels;
            left_speed = left_base + (error * kp);
            right_speed = right_base + (error * kp);
            set_motors(left_motor, left_speed);
            set_motors(right_motor, right_speed);
        } else {
            // move backwards
            left_speed = right_base;
            right_speed = left_base;
            set_motors(left_motor, left_speed);
            set_motors(right_motor, right_speed);
        }
        hardware_exchange();

        sleep(tick);
    }
}


bool quadrantChangeDetector() {
    /**
     * This function counts the number of red pixels
     * If there are enough red pixels it returns true
     * All other cases false
    */
    take_picture();
    int count = 0;
    const int MIN_REQ_RED = 0;
    for (int col = 0; col < 320; col++){
        if (isRed(120, col)) count++;
    }
    if (count <= MIN_REQ_RED) return false;
    return true;
}

void findBoundingBox(int* boundingBox) {
    /**
     * Takes a Reference (Might need to be a pointer) to an array of ints. And changes the values to represent the bounding box of black lines
     * The array should have four elements in it.
    */
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

void branchingPathDetection(bool* directions){
    /**
     * Takes a reference to an array of strings.
     * Gets the bounding box and determines what directions can be moved in
     * Then updates the array of strings with that.
     * This function expects the robot to be aligned with the path
     * Directions 0 is left, 1 is forward, 2 is right
    */
    int boundingBox[4];
    findBoundingBox(boundingBox);
    std::cout<<"Leftmost: "<< boundingBox[0] <<"Rightmost: "<<boundingBox[1]<<"topMost: "<<boundingBox[2]<<"bottomMost"<<boundingBox[3]<<std::endl;
    if (boundingBox[0] < 5) directions[0] = true;
    if (boundingBox[1] > 315) directions[2] = true;
    if (boundingBox[2] < 5) directions[1] = true;

}

void quad3Turn(int direction){
    /**
     * This function handles turning in quadrant 3.  
     * It takes the direction to turn in as an int where 0 is left and 1 is right.
     * Then it turns in that direction until a white pixel is in the center of the screen.
     * Then it stops turning when a black pixel is detected.
     * After this function is called the robot needs to be centered somehow
    */
    while(isBlack(120, 160)){
        if (direction == 0){
            set_motors(1, 42);
            set_motors(5, 42);
        } else {
            set_motors(1, 54);
            set_motors(5, 54);
        }
        hardware_exchange();
    }
    while (!isBlack(120, 160)){
        if (direction == 0){
            set_motors(1, 42);
            set_motors(5, 42);
        } else {
            set_motors(1, 54);
            set_motors(5, 54);
        }

        hardware_exchange();
    }
    // Stop movement
    // left motor
    set_motors(1, 48);
    // right motor
    set_motors(5, 48);
    hardware_exchange();
}

void forward(){
    /**
    * Simple function to set the robot to going forwards
    */
    set_motors(1, 37);
    set_motors(5, 54);
    hardware_exchange();
}

void quad3(){
    /**
     * Function to navigate the robot through quadrant 3.
     * Should keep calling branching path detection.
     * When it reaches a branching path it should prioritise turning left, then straight, then right.
     * 
     * 
    */
    // I SHOULD OF USED CONSTANTS IM SORRY
    // Will require to not be on the change from the previous quadrant too work.
    // Should probably just move forward a bit before this happens
    // Keep going forward until fully past the previous quadrant change detector
    forward();
    std::vector<int> line;
    int blackPixelCount;
    // using an int for speed
    int error;
    const int error_range= 5;

    //while (!quadrantChangeDetector())
        ;
    std::cout<<"Past the quadrant change"<<std::endl;
    sleep(1);
    while (!quadrantChangeDetector()){
        bool directions[3];
        branchingPathDetection(directions);
        std::cout<<"Avaliable directions:";
        if (directions[0]) {std::cout<<" left ";}
        if (directions[1]) {std::cout<<" forward ";}
        if (directions[2]) {std::cout<<" right ";}
        std::cout<<std::endl;
        if (directions[0]){
            std::cout<<"Going left"<<std::endl;
            // Do stuff to turn left
            // Keep going forward until the camera can no longer see left
            while(directions[0]){
                forward();
                branchingPathDetection(directions);
            }
            std::cout<<"Initiating left turn"<<std::endl;
            quad3Turn(0);
        } else if (directions[1]){
            std::cout<<"Going forwards"<<std::endl;
            // go forwards
            // Will need to center somehow
            // Centering somehow
            line.clear();
            take_picture();
            //unused here
            int redPixelCount = 0;
            blackPixelCount = read_middle_line(line, redPixelCount, 240, 320);
            error = calc_error(line)/blackPixelCount;
            while (!(error > -(error_range) && error < error_range)){
                blackPixelCount = read_middle_line(line, redPixelCount, 240, 320);
                error = calc_error(line)/blackPixelCount;
                if (error < 0){
                    // Turn right
                    set_motors(1, 54);
                    set_motors(5, 54);
                } else {
                    // Turn left
                    set_motors(1, 42);
                    set_motors(5, 42);
                }
                hardware_exchange();
            }


            forward();
        } else if (directions[2]){
            std::cout<<"Going right"<<std::endl;
            // Do stuff to turn right
            while(directions[2]){
                forward();
                branchingPathDetection(directions);
            }
            std::cout<<"Initiating right turn"<<std::endl;
            quad3Turn(1);
        } else {
            // When in doubt turn left
            std::cout<<"Going left"<<std::endl;
            quad3Turn(0);
        }
        hardware_exchange();
    }
    std::cout<<"past while loop"<<std::endl;
}

int main() {
    int err;
    err = init(0);


    // quad1
    //open_gate();

    //move forward for an amount of time until reach quad2
    // quad2
    quad2();

    // quad 3
    quad3();
    // quad4


    return err;
}
