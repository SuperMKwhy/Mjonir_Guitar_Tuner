#pragma once

#include "ssd1306.h"
#include "driver/i2c.h"

#include "string.h"

#define I2C_PORT I2C_NUM_0

class Display {
    private:
        int SDA_PIN;
        int SCL_PIN;

        ssd1306_handle_t dev;
        
    public:
    Display(int SDA, int SCL);

    void open();

    ssd1306_handle_t getInst();

    void triangleUp(int xPos, int yPos, int size);
    void triangleLeft(int xPos, int yPos, int size);
    void triangleRight(int xPos, int yPos, int size);

};