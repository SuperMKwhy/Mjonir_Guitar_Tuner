#include "include/Control.h"

Control::Control(gpio_num_t A1, gpio_num_t A2) : AIN1(A1), AIN2(A2) {}

void Control::open() {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << AIN1);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << AIN2);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    ledc_timer_config_t ledc_timer = {};
    ledc_timer.speed_mode       = LEDC_HIGH_SPEED_MODE;
    ledc_timer.duty_resolution  = LEDC_TIMER_10_BIT;   // 0-1023
    ledc_timer.timer_num        = LEDC_TIMER_0;
    ledc_timer.freq_hz          = 1000;                 // 1kHz PWM
    ledc_timer.clk_cfg          = LEDC_AUTO_CLK;
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel_0 = {};
    ledc_channel_0.speed_mode     = LEDC_HIGH_SPEED_MODE;
    ledc_channel_0.channel        = LEDC_A1;
    ledc_channel_0.timer_sel      = LEDC_TIMER_0;
    ledc_channel_0.intr_type      = LEDC_INTR_DISABLE;
    ledc_channel_0.gpio_num       = AIN1;  // แก้เป็นขา GPIO ที่ใช้
    ledc_channel_0.duty           = 0;             // เริ่มต้น duty เป็น 0%
    ledc_channel_0.hpoint         = 0;
    ledc_channel_config(&ledc_channel_0);

    ledc_channel_config_t ledc_channel_1 = {};
    ledc_channel_1.speed_mode     = LEDC_HIGH_SPEED_MODE;
    ledc_channel_1.channel        = LEDC_A2;
    ledc_channel_1.timer_sel      = LEDC_TIMER_0;
    ledc_channel_1.intr_type      = LEDC_INTR_DISABLE;
    ledc_channel_1.gpio_num       = AIN2;  // แก้เป็นขา GPIO ที่ใช้
    ledc_channel_1.duty           = 0;             // เริ่มต้น duty เป็น 0%
    ledc_channel_1.hpoint         = 0;
    ledc_channel_config(&ledc_channel_1);
}

void Control::pwmPin(ledc_channel_t ledc_pin, int duty) {
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, ledc_pin, duty * 1023 /100);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, ledc_pin);
}

void Control::forwardMax() {
    pwmPin(LEDC_A1, 100);
    pwmPin(LEDC_A2, 0);
}

void Control::reverseMax() {
    pwmPin(LEDC_A1, 0);
    pwmPin(LEDC_A2, 100);
}

void Control::moveSpeed(int speed) {
    if(speed >= 0){
        pwmPin(LEDC_A1, speed);
        pwmPin(LEDC_A2, 0);
    }else{
        pwmPin(LEDC_A1, 0);
        pwmPin(LEDC_A2, -speed);
    }
}

void Control::stopSoft() {
    pwmPin(LEDC_A1, 0);
    pwmPin(LEDC_A2, 0);
}

void Control::stopHard() {
    pwmPin(LEDC_A1, 100);
    pwmPin(LEDC_A2, 100);
}

void Control::moveTo(float error){
    if(error < -0.1){
        moveSpeed(100);
    }else if(error >= -0.1 && error < -0.05){
        moveSpeed(50);
    }else if(error >= -0.05 && error < -0.025){
        moveSpeed(20);
    }else if(error >= -0.025 && error < 0){
        moveSpeed(10);
    }else if(error >= 0 && error < 0.025){
        moveSpeed(-10);
    }else if(error >= 0.025 && error < 0.05){
        moveSpeed(-20);
    }else if(error >= 0.05 && error < 0.1){
        moveSpeed(-50);
    }else if(error >= 0.1){
        moveSpeed(-100);
    }else stopSoft();
}