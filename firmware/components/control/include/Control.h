#pragma once

#include "driver/gpio.h"
#include "driver/ledc.h"

#define LEDC_A1 LEDC_CHANNEL_0
#define LEDC_A2 LEDC_CHANNEL_1

class Control {
    private:
        gpio_num_t AIN1;
        gpio_num_t AIN2; 
        
    public:
        Control(gpio_num_t A1, gpio_num_t A2);

        void open();
        
        void pwmPin(ledc_channel_t ledc_pin, int duty);

        void forwardMax();

        void reverseMax();

        void moveSpeed(int speed);

        void stopSoft();

        void stopHard();

        void moveTo(float error);

};