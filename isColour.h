#include "E101.h"
bool isBlack(int row, int col) {
    /**
     * This function takes the row and column of the desired pixel as integers.
     * Then it determines if the pixel is black and returns True if it is and False if it isn't
     */
    const int MAX_INTENSITY = 77;
    // Find each colour to determine if any are too bright
    int red = (int)get_pixel(row, col, 0);
    int green = (int)get_pixel(row, col, 1);
    int blue = (int)get_pixel(row, col, 2);
    // Check if the pixels have too much colour
    if (red > MAX_INTENSITY || green > MAX_INTENSITY || blue > MAX_INTENSITY) return false;
    return true;
}

bool isGreen(int row, int col) {
    /**
     * This function takes the row and column of the desired pixel as integers.
     * Then it determines if the pixel is green and returns True if it is and False if it isn't
     */
    const double unwanted_divisor = 2.1;
    const int MIN_DIFF = 20;
    // Find each colour to determine if any are too bright
    int red = (int)get_pixel(row, col, 0);
    int green = (int)get_pixel(row, col, 1);
    int blue = (int)get_pixel(row, col, 2);
    // Check if the pixels have too much colour
    //if (green < MIN_GREEN || green/unwanted_divisor < red || green/unwanted_divisor < blue) return false;
    //return true;
    return green - red >= MIN_DIFF && green - blue >= MIN_DIFF;
}

bool isRed(int row, int col) {
    /**
     * This function takes the row and column of the desired pixel as integers.
     * Then it determines if the pixel is red and returns True if it is and False if it isn't
     */
    const double unwanted_divisor = 2.1;
    const int MIN_DIFF = 40;
    // Find each colour to determine if any are too bright
    int red = (int)get_pixel(row, col, 0);
    int green = (int)get_pixel(row, col, 1);
    int blue = (int)get_pixel(row, col, 2);
    // Check if the pixels have too much colour
    //if (red < MIN_RED || red/unwanted_divisor < green || red/unwanted_divisor < blue) return false;
    //return true;
    return red - blue >= MIN_DIFF && red - green >= MIN_DIFF;
}

bool isBlue(int row, int col) {
    /**
     * This function takes the row and column of the desired pixel as integers.
     * Then it determines if the pixel is red and returns True if it is and False if it isn't
     * This function will likely require calibration to work Still needs to be tested on the real tubes
     */
    const double unwanted_divisor = 2.1;
    const int MIN_BLUE = 50;
    // Find each colour to determine if any are too bright
    int red = (int)get_pixel(row, col, 0);
    int green = (int)get_pixel(row, col, 1);
    int blue = (int)get_pixel(row, col, 2);
    // Check if the pixels have too much colour
    if (blue < MIN_BLUE || blue/unwanted_divisor < red || blue/unwanted_divisor < green) return false;
    return true;
    //return blue - red >= MIN_DIFF && blue - green >= MIN_DIFF;
}
