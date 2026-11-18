#include "include/UpperBar.h"

UpperBar::UpperBar(Display* display, System* system) : display(display), screen(display->getInst()), system(system) {}

void UpperBar::tick() {
    ssd1306_draw_line(screen, 100, 1, 121, 1);
    ssd1306_draw_line(screen, 100, 7, 121, 7);
    ssd1306_draw_line(screen, 100, 1, 101, 7);
    ssd1306_draw_line(screen, 120, 1, 121, 7);

    int batteryOnScale = std::min(System::map(101, 120, 0, 100, system->readBattery()), 120);
    
    ssd1306_fill_rectangle(screen, 101, 2, batteryOnScale, 6, 1);

    ssd1306_refresh_gram(screen);
}

void batBox(int numBox){
    //ssd1306_fill_rectangle(screen, 2, 101, )
}
