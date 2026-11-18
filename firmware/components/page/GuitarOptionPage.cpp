#include "include/GuitarOptionPage.h"

GuitarOptionPage::GuitarOptionPage(Display* display, System* system) : display(display), screen(display->getInst()), system(system) {}

void GuitarOptionPage::open() {
    system->setCursor(0);
    printf("opened GuitarOptionPage\n");
}

void GuitarOptionPage::close() {
    printf("closed GuitarOptionPage!\n");
}

void GuitarOptionPage::tick() {
    system->resetCursor(0, 2);
    ssd1306_clear_screen(screen, 0x00);
    
    ssd1306_draw_string(screen, 8, 15, (uint8_t*)(options[0].c_str()), 12, 1);
    ssd1306_draw_string(screen, 8, 30, (uint8_t*)(options[1].c_str()), 12, 1);
    ssd1306_draw_string(screen, 8, 45, (uint8_t*)(options[2].c_str()), 12, 1);

    switch(system->readCursor()){
        case 0:{
            display->triangleLeft(65, 21, 5);
            break;
        }case 1:{
            display->triangleLeft(65, 36, 5);
            break;
        }case 2:{
            display->triangleLeft(65, 51, 5);
            break;
        }
        default:{}
    }
    //ssd1306_refresh_gram(screen);

    if(system->pressedOK()) {
        switch(system->readCursor()){
            case 0:{
                next = new GuitarTunerPage(display, system);
                break;
            }case 1:{
                std::string noteList[6] = {"Eb4", "Bb3", "Gb3", "Db3", "Ab2", "Eb2"};
                next = new GuitarTunerPage(display, system, &noteList[0]);
                break;
            }case 2:{
                next = new GuitarCustomPage(display, system);
                break;
            }
            default:{}
        }
    }else if(system->pressedBack()) {
        exit = true;
    }
}

bool GuitarOptionPage::wantsToExit() {
    return exit;
}

Page* GuitarOptionPage::nextPage() {
    Page* temp = next;
    next = nullptr;
    return temp;
}