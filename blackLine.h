#include "isColour.h"

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
    std::vector<int> error_vec;
    
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
