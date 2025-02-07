#include <iostream>
#include "E101.h"
#include "isColour.h"
#include <vector>
#include <chrono>
#include <thread>

const int ZERO_SPEED = 48;
const int FLAT_ANGLE = 30;
const int VERTICAL_ANGLE = 60;
const int LEFT_MOTOR = 5;
const int RIGHT_MOTOR = 1;
const int CAMERA_SERVO = 3;
const int LEFT_BASE = ZERO_SPEED + 6;
const int LEFT_BACK = ZERO_SPEED - 6;
const int RIGHT_BASE = ZERO_SPEED - 11;
const int RIGHT_BACK = ZERO_SPEED + 11;
const int CAMERA_WIDTH = 320;
const int CAMERA_HEIGHT = 240;
const int RED = 0;
const int GREEN = 1;
const int BLUE = 2;
const double KP = 0.1;

/**
 * Resets the motors to default state - stopped and flat
 */
void reset_motors() {
    set_motors(LEFT_MOTOR, ZERO_SPEED);
    set_motors(RIGHT_MOTOR, ZERO_SPEED);
    set_motors(CAMERA_SERVO, FLAT_ANGLE);
    hardware_exchange();
}

/**
 * Sets wheels to moving forward
 */
void forward(){
    set_motors(LEFT_MOTOR, LEFT_BASE);
    set_motors(RIGHT_MOTOR, RIGHT_BASE);
    hardware_exchange();
}

/**
 * Sets wheels to moving backwards
 */
void backward() {
    set_motors(LEFT_MOTOR, LEFT_BACK);
    set_motors(RIGHT_MOTOR, RIGHT_BACK);
    hardware_exchange();
}

/**
 * Stops wheels
 */
void stop() {
    set_motors(LEFT_MOTOR, ZERO_SPEED);
    set_motors(RIGHT_MOTOR, ZERO_SPEED);
    hardware_exchange();
}

/**
 * Robot turns left around left wheel
 */
void turn_left() {
    set_motors(LEFT_MOTOR, ZERO_SPEED);
    set_motors(RIGHT_MOTOR, RIGHT_BASE);
    hardware_exchange();
}

/**
 * Robot turns right around right wheel
 */
void turn_right() {
    set_motors(LEFT_MOTOR, LEFT_BASE);
    set_motors(RIGHT_MOTOR, ZERO_SPEED);
    hardware_exchange();
}

/**
 * Robot turns left around the middle of the two motors
 */
void turn_left_pivot() {
    set_motors(LEFT_MOTOR, LEFT_BACK);
    set_motors(RIGHT_MOTOR, RIGHT_BASE);
    hardware_exchange();
}

/**
 * Makes Robot turn right around the middle of the two motors
 */
void turn_right_pivot() {
    set_motors(LEFT_MOTOR, LEFT_BASE);
    set_motors(RIGHT_MOTOR, RIGHT_BACK);
    hardware_exchange();
}

/**
 * Opens the gate
 */
void open_gate() {
    std::cout<<"Opening Gate"<<std::endl<<std::endl;
    char server_address [15] = {'1', '3', '0', '.', '1', '9','5', '.', '3','.', '9', '1', '\0'};
    int server_port = 1024;
    char message [24] = {'P', 'l', 'e', 'a', 's', 'e', '\0'};
    char password [24] = "";
    // Connect to server
    connect_to_server(server_address, server_port);
    // Initialise contact with server
    send_to_server(message);
    // receive password from server
    receive_from_server(password);
    // send password back to server
    send_to_server(password);
    // done!
}

/**
 * Calculates how off-centre values in a vector of 1s and 0s are
 * @param line
 * @return
 */
double calculate_error(std::vector<int>& line) {
    double error = 0;
    int j = -1 * (int)(line.size()/2);
    for (int i = 0; i < 320; i++) {
        error += line[i] * j;
        j++;
    }
    return error;
}

/**
 * Reads the middle line of the camera
 * Stores non-black pixels as 0, black pixels as 1
 * Also counts total number of black pixels and total number of red pixels
 * @param line
 * @param num_red_pixels
 * @return
 */
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

/**
 * Reads middle line on camera and returns how much the servos must be adjusted to stay on a black line
 * @param line
 * @param num_black_pixels
 * @param num_red_pixels
 * @return
 */
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
    return (calculate_error(line) / num_black_pixels) * KP;
}


/**
 * Returns the adjustment required to centre the robot to a coloured object (such as a cylinder)
 * @param line Middle line of image to scan
 * @param num_colored_pixels Number of coloured pixels that were scanned
 * @param color Colour to scan for
 * @return
 */
