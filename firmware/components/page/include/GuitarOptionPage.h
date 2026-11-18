#pragma once

#include "Page.h"
#include "GuitarTunerPage.h"
#include "GuitarCustomPage.h"

class GuitarOptionPage : public Page {
protected:
    Display* display;
    ssd1306_handle_t screen;
    System* system;

    Page* next = nullptr;
    bool exit = false;

    std::string options[3] = {
        "Default", "All Flat", "Custom"
    };
    int manuSize = sizeof(options)/sizeof(options[0]);

public:
    GuitarOptionPage(Display* display, System* system);

    void open() override;
    void close() override;
    void tick() override;
    bool wantsToExit() override;
    Page* nextPage() override;
};
