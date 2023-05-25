#include <iostream>
#include "E101.h"
#include "isColour.h"
#include <unistd.h>
#include <vector>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <thread>

const int ZERO_SPEED = 48;
const int FLAT_ANGLE = 30;
const int VERTICAL_ANGLE = 65;
const int LEFT_MOTOR = 5;
const int RIGHT_MOTOR = 1;
const int CAMERA_SERVO = 3;
const int LEFT_BASE = ZERO_SPEED + 6;
const int LEFT_BACK = ZERO_SPEED - 6;
const int RIGHT_BASE = ZERO_SPEED - 11;
const int RIGHT_BACK = ZERO_SPEED + 11;
const int CAMERA_WIDTH = 320;
const int CAMERA_HEIGHT = 240;
const double KP = 0.1;
const double TICK = 10; // 10ms

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

int calc_error(std::vector<int>& line) {
    int error = 0;
    int j = -1 * (int)(line.size()/2);
    for (int i = 0; i < 320; i++) {
        error += line[i] * j;
        j++;
    }
    return error;
}


int read_middle_line(std::vector<int>& line, int& num_red_pixels) {
    int num_black_pixels = 0;
    num_red_pixels = 0;
    for (int i = 0; i < CAMERA_WIDTH; i++) {
        if (isBlack(CAMERA_HEIGHT/2, i)) {
            line.push_back(1);
            num_black_pixels++;
        } else {
            line.push_back(0);
        }
        if (isRed(CAMERA_HEIGHT/2, i)) {
            num_red_pixels++;
        }
    }
    return num_black_pixels;
}

double centre_on_line(std::vector<int>& line,
                      int& num_black_pixels, int& num_red_pixels) {
    take_picture();
    line.clear();
    num_black_pixels = 0;
    num_red_pixels = 0;
    num_black_pixels = read_middle_line(line, num_red_pixels);
    if (num_black_pixels == 0) {
        return 0;
    }
    return (calc_error(line) / num_black_pixels) * KP;
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
    set_motors(LEFT_MOTOR, LEFT_BASE);
    set_motors(RIGHT_MOTOR, RIGHT_BASE);
    hardware_exchange();
}

void backward() {
    /**
     * Simple function to set the robot to going backwards
     */
    set_motors(LEFT_MOTOR, LEFT_BACK);
    set_motors(RIGHT_MOTOR, RIGHT_BACK);
    hardware_exchange();
}

void stop() {
    /**
     * Simple function to stop the robot
     */
    set_motors(LEFT_MOTOR, ZERO_SPEED);
    set_motors(RIGHT_MOTOR, ZERO_SPEED);
    hardware_exchange();
}

void turn_left() {
    /**
     * Makes robot turn left
     */
    set_motors(LEFT_MOTOR, ZERO_SPEED);
    set_motors(RIGHT_MOTOR, RIGHT_BASE);
    hardware_exchange();
}

void turn_right() {
    /**
     * Makes robot turn right
     */
    set_motors(LEFT_MOTOR, LEFT_BASE);
    set_motors(RIGHT_MOTOR, ZERO_SPEED);
    hardware_exchange();
}

void turn_left_around() {
    set_motors(LEFT_MOTOR, LEFT_BACK);
    set_motors(RIGHT_MOTOR, RIGHT_BASE);
    hardware_exchange();
}

void turn_until_centred(std::vector<int>& line) {
    /**
     * Robot turns left until centred on line
     */
    int num_black_pixels;
    int num_red_pixels;
    double adjustment;

    do {
        take_picture();
        adjustment = centre_on_line(line, num_black_pixels, num_red_pixels);
        turn_left();
    } while (adjustment == 0);
    stop();
}

void lineFollower() {
    /**
     * Navigates quadrant 2
     */
    std::vector<int> line;
    int num_black_pixels;
    int num_red_pixels;
    int back_count = 0;
    int consecutive_back = 0;

    double adjustment;

    while (true) {
        take_picture();
        // clear line so that new values may be entered
        line.clear();
        adjustment = centre_on_line(line, num_black_pixels, num_red_pixels);
        if (num_black_pixels < (CAMERA_WIDTH - 75)) {
            // normalise error
            if (num_red_pixels > 30) { //?? on the threshold value there
                stop();
                break; // leave lineFollower code
            } else if (num_black_pixels != 0) {
                consecutive_back = 0;
                set_motors(LEFT_MOTOR, LEFT_BASE + adjustment);
                set_motors(RIGHT_MOTOR, RIGHT_BASE + adjustment);
                hardware_exchange();
            } else {
                if (back_count < 20) {
                    backward();
                    back_count++;
                    consecutive_back++;
                } else if (consecutive_back > 10){
                    turn_left_around();
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    back_count = 0;
                    consecutive_back = 0;
                } else {
                    turn_left_around();
                }
            }
        } else {
            turn_left();
            //turn_until_centred(line);
        }
        hardware_exchange();

    }
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
            blackPixelCount = read_middle_line(line, redPixelCount);
            error = calc_error(line)/blackPixelCount;
            while (!(error > -(error_range) && error < error_range)){
                blackPixelCount = read_middle_line(line, redPixelCount);
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
    open_gate();

    //move forward for an amount of time until reach lineFollower
    // lineFollower
    lineFollower();
    std::cout<<"Quad 2 to Quad 3 transition"<<std::endl;
    forward();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    lineFollower();

    // quad 3
    //quad3testing();
    // quad4


    return err;
}
