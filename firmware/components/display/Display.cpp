#include "Display.h"

Display::Display(int SDA, int SCL) : SDA_PIN(SDA), SCL_PIN(SCL) {}

void Display::open() {
    i2c_config_t conf = {};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = SDA_PIN;
    conf.scl_io_num = SCL_PIN;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 400000;

    esp_err_t err = i2c_param_config(I2C_PORT, &conf);
    if (err != ESP_OK) {
        printf("I2C param config failed: %s", esp_err_to_name(err));
        return;
    }
    err = i2c_driver_install(I2C_PORT, conf.mode, 0, 0, 0);
    if (err != ESP_OK) {
        printf("I2C driver install failed: %s", esp_err_to_name(err));
        return;
    }
    // 2. Set up device configuration
    dev = ssd1306_create(I2C_PORT, 0x3C);

    ssd1306_clear_screen(dev, 0x00);
    ssd1306_refresh_gram(dev);
};

ssd1306_handle_t Display::getInst(){
    return dev;
}

void Display::triangleUp(int xPos, int yPos, int size) {
    for(int i = 0; i < size; i++){
        for(int j = 0; j < 2 * i + 1; j++){
            ssd1306_fill_point(dev, xPos + i - j, yPos + i, 1);
        }
    }
}

void Display::triangleLeft(int xPos, int yPos, int size) {
    for(int i = 0; i < size; i++){
        ssd1306_draw_line(dev, xPos + i, yPos + i, xPos + i, yPos - i); 
    }
}

void Display::triangleRight(int xPos, int yPos, int size) {
    for(int i = 0; i < size; i++){
        ssd1306_draw_line(dev, xPos - i, yPos - i, xPos - i, yPos + i); 
    }
}