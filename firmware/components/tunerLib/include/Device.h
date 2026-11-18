#pragma once

#include "Command.h"
#include <iostream>
#include <cmath>
#include <cstdint>
#include <chrono>
#include <thread>
#include <fstream>

class Device {
    public:
    virtual ~Device() = default;

    virtual void open() = 0;
    virtual void write(int value) = 0;
    virtual void close() = 0;
    
    virtual int ioctl(Command cmd, void* out){
        return -1;
    }
    virtual int ioctl(Command cmd, int16_t* input, void* out){
        return -1;
    }
};