double centre_on_color(std::vector<int>& line, int& num_colored_pixels, int color) {
    num_colored_pixels = 0;
    for (int i = 0; i < CAMERA_WIDTH; i++ ) {
        if ((color == RED && isRed(CAMERA_HEIGHT/2, i)) ||
            (color == GREEN && isGreen(CAMERA_HEIGHT/2, i)) ||
            (color == BLUE && isBlue(CAMERA_HEIGHT/2, i))) {
            line.push_back(1);
            num_colored_pixels++;
        } else {
            line.push_back(0);
        }
    }
    if (num_colored_pixels == 0) return 0;
    return (calculate_error(line) / num_colored_pixels) * KP;
}

/**
 * Evaluates the situation of there is no line detected
 * Either moved backwards or turns around if the robot has turned around too much
 * @param back_count
 * @param consecutive_back
 */
void no_line(int& back_count, int& consecutive_back) {
    if (back_count < 20) {
        backward();
        back_count++;
        consecutive_back++;
    } else if (consecutive_back > 10) {
        turn_left_pivot();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        back_count = 0;
        consecutive_back = 0;
    } else {
        turn_left_pivot();
    }
}

/**
 * Robot follows the line
 * Capable of navigating tight turns, right angle turns, and junctions.
 * Always turn left if at junction
 */
void line_follower() {
    std::vector<int> line;
    int num_black_pixels;
    int num_red_pixels;
    int back_count;
    int consecutive_back;

    double adjustment;
    for (int i = 0; i < 2; i++) {
        // initialise tracking variables
        num_black_pixels = 0;
        num_red_pixels = 0;
        back_count = 0;
        consecutive_back = 0;
        while (true) {
            take_picture();
            // clear line so that new values may be entered
            line.clear();
            // find adjustment value, as well as number of black and red pixels
            adjustment = centre_on_line(line, num_black_pixels, num_red_pixels);
            if (num_black_pixels < (CAMERA_WIDTH - 75)) {
                if (num_red_pixels > 30) { // Red is used in quad2 & 3 as divider between quadrants
                    stop();
                    break; // leave quadrant
                } else if (num_black_pixels != 0) { // Navigates the line if there is one
                    consecutive_back = 0;
                    set_motors(LEFT_MOTOR, LEFT_BASE + adjustment);
                    set_motors(RIGHT_MOTOR, RIGHT_BASE + adjustment);
                    hardware_exchange();
                } else {
                    no_line(back_count, consecutive_back);
                }
            } else {
                turn_left();
                //turn_until_centred(line);
            }
            hardware_exchange();

        }
        if (i == 0) {
            std::cout << "Quad 2 to Quad 3 transition" << std::endl;
            // Moves the robot forward for 1 second to avoid getting stuck on red square
            forward();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        } else {
            // Moves the robot forwards again before moving camera up in quad4
            // so robot doesn't get confused with red square
            forward();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
}

/**
 * Moves the robot in accordance to the coloured cylinder section of the course
 */
void color_move() {
    set_motors(CAMERA_SERVO, VERTICAL_ANGLE);
    hardware_exchange();
    stop();
    std::vector<int> line;
    double adjustment;
    int num_colored_pixels;
    int color;
    for (int i = 0; i < 4; i++) {
        // Set color variable according to which iteration of the loop it is
        if (i == 0 || i == 3) color = RED;
        else if (i == 1) color = GREEN;
        else if (i == 2) color = BLUE;
        while (true) {
            take_picture();
            line.clear();
            // Finds necessary adjustment for pointing self towards cylinder
            num_colored_pixels = 0;
            adjustment = centre_on_color(line, num_colored_pixels, color);
            // Breaks and moves to next color if too close to cylinder
            if (num_colored_pixels > 250) {
                // If in the 4th phase, move forward until ball is off the table
                if (i != 3) break;
                do {
                    forward();
                } while (num_colored_pixels > 5);
            } else if (num_colored_pixels != 0) {
                // Drives towards cylinders using quad2/3 logic
                set_motors(LEFT_MOTOR, LEFT_BASE + adjustment);
                set_motors(RIGHT_MOTOR, RIGHT_BASE + adjustment);
                hardware_exchange();
            } else {
                // Pivots if no colored cylinders are seen
                turn_right_pivot();
            }
        }
        // After approaching the cylinder, moves backwards and sleeps for a moment.
        backward();
        std::this_thread::sleep_for(std::chrono::milliseconds(750));
        std::cout << "Moving to new color" << std::endl;
    }
}

int main() {
    int err;
    err = init(0);
    reset_motors();

    // quad1
    open_gate();

    // Follow line through from quad2 to end of quad3
    line_follower();
    std::cout<<"Moving to quad4"<<std::endl;
    // quad4
    color_move();

    return err;
}
