#pragma once

#include "Page.h"

class UpperBar {
protected:
    Display* display;
    ssd1306_handle_t screen;
    System* system;

    void batBox(int numBox);

public:
    UpperBar(Display* display, System* system);

    void tick();
};
