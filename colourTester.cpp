#include<iostream>
#include"E101.h"
#include"isColour.h"

using namespace std;

int main(){
    int err;
    err = init(0);
    //Run a few times
    for (int i = 0; i < 500; i++){
        take_picture();
        int orangeCount = 0;
        for (int row = 0; row <240; row++){
            for (int col = 0; col < 320; col++){
                if (isOrange(row, col)) orangeCount++;
            }
        }
        cout<<i<<": Orange pixels ="<<orangeCount<<endl;
    }   
    return 0;
}