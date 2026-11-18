#pragma once

#include "Device.h"
#include <iostream>
#include <cstring>
#include "esp_err.h"
#include "esp_dsp.h"
#include "esp_heap_caps.h"

class FFTDevice : public Device {
    private: 
        Device* dev;

        float* wind;
        float* fft_data;
        float* R;
        int16_t* input;

        float* baseline;

        int sample_size;
        int fs;
        int down_factor;

        int calibration_time = 0;

    public:
        FFTDevice(Device* dev, int sample_size, int fs, int down_factor);

        void open() override;
        void close() override;
        void write(int value) override;
        int ioctl(Command cmd, void* out) override;
        int ioctl(Command cmd, int16_t* input, void* out) override;
        
};