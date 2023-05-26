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

void lineFollower() {
    /**
     * Navigates quadrants 2 & 3
     */
    std::vector<int> line;
    int num_black_pixels;
    int num_red_pixels;
    int back_count = 0;
    int consecutive_back = 0;

    double adjustment;
    for (int i = 0; i < 2; i++) {
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
                    } else if (consecutive_back > 10) {
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
        if (i == 0) {
            std::cout << "Quad 2 to Quad 3 transition" << std::endl;
            forward();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
}

int main() {
    int err;
    err = init(0);

    // quad1
    open_gate();

    // Follow line through from quad2 to quad3
    lineFollower();

    // quad4


    return err;
}
