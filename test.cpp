#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <chrono>

//#include "Producer.h"

template<class Clock>
void display_precision()
{
    typedef std::chrono::duration<double, std::micro> US;
    US us = typename Clock::duration(1);
    std::cout << us.count() << " us" << std::endl;
}

int main (int argc, char *argv[])
{
    //Producer p;

    //int num_frames = atoi(argv[1]);

    //if ( p.connectToDataStream("com_mod_input.bin") ) {

    //    for (size_t j = 0; j < num_frames; ++j ) {
    //        p.parseFrame();
    //    }

    //} else {
    //    std::cout << "Failed to open input file!" << std::endl;
    //}
    display_precision<std::chrono::high_resolution_clock>();    

    return 0;
}
