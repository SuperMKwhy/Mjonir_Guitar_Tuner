#include "include/MenuPage.h"
#include "include/GuitarOptionPage.h"

MenuPage::MenuPage(Display* display, System* system) : display(display), screen(display->getInst()), system(system) {}

void MenuPage::open() {
    system->resetCursor(0, 1);
    printf("opened MenuPage\n");
}

void MenuPage::close() {}

void MenuPage::tick() {
    ssd1306_clear_screen(screen, 0x00);
    
    if(system->readCursor() > 0){
        ssd1306_draw_string(screen, 6, 5, (uint8_t*)&(Manu)[system->readCursor() - 1][0], 12, 1);        
        ssd1306_draw_line(screen, 3, 3, 3, 20);
    }

    ssd1306_draw_line(screen, 5, 27, 5, 31); 
    ssd1306_draw_line(screen, 6, 28, 6, 30);  
    ssd1306_fill_point(screen, 7, 29, 1);    

    ssd1306_draw_string(screen, 8, 20, (uint8_t*)&(Manu)[system->readCursor()][0], 16, 1);
    ssd1306_draw_line(screen, 3, 20, 3, 40);

    if(system->readCursor() < manuSize - 1){
        ssd1306_draw_string(screen, 6, 45, (uint8_t*)&(Manu)[system->readCursor() + 1][0], 12, 1);
        ssd1306_draw_line(screen, 3, 40, 3, 60);
    }
    //ssd1306_refresh_gram(screen);

    if(system->pressedOK()) {
        switch(system->readCursor()){
            case 0:{
                next = new GuitarOptionPage(display, system);
                break;
            }case 1:{
                next = nullptr;
                break;
            }
            default:{}
        }
    }
}

bool MenuPage::wantsToExit() {
    return false;
}

Page* MenuPage::nextPage() {
    Page* temp = next;
    next = nullptr;
    return temp;
}