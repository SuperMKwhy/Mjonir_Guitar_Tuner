#pragma once

#include "Page.h"

class MenuPage : public Page {
protected:
    Display* display;
    ssd1306_handle_t screen;
    System* system;

    Page* next = nullptr;

    std::string Manu[2] = {
        "Guitar", "Bass"
    };
    int manuSize = sizeof(Manu)/sizeof(Manu[0]);

public:
    MenuPage(Display* display, System* system);

    void open() override;
    void close() override;
    void tick() override;
    bool wantsToExit() override;
    Page* nextPage() override;
};
