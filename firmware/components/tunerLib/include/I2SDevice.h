#pragma once

#include "Device.h"
#include <iostream>
#include <cstring>
#include "driver/i2s.h"
#include "esp_err.h"
#include "esp_dsp.h"
#include <cmath>

class I2SDevice : public Device{
    private:
        int I2S_WS;
        int I2S_SCK;
        int I2S_SD;
        int sample_size;
        int fs;
        int down_factor;

        int input_size = sample_size * down_factor;
        int32_t* samples;
        int16_t* temp;
        int16_t* filtered;

    public:
        I2SDevice(int WS, int SCK, int SD, int sample_size, int fs, int down_factor);

        void open() override;
        void close() override;
        void write(int value) override;
        int ioctl(Command cmd, void* out) override;
};