#pragma once

#include "Page.h"

#include "../../tunerLib/include/I2SDevice.h"
#include "../../tunerLib/include/FFTDevice.h"
#include "../../tunerLib/include/Command.h"

#include "../../control/include/Control.h"

#define I2S_WS 12
#define I2S_SCK 14
#define I2S_SD 13

#define sample_size 2048
#define down_factor 5
#define fs 11025

#define A1_PIN GPIO_NUM_18
#define A2_PIN GPIO_NUM_19

class GuitarTunerPage : public Page {
protected:
    Display* display;
    ssd1306_handle_t screen;
    System* system;

    Page* next = nullptr;
    bool exit = false;

    //cursor 0 = 
    int mode = 1; //0 auto, 1 manual
    std::string modeName[2] = {"auto", "manual"};

    bool selectingString = false;
    int currentString = 6;

    int frame = 0;

    std::string options[2] = {
        "Default", "Custom"
    };
    int manuSize = sizeof(options)/sizeof(options[0]);

    std::string stringNote[6] = {"E4", "B3", "G3", "D3", "A2", "E2"};
    bool stringTuned[6] = {false, false, false, false, false, false};
    
    float maxErrorPercent = 0.02;

    Control* motor;

    Device* inmp441;
    Device* fft;

public:
    GuitarTunerPage(Display* display, System* system);
    GuitarTunerPage(Display* display, System* system, std::string* noteList);

    void setting();
    void open() override;
    void close() override;
    void tick() override;
    bool wantsToExit() override;
    Page* nextPage() override;
};