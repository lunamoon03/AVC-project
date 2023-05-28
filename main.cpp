#include <iostream>
#include "E101.h"
#include "isColour.h"
#include <vector>
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
    /**
     * Makes robot turn right
     */
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
    char server_address [15] = {'1', '3', '0', '.', '1', '9','5', '.', '3', '.', '9', '1', '\0'};
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
int calculate_error(std::vector<int>& line) {
    int error = 0;
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
 * Evaluates the situation of there is no line detected
 * Either moved backwards or turns around if the robot has turned around too much
 * @param back_count
 * @param consecutive_back
 */
void no_line(int& consecutive_back) {
    if (consecutive_back < 10) {
        backward();
        consecutive_back++;
    } else {
        turn_left_pivot();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        consecutive_back = 0;
    }
}

/**
 * Robot follows the line
 * Cabable of naviagting tight turns, right angle turns, and junctions.
 * Always turn left if at junction
 */
void line_follower() {
    std::vector<int> line;
    int num_black_pixels;
    int num_red_pixels;
    int consecutive_back;

    double adjustment;
    for (int i = 0; i < 2; i++) {
        // initialise tracking variables
        num_black_pixels = 0;
        num_red_pixels = 0;
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
                    no_line(consecutive_back);
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
        }
    }
}

double centre_on_color(std::vector<int>& line, int& num_colored_pixels, int color) {
    num_colored_pixels = 0;
    for (int i = 0; i < CAMERA_WIDTH; i++ ) {
        if (color == RED && isRed(CAMERA_HEIGHT/2, i)) {
            line.push_back(1);
            num_colored_pixels++;
        } else if (color == GREEN && isGreen(CAMERA_HEIGHT/2, i)) {
            line.push_back(1);
            num_colored_pixels++;
        } else if (color == BLUE && isBlue(CAMERA_HEIGHT/2, i)) {
            line.push_back(1);
            num_colored_pixels++;
        } else {
            line.push_back(0);
        }
    }
    if (num_colored_pixels == 0) return 0;
    return (calculate_error(line) / num_colored_pixels) * KP;
}

void cylinder_move() {
    set_motors(CAMERA_SERVO, VERTICAL_ANGLE);
    std::vector<int> line;
    double adjustment;
    int num_colored_pixels;
    int color;
    // Turn on pivot while scanning for red. Stop when red is in the middle of the cameras view
    for (int i = 0; i < 3; i++) {
        if (i == 0) color = RED;
        else if (i == 1) color = GREEN;
        else if (i == 2) color = BLUE;
        while (true) {
            take_picture();
            line.clear();
            num_colored_pixels = 0;
            adjustment = centre_on_color(line, num_colored_pixels, color);
            if (num_colored_pixels > 100) {
                break;
            } else if (num_colored_pixels != 0) {
                set_motors(LEFT_MOTOR, LEFT_BASE + adjustment);
                set_motors(RIGHT_MOTOR, RIGHT_BASE + adjustment);
                hardware_exchange();
            } else {
                turn_right_pivot();
            }
        }
    }
    // Move towards red using similar algorithm to line follower (keeping red centered)
    // Stop once the number of red pixels is greater than a certain amount (probably 250-300)

    // Repeat above (loop) for green and blue
}

void ball_move() {
    // Turn on pivot while scanning for red as in cylinder move.
    // Move towards red in same way.
    // Once red is higher than threshold, keep moving forwards until red is below threshold (50?)
}

int main() {
    int err;
    err = init(0);
    reset_motors();

    // quad1
    open_gate();

    // Follow line through from quad2 to end of quad3
    line_follower();

    // quad4


    return err;
}
