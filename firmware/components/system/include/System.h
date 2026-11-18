#pragma once

#include <iostream>
#include <cmath>
#include <unordered_map>
#include <cctype>
#include <stdexcept>
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_heap_caps.h"

#define UpButton GPIO_NUM_25
#define RightButton GPIO_NUM_26 //26
#define DownButton GPIO_NUM_32 
#define LeftButton GPIO_NUM_33 //33

class System {
private:
    int upState = 1; //1 not pressed, 0 pressed 
    int rightState = 1;
    int leftState = 1;
    int downState = 1;

    int upPrevState = 1;  
    int rightPrevState = 1;
    int leftPrevState = 1;
    int downPrevState = 1;

    int cursor = 0;
    int max = 0;
    int min = 0;
    bool disabled = false;

    int battery;

    void dpadSetup();

    void batterySetup();

public:
    std::unordered_map<std::string, double> noteFreq;
    
    System();

    void open();
    
    void updateInput();

    bool pressedOK();

    bool pressedBack();

    bool pressedUp();

    bool pressedDown();

    void updateCursor();

    int readCursor();

    void resetCursor(int min, int max);

    void setCursor(int val);

    void disableCursor();

    void enableCursor();

    static void check_heap_usage();

    static float noteToFreq(std::string note, float a4 = 440.0f);
    
    static std::string midiToNote(int midi);

    static int noteToMidi(std::string note);

    static int freqToMidi(float freq);

    void setupNoteMap(float a4 = 440.0f);

    void deleteNoteMap();

    static int mapOnScale(int scaleMin, int scaleMax, float freqMin, float freqMax, float input);

    static int map(int x1, int x2, int y1, int y2, int input);

    int readBattery();
};