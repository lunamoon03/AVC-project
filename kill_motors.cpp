#include "E101.h"

int main() {
    init(0);
    hardware_exchange();
    set_motors(1, 48);
    set_motors(5, 48);
    set_motors(3, 35);
    hardware_exchange();
}