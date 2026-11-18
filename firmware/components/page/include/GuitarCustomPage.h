#pragma once

#include "Page.h"
#include "GuitarTunerPage.h"

class GuitarCustomPage : public Page {
protected:
    Display* display;
    ssd1306_handle_t screen;
    System* system;

    int frame = 0;

    bool selecting = false;
    int currentMidi;
    int backTemp;

    Page* next = nullptr;
    bool exit = false;

    std::string stringNote[6] = {"E4", "B3", "G3", "D3", "A2", "E2"};

public:
    GuitarCustomPage(Display* display, System* system);

    void open() override;
    void close() override;
    void tick() override;
    bool wantsToExit() override;
    Page* nextPage() override;
};